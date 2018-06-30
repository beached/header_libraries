// The MIT License (MIT)
//
// Copyright (c) 2017-2018 Darrell Wright
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

#include <random>

#include "daw_exception.h"
#include "daw_traits.h"

namespace daw {
	namespace impl {
		inline auto get_seed( ) -> std::default_random_engine::result_type {
			std::default_random_engine::result_type lo;
#if defined( __i386__ ) || defined( __x86_64__ )
			__asm__ __volatile__( "rdtsc" : "=a"( lo ) );
#endif
			return lo;
		}

		inline auto global_rng( ) -> std::default_random_engine & {
			thread_local std::default_random_engine e{get_seed( )};
			return e;
		}
	} // namespace impl

	template<typename IntType>
	inline IntType randint( IntType a, IntType b ) {
		static_assert( daw::is_integral_v<IntType>,
		               "IntType must be a valid integral type" );
		daw::exception::daw_throw_on_false( a <= b, "a <= b must be true" );
		using distribution_type = std::uniform_int_distribution<IntType>;
		using param_type = typename distribution_type::param_type;

		thread_local distribution_type d;
		return d( impl::global_rng( ), param_type{a, b} );
	}

	template<typename IntType>
	inline IntType randint( ) {
		return randint<IntType>( std::numeric_limits<IntType>::min( ),
		                         std::numeric_limits<IntType>::max( ) );
	}

	inline void reseed( ) {
		impl::global_rng( ).seed( impl::get_seed( ) );
	}

	inline void reseed( std::default_random_engine::result_type value ) {
		impl::global_rng( ).seed( value );
	}

	template<typename RandomIterator>
	void shuffle( RandomIterator first, RandomIterator last ) {
		using diff_t =
		  typename std::iterator_traits<RandomIterator>::difference_type;

		diff_t n = last - first;
		for( diff_t i = n - 1; i > 0; --i ) {
			using std::swap;
			swap( first[i], first[randint<diff_t>( 0, i )] );
		}
	}

	template<typename IntType, typename ForwardIterator>
	void random_fill( ForwardIterator first, ForwardIterator const last,
	                  IntType a, IntType b ) {
		static_assert( daw::is_integral_v<IntType>,
		               "IntType must be a valid integral type" );
		daw::exception::daw_throw_on_false( a <= b, "a <= b must be true" );
		while( first != last ) {
			*first++ = randint( a, b );
		}
	}

	template<typename IntType, typename Result = std::vector<IntType>>
	Result make_random_data( size_t count,
	                         IntType a = std::numeric_limits<IntType>::min( ),
	                         IntType b = std::numeric_limits<IntType>::max( ) ) {

		static_assert( daw::is_integral_v<IntType>,
		               "IntType must be a valid integral type" );
		daw::exception::daw_throw_on_false( a <= b, "a <= b must be true" );

		Result result;
		result.resize( count );
		random_fill( result.begin( ), result.end( ), a, b );
		return result;
	}
} // namespace daw
