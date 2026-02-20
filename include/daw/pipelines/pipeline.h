// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_attributes.h"
#include "daw/daw_concepts.h"
#include "daw/daw_constant.h"
#include "daw/daw_contiguous_view.h"
#include "daw/daw_move.h"
#include "daw/daw_remove_cvref.h"
#include "daw/daw_string_view.h"
#include "daw/daw_tuple_forward.h"
#include "pipeline_traits.h"

#include <cstddef>
#include <functional>
#include <utility>

namespace daw::pipelines::pimpl {
	/// This allows specialized overloads like To( ) to work without putting the (
	/// ) e.g. To<std::vector>( ) can be typed as To<std::vector>
	template<typename Fn>
	[[nodiscard]] constexpr auto invoke_if_needed( Fn &&fn ) {
		if constexpr( requires { fn( ); } ) {
			return fn( );
		} else {
			return DAW_FWD( fn );
		}
	}

	template<std::size_t Idx, typename R, typename... Ranges, typename... Ts>
	[[nodiscard]] DAW_ATTRIB_FLATINLINE constexpr auto
	pipeline( std::tuple<Ts...> const &functions, R &&r, Ranges &&...ranges ) {
		using std::get;
		if constexpr( Idx > 0 ) {
			auto &&param =
			  pipeline<Idx - 1>( functions, DAW_FWD( r ), DAW_FWD( ranges )... );
			return get<Idx>( functions )( DAW_FWD( param ) );
		} else {
			return get<0>( functions )( DAW_FWD( r ), DAW_FWD( ranges )... );
		}
	}

	// We know the string s is owned above the call to fix_range
	template<std::size_t N>
	[[nodiscard]] constexpr auto fix_range( char const ( &s )[N] ) {
		return daw::string_view( s, N - 1 );
	}

	template<typename T, std::size_t N>
	[[nodiscard]] constexpr auto fix_range( T ( &s )[N] ) {
		return daw::contiguous_view( s, N );
	}

	[[nodiscard]] constexpr decltype( auto ) fix_range( Range auto &&r ) {
		return DAW_FWD( r );
	}

	template<typename F>
	struct lifted_t {
		F f;

		template<typename... Args>
		[[nodiscard]] constexpr auto operator( )( Args &&...args ) const {
			static_assert( std::invocable<F, Args...> );
			return f( DAW_FWD( args )... );
		}
	};
	template<typename F>
	lifted_t( F ) -> lifted_t<F>;

	template<typename R, typename... Args>
	[[nodiscard]] constexpr auto maybe_lift( R ( *f )( Args... ) ) {
		return lifted_t{ f };
	}

	template<typename R, typename... Args>
	[[nodiscard]] constexpr auto maybe_lift( R ( *&f )( Args... ) ) {
		return lifted_t{ f };
	}

	[[nodiscard]] constexpr auto maybe_lift( auto f ) {
		return std::move( f );
	}

	template<bool DoForward>
	[[nodiscard]] constexpr auto make_tpfns( auto &&...fs ) {
		if constexpr( DoForward ) {
			return daw::forward_nonrvalue_as_tuple(
			  invoke_if_needed( maybe_lift( DAW_FWD( fs ) ) )... );
		} else {
			return std::tuple{ invoke_if_needed( maybe_lift( DAW_FWD( fs ) ) )... };
		}
	};
} // namespace daw::pipelines::pimpl

namespace daw::pipelines {
	template<Range R, typename... Fns>
	DAW_ATTRIB_FLATTEN constexpr auto pipeline( R &&r, Fns &&...fns ) {
		return pimpl::pipeline<sizeof...( Fns ) - 1>(
		  pimpl::make_tpfns<true>( DAW_FWD( fns )... ),
		  pimpl::fix_range( DAW_FWD( r ) ) );
	}

	template<typename Fn, typename... Fns>
	requires( not Range<Fn> ) //
	  DAW_ATTRIB_FLATTEN constexpr auto pipeline( Fn &&fn, Fns &&...fns ) {
		// Store all passed functions as they must outlive the call to pipeline
		return
		  [tpfns = pimpl::make_tpfns<false>( DAW_FWD( fn ), DAW_FWD( fns )... )](
		    Range auto &&r, Range auto &&...rs ) {
			  // We are going to forward refs to the current tuple to ensure it is
			  // only stored here if there are large callables passed
			  using tp_size = daw::constant<std::tuple_size_v<DAW_TYPEOF( tpfns )>>;
			  return [&]<std::size_t... Is>( std::index_sequence<Is...> ) {
				  return pimpl::pipeline<tp_size::value - 1>(
				    tpfns,
				    // std::forward_as_tuple( std::get<Is>( tpfns )... ),
				    pimpl::fix_range( DAW_FWD( r ) ),
				    pimpl::fix_range( DAW_FWD( rs ) )... );
			  }( std::make_index_sequence<std::tuple_size_v<decltype( tpfns )>>{ } );
		  };
	}
} // namespace daw::pipelines
