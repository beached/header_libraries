// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "cpp_17.h"
#include "daw_move.h"

#include <array>
#include <functional>
#include <string>
#include <type_traits>
#include <utility>

namespace daw {
	namespace detail {
		template<typename T, std::size_t N, std::size_t... I>
		constexpr std::array<std::remove_cv_t<T>, N>
		to_array_impl( T ( &a )[N], std::index_sequence<I...> ) {
			return { { a[I]... } };
		}
	} // namespace detail

	template<typename T, std::size_t N>
	constexpr std::array<std::remove_cv_t<T>, N> to_array( T ( &a )[N] ) {
		return detail::to_array_impl( a, std::make_index_sequence<N>{ } );
	}
	namespace details {
		template<typename>
		struct is_ref_wrapper : std::false_type {};

		template<typename T>
		struct is_ref_wrapper<std::reference_wrapper<T>> : std::true_type {};

		template<typename T>
		using not_ref_wrapper = negation<is_ref_wrapper<std::decay_t<T>>>;

		template<typename D, typename...>
		struct return_type_helper {
			using type = D;
		};
		template<typename... Types>
		struct return_type_helper<void, Types...> : std::common_type<Types...> {
			static_assert( conjunction_v<not_ref_wrapper<Types>...>,
			               "Types cannot contain reference_wrappers when D is void" );
		};

		template<typename D, typename... Types>
		using return_type =
		  std::array<typename return_type_helper<D, Types...>::type,
		             sizeof...( Types )>;
	} // namespace details

	/*
	template<typename D = void, typename... Types>
	constexpr details::return_type<D, Types...> make_array( Types &&... t ) {
	  return {std::forward<Types>( t )...};
	}
	*/
	namespace make_array_impl {
		struct unspecified_type {};
	} // namespace make_array_impl
	template<
	  typename T = make_array_impl::unspecified_type, typename... Args,
	  std::enable_if_t<not std::is_same_v<T, void>, std::nullptr_t> = nullptr>
	constexpr auto make_array( Args &&...args ) {
		constexpr size_t sz = sizeof...( Args );
		if constexpr( std::is_same_v<T, make_array_impl::unspecified_type> ) {

			using result_t = std::common_type_t<daw::remove_cvref_t<Args>...>;
			return std::array<result_t, sz>{ std::forward<Args>( args )... };
		} else {
			return std::array<T, sz>{ std::forward<Args>( args )... };
		}
	}

	namespace make_array_impl {
		template<typename T, size_t N, size_t... Is>
		constexpr auto make_array_impl( T const ( &&arry )[N],
		                                std::index_sequence<Is...> ) {
			return std::array<T, N>{ DAW_MOVE( arry[Is] )... };
		}
	} // namespace make_array_impl

	template<typename T, size_t N>
	constexpr auto make_array( T const ( &&arry )[N] ) {
		return make_array_impl::make_array_impl( DAW_MOVE( arry ),
		                                         std::make_index_sequence<N>{ } );
	}

	template<typename... Ts>
	std::array<std::string, sizeof...( Ts )> make_string_array( Ts &&...t ) {
		return { std::string( std::forward<Ts>( t ) )... };
	}

	namespace daw_array_impl {
		template<typename T, std::size_t Extent, std::size_t... Extents>
		auto md_array_impl( ) {
			if constexpr( sizeof...( Extents ) > 0 ) {
				using next_array_t =
				  std::remove_pointer_t<decltype( md_array_impl<T, Extents...>( ) )>;
				return static_cast<std::array<next_array_t, Extent> *>( nullptr );
			} else {
				return static_cast<std::array<T, Extent> *>( nullptr );
			}
		}
	} // namespace daw_array_impl

	template<typename T, std::size_t... Extents>
	struct md_stdarray {
		static_assert( sizeof...( Extents ) > 0 );
		using type = std::remove_pointer_t<
		  decltype( daw_array_impl::md_array_impl<T, Extents...>( ) )>;
	};

	template<typename T, std::size_t... Extents>
	using md_stdarray_t = typename md_stdarray<T, Extents...>::type;
} // namespace daw
