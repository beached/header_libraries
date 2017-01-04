// The MIT License (MIT)
//
// Copyright (c) 2016 Darrell Wright
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

#include <array>
#include <exception>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

#include "daw_traits.h"

namespace daw {
	namespace impl {
		template<typename T>
			struct value_storage {
				using value_type = typename std::remove_cv_t<std::remove_reference_t<T>>;
				using pointer = value_type *;
				using const_pointer = value_type const *;
				using reference = value_type &;
				using const_reference = value_type const &;

			private:
				bool m_occupied;
				alignas( value_type ) std::array<uint8_t, sizeof( T )> m_data;

				void * raw_ptr( ) {
					return static_cast<void *>( m_data.data( ) );
				}

				void const * raw_ptr( ) const {
					return static_cast<void const *>( m_data.data( ) );
				}

				pointer ptr( ) {
					return static_cast<pointer>(raw_ptr( ));
				}

				const_pointer ptr( ) const {
					return static_cast<const_pointer>(raw_ptr( ));
				}

				reference ref( ) {
					return *ptr( );
				}

				const_reference ref( ) const {
					return *ptr( );
				}

				void store( value_type value ) {
					if( m_occupied ) {
						*ptr( ) = std::move( value );
					} else {
						m_occupied = true;	
						new(raw_ptr( )) value_type{ std::move( value ) };
					}
				}

			public:
				value_storage( ): 
					m_occupied{ false }, 
					m_data{ } {

						std::fill( m_data.begin( ), m_data.end( ), static_cast<uint8_t>(0) );		
					}

				value_storage( value_type value ):
					m_occupied{ true }, 
					m_data{ } { 

						store( std::move( value ) );
					}

				value_storage( value_storage const & other ):
					m_occupied{ other.m_occupied },
					m_data{ other.m_data } { }

				value_storage( value_storage && other ):
					m_occupied{ std::exchange( other.m_occupied, false ) },
					m_data{ std::move( other.m_data ) } { }

				void swap( value_storage & rhs ) noexcept {
					using std::swap;
					swap( m_occupied, rhs.m_occupied );
					swap( m_data, rhs.m_data );
				}

				value_storage & operator=( value_storage const & rhs ) {
					if( this != &rhs ) {
						value_storage tmp{ rhs };
						using std::swap;
						swap( *this, rhs );
					}
					return *this;
				}

				value_storage & operator=( value_storage && rhs ) {
					if( this != &rhs ) {
						value_storage tmp{ std::move( rhs ) };
						using std::swap;
						swap( *this, rhs );
					}
					return *this;
				}

				value_storage & operator=( value_type value ) {
					if( m_occupied ) {
						*ptr( ) = std::move( value );
					} else {
						store( std::move( value ) );
						m_occupied = true;
					}
					return *this;
				}

				~value_storage( ) {
					reset( );
				}

				void reset( ) {
					if( m_occupied ) {
						m_occupied = false;
						ref( ).~value_type( );
					}
				}

				bool empty( ) const noexcept {
					return !m_occupied;
				}

				explicit operator bool( ) const noexcept {
					return m_occupied;
				}

				reference operator*( ) {
					if( empty( ) ) {
						throw std::runtime_error( "Attempt to access an empty value" );
					}
					return ref( );
				}

				const_reference operator*( ) const {
					if( empty( ) ) {
						throw std::runtime_error( "Attempt to access an empty value" );
					}
					return ref( );
				}
			};	// value_storage

		template<typename T>
			void swap( value_storage<T> & lhs, value_storage<T> & rhs ) noexcept {
				lhs.swap( rhs );
			}
	}	// namespace impl

	template<class ValueType>
		struct optional {
			using value_type = std::remove_cv_t<std::remove_reference_t<ValueType>>;
			using reference = value_type &;
			using const_reference = value_type const &;
			using pointer = value_type *;
			using pointer_const = value_type const *;
			private:
			impl::value_storage<value_type> m_value;
			public:
			optional( ):
				m_value{ } { }

			optional( optional const & other ):
				m_value{ other.m_value } { }

			optional( optional && other ):
				m_value{ std::move( other.m_value ) } { }

			explicit optional( value_type value ):
				m_value{ std::move( value ) } { }

			optional &operator=( optional const & rhs ) {
				if( this != &rhs ) {
					optional tmp{ rhs };
					swap( *this, tmp );
				}
				return *this;
			}

			template<typename... Args>
				void emplace( Args&&... args ) {
					m_value = value_type{ std::forward<Args>( args )... };
				}

			optional &operator=( optional &&rhs ) {
				if( this != &rhs ) {
					optional tmp{ std::move( rhs ) };
					swap( *this, tmp );
				}
				return *this;
			}

			optional &operator=( value_type value ) {
				m_value = std::move( value );
				return *this;
			}

			~optional( ) = default;

			friend void swap( optional &lhs, optional &rhs ) noexcept {
				using std::swap;
				swap( lhs.m_value, rhs.m_value );
			}

			bool empty( ) const noexcept {
				return m_value.empty( );
			}

			bool has_value( ) const noexcept {
				return static_cast<bool>( m_value );
			}

			explicit operator bool( ) const noexcept {
				return has_value( );
			}

			reference get( ) {
				return *m_value;
			}

			const_reference get( ) const {
				return *m_value;
			}

			reference operator*( ) {
				return get( );
			}

			const_reference operator*( ) const {
				return get( );
			}

			pointer operator->( ) {
				return m_value;
			}

			pointer_const operator->( ) const {
				return m_value;
			}

			void reset( ) {
				m_value.reset( );
			}


