// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_concepts.h"
#include "daw/daw_restrict.h"

#include <cstdlib>
#include <span>
#include <utility>

namespace daw {
	///
	/// @tparam T Destination type
	/// @param out output span to write elements to
	/// @param values parameters that are convertible to T to write to out
	/// @return out.subspan( sizeof...( values ) )
	template<typename T>
	[[nodiscard]] constexpr std::span<T>
	span_writer( std::span<T> out,
	             daw::explicitly_convertible_to<T> auto &&...values ) {
		if constexpr( sizeof...( values ) == 0 ) {
			return out;
		}
		daw_ensure( sizeof...( values ) <= out.size( ) );
		return [&]<std::size_t... Is>( std::index_sequence<Is...> ) {
			( (void)( out[Is] = static_cast<T>( values ) ), ... );
			return out.subspan( sizeof...( values ) );
		}( std::make_index_sequence<sizeof...( values )>{ } );
	}

	///
	/// @tparam T Destination type
	/// @tparam R Range of elements convertible to T
	/// @param out output span to write elements to
	/// @param r input range values to copy write to out
	/// @return out.subspan( std::ranges::size( r ) )
	/// The two ranges cannot overlap
	template<typename T, typename R>
	requires(
	  std::ranges::contiguous_range<R>
	    and daw::explicitly_convertible_to<std::ranges::range_reference_t<R>, T> )
	  [[nodiscard]] constexpr std::span<T> span_writer( std::span<T> out,
	                                                    R const &r ) {
		auto const sz = std::ranges::size( r );
		daw_ensure( sz <= out.size( ) );
		T *DAW_RESTRICT out_ptr = out.data( );
		auto *DAW_RESTRICT in_ptr = std::ranges::data( r );
		for( std::size_t n = 0; n < sz; ++n ) {
			out_ptr[n] = static_cast<T>( in_ptr[n] );
		}
		return out.subspan( sz );
	}
} // namespace daw
