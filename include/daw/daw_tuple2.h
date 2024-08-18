// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#pragma once

#include "cpp_17.h"
#include "daw_consteval.h"
#include "daw_move.h"
#include "daw_traits.h"
#include "daw_utility.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

namespace daw {
	namespace tuple2_impl {
		DAW_ATTRIB_INLINE constexpr std::size_t
		encode_location( std::size_t outter, std::size_t inner ) {
			constexpr std::size_t half_sz = ( sizeof( std::size_t ) * 8U ) / 2U;
			return ( outter << half_sz ) | inner;
		}

		template<std::size_t... Sizes, std::size_t... Is>
		DAW_ATTRIB_INLINE DAW_CONSTEVAL auto
		cartesian_product_impl2( std::index_sequence<Sizes...>,
		                         std::index_sequence<Is...> ) {
			static_assert( sizeof...( Sizes ) != 0 );
			constexpr std::array x = [] {
				constexpr std::size_t orig[]{ Sizes... };
				std::array<std::size_t, sizeof...( Is )> result{ };
				std::size_t idx = 0;
				for( std::size_t n = 0; n < sizeof...( Sizes ); ++n ) {
					for( std::size_t m = 0; m < orig[n]; ++m ) {
						result[idx] = encode_location( n, m );
						++idx;
					}
				}
				return result;
			}( );
			return std::index_sequence<x[Is]...>{ };
		}

		template<std::size_t... Sizes>
		DAW_ATTRIB_INLINE DAW_CONSTEVAL auto
		cartesian_product_impl( std::index_sequence<Sizes...> sizes ) {
			return cartesian_product_impl2(
			  sizes, std::make_index_sequence<( Sizes + ... )>{ } );
		}

		template<typename T>
		using cartesian_product =
		  decltype( cartesian_product_impl( std::declval<T>( ) ) );

		template<std::size_t Index, typename T, bool /*is_class*/>
		struct leaf {
			T value;
		};

		template<std::size_t Index, typename T>
		struct leaf<Index, T, /*is_class*/ true> : T {};

		template<typename T, typename...>
		struct tuple2_base;

		template<std::size_t... Index, typename... Ts>
		struct tuple2_base<std::index_sequence<Index...>, Ts...>
		  : leaf<Index,
		         Ts,
		         ( std::is_class_v<Ts> and not std::is_final_v<Ts> )>... {};
	} // namespace tuple2_impl

	template<typename... Ts>
	struct tuple2
	  : tuple2_impl::tuple2_base<std::index_sequence_for<Ts...>, Ts...> {
		static constexpr std::size_t size( ) noexcept {
			return sizeof...( Ts );
		}
	};

	template<>
	struct tuple2<> {
		DAW_ATTRIB_INLINE static DAW_CONSTEVAL std::size_t size( ) noexcept {
			return 0;
		}
	};

	template<typename... Ts>
	tuple2( Ts... ) -> tuple2<Ts...>;

	template<typename>
	inline constexpr bool is_tuple2_v = false;

	template<typename... Ts>
	inline constexpr bool is_tuple2_v<tuple2<Ts...>> = true;

	template<typename Tp>
	struct is_tuple2 : std::bool_constant<is_tuple2_v<Tp>> {};

	template<std::size_t Index, typename Ts>
	constexpr Ts const &get( tuple2_impl::leaf<Index, Ts, true> const &tp ) {
		return static_cast<Ts const &>( tp );
	}

	template<std::size_t Index, typename Ts>
	constexpr Ts const &get( tuple2_impl::leaf<Index, Ts, false> const &tp ) {
		return tp.value;
	}

	template<std::size_t Index, typename Ts>
	constexpr Ts &get( tuple2_impl::leaf<Index, Ts, true> &tp ) {
		return static_cast<Ts const &>( tp );
	}

	template<std::size_t Index, typename Ts>
	constexpr Ts &get( tuple2_impl::leaf<Index, Ts, false> &tp ) {
		return tp.value;
	}

	template<std::size_t Index, typename Ts>
	constexpr Ts const &&get( tuple2_impl::leaf<Index, Ts, true> const &&tp ) {
		return static_cast<Ts const &>( tp );
	}

	template<std::size_t Index, typename Ts>
	constexpr Ts const &&get( tuple2_impl::leaf<Index, Ts, false> const &&tp ) {
		return tp.value;
	}

	template<std::size_t Index, typename Ts>
	constexpr Ts &&get( tuple2_impl::leaf<Index, Ts, true> &&tp ) {
		return static_cast<Ts const &>( tp );
	}

	template<std::size_t Index, typename Ts>
	constexpr Ts &&get( tuple2_impl::leaf<Index, Ts, false> &&tp ) {
		return tp.value;
	}

	template<std::size_t, typename>
	struct tuple2_element;

	template<std::size_t Idx, typename... Ts>
	struct tuple2_element<Idx, tuple2<Ts...>> {
	public:
		using type = pack_element_t<Idx, daw::pack_list<Ts...>>;
	};

