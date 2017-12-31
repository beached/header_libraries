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

		template<typename Type0, typename Type1>
		class union_pair_t {
			enum class data_types : uint_least8_t { type_nothing, type_0, type_1 };

			union {
				Type0 value0;
				Type1 value1;
			};

			data_types type;

			Type0 *get_type0_ptr( ) noexcept {
				return &value0;
			}

			Type0 const *get_type0_ptr( ) const noexcept {
				return &value0;
			}

			Type1 *get_type1_ptr( ) noexcept {
				return &value1;
			}

			Type1 const *get_type1_ptr( ) const noexcept {
				return &value1;
			}

			void destroy_type0( ) {
				type = data_types::type_nothing;
				get_type0_ptr( )->~Type0( );
			}

			void destroy_type1( ) {
				type = data_types::type_nothing;
				get_type1_ptr( )->~Type1( );
			}

			void clear( ) noexcept {
				switch( type ) {
				case data_types::type_nothing:
					break;
				case data_types::type_0:
					destroy_type0( );
					break;
				case data_types::type_1:
					destroy_type1( );
					break;
				}
			}

			template<typename t>
			void store_type0( t &&value ) {
				if( type != data_types::type_0 ) {
					clear( );
				}
				type = data_types::type_nothing;
				value0 = std::forward<t>( value );
				type = data_types::type_0;
			}

			template<typename t>
			void store_type1( t &&value ) {
				if( type != data_types::type_1 ) {
					clear( );
				}
				type = data_types::type_nothing;
				value1 = std::forward<t>( value );
				type = data_types::type_1;
			}

		public:
			union_pair_t( ) = delete;

			~union_pair_t( ) noexcept {
				clear( );
			}

			union_pair_t( Type0 &&ptr )
			  : type{data_types::type_nothing} {

				store_type0( std::move( ptr ) );
			}

			union_pair_t( Type0 const &ptr )
			  : type{data_types::type_nothing} {

				store_type0( ptr );
			}

			union_pair_t( Type1 &&ptr )
			  : type{data_types::type_nothing} {

				store_type1( std::move( ptr ) );
			}

			union_pair_t( Type1 const &ptr )
			  : type{data_types::type_nothing} {

				store_type1( ptr );
			}

			union_pair_t( union_pair_t const &other )
			  : type{data_types::type_nothing} {

				switch( other.type ) {
				case data_types::type_0:
					store_type0( *other.get_type0_ptr( ) );
					break;
				case data_types::type_1:
					store_type1( *other.get_type1_ptr( ) );
					break;
				case data_types::type_nothing:
					break;
				}
			}

			union_pair_t( union_pair_t &&other ) noexcept
			  : type{data_types::type_nothing} {

				switch( other.type ) {
				case data_types::type_0:
					store_type0( std::move( *other.get_type0_ptr( ) ) );
					break;
				case data_types::type_1:
					store_type1( std::move( *other.get_type1_ptr( ) ) );
					break;
				case data_types::type_nothing:
					break;
				}
			}

			union_pair_t &operator=( union_pair_t const &rhs ) {
				if( this != &rhs ) {
					switch( rhs.type ) {
					case data_types::type_0:
						store_type0( *rhs.get_type0_ptr( ) );
						break;
					case data_types::type_1:
						store_type1( *rhs.get_type1_ptr( ) );
						break;
					case data_types::type_nothing:
						break;
					}
				}
				return *this;
			}

			union_pair_t &operator=( union_pair_t &&rhs ) noexcept {
				if( this != rhs ) {
					switch( rhs.type ) {
					case data_types::type_0:
						store_type0( std::move( *rhs.get_type0_ptr( ) ) );
						break;
					case data_types::type_1:
						store_type1( std::move( *rhs.get_type1_ptr( ) ) );
						break;
					case data_types::type_nothing:
						break;
					}
				}
				return *this;
			}

			template<typename Visitor>
			decltype( auto ) visit( Visitor vis ) {
				switch( type ) {
				case data_types::type_0:
					return vis( *get_type0_ptr( ) );
				case data_types::type_1:
					return vis( *get_type1_ptr( ) );
				case data_types::type_nothing:
					break;
				}
				abort( );
			}

			template<typename Visitor>
			decltype( auto ) visit( Visitor vis ) const {
				switch( type ) {
				case data_types::type_0:
					return vis( *get_type0_ptr( ) );
				case data_types::type_1:
					return vis( *get_type1_ptr( ) );
				case data_types::type_nothing:
					break;
				}
				abort( );
			}
		}; // namespace daw
	}    // namespace impl

	template<typename T>
	class observable_ptr_pair {
		impl::union_pair_t<observable_ptr<T>, observer_ptr<T>> m_ptrs;

	public:
		observable_ptr_pair( )
		  : m_ptrs{observable_ptr<T>{}} {}

		observable_ptr_pair( T *ptr )
		  : m_ptrs{observable_ptr<T>{ptr}} {}

		observable_ptr_pair( observable_ptr_pair && ) noexcept = default;
		observable_ptr_pair &operator=( observable_ptr_pair &&rhs ) = default;

		observable_ptr_pair( observable_ptr_pair const &other )
		  : m_ptrs{other.m_ptrs.visit( []( auto const &v ) -> decltype( auto ) { return v->get_observer( ); } )} {}

		observable_ptr_pair &operator=( observable_ptr_pair const &rhs ) {
			if( this != &rhs ) {
				m_ptrs = rhs.m_ptrs.visit( []( auto const &v ) -> decltype( auto ) { return v->get_observer( ); } );
			}
			return *this;
		}

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
