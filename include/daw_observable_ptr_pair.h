// The MIT License (MIT)
//
// Copyright (c) 2017 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "daw_observable_ptr.h"
#include "daw_traits.h"

#include <boost/variant.hpp>

namespace daw {
	namespace impl {
		template<typename Func, typename... Args>
		using has_void_result = decltype( std::declval<Func>( )( std::declval<Args>( )... ) );

		template<typename Func, typename... Args>
		constexpr bool has_void_result_v = is_detected_v<has_void_result, Func, Args...>;
	} // namespace impl

	template<typename T>
	class observable_ptr_pair {
		boost::variant<daw::observable_ptr<T>, daw::observer_ptr<T>> m_ptrs;

	public:
		observable_ptr_pair( )
		  : m_ptrs{observable_ptr<T>{}} {}

		observable_ptr_pair( T *ptr )
		  : m_ptrs{observable_ptr<T>{ptr}} {}

		observable_ptr_pair( observable_ptr_pair && ) noexcept = default;
		observable_ptr_pair &operator=( observable_ptr_pair && ) noexcept = default;
		observable_ptr_pair( observable_ptr_pair const &other )
		  : m_ptrs{other.get_observer( )} {}

		observable_ptr_pair &operator=( observable_ptr_pair const &rhs ) {
			m_ptrs = rhs.get_observer( );
		}

		template<typename Visitor>
		decltype( auto ) apply_visitor( Visitor vis ) {
			return boost::apply_visitor( std::move( vis ), m_ptrs );
		}

		template<typename Visitor>
		decltype( auto ) apply_visitor( Visitor vis ) const {
			return boost::apply_visitor( std::move( vis ), m_ptrs );
		}

		template<typename Visitor>
		decltype( auto ) visit( Visitor vis ) {
			return boost::apply_visitor( [&]( auto &p ) -> decltype( auto ) { return vis( *p.borrow( ) ); }, m_ptrs );
		}

		template<typename Visitor>
		decltype( auto ) visit( Visitor vis ) const {
			return boost::apply_visitor( [&]( auto const &p ) -> decltype( auto ) { return vis( *p.borrow( ) ); }, m_ptrs );
		}

		decltype( auto ) operator-> ( ) const {
			return apply_visitor( []( auto const &obs_ptr ) { return obs_ptr.operator->( ); } );
		}

		decltype( auto ) operator*( ) const {
			return apply_visitor( []( auto const &obs_ptr ) { return obs_ptr.operator*( ); } );
		}

		operator bool( ) const {
			return apply_visitor( []( auto const &obs_ptr ) { return obs_ptr.operator bool( ); } );
		}

		operator observer_ptr<T>( ) const {
			return apply_visitor( []( auto const &obs_ptr ) { return obs_ptr.operator observer_ptr<T>( ); } );
		}

		decltype( auto ) get_observer( ) const {
			return apply_visitor( []( auto const &obs_ptr ) { return obs_ptr.get_observer( ); } );
		}

		decltype( auto ) borrow( ) const {
			return apply_visitor( []( auto const &obs_ptr ) { return obs_ptr.borrow( ); } );
		}

		decltype( auto ) try_borrow( ) const {
			return apply_visitor( []( auto const &obs_ptr ) { return obs_ptr.try_borrow( ); } );
		}

		decltype( auto ) get( ) const {
			return apply_visitor( []( auto const &obs_ptr ) { return obs_ptr.get( ); } );
		}

		template<typename Callable>
		decltype( auto ) lock( Callable c ) const {
			return apply_visitor( [&c]( auto const &obs_ptr ) { return obs_ptr.lock( std::move( c ) ); } );
		}
	};

	template<typename T, typename... Args>
	observable_ptr_pair<T>
	make_observable_ptr_pair( Args &&... args ) noexcept( noexcept( new T( std::forward<Args>( args )... ) ) ) {
		T *tmp = new T( std::forward<Args>( args )... );
		if( !tmp ) {
			return observable_ptr_pair<T>{nullptr};
		}
		return observable_ptr_pair<T>{tmp};
	}

} // namespace daw
