// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_cpp_feature_check.h"
#include "daw/daw_cxmath.h"
#include "daw/daw_iterator_traits.h"
#include "daw/daw_move.h"

#include <cstddef>
#include <iterator>

namespace daw::pipelines {
	namespace pimpl {
		struct sum_t {
			[[nodiscard]] DAW_CPP23_STATIC_CALL_OP constexpr auto
			operator( )( Range auto &&r ) DAW_CPP23_STATIC_CALL_OP_CONST {
				using value_type = daw::iter_value_t<decltype( std::begin( r ) )>;
				auto sum = value_type{ };
				for( auto const &v : r ) {
					sum = sum + v;
				}
				return sum;
			}
		};

		struct count_t {
			template<typename R>
			[[nodiscard]] DAW_CPP23_STATIC_CALL_OP constexpr std::size_t
			operator( )( R const &r ) DAW_CPP23_STATIC_CALL_OP_CONST {
				if constexpr( not Range<R> ) {
					return 1;
				} else if constexpr( requires { r.size( ); } ) {
					return r.size( );
				} else {
					return static_cast<std::size_t>(
					  std::distance( std::begin( r ), std::end( r ) ) );
				}
			}
		};

		template<typename Fn>
		struct CountIf_t {
			DAW_NO_UNIQUE_ADDRESS Fn fn;

			[[nodiscard]] constexpr std::size_t
			operator( )( Range auto const &r ) const {
				std::size_t result = 0;
				for( auto const &v : r ) {
					result += static_cast<std::size_t>( fn( v ) );
				}
				return result;
			}
		};
		template<typename Fn>
		CountIf_t( Fn ) -> CountIf_t<Fn>;

		inline constexpr struct IdentityFn_t {
			template<typename T>
			[[nodiscard]] constexpr DAW_CPP23_STATIC_CALL_OP T &&
			operator( )( T &&v ) DAW_CPP23_STATIC_CALL_OP_CONST {
				return DAW_FWD( v );
			}
		} IdentityFn;

		template<typename Projection = IdentityFn_t>
		struct SumKahanBabushkaNeumaier_t {
			DAW_NO_UNIQUE_ADDRESS Projection m_projection = Projection{ };

			template<typename P>
			requires( not Range<P> ) [[nodiscard]] DAW_CPP23_STATIC_CALL_OP
			  constexpr auto
			  operator( )( P &&proj ) DAW_CPP23_STATIC_CALL_OP_CONST {
				return SumKahanBabushkaNeumaier_t<daw::remove_cvrvref_t<P>>{
				  DAW_FWD( proj ) };
			}

			template<Range R>
			[[nodiscard]] constexpr auto operator( )( R &&r ) const {
				using fp_t = range_value_t<R>;
				static_assert( std::is_floating_point_v<fp_t>,
				               "Use of Kahan Babushka Neumaier summation requires the "
				               "range value type to be a floating point type" );
				auto sum = fp_t{ 0.0 };
				auto c = fp_t{ 0.0 };
				for( auto &&cur_v : r ) {
					fp_t input = [&] {
						if constexpr( std::is_same_v<Projection, IdentityFn_t> ) {
							return cur_v;
						} else {
							return std::invoke( cur_v, m_projection );
						}
					}( );
					auto t = sum + input;
					if( daw::cxmath::abs( sum ) >= daw::cxmath::abs( input ) ) {
						c += ( sum - t ) + input;
					} else {
						c += ( input - t ) + sum;
					}
					sum = t;
				}
				return sum + c;
			}
		};
		SumKahanBabushkaNeumaier_t( ) -> SumKahanBabushkaNeumaier_t<>;

		template<typename Projection>
		SumKahanBabushkaNeumaier_t( Projection )
		  -> SumKahanBabushkaNeumaier_t<Projection>;
	} // namespace pimpl

	inline constexpr auto Count = pimpl::count_t{ };
	inline constexpr auto Sum = pimpl::sum_t{ };
	inline constexpr auto SumKahanBabushkaNeumaier =
	  pimpl::SumKahanBabushkaNeumaier_t{ };

	[[nodiscard]] constexpr auto CountIf( auto &&fn ) {
		return pimpl::CountIf_t{ DAW_FWD( fn ) };
	};

} // namespace daw::pipelines
