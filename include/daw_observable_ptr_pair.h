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
#include "daw_static_array.h"
#include "daw_traits.h"

namespace daw {
	namespace impl {
		template<typename Func, typename... Args>
		using has_void_result = decltype( std::declval<Func>( )( std::declval<Args>( )... ) );

		template<typename Func, typename... Args>
		constexpr bool has_void_result_v = is_detected_v<has_void_result, Func, Args...>;

		template<typename T>
		class observable_ptr_pair_impl {
			enum class data_types : uint_least8_t { type_nothing, type_observable, type_observer };

			union {
				observable_ptr<T> observable_data;
				observer_ptr<T> observer_data;
			};

			data_types type;

			observable_ptr<T> *get_observable_ptr( ) noexcept {
				return &observable_data;
			}

			observable_ptr<T> const *get_observable_ptr( ) const noexcept {
				return &observable_data;
			}

			observer_ptr<T> *get_observer_ptr( ) noexcept {
				return &observer_data;
			}

			observer_ptr<T> const *get_observer_ptr( ) const noexcept {
				return &observer_data;
			}

			void destroy_obserable( ) {
				type = data_types::type_nothing;
				get_observable_ptr( )->~observable_ptr<T>( );
			}

			void destroy_observer( ) {
				type = data_types::type_nothing;
				get_observer_ptr( )->~observer_ptr<T>( );
			}

			void clear( ) noexcept {
				switch( type ) {
				case data_types::type_observable:
					destroy_obserable( );
					break;
				case data_types::type_observer:
					destroy_observer( );
					break;
				case data_types::type_nothing:
					break;
				}
			}

			template<typename Value>
			void store_observable( Value && value ) {
				if( type != data_types::type_observable ) {
					clear( );
				}
				type = data_types::type_nothing;
				observable_data = std::forward<Value>(value);
				type = data_types::type_observable;
			}

			template<typename Value>
			void store_observer( Value && value ) {
				if( type != data_types::type_observer ) {
					clear( );
				}
				type = data_types::type_nothing;
				observer_data = std::forward<Value>(value);
				type = data_types::type_observer;
			}

		public:
			observable_ptr_pair_impl( )
			  //: m_buffer{}
			  : type{data_types::nothing} {
				store_observable( );
			}

			~observable_ptr_pair_impl( ) noexcept {
				clear( );
			}

			observable_ptr_pair_impl( observable_ptr<T> &&ptr )
			  : type{data_types::type_nothing} {

				store_observable( std::move( ptr ) );
			}

			observable_ptr_pair_impl( observable_ptr<T> const &ptr )
			  : type{data_types::type_nothing} {

				store_observable( ptr );
			}

			observable_ptr_pair_impl( observer_ptr<T> &&ptr )
			  : type{data_types::type_nothing} {

				store_observer( std::move( ptr ) );
			}

			observable_ptr_pair_impl( observer_ptr<T> const &ptr )
			  : type{data_types::type_nothing} {

				store_observer( ptr );
			}

			observable_ptr_pair_impl( observable_ptr_pair_impl const &other )
			  : type{data_types::type_nothing} {

				switch( other.type ) {
				case data_types::type_observable:
					store_observer( other.get_observable_ptr( )->get_observer( ) );
					break;
				case data_types::type_observer:
					store_observer( *other.get_observer_ptr( ) );
					break;
				case data_types::type_nothing:
					break;
				}
			}

			observable_ptr_pair_impl( observable_ptr_pair_impl &&other ) noexcept
			  : type{data_types::type_nothing} {

				switch( other.type ) {
				case data_types::type_observable:
					store_observable( std::move( *other.get_observable_ptr( ) ) );
					break;
				case data_types::type_observer:
					store_observer( std::move( *other.get_observer_ptr( ) ) );
					break;
				case data_types::type_nothing:
				default:
					break;
				}
			}

			observable_ptr_pair_impl &operator=( observable_ptr_pair_impl const &rhs ) {
				switch( rhs.type ) {
				case data_types::type_observable:
					store_observer( rhs.get_observable_ptr( )->get_observer( ) );
					break;
				case data_types::type_observer:
					store_observer( *rhs.get_observer_ptr( ) );
					break;
				case data_types::type_nothing:
					break;
				}
			}

			observable_ptr_pair_impl &operator=( observable_ptr_pair_impl &&rhs ) {
				switch( rhs.type ) {
				case data_types::type_observable:
					store_observable( std::move( *rhs.get_observable_ptr( ) ) );
					break;
				case data_types::type_observer:
					store_observer( std::move( *rhs.get_observer_ptr( ) ) );
					break;
				case data_types::type_nothing:
				default:
					break;
				}
			}

			template<typename Visitor>
			decltype( auto ) visit( Visitor vis ) {
				if( type == data_types::type_nothing ) {
					abort( );
				}
				if( type == data_types::type_observable ) {
					return vis( *get_observable_ptr( ) );
				}
				return vis( *get_observer_ptr( ) );
			}

			template<typename Visitor>
			decltype( auto ) visit( Visitor vis ) const {
				if( type == data_types::type_nothing ) {
					abort( );
				}
				if( type == data_types::type_observable ) {
					return vis( *get_observable_ptr( ) );
				}
				return vis( *get_observer_ptr( ) );
			}
		}; // namespace daw
	}    // namespace impl

	template<typename T>
	class observable_ptr_pair {
		impl::observable_ptr_pair_impl<T> m_ptrs;

	public:
		observable_ptr_pair( )
		  : m_ptrs{observable_ptr<T>{}} {}

		observable_ptr_pair( T *ptr )
		  : m_ptrs{observable_ptr<T>{ptr}} {}

		observable_ptr_pair( observable_ptr_pair && ) noexcept = default;
		observable_ptr_pair( observable_ptr_pair const &other ) = default;
		observable_ptr_pair &operator=( observable_ptr_pair const &rhs ) = default;

		template<typename Visitor>
		decltype( auto ) apply_visitor( Visitor vis ) {
			return m_ptrs.visit( std::move( vis ) );
		}

		template<typename Visitor>
		decltype( auto ) apply_visitor( Visitor vis ) const {
			return m_ptrs.visit( std::move( vis ) );
		}

		template<typename Visitor>
		decltype( auto ) visit( Visitor vis ) {
			return m_ptrs.visit( [&]( auto &p ) -> decltype( auto ) { return vis( *p.borrow( ) ); } );
		}

		template<typename Visitor>
		decltype( auto ) visit( Visitor vis ) const {
			return m_ptrs.visit( [&]( auto const &p ) -> decltype( auto ) { return vis( *p.borrow( ) ); } );
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
