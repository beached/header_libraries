// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries

#pragma once

#include "daw_attributes.h"
#include "daw_move.h"
#include "daw_pack_element.h"
#include "traits/daw_traits_nth_element.h"
#include "traits/daw_traits_pack_list.h"

#include <cstddef>
#include <daw/stdinc/integer_sequence.h>
#include <daw/stdinc/move_fwd_exch.h>
#include <daw/stdinc/remove_cvref.h>
#include <type_traits>

namespace daw {
	namespace fwd_pack_impl {
		template<std::size_t Idx, typename T>
		struct pack_leaf {
			std::remove_reference_t<T> *value;
		};

		template<typename Pack, typename IndexSeq>
		struct fwd_pack_base;

		template<typename Pack, std::size_t... Is>
		struct fwd_pack_base<Pack, std::index_sequence<Is...>>
		  : pack_leaf<Is, pack_element_t<Is, Pack>>... {};
	} // namespace fwd_pack_impl

	template<typename... Ts>
	struct fwd_pack
	  : fwd_pack_impl::fwd_pack_base<pack_list<Ts...>,
	                                 std::index_sequence_for<Ts...>> {

		using base_type =
		  fwd_pack_impl::fwd_pack_base<pack_list<Ts...>,
		                               std::index_sequence_for<Ts...>>;

		fwd_pack( ) = default;

		DAW_ATTRIB_INLINE constexpr fwd_pack( Ts... args )
		  : base_type{ { &args }... } {}

		template<std::size_t Idx>
		DAW_ATTRIB_FLATINLINE constexpr decltype( auto ) get( ) const & {
			static_assert( sizeof...( Ts ) > 0, "Error to call get on empty pack" );
			using result_t = daw::traits::nth_type<Idx, Ts...>;
			using leaf_t = fwd_pack_impl::pack_leaf<Idx, result_t>;
			return static_cast<result_t>(
			  *static_cast<leaf_t const *>( this )->value );
		}

		template<std::size_t Idx>
		DAW_ATTRIB_FLATINLINE constexpr decltype( auto ) get( ) & {
			static_assert( sizeof...( Ts ) > 0, "Error to call get on empty pack" );
			using result_t = daw::traits::nth_type<Idx, Ts...>;
			using leaf_t = fwd_pack_impl::pack_leaf<Idx, result_t>;
			return static_cast<result_t>( *static_cast<leaf_t *>( this )->value );
		}

		template<std::size_t Idx>
		DAW_ATTRIB_FLATINLINE constexpr decltype( auto ) get( ) && {
			static_assert( sizeof...( Ts ) > 0, "Error to call get on empty pack" );
			using result_t = daw::traits::nth_type<Idx, Ts...>;
			using leaf_t = fwd_pack_impl::pack_leaf<Idx, result_t>;
			return static_cast<result_t>( *static_cast<leaf_t *>( this )->value );
		}

		template<std::size_t Idx>
		DAW_ATTRIB_FLATINLINE constexpr decltype( auto ) get( ) const && {
			static_assert( sizeof...( Ts ) > 0, "Error to call get on empty pack" );
			using result_t = daw::traits::nth_type<Idx, Ts...>;
			using leaf_t = fwd_pack_impl::pack_leaf<Idx, result_t>;
			return static_cast<result_t>(
			  *static_cast<leaf_t const *>( this )->value );
		}
	};

	template<>
	struct fwd_pack<> {};

	template<typename... Ts>
	fwd_pack( Ts &&... ) -> fwd_pack<Ts &&...>;

	template<std::size_t Idx, typename... Ts>
	DAW_ATTRIB_FLATINLINE constexpr decltype( auto ) get( fwd_pack<Ts...> &&p ) {
		return std::move( p ).template get<Idx>( );
	}

	template<std::size_t Idx, typename... Ts>
	DAW_ATTRIB_FLATINLINE constexpr decltype( auto ) get( fwd_pack<Ts...> &p ) {
		return p.template get<Idx>( );
	}

	template<std::size_t Idx, typename... Ts>
	DAW_ATTRIB_FLATINLINE constexpr decltype( auto )
	get( fwd_pack<Ts...> const &p ) {
		return p.template get<Idx>( );
	}

	namespace fwd_pack_impl {
		template<typename Func, typename... Ts, std::size_t... Is>
		DAW_ATTRIB_FLATINLINE constexpr decltype( auto ) apply_impl(
		  Func &&func,
		  fwd_pack<Ts...> &&p,
		  std::index_sequence<
		    Is...> ) noexcept( std::is_nothrow_invocable_v<Func, Ts...> ) {
			return DAW_FWD( func )( get<Is>( std::move( p ) )... );
		}
	} // namespace fwd_pack_impl

	template<typename Func, typename... Ts>
	DAW_ATTRIB_FLATTEN constexpr decltype( auto ) apply(
	  Func &&func,
	  fwd_pack<Ts...> &&p ) noexcept( std::is_nothrow_invocable_v<Func, Ts...> ) {
		return fwd_pack_impl::apply_impl(
		  DAW_FWD( func ), std::move( p ), std::index_sequence_for<Ts...>{ } );
	}

	template<typename>
	struct tuple_size;

	template<typename... Ts>
	struct tuple_size<daw::fwd_pack<Ts...>>
	  : std::integral_constant<std::size_t, sizeof...( Ts )> {};

	template<std::size_t, typename>
	struct tuple_element;

	template<std::size_t Idx, typename... Ts>
	struct tuple_element<Idx, fwd_pack<Ts...>> {
		using type = daw::traits::nth_type<Idx, Ts...>;
	};
} // namespace daw
