// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_cxmath.h"

#include <algorithm>
#include <cassert>
#include <ciso646>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

#if defined( _WIN32 ) or defined( _WIN64 )
#include <malloc.h>
#define MALLOC_SIZE( ... ) _msize( __VA_ARGS__ )
#elif defined( __linux__ )
#include <malloc.h>
#define MALLOC_SIZE( ... ) malloc_usable_size( __VA_ARGS__ )
#else
#include <malloc/malloc.h>
#define MALLOC_SIZE( ... ) malloc_size( __VA_ARGS__ )
#endif

#if defined( __cpp_constexpr_dynamic_alloc )
#define DAW_CPP20CXDTOR constexpr
#else
#define DAW_CPP20CXDTOR
#endif

template<typename T, typename U, typename = void>
struct has_op_minus : std::false_type {};

template<typename T, typename U>
struct has_op_minus<
  T, U, std::void_t<decltype( std::declval<T>( ) - std::declval<U>( ) )>>
  : std::true_type {};

template<typename T, typename = void>
struct is_iterator : std::false_type {};

template<typename T>
struct is_iterator<
  T, std::void_t<decltype(
       std::add_pointer_t<typename std::iterator_traits<T>::value_type>{ } )>>
  : std::true_type {};

template<typename T, typename = void>
struct has_reallocate : std::false_type {};

template<typename T>
struct has_reallocate<T, std::void_t<typename T::has_realloc>>
  : std::true_type {};

template<typename T>
struct MemoryAlloc {
	using value_type = T;
	using pointer = T *;
	using size_type = std::size_t;
	using has_realloc = void;

	constexpr MemoryAlloc( ) = default;

	static inline pointer allocate( size_type count ) {
		return reinterpret_cast<T *>( std::malloc( sizeof( T ) * count ) );
	}

	// Attempt to reallocate if it can be done within the current block
	// The ptr returned should
	static inline pointer reallocate( T *old_ptr, size_type new_size ) {
		auto const max_sz = MALLOC_SIZE( old_ptr );
		if( (new_size * 8U) <= max_sz ) {
			return old_ptr;
		}
		return reinterpret_cast<T *>( std::malloc( new_size * sizeof( T ) ) );
	}

	static inline void deallocate( T *ptr, size_type ) {
		std::free( ptr );
	}
};

template<typename Iterator, typename OutputIterator>
constexpr OutputIterator move_n( Iterator first, std::size_t sz,
                                 OutputIterator out_it ) noexcept {
	while( sz-- > 0 ) {
		*out_it++ = std::move( *first++ );
	}
	return out_it;
}

template<typename T, typename... Args>
constexpr T *construct_at( T *p, Args &&...args ) {
	if constexpr( std::is_aggregate_v<T> ) {
		return ::new( static_cast<void *>( p ) ) T{ std::forward<Args>( args )... };
	} else {
		return ::new( static_cast<void *>( p ) ) T( std::forward<Args>( args )... );
	}
}

template<typename Allocator>
struct AllocDeleter : private Allocator {
	std::size_t alloc_size;

	using pointer = typename std::allocator_traits<Allocator>::pointer;

	constexpr AllocDeleter( std::size_t sz, Allocator a = Allocator{ } )
	  : Allocator{ a }
	  , alloc_size( sz ) {}

	constexpr void operator( )( pointer p ) const {
		this->deallocate( p, alloc_size );
	}
};

struct sized_for_overwrite_t {};
inline constexpr sized_for_overwrite_t sized_for_overwrite =
  sized_for_overwrite_t{ };

template<typename T, typename Alloc = MemoryAlloc<T>>
struct Vector : private Alloc {
	using value_type = T;
	using allocator_type = Alloc;
	using pointer = T *;
	using const_pointer = T const *;
	using reference = T &;
	using const_reference = T const &;
	using rvalue_reference = T &&;
	using iterator = T *;
	using const_iterator = T const *;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;

private:
	using uptr_del = AllocDeleter<allocator_type>;
	using uptr_t = std::unique_ptr<value_type[], uptr_del>;
	uptr_t m_data = nullptr;
	size_type m_size = 0;

	uptr_t make_copy( Vector const &other ) {
		auto result =
		  uptr_t( this->allocate( other.m_capacity ), { other.m_capacity } );
		std::copy_n( other.m_data.get( ), other.m_size, result.data( ) );
		return result;
	}

	constexpr Vector( sized_for_overwrite_t, size_type sz )
	  : m_data( this->allocate( sz ), uptr_del{ sz } )
	  , m_size( sz ) {}

