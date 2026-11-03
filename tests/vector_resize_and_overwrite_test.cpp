// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/vector_resize_and_overwrite.h"

#include "daw/daw_ensure.h"

#include <cstddef>
#include <memory>
#include <new>
#include <vector>

namespace {
	struct no_default {
		int value;

		no_default( ) = delete;
		explicit no_default( int v )
		  : value( v ) {}
	};

	template<typename T>
	struct stateful_allocator {
		using value_type = T;

		int state = 0;

		stateful_allocator( ) = default;
		explicit stateful_allocator( int value )
		  : state( value ) {}

		template<typename U>
		stateful_allocator( stateful_allocator<U> const &other )
		  : state( other.state ) {}

		[[nodiscard]] T *allocate( std::size_t n ) {
			return std::allocator<T>{ }.allocate( n );
		}

		void deallocate( T *ptr, std::size_t n ) {
			std::allocator<T>{ }.deallocate( ptr, n );
		}

		template<typename U>
		friend struct stateful_allocator;
	};

	template<typename T, typename U>
	bool operator==( stateful_allocator<T> const &lhs,
	                 stateful_allocator<U> const &rhs ) {
		return lhs.state == rhs.state;
	}

	template<typename T, typename U>
	bool operator!=( stateful_allocator<T> const &lhs,
	                 stateful_allocator<U> const &rhs ) {
		return not( lhs == rhs );
	}
} // namespace

int main( ) {
	{
		auto values = std::vector<int>{ 1, 2 };
		auto const result = daw::resize_and_overwrite(
		  values, 5, []( int *ptr, std::size_t size ) -> std::size_t {
			  for( std::size_t n = 0; n < 2; ++n ) {
				  ptr[n] = static_cast<int>( n * 2 );
			  }
			  for( std::size_t n = 2; n < size; ++n ) {
				  ::new( static_cast<void *>( ptr + n ) ) int(
				    static_cast<int>( n * 2 ) );
			  }
			  return size;
		  } );
		daw_ensure( result == 5 );
		daw_ensure( ( values == std::vector<int>{ 0, 2, 4, 6, 8 } ) );
	}
	{
		auto values = std::vector<int>{ 1, 2, 3, 4 };
		auto const result = daw::resize_and_overwrite(
		  values,
		  3,
		  []( int *ptr, std::size_t, std::allocator<int> &alloc ) -> std::size_t {
			  daw_ensure( alloc == std::allocator<int>{ } );
			  ptr[0] = 9;
			  ptr[1] = 8;
			  return 2;
		  } );
		daw_ensure( result == 2 );
		daw_ensure( ( values == std::vector<int>{ 9, 8 } ) );
	}
	{
		auto values = std::vector<no_default>{ };
		auto const result = daw::resize_and_overwrite(
		  values,
		  3,
		  []( no_default *ptr,
		      std::size_t size,
		      std::allocator<no_default> &alloc ) -> std::size_t {
			  for( std::size_t n = 0; n < size; ++n ) {
				  std::allocator_traits<std::allocator<no_default>>::construct(
				    alloc, ptr + n, static_cast<int>( n + 1 ) );
			  }
			  return size;
		  } );
		daw_ensure( result == 3 );
		daw_ensure( values[0].value == 1 );
		daw_ensure( values[1].value == 2 );
		daw_ensure( values[2].value == 3 );
	}
	{
		using allocator = stateful_allocator<int>;
		auto values = std::vector<int, allocator>( allocator{ 42 } );
		auto const result = daw::resize_and_overwrite(
		  values,
		  2,
		  []( int *ptr, std::size_t size, allocator &alloc ) -> std::size_t {
			  daw_ensure( alloc.state == 42 );
			  for( std::size_t n = 0; n < size; ++n ) {
				  std::allocator_traits<allocator>::construct(
				    alloc, ptr + n, static_cast<int>( n + 7 ) );
			  }
			  return size;
		  } );
		daw_ensure( result == 2 );
		daw_ensure( values[0] == 7 );
		daw_ensure( values[1] == 8 );
	}
}
