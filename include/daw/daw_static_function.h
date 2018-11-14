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
		std::aligned_storage_t<N> m_data{};

		constexpr function_storage( ) noexcept = default;
		constexpr function_storage( function_storage const & ) = default;
		constexpr function_storage( function_storage && ) = default;
		function_storage &operator=( function_storage const & ) = default;
		function_storage &operator=( function_storage && ) = default;

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

		~function_storage( ) {
			ptr( )->~Base( );
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

			function_base( ) noexcept = default;
			virtual ~function_base( ) = default;
			function_base( function_base const & ) noexcept = default;
			function_base( function_base && ) noexcept = default;
			function_base &operator=( function_base const & ) noexcept = default;
			function_base &operator=( function_base && ) noexcept = default;
		};

		template<typename Func>
		struct function_child : function_base {
			Func m_func;
			function_child( Func const &func )
			  : m_func( func ) {}
			function_child( Func &&func )
			  : m_func( std::move( func ) ) {}

			~function_child( ) override = default;
			function_child( function_child const & ) = default;
			function_child( function_child && ) = default;
			function_child &operator=( function_child const & ) = default;
			function_child &operator=( function_child && ) = default;

			Result operator( )( FuncArgs... args ) override {
				return m_func( std::move( args )... );
			}

			Result operator( )( FuncArgs... args ) const override {
				return m_func( std::move( args )... );
			}
		};

		function_storage<MaxSize, function_base> m_storage;

	public:
		template<typename Func>
		basic_function( Func &&f )
		  : m_storage( function_child<Func>( std::forward<Func>( f ) ) ) {}

		template<typename... Args>
		Result operator( )( Args &&... args ) {
			return ( *m_storage )( std::forward<Args>( args )... );
		}

		template<typename... Args>
		Result operator( )( Args &&... args ) const {
			return ( *m_storage )( std::forward<Args>( args )... );
		}
	};
} // namespace daw