	constexpr void resize_for_overwrite( size_type sz ) noexcept {
		if( sz < m_size ) {
			if constexpr( std::is_trivially_destructible_v<value_type> ) {
				m_size = sz;
			} else {
				if( m_size == 0 ) {
					return;
				}
				while( m_size-- > sz ) {
					std::destroy_at( m_data.get( ) + m_size );
				}
			}
		} else {
			if( sz > capacity( ) ) {
				auto const new_cap =
				  daw::cxmath::round_pow2( std::max( sz, capacity( ) * 2U ) );
				pointer const new_ptr = [&] {
					if constexpr( has_reallocate<allocator_type>::value ) {
						return this->reallocate( m_data.get( ), new_cap );
					} else {
						return this->allocate( new_cap );
					}
				}( );
				if( new_ptr != m_data.get( ) ) {
					if constexpr( std::is_nothrow_move_constructible_v<T> ) {
						(void)move_n( m_data.get( ), m_size, new_ptr );
					} else {
						(void)std::copy_n( m_data.get( ), m_size, new_ptr );
						resize( 0 );
					}
					m_data.reset( new_ptr );
				} else {
					std::cout << "Yay\n";
				}
				m_data.get_deleter( ).alloc_size = new_cap;
			}
			m_size = sz;
		}
	}

public:
	explicit constexpr Vector( allocator_type const &alloc = allocator_type{ } )
	  : allocator_type{ alloc } {}

	constexpr Vector( Vector && ) noexcept = default;
	constexpr Vector &operator=( Vector && ) noexcept = default;
	DAW_CPP20CXDTOR ~Vector( ) {}

	constexpr Vector( Vector const &other )
	  : m_data( make_copy( other ) )
	  , m_size( other.m_size ) {}

	constexpr Vector &operator=( Vector const &rhs ) {
		if( this != &rhs ) {
			m_data.reset( make_copy( rhs ) );
			m_size = rhs.m_size;
		}
		return *this;
	}

	template<typename Operation>
	constexpr Vector( sized_for_overwrite_t, size_type sz, Operation op )
	  : Vector( sized_for_overwrite, sz ) {
		m_size = op( m_data.get( ), m_size );
		assert( m_size <= sz );
	}

	template<size_type N>
	constexpr Vector( value_type const ( &ary )[N] )
	  : Vector( sized_for_overwrite, N ) {

		pointer const p = std::copy_n( ary, N, m_data.get( ) );
		m_size = static_cast<size_type>( p - m_data.get( ) );
	}

	template<size_type N>
	constexpr Vector( value_type( &&ary )[N] )
	  : Vector( sized_for_overwrite, N ) {

		pointer const p = move_n( ary, N, m_data.get( ) );
		m_size = static_cast<size_type>( p - m_data.get( ) );
	}

	template<typename IteratorF, typename IteratorL,
	         std::enable_if_t<( has_op_minus<IteratorF, IteratorL>::value and
	                            is_iterator<IteratorF>::value ),
	                          std::nullptr_t> = nullptr>
	constexpr Vector( IteratorF first, IteratorL last )
	  : Vector( sized_for_overwrite, static_cast<size_type>( last - first ) ) {

		pointer const p = std::copy_n( first, last - first, m_data.get( ) );
		m_size = static_cast<size_type>( p - m_data.get( ) );
	}

	template<typename IteratorF, typename IteratorL,
	         std::enable_if_t<( not has_op_minus<IteratorF, IteratorL>::value and
	                            is_iterator<IteratorF>::value ),
	                          std::nullptr_t> = nullptr>
	constexpr Vector( IteratorF first, IteratorL last ) {
		while( first != last ) {
			puah_back( *first++ );
		}
	}

	template<typename Operation>
	constexpr void resize_for_overwrite( size_type sz, Operation op ) noexcept {
		resize_for_overwrite( sz );
		m_size = op( data( ), size( ) );
	}

	constexpr void resize( size_type sz ) noexcept {
		resize_for_overwrite( sz, [old_sz = size( )]( pointer p, size_type n ) {
			if( n <= old_sz ) {
				return n;
			}
			(void)std::fill_n( p + old_sz, n - old_sz, value_type{ } );
			return n;
		} );
	}

