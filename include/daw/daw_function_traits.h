// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <cstddef>
#include <daw/stdinc/integer_sequence.h>
#include <tuple>

namespace daw::func {
	namespace func_traits_impl {
		template<typename A>
		using type_t = typename A::type;

		template<template<std::size_t> class Arguments, typename>
		struct make_arg_pack;

		template<template<std::size_t> class Arguments, std::size_t... Is>
		struct make_arg_pack<Arguments, std::index_sequence<Is...>> {
			using type = std::tuple<type_t<Arguments<Is>>...>;
		};
	} // namespace func_traits_impl

	template<class>
	struct function_traits;

	// function pointer
	template<class R, class... Args>
	struct function_traits<R ( * )( Args... )> : function_traits<R( Args... )> {};

	template<class R, class... Args>
	struct function_traits<R( Args... )> {
		using result_t = R;
		using params_t = std::tuple<Args...>;

		static constexpr std::size_t arity = sizeof...( Args );

		template<std::size_t N>
		struct argument {
			static_assert( N < arity, "error: invalid parameter index." );
			using type = std::tuple_element_t<N, std::tuple<Args...>>;
		};
	};

	// member function pointer
	template<class C, class R, class... Args>
	struct function_traits<R ( C::* )( Args... )>
	  : function_traits<R( C &, Args... )> {};

	// const member function pointer
	template<class C, class R, class... Args>
	struct function_traits<R ( C::* )( Args... ) const>
	  : function_traits<R( C &, Args... )> {};

	// member object pointer
	template<class C, class R>
	struct function_traits<R( C::* )> : function_traits<R( C & )> {};

	// functor
	template<class F>
	struct function_traits {
	private:
		using call_type = function_traits<decltype( &F::operator( ) )>;

	public:
		using result_t = typename call_type::result_t;

		static constexpr std::size_t arity = call_type::arity - 1;
		template<std::size_t N>
		struct argument {
			static_assert( N < arity, "error: invalid parameter index." );
			using type = typename call_type::template argument<N + 1>::type;
		};

		using params_t = typename func_traits_impl::make_arg_pack<
		  argument, std::make_index_sequence<arity>>::type;
	};
} // namespace daw::func
