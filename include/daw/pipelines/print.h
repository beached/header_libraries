// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_attributes.h"
#include "daw/daw_iterator_traits.h"
#include "daw/daw_move.h"
#include "daw/daw_print.h"
#include "daw/daw_typeof.h"
#include "range.h"

#include <type_traits>

namespace daw::pipelines {
	enum class AllowPrintInputRange : bool { Disallow, Allow };
	namespace pimpl {
		template<AllowPrintInputRange InputRangeAllowed =
		           AllowPrintInputRange::Disallow>
		struct Print_t {
			static consteval auto AllowInput( ) {
				return Print_t<AllowPrintInputRange::Allow>{ };
			}

			DAW_ATTRIB_NOINLINE
			[[nodiscard]] inline decltype( auto ) operator( )( auto &&r ) const {
				using R = DAW_TYPEOF( r );
				if constexpr( is_tuple_like_v<R> ) {
					daw::println( "{}", daw::fmt_tuple( r ) );
				} else if constexpr( Range<R> ) {
					static_assert( InputRangeAllowed == AllowPrintInputRange::Allow or
					                 ForwardRange<R>,
					               "Printing a non-forward range is not allowed when "
					               "template param InputRangeAllowed is Disallow.  Use "
					               "Print::AllowInput to enable this" );
					daw::println( "{}", daw::fmt_range( r ) );
				} else {
					daw::println( "{}", r );
				}
				return DAW_FWD( r );
			}
		};
		Print_t( ) -> Print_t<>;
	} // namespace pimpl

	inline constexpr auto Print = pimpl::Print_t{ };
} // namespace daw::pipelines
