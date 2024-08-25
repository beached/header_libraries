// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_ensure.h"
#include "filter_view.h"
#include "range.h"

#include <cstddef>
#include <random>

namespace daw::pipelines::pimpl {
	template<typename RandomEngine,
	         typename Distribution = std::uniform_int_distribution<std::size_t>>
	struct Sample_t {
		std::size_t m_number_to_keep;
		std::size_t m_sample_size;
		RandomEngine m_engine = RandomEngine{ };

		template<Range R>
		[[nodiscard]] constexpr auto operator( )( R &&r ) const {
			using result_type = typename Distribution::result_type;
			static_assert( std::is_integral_v<result_type> );
			daw_ensure( m_number_to_keep < m_sample_size );
			daw_ensure( std::cmp_less_equal(
			  m_sample_size, std::numeric_limits<result_type>::max( ) ) );
			auto distribution = Distribution{
			  result_type{ 1 }, static_cast<result_type>( m_sample_size ) };

			return filter_view{ std::begin( r ),
			                    std::end( r ),
			                    [engine = m_engine,
			                     number_to_keep = m_number_to_keep,
			                     distribution]( auto const & ) mutable {
				                    auto n = distribution( engine );
				                    auto result = n <= number_to_keep;
				                    return result;
			                    } };
		}
	};
} // namespace daw::pipelines::pimpl

namespace daw::pipelines {
	template<typename Distribution = std::uniform_int_distribution<std::size_t>,
	         typename Engine = std::mt19937>
	[[nodiscard]] constexpr auto
	Sample( std::size_t numberToKeep,
	        std::size_t sampleSize,
	        Engine engine = Engine{ std::random_device{ }( ) } ) {
		return pimpl::Sample_t<Engine, Distribution>{
		  numberToKeep, sampleSize, std::move( engine ) };
	}
} // namespace daw::pipelines
