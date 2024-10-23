// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "traits/daw_traits_remove_cvref.h"

#include <cstddef>
#include <daw/stdinc/integer_sequence.h>
#include <tuple>

namespace daw::func {
	namespace func_traits_impl {
		template<typename A>
		using type_t = typename A::type;

		template<template<std::size_t> typename Arguments, typename>
		struct make_arg_pack;

		template<template<std::size_t> typename Arguments, std::size_t... Is>
		struct make_arg_pack<Arguments, std::index_sequence<Is...>> {
			using type = std::tuple<type_t<Arguments<Is>>...>;
			using plain_type =
			  std::tuple<daw::traits::remove_cvref_t<type_t<Arguments<Is>>>...>;
		};
	} // namespace func_traits_impl

	template<typename, typename = void>
	struct function_traits;

	// function pointer
	template<typename R, typename... Args>
	struct function_traits<R ( * )( Args... )> : function_traits<R( Args... )> {};

	template<typename R, typename... Args>
	struct function_traits<R( Args... )> {
		using result_t = R;
		using plain_result_t = daw::traits::remove_cvref_t<R>;

		using params_t = std::tuple<Args...>;
		using plain_params_t = std::tuple<daw::traits::remove_cvref_t<Args>...>;

		static constexpr std::size_t arity = sizeof...( Args );

		template<std::size_t N>
		struct argument {
			static_assert( N < arity, "error: invalid parameter index." );
			using type = std::tuple_element_t<N, std::tuple<Args...>>;
			using plain_type = daw::traits::remove_cvref_t<type>;
		};
	};

	// member function pointer
	template<typename C, typename R, typename... Args>
	struct function_traits<R ( C::* )( Args... )>
	  : function_traits<R( C &, Args... )> {};

	// const member function pointer
	template<typename C, typename R, typename... Args>
	struct function_traits<R ( C::* )( Args... ) const>
	  : function_traits<R( C &, Args... )> {};

	// member object pointer
	template<typename C, typename R>
	struct function_traits<R( C::* )> : function_traits<R( C & )> {};

	// functor
	template<typename F>
	struct function_traits<
	  F, std::void_t<decltype( &std::remove_reference_t<F>::operator( ) )>> {
	private:
		using call_type =
		  function_traits<decltype( &std::remove_reference_t<F>::operator( ) )>;

	public:
		using result_t = typename call_type::result_t;
		using plain_result_t = daw::traits::remove_cvref_t<result_t>;

		static constexpr std::size_t arity = call_type::arity - 1;

		template<std::size_t N>
		struct argument {
			static_assert( N < arity, "error: invalid parameter index." );
			using type = typename call_type::template argument<N + 1>::type;
			using plain_type = daw::traits::remove_cvref_t<type>;
		};

		using params_t = typename func_traits_impl::make_arg_pack<
		  argument, std::make_index_sequence<arity>>::type;
		using plain_params_t = typename func_traits_impl::make_arg_pack<
		  argument, std::make_index_sequence<arity>>::plain_type;
	};

	template<typename F, typename = void>
	inline constexpr bool has_function_traits_v = false;

	template<typename F>
	inline constexpr bool
	  has_function_traits_v<F, std::void_t<decltype( function_traits<F>{ } )>> =
	    true;
} // namespace daw::func
