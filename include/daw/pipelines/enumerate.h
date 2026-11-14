// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_arith_traits.h"
#include "daw/daw_cpp_feature_check.h"
#include "daw/daw_move.h"
#include "daw/pipelines/iota.h"
#include "daw/pipelines/zip.h"

#include <algorithm>
#include <cstddef>
#include <limits>

namespace daw::pipelines::pimpl {
	template<typename EnumType = std::size_t>
	struct Enumerate_t {

		template<ForwardRange R, ForwardRange... Rs>
		[[nodiscard]] DAW_CPP23_STATIC_CALL_OP constexpr auto
		operator( )( R &&r, Rs &&...rs ) DAW_CPP23_STATIC_CALL_OP_CONST {
			if constexpr( RandomRange<R> and ( RandomRange<Rs> and ... ) ) {
				auto const sz = std::min( { ranges_distance<EnumType>( r ),
				                            ranges_distance<EnumType>( rs )... } );
				return zip_view<iota_view<EnumType>, R, Rs...>(
				  iota_view<EnumType>( EnumType{ }, sz ),
				  DAW_FWD( r ),
				  DAW_FWD( rs )... );
			} else {
				return zip_view<iota_view<EnumType>, R, Rs...>(
				  iota_view<EnumType>( EnumType{ }, max_value<EnumType> ),
				  DAW_FWD( r ),
				  DAW_FWD( rs )... );
			}
		}
	};

	template<typename EnumType = std::size_t>
	struct EnumerateFrom_t {
		EnumType offset = EnumType{ };
		template<ForwardRange R, ForwardRange... Rs>
		[[nodiscard]] constexpr auto operator( )( R &&r, Rs &&...rs ) const {
			return zip_view<iota_view<EnumType>, R, Rs...>(
			  iota_view<EnumType>( offset, max_value<EnumType> ),
			  DAW_FWD( r ),
			  DAW_FWD( rs )... );
		}
	};

	template<typename EnumType = std::size_t>
	struct EnumerateApply_t {
		template<ForwardRange R, ForwardRange... Rs>
		[[nodiscard]] DAW_CPP23_STATIC_CALL_OP constexpr auto
		operator( )( R &&r, Rs &&...rs ) DAW_CPP23_STATIC_CALL_OP_CONST {
			if constexpr( RandomRange<R> and ( RandomRange<Rs> and ... ) ) {
				auto const sz =
				  std::min( { pimpl::ranges_distance<EnumType>( r ),
					            pimpl::ranges_distance<EnumType>( rs )... } );
				return ZipMore( iota_view<EnumType>( EnumType{ }, sz ),
				                DAW_FWD( r ),
				                DAW_FWD( rs )... );
			} else {
				return ZipMore( iota_view<EnumType>( EnumType{ }, max_value<EnumType> ),
				                DAW_FWD( r ),
				                DAW_FWD( rs )... );
			}
		}
	};

	template<typename EnumType = std::size_t>
	struct EnumerateApplyFrom_t {
		EnumType offset = EnumType{ };
		template<ForwardRange R>
		[[nodiscard]] constexpr auto operator( )( R &&r ) const {
			if constexpr( RandomRange<R> ) {
				auto const sz = pimpl::ranges_distance<EnumType>( r );
				return ZipMore( iota_view<EnumType>( offset, offset + sz ),
				                DAW_FWD( r ) );
			} else {
				return ZipMore( iota_view<EnumType>( offset, max_value<EnumType> ),
				                DAW_FWD( r ) );
			}
		}
	};
} // namespace daw::pipelines::pimpl

namespace daw::pipelines {
	inline constexpr auto Enumerate = pimpl::Enumerate_t<>{ };

	template<typename EnumType>
	inline constexpr auto EnumerateWith = pimpl::Enumerate_t<EnumType>{ };

	inline constexpr auto EnumerateApply = pimpl::EnumerateApply_t<>{ };

	template<typename EnumType>
	inline constexpr auto EnumerateApplyWith =
	  pimpl::EnumerateApply_t<EnumType>{ };

	template<typename EnumType = std::size_t>
	constexpr auto EnumerateFrom( EnumType offset ) {
		return pimpl::EnumerateFrom_t<EnumType>{ offset };
	}

	template<typename EnumType = std::size_t>
	constexpr auto EnumerateApplyFrom( EnumType offset ) {
		return pimpl::EnumerateApplyFrom_t<EnumType>{ offset };
	}
} // namespace daw::pipelines
