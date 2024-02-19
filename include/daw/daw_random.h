// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "daw_arith_traits.h"
#include "daw_exception.h"
#include "daw_fnv1a_hash.h"
#include "daw_swap.h"
#include "daw_traits.h"
#include "daw_utility.h"
#include "traits/daw_traits_conditional.h"

#include <iterator>
#include <limits>
#include <memory>
#include <random>
#include <type_traits>
#include <vector>

namespace daw::impl {
	static inline auto &global_rng( ) {
		static thread_local auto e = std::mt19937_64( std::random_device{ }( ) );
		return e;
	}
} // namespace daw::impl

namespace daw {
	template<typename IntType>
	inline IntType randint( IntType a, IntType b ) {
		static_assert( std::is_integral_v<IntType>,
		               "IntType must be a valid integral type" );
		daw::exception::daw_throw_on_false( a <= b, "a <= b must be true" );
		using distribution_type = std::uniform_int_distribution<IntType>;
		using param_type = typename distribution_type::param_type;

		thread_local auto d = distribution_type( );
		return d( impl::global_rng( ), param_type( a, b ) );
	}

	template<typename IntType>
	inline IntType randint( ) {
		return randint<IntType>( ( std::numeric_limits<IntType>::min )( ),
		                         ( std::numeric_limits<IntType>::max )( ) );
	}

	inline void reseed( ) {
		impl::global_rng( ) = std::mt19937_64( );
	}

	inline void reseed( std::default_random_engine::result_type value ) {
		impl::global_rng( ).seed( value );
	}

	template<typename RandomIterator>
	inline void shuffle( RandomIterator first, RandomIterator last ) {
		using diff_t =
		  typename std::iterator_traits<RandomIterator>::difference_type;

		diff_t n = last - first;
		for( diff_t i = n - 1; i > 0; --i ) {
			daw::cswap( first[i], first[randint<diff_t>( 0, i )] );
		}
	}

	template<typename IntType, typename ItValueType = IntType,
	         typename ForwardIterator>
	inline void random_fill( ForwardIterator first, ForwardIterator const last,
	                         IntType a, IntType b ) {
		static_assert( std::is_integral_v<IntType>,
		               "IntType must be a valid integral type" );
		daw::exception::daw_throw_on_false( a <= b, "a <= b must be true" );
		while( first != last ) {
			*first = static_cast<ItValueType>( randint<IntType>( a, b ) );
			++first;
		}
	}

	/***
	 * Generate Random Numbers
	 * @tparam IntType type of integral to make
	 * @tparam Result Container type to hold values
	 * @param count How many to make
	 * @param a lower bound of random numbers
	 * @param b upper bound of random numbers
	 * @return container with values
	 */
	template<typename IntType, typename Result = std::vector<IntType>>
	inline Result
	make_random_data( size_t count,
	                  IntType a = ( std::numeric_limits<IntType>::min )( ),
	                  IntType b = ( std::numeric_limits<IntType>::max )( ) ) {

		static_assert( std::is_integral_v<IntType>,
		               "IntType must be a valid integral type" );

		daw::exception::precondition_check( a <= b, "a <= b must be true" );

		Result result{ };
		result.resize( count );
		random_fill<IntType, typename Result::value_type>( result.begin( ),
		                                                   result.end( ), a, b );
		return result;
	}

	namespace cxrand_impl {
#ifdef USE_CXSEED
		constexpr size_t generate_seed( char const *first,
		                                size_t seed = 0 ) noexcept {
			// Use djb2 to hash the string to generate seed
			size_t result = seed == 0 ? 5381U : seed;
			while( first and *first != 0 ) {
				result = ( ( result << 5U ) + result ) + static_cast<size_t>( *first );
				++first;
			}
			return result;
		}

		constexpr size_t generate_seed( ) noexcept {
			size_t result = generate_seed( __FILE__ );
			result = generate_seed( __DATE__, result );
			result = generate_seed( __TIME__, result );
			result = ( ( result << 5U ) + result ) + __LINE__;
			return result;
		}
#endif
		template<size_t N, std::enable_if_t<( N == 4 ), std::nullptr_t> = nullptr>
		constexpr size_t rand_lcg( size_t x_prev ) noexcept {
			return x_prev * 1664525UL + 1013904223UL;
		}

		template<size_t N, std::enable_if_t<( N == 8 ), std::nullptr_t> = nullptr>
		constexpr size_t rand_lcg( size_t x_prev ) noexcept {
			return x_prev * 2862933555777941757ULL + 3037000493ULL;
		}
	} // namespace cxrand_impl

	struct static_random {
#ifdef USE_CXSEED
		static constexpr auto m_seed = cxrand_impl::generate_seed( );
#endif

	private:
#ifdef USE_CXSEED
		size_t m_state = cxrand_impl::rand_lcg<sizeof( size_t )>( m_seed );
#else
		size_t m_state;
#endif

	public:
#ifdef USE_CXSEED
		constexpr static_random( ) noexcept = default;
#endif
		constexpr static_random( size_t seed ) noexcept
		  : m_state( cxrand_impl::rand_lcg<sizeof( size_t )>( seed ) ) {}

		constexpr size_t operator( )( ) noexcept {
			m_state = cxrand_impl::rand_lcg<sizeof( size_t )>( m_state );
			return daw::fnv1a_hash( m_state );
		}
	};

	template<typename Integer, typename Engine = std::default_random_engine>
	struct RandomInteger {
		static_assert( std::is_integral_v<Integer>,
		               "Only integer types supported" );
		using result_type = Integer;

	private:
		using engine_t = Engine;
		engine_t m_engine =
		  engine_t( static_cast<result_type>( std::random_device{ }( ) ) );
		using distribution_type = std::uniform_int_distribution<Integer>;
		using param_type = typename distribution_type::param_type;
		distribution_type m_dist = distribution_type( );

	public:
		explicit RandomInteger( ) = default;
		explicit constexpr RandomInteger( result_type seed )
		  : m_engine( seed ) {}

		constexpr result_type
		operator( )( Integer MaxInclusive = std::numeric_limits<Integer>::max( ),
		             Integer MinInclusive = std::numeric_limits<Integer>::min( ) ) {
			daw::exception::daw_throw_on_false(
			  MinInclusive < MaxInclusive,
			  "MinInclusive < MaxInclusive must be true" );

			return m_dist( m_engine, param_type( MinInclusive, MaxInclusive ) );
		}
	};

	namespace RandomInteger_impl {
		template<typename Float>
		using uint_type =
		  conditional_t<std::is_same_v<Float, float>, std::uint32_t, std::uint64_t>;
	}

	template<typename Float, typename Engine = std::default_random_engine>
	struct RandomFloat {
	public:
		using result_type = Float;

	private:
		using uint_type = typename Engine::result_type;
		using engine_t = Engine;
		engine_t m_engine =
		  engine_t( static_cast<uint_type>( std::random_device{ }( ) ) );
		using distribution_type = std::uniform_real_distribution<Float>;
		using param_type = typename distribution_type::param_type;
		distribution_type m_dist = distribution_type( );

	public:
		explicit RandomFloat( ) = default;
		explicit constexpr RandomFloat( uint_type seed )
		  : m_engine( seed ) {}

		constexpr result_type operator( )( Float MaxInclusive = Float{ 1.0 },
		                                   Float MinInclusive = Float{ 0.0 } ) {
			daw::exception::daw_throw_on_false(
			  MinInclusive < MaxInclusive,
			  "MinInclusive < MaxInclusive must be true" );

			return m_dist( m_engine, param_type( MinInclusive, MaxInclusive ) );
		}
	};
} // namespace daw