	template<std::size_t Idx, typename Tuple>
	using tuple2_element_t = typename tuple2_element<Idx, Tuple>::type;

	template<typename T>
	struct tuple2_size;

	template<typename... Ts>
	struct tuple2_size<tuple2<Ts...>>
	  : std::integral_constant<std::size_t, sizeof...( Ts )> {};

	template<typename T>
	inline static constexpr std::size_t tuple2_size_v = tuple2_size<T>::value;

	template<typename... Ts>
	constexpr tuple2<Ts...> forward_as_tuple2( Ts &&...args ) {
		return tuple2<Ts...>{ DAW_FWD( args )... };
	}

	// ******************************************
	// Tuple Cat
	// ******************************************
	namespace tuple2_impl {
		template<std::size_t location, typename TupleTuple>
		constexpr decltype( auto ) decode_get( TupleTuple &&tp ) {
			constexpr std::size_t half_sz = ( sizeof( std::size_t ) * 8U ) / 2U;
			constexpr std::size_t outter = location >> half_sz;
			constexpr std::size_t inner = ( location << half_sz ) >> half_sz;
			return get<inner>( get<outter>( DAW_FWD( tp ) ) );
		}

		template<std::size_t location, typename TupleTuple>
		DAW_ATTRIB_INLINE DAW_CONSTEVAL auto decode_get_type_impl( ) {
			constexpr std::size_t half_sz = ( sizeof( std::size_t ) * 8U ) / 2U;
			constexpr std::size_t outter = location >> half_sz;
			constexpr std::size_t inner = ( location << half_sz ) >> half_sz;
			return daw::traits::identity<tuple2_element_t<
			  inner,
			  daw::remove_cvref_t<tuple2_element_t<outter, TupleTuple>>>>{ };
		}
		template<std::size_t location, typename TupleTuple>
		using decode_get_type =
		  typename decltype( decode_get_type_impl<location, TupleTuple>( ) )::type;

		template<std::size_t... Locations, typename TupleTuple>
		constexpr auto tuple2_cat_impl( std::index_sequence<Locations...>,
		                                TupleTuple &&tps ) {
			using tp_t =
			  tuple2<decode_get_type<Locations, daw::remove_cvref_t<TupleTuple>>...>;

			return tp_t{ decode_get<Locations>( DAW_FWD( tps ) )... };
		}
	} // namespace tuple2_impl

	constexpr tuple2<> tuple2_cat( ) {
		return tuple2<>{ };
	}

	template<typename Tuple>
	constexpr tuple2<> tuple2_cat( Tuple &&tp ) {
		return { tp };
	}

	template<typename T0, typename T1, typename... Tps>
	constexpr auto tuple2_cat( T0 &&t0, T1 &&t1, Tps &&...tps ) {
		return tuple2_impl::tuple2_cat_impl(
		  tuple2_impl::cartesian_product<
		    std::index_sequence<tuple2_size_v<daw::remove_cvref_t<T0>>,
		                        tuple2_size_v<daw::remove_cvref_t<T1>>,
		                        tuple2_size_v<daw::remove_cvref_t<Tps>>...>>{ },
		  forward_as_tuple2( DAW_FWD( t0 ), DAW_FWD( t1 ), DAW_FWD( tps )... ) );
	}

	namespace tuple2_impl {
		template<typename Tuple, typename Func, std::size_t... Is>
		inline constexpr decltype( auto )
		apply_impl( Tuple &&tp, Func &&fn, std::index_sequence<Is...> ) {
			return DAW_FWD( fn )( get<Is>( DAW_FWD( tp ) )... );
		}
	} // namespace tuple2_impl

	template<typename Func, typename... Ts>
	inline constexpr decltype( auto ) apply( tuple2<Ts...> const &tp,
	                                         Func &&fn ) {
		return tuple2_impl::apply_impl(
		  tp,
		  DAW_FWD( fn ),
		  std::make_index_sequence<tuple2_size_v<tuple2<Ts...>>>{ } );
	}

	template<typename Func, typename... Ts>
	inline constexpr decltype( auto ) apply( tuple2<Ts...> &tp, Func &&fn ) {
		return tuple2_impl::apply_impl(
		  tp,
		  DAW_FWD( fn ),
		  std::make_index_sequence<tuple2_size_v<tuple2<Ts...>>>{ } );
	}

	template<typename Func, typename... Ts>
	inline constexpr decltype( auto ) apply( tuple2<Ts...> const &&tp,
	                                         Func &&fn ) {
		return tuple2_impl::apply_impl(
		  std::move( tp ),
		  DAW_FWD( fn ),
		  std::make_index_sequence<tuple2_size_v<tuple2<Ts...>>>{ } );
	}

	template<typename Func, typename... Ts>
	inline constexpr decltype( auto ) apply( tuple2<Ts...> &&tp, Func &&fn ) {
		return tuple2_impl::apply_impl(
		  tp,
		  std::move( fn ),
		  std::make_index_sequence<tuple2_size_v<tuple2<Ts...>>>{ } );
	}
} // namespace daw