	template<typename U>
	constexpr void resize( size_type sz, U const &value ) noexcept {
		resize_for_overwrite( sz,
		                      [old_sz = size( ), &value]( pointer p, size_type n ) {
			                      if( n <= old_sz ) {
				                      return n;
			                      }
			                      (void)std::fill_n( p + old_sz, n - old_sz, value );
		                      } );
	}

	constexpr pointer data( ) {
		return m_data.get( );
	}

	constexpr const_pointer data( ) const {
		return m_data.get( );
	}

	constexpr size_type size( ) const {
		return m_size;
	}

	constexpr size_type capacity( ) const {
		if( m_data ) {
			return m_data.get_deleter( ).alloc_size;
		}
		return 0;
	}

	constexpr void reserve( size_type n ) {
		if( n > capacity( ) ) {
			resize_for_overwrite( n );
		}
	}
	constexpr reference push_back( const_reference v ) {
		if( m_size >= capacity( ) ) {
			resize( m_size + 1 );
		}
		return *construct_at( m_data.get( ) + ( m_size++ ), v );
	}

	constexpr reference push_back( rvalue_reference v ) {
		if( m_size >= capacity( ) ) {
			resize( m_size + 1 );
		}
		return *construct_at( m_data.get( ) + ( m_size++ ), std::move( v ) );
	}

	template<typename... Args>
	constexpr reference emplace_back( Args &&...args ) {
		if( m_size >= capacity( ) ) {
			resize( m_size + 1 );
		}
		return *construct_at( m_data.get( ) + ( +m_size++ ),
		                      std::forward<Args>( args )... );
	}

	iterator begin( ) {
		return iterator{ data( ) };
	}

	const_iterator begin( ) const {
		return const_iterator{ data( ) };
	}

	const_iterator cbegin( ) const {
		return const_iterator{ data( ) };
	}

	iterator end( ) {
		return iterator{ data( ) + size( ) };
	}

	const_iterator end( ) const {
		return const_iterator{ data( ) + size( ) };
	}

	const_iterator cend( ) const {
		return const_iterator{ data( ) + size( ) };
	}

	constexpr const_reference operator[]( size_type idx ) const {
		return data( )[idx];
	}

	constexpr reference operator[]( size_type idx ) {
		return data( )[idx];
	}

	constexpr iterator insert( const_iterator where, const_reference value ) {
		// no exception guarantee
		auto const idx = static_cast<size_type>( where - data( ) );
		if( m_size >= capacity( ) ) {
			resize( m_size + 1 );
		}
		pointer end_ptr = data( ) + size( );
		pointer where_ptr = data( ) + idx;
		while( end_ptr > where_ptr ) {
			construct_at( end_ptr, std::move( *( end_ptr - 1 ) ) );
			--end_ptr;
		}
		return { construct_at( where_ptr, value ) };
	}

	template<typename IteratorF, typename IteratorL,
	         std::enable_if_t<( not has_op_minus<IteratorF, IteratorL>::value and
	                            is_iterator<IteratorF>::value ),
	                          std::nullptr_t> = nullptr>
	constexpr iterator insert( const_iterator where, IteratorF first,
	                           IteratorL last ) {
		auto idx = static_cast<size_type>( where - data( ) );
		while( first != last ) {
			insert( data( )[idx++], *first++ );
		}
	}

	template<typename IteratorF, typename IteratorL,
	         std::enable_if_t<( has_op_minus<IteratorF, IteratorL>::value and
	                            is_iterator<IteratorF>::value ),
	                          std::nullptr_t> = nullptr>
	constexpr iterator insert( const_iterator where, IteratorF first,
	                           IteratorL last ) {
		difference_type const where_idx = where - data( );
		difference_type const insert_size = last - first;
		auto const needed_size = static_cast<size_type>( insert_size ) + size( );
		if( needed_size > capacity( ) ) {
			resize_for_overwrite( needed_size );
		}
		pointer pos = data( ) + where_idx;
		// deal with zero size cases
		pointer data_src = pos + insert_size;
		pointer data_dst = data( ) + needed_size - 1;
		while( data_src >= pos ) {
			construct_at( data_dst, std::move( *data_src ) );
			*data_src = -1;
			--data_src;
			--data_dst;
		}
		// m_size + auto result = pos;
		while( first != last ) {
			*pos++ = *first++;
		}
		m_size = needed_size;
		return data( ) + where_idx;
	}

	template<typename U, std::size_t N>
	iterator insert( const_iterator where, U const ( &values )[N] ) {
		return insert( where, values, values + N );
	}
};

template<typename T, std::size_t N>
Vector( T const ( & )[N] ) -> Vector<T>;
