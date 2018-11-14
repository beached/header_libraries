// The MIT License (MIT)
//
// Copyright (c) 2018 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <cstddef>
#include <new>
#include <type_traits>

#include "daw_traits.h"

namespace daw {
	template<size_t N, typename Base>
	struct function_storage {
		std::aligned_storage_t<N> m_data;

		// TODO look at noexcept stuff
		function_storage( function_storage const & ) noexcept = default;
		function_storage( function_storage && ) noexcept = default;

		function_storage & operator=( function_storage const & rhs ) {
			if( this == &rhs ) {
				return *this;
			}
			clean( );
			m_data = rhs.m_data;
			return *this;
		}
			
		function_storage &operator=( function_storage && ) noexcept = default;

		template<typename Func>
		function_storage( Func &&f ) {
			store( std::forward<Func>( f ) );
		}

		Base *ptr( ) noexcept {
			return reinterpret_cast<Base *>( &m_data );
		}

		Base const *ptr( ) const noexcept {
			return reinterpret_cast<Base const *>( &m_data );
		}

		void clean( ) {
			ptr( )->~Base( );
		}

		~function_storage( ) {
			clean( );
		}

		template<typename Func>
		void store( Func &&f ) {
			static_assert(
			  sizeof( Func ) <= N,
			  "Attempt to store a function that is larger than MaxSize." );
			new( &m_data ) daw::remove_cvref_t<Func>( std::forward<Func>( f ) );
		}

		Base &operator*( ) noexcept {
			return *ptr( );
		}

		Base const &operator*( ) const noexcept {
			return *ptr( );
		}

		Base *operator->( ) noexcept {
			return ptr( );
		}

		Base const *operator->( ) const noexcept {
			return ptr( );
		}
	};

	template<size_t, typename>
	class basic_function;

	template<size_t MaxSize, typename Result, typename... FuncArgs>
	class basic_function<MaxSize, Result( FuncArgs... )> {
		struct function_base {
			virtual Result operator( )( FuncArgs... ) const = 0;
			virtual Result operator( )( FuncArgs... ) = 0;
			virtual bool empty( ) const = 0;

			function_base( ) noexcept = default;
			virtual ~function_base( ) = default;
			function_base( function_base const & ) noexcept = default;
			function_base( function_base && ) noexcept = default;
			function_base &operator=( function_base const & ) noexcept = default;
			function_base &operator=( function_base && ) noexcept = default;
		};

		struct empty_child final: function_base {
			[[noreturn]]
			Result operator( )( FuncArgs... ) const noexcept override { std::terminate( ); }
			[[noreturn]]
			Result operator( )( FuncArgs... ) noexcept override { std::terminate( ); }

			bool empty( ) const override { return true; }
		};

		template<typename Func>
		struct function_child final: function_base {
			Func m_func;

			template<typename F>
			function_child( F &&func ) noexcept(
			  std::is_nothrow_constructible_v<Func, F> )
			  : m_func( std::forward<F>( func ) ) {}

			Result operator( )( FuncArgs... args ) override {
				return m_func( std::move( args )... );
			}

			Result operator( )( FuncArgs... args ) const override {
				return m_func( std::move( args )... );
			}

			bool empty( ) const override { return false; }
		};

		function_storage<MaxSize, function_base> m_storage;

	public:
		basic_function( ) noexcept: m_storage( empty_child{} ) {} 

		template<typename Func>
		basic_function( Func &&f )
		  : m_storage( function_child<Func>( std::forward<Func>( f ) ) ) {

			static_assert(
			  sizeof( std::decay_t<Func> ) <= MaxSize,
			  "Attempt to store a function that is larger than MaxSize." );
		}

		template<typename Func>
		basic_function &operator=( Func &&f ) {
			static_assert(
			  sizeof( std::decay_t<Func> ) <= MaxSize,
			  "Attempt to store a function that is larger than MaxSize." );

			m_storage = function_storage<MaxSize, function_base>(
			  function_child<Func>( std::forward<Func>( f ) ) );
			return *this;
		}

		template<typename... Args>
		Result operator( )( Args &&... args ) {
			return ( *m_storage )( std::forward<Args>( args )... );
		}

		template<typename... Args>
		Result operator( )( Args &&... args ) const {
			return ( *m_storage )( std::forward<Args>( args )... );
		}

		bool empty( ) const {
			return m_storage->empty( );
		}
	};
} // namespace daw