			template<typename T, typename = std::enable_if_t<daw::traits::operators::has_op_eq<value_type, T>::value, bool>>
			friend constexpr bool operator==( optional const & lhs, optional<T> const & rhs ) {
				if( lhs.has_value( ) ) {
					if( rhs.has_value( ) ) {
						return lhs.get( ) == rhs.get( );
					}
					return false;
				} if( rhs.has_value( ) ) {
					return false;
				}
				return true;
			}

			template<typename T, typename = std::enable_if_t<daw::traits::operators::has_op_eq<value_type, T>::value, bool>>
			friend constexpr bool operator==( optional const & lhs, T const & rhs ) {
				if( lhs.has_value( ) ) {
					return lhs.get( ) == rhs;
				}
				return false;
			}

			template<typename T, typename = std::enable_if_t<daw::traits::operators::has_op_ne<value_type, T>::value, bool>>
			friend constexpr bool operator!=( optional const & lhs, optional<T> const & rhs ) {
				if( lhs.has_value( ) ) {
					if( rhs.has_value( ) ) {
						return lhs.get( ) != rhs.get( );
					}
					return true;
				} if( rhs.has_value( ) ) {
					return true;
				}
				return false;
			}

			template<typename T, typename = std::enable_if_t<daw::traits::operators::has_op_ne<value_type, T>::value, bool>>
			friend constexpr bool operator!=( optional const & lhs, T const & rhs ) {
				if( lhs.has_value( ) ) {
					return lhs.get( ) != rhs;
				}
				return false;
			}

			template<typename T, typename = std::enable_if_t<daw::traits::operators::has_op_lt<value_type, T>::value, bool>>
			friend constexpr bool operator<( optional const & lhs, optional<T> const & rhs ) {
				if( lhs.has_value( ) ) {
					if( rhs.has_value( ) ) {
						return lhs.get( ) < rhs.get( );
					}
					return false;
				} if( rhs.has_value( ) ) {
					return true;
				}
				return false;
			}

			template<typename T, typename = std::enable_if_t<daw::traits::operators::has_op_lt<value_type, T>::value, bool>>
			friend constexpr bool operator<( optional const & lhs, T const & rhs ) {
				if( lhs.has_value( ) ) {
					return lhs.get( ) < rhs;
				}
				return true;
			}

			template<typename T, typename = std::enable_if_t<daw::traits::operators::has_op_gt<value_type, T>::value, bool>>
			friend constexpr bool operator>( optional const & lhs, optional<T> const & rhs ) {
					if( lhs.has_value( ) ) {
						if( rhs.has_value( ) ) {
							return lhs.get( ) > rhs.get( );
						}
						return true;
					} if( rhs.has_value( ) ) {
						return false;
					}
					return false;
				}

			template<typename T, typename = std::enable_if_t<daw::traits::operators::has_op_gt<value_type, T>::value, bool>>
			friend constexpr bool operator>( optional const & lhs, T const & rhs ) {
				if( lhs.has_value( ) ) {
					return lhs.get( ) > rhs;
				}
				return false;
			}

			template<typename T, typename = std::enable_if_t<daw::traits::operators::has_op_le<value_type, T>::value, bool>>
			friend constexpr bool operator<=( optional const & lhs, optional<T> const & rhs ) {
				if( lhs.has_value( ) ) {
					if( rhs.has_value( ) ) {
						return lhs.get( ) <= rhs.get( );
					}
					return false;
				} if( rhs.has_value( ) ) {
					return true;
				}
				return true;
			}

			template<typename T, typename = std::enable_if_t<daw::traits::operators::has_op_le<value_type, T>::value, bool>>
			friend constexpr bool operator<=( optional const & lhs, T const & rhs ) {
				if( lhs.has_value( ) ) {
					return lhs.get( ) <= rhs;
				}
				return true;
			}

			template<typename T, typename = std::enable_if_t<daw::traits::operators::has_op_ge<value_type, T>::value, bool>>
			friend constexpr bool operator>=( optional const & lhs, optional<T> const & rhs ) {
					if( lhs.has_value( ) ) {
						if( rhs.has_value( ) ) {
							return lhs.get( ) >= rhs.get( );
						}
						return true;
					} if( rhs.has_value( ) ) {
						return false;
					}
					return true;
				}

			template<typename T, typename = std::enable_if_t<daw::traits::operators::has_op_ge<value_type, T>::value, bool>>
			friend constexpr bool operator>=( optional const & lhs, T const & rhs ) {
				if( lhs.has_value( ) ) {
					return lhs.get( ) >= rhs;
				}
				return false;
			}


		};    // class optional

	template<typename T, typename... Args>
		auto make_optional( Args&&... args ) {
			optional<T> result{ };
			result.emplace( std::forward<Args>( args )... );
			return result;
		}

	template<typename T, typename U>
		constexpr bool operator!=( optional<T> const &lhs, U const &rhs ) {
			if( lhs ) {
				return lhs != rhs;
			}
			return false;
		}

	template<typename T, typename U>
		constexpr bool operator<( optional<T> const &lhs, U const &rhs ) {
			if( lhs ) {
				return lhs < rhs;
			}
			return false;
		}

	template<typename T, typename U>
		constexpr bool operator>( optional<T> const &lhs, U const &rhs ) {
			if( lhs ) {
				return lhs > rhs;
			}
			return false;
		}

	template<typename T, typename U>
		constexpr bool operator<=( optional<T> const &lhs, U const &rhs ) {
			if( lhs ) {
				return lhs <= rhs;
			}
			return false;
		}

	template<typename T, typename U>
		constexpr bool operator>=( optional<T> const &lhs, U const &rhs ) {
			if( lhs ) {
				return lhs >= rhs;
			}
			return false;
		}
}    // namespace daw

