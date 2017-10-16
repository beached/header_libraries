// The MIT License (MIT)
//
// Copyright (c) 2016-2017 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <tuple>
#include <type_traits>

namespace daw {
	template<typename...>
	using void_t = void;

	template<typename T>
	constexpr bool is_floating_point_v = std::is_floating_point<T>::value;

	template<typename T>
	constexpr bool is_integral_v = std::is_integral<T>::value;

	template<typename T>
	constexpr bool is_array_v = std::is_array<T>::value;

	template<typename T, typename U>
	constexpr bool is_assignable_v = std::is_assignable<T, U>::value;

	template<typename T>
	constexpr bool is_trivial_v = std::is_trivial<T>::value;

	template<typename T, typename U>
	constexpr bool is_trivially_assignable_v = std::is_trivially_assignable<T, U>::value;

	template<typename T, typename U>
	constexpr bool is_nothrow_assignable_v = std::is_nothrow_assignable<T, U>::value;

	template<typename From, typename To>
	constexpr bool is_convertible_v = std::is_convertible<From, To>::value;

	template<typename T, typename U>
	constexpr bool is_same_v = std::is_same<T, U>::value;

	template<typename T>
	constexpr bool is_default_constructible_v = std::is_default_constructible<T>::value;

	template<typename T>
	constexpr bool is_copy_constructible_v = std::is_copy_constructible<T>::value;

	template<typename T>
	constexpr bool is_copy_assignable_v = std::is_copy_assignable<T>::value;

	template<typename T>
	constexpr bool is_move_constructible_v = std::is_move_constructible<T>::value;

	template<typename T>
	constexpr bool is_move_assignable_v = std::is_move_assignable<T>::value;

	template<typename T>
	constexpr bool is_const_v = std::is_const<T>::value;

	template<typename T>
	constexpr bool is_rvalue_reference_v = std::is_rvalue_reference<T>::value;

	template<class T>
	constexpr size_t tuple_size_v = std::tuple_size<T>::value;

	template<typename T>
	constexpr bool is_function_v = std::is_function<T>::value;

	template<typename Base, typename Derived>
	constexpr bool is_base_of_v = std::is_base_of<Base, Derived>::value;

	template<typename T>
	constexpr bool is_class_v = std::is_class<T>::value;

	template<typename T>
	constexpr bool is_member_pointer_v = std::is_member_pointer<T>::value;

	template<typename B>
	struct negation : std::integral_constant<bool, !bool( B::value )> {};

	template<typename...>
	struct conjunction : std::true_type {};

	template<typename B1>
	struct conjunction<B1> : B1 {};

	template<typename B1, typename... Bn>
	struct conjunction<B1, Bn...> : std::conditional_t<bool( B1::value ), conjunction<Bn...>, B1> {};

	template<typename... T>
	using conjunction_t = typename conjunction<T...>::type;

	template<typename... T>
	constexpr bool const conjunction_v = conjunction<T...>::value;

	template<typename T>
	constexpr bool is_arithmetic_v = std::is_arithmetic<T>::value;
	// base case; actually only used for empty pack
	template<bool... values>
	struct all_true : std::true_type {};

	// if first is true, check the rest
	template<bool... values>
	struct all_true<true, values...> : all_true<values...> {};

	// if first is false, the whole thing is false
	template<bool... values>
	struct all_true<false, values...> : std::false_type {};

	template<bool... values>
	constexpr bool all_true_v = all_true<values...>::value;

	namespace impl {
		template<bool value, bool... values>
		constexpr bool any_true( ) noexcept {
			return value || any_true<values...>( );
		}
	} // namespace impl

	template<bool... values>
	constexpr bool any_true_v = impl::any_true<values...>( );

	namespace impl {
		template<typename Function>
		class not_fn_t {
			Function m_function;

		public:
			constexpr not_fn_t( ) noexcept = default;
			constexpr explicit not_fn_t( Function func ) : m_function{std::move( func )} {}
			~not_fn_t( );
			constexpr not_fn_t( not_fn_t const & ) = default;
			constexpr not_fn_t( not_fn_t && ) noexcept = default;
			constexpr not_fn_t &operator=( not_fn_t const & ) = default;
			constexpr not_fn_t &operator=( not_fn_t && ) noexcept = default;

			template<typename... Args>
			constexpr auto operator( )( Args &&... args ) {
				return !m_function( std::forward<Args>( args )... );
			}
		};

		template<typename Function>
		not_fn_t<Function>::~not_fn_t( ) {}

	} // namespace impl

	template<typename Function>
	constexpr impl::not_fn_t<Function> not_fn( Function func ) {
		return impl::not_fn_t<Function>{std::move( func )};
	}

	template<typename Function>
	constexpr impl::not_fn_t<Function> not_fn( ) {
		return impl::not_fn_t<Function>{};
	}

	template<bool B>
	using bool_constant = std::integral_constant<bool, B>;

	template<bool B>
	constexpr bool bool_consant_v = bool_constant<B>::value;

	namespace impl {
		template<typename T>
		struct is_reference_wrapper : std::false_type {};

		template<typename U>
		struct is_reference_wrapper<std::reference_wrapper<U>> : std::true_type {};

		template<typename T>
		constexpr bool is_reference_wrapper_v = is_reference_wrapper<T>::value;

		template<typename Base, typename T, typename Derived, typename... Args>
		auto INVOKE( T Base::*pmf, Derived &&ref, Args &&... args ) noexcept(
		  noexcept( ( std::forward<Derived>( ref ).*pmf )( std::forward<Args>( args )... ) ) )
		  -> std::enable_if_t<daw::is_function_v<T> && daw::is_base_of_v<Base, std::decay_t<Derived>>,
		                      decltype( ( std::forward<Derived>( ref ).*pmf )( std::forward<Args>( args )... ) )> {
			return ( std::forward<Derived>( ref ).*pmf )( std::forward<Args>( args )... );
		}

		template<typename Base, typename T, typename RefWrap, typename... Args>
		constexpr auto
		INVOKE( T Base::*pmf, RefWrap &&ref,
		        Args &&... args ) noexcept( noexcept( ( ref.get( ).*pmf )( std::forward<Args>( args )... ) ) )
		  -> std::enable_if_t<daw::is_function_v<T> && is_reference_wrapper_v<std::decay_t<RefWrap>>,
		                      decltype( ( ref.get( ).*pmf )( std::forward<Args>( args )... ) )> {

			return ( ref.get( ).*pmf )( std::forward<Args>( args )... );
		}

		template<typename Base, typename T, typename Pointer, typename... Args>
		constexpr auto INVOKE( T Base::*pmf, Pointer &&ptr, Args &&... args ) noexcept(
		  noexcept( ( ( *std::forward<Pointer>( ptr ) ).*pmf )( std::forward<Args>( args )... ) ) )
		  -> std::enable_if_t<daw::is_function_v<T> && !is_reference_wrapper_v<std::decay_t<Pointer>> &&
		                        !daw::is_base_of_v<Base, std::decay_t<Pointer>>,
		                      decltype( ( ( *std::forward<Pointer>( ptr ) ).*pmf )( std::forward<Args>( args )... ) )> {

			return ( ( *std::forward<Pointer>( ptr ) ).*pmf )( std::forward<Args>( args )... );
		}

		template<typename Base, typename T, typename Derived>
		constexpr auto INVOKE( T Base::*pmd, Derived &&ref ) noexcept( noexcept( std::forward<Derived>( ref ).*pmd ) )
		  -> std::enable_if_t<!daw::is_function_v<T> && daw::is_base_of_v<Base, std::decay_t<Derived>>,
		                      decltype( std::forward<Derived>( ref ).*pmd )> {

			return std::forward<Derived>( ref ).*pmd;
		}

		template<typename Base, typename T, typename RefWrap>
		constexpr auto INVOKE( T Base::*pmd, RefWrap &&ref ) noexcept( noexcept( ref.get( ).*pmd ) )
		  -> std::enable_if_t<!daw::is_function_v<T> && is_reference_wrapper_v<std::decay_t<RefWrap>>,
		                      decltype( ref.get( ).*pmd )> {
			return ref.get( ).*pmd;
		}

		template<typename Base, typename T, typename Pointer>
		constexpr auto INVOKE( T Base::*pmd, Pointer &&ptr ) noexcept( noexcept( ( *std::forward<Pointer>( ptr ) ).*pmd ) )
		  -> std::enable_if_t<!daw::is_function_v<T> && !is_reference_wrapper_v<std::decay_t<Pointer>> &&
		                        !daw::is_base_of_v<Base, std::decay_t<Pointer>>,
		                      decltype( ( *std::forward<Pointer>( ptr ) ).*pmd )> {
			return ( *std::forward<Pointer>( ptr ) ).*pmd;
		}

		template<typename F, typename... Args>
		constexpr auto
		INVOKE( F &&f, Args &&... args ) noexcept( noexcept( std::forward<F>( f )( std::forward<Args>( args )... ) ) )
		  -> std::enable_if_t<!daw::is_member_pointer_v<std::decay_t<F>>,
		                      decltype( std::forward<F>( f )( std::forward<Args>( args )... ) )> {

			return std::forward<F>( f )( std::forward<Args>( args )... );
		}
	} // namespace impl

	template<typename F, typename... ArgTypes>
	constexpr auto invoke( F &&f, ArgTypes &&... args )
	  // exception specification for QoI
	  noexcept( noexcept( impl::INVOKE( std::forward<F>( f ), std::forward<ArgTypes>( args )... ) ) )
	    -> decltype( impl::INVOKE( std::forward<F>( f ), std::forward<ArgTypes>( args )... ) ) {

		return impl::INVOKE( std::forward<F>( f ), std::forward<ArgTypes>( args )... );
	}

	namespace impl {
		template<typename F, typename Tuple, std::size_t... I>
		constexpr decltype( auto ) apply_impl( F &&f, Tuple &&t, std::index_sequence<I...> ) {
			return daw::invoke( std::forward<F>( f ), std::get<I>( std::forward<Tuple>( t ) )... );
		}
	} // namespace impl

	template<typename F, typename Tuple>
	constexpr decltype( auto ) apply( F &&f, Tuple &&t ) {
		return impl::apply_impl( std::forward<F>( f ), std::forward<Tuple>( t ),
		                         std::make_index_sequence<daw::tuple_size_v<std::decay_t<Tuple>>>{} );
	}

	template<typename T>
	constexpr std::add_const_t<T> &as_const( T &t ) noexcept {
		return t;
	}

	struct nonesuch {
		nonesuch( ) = delete;
		~nonesuch( ) = delete;
		nonesuch( nonesuch const & ) = delete;
		void operator=( nonesuch const & ) = delete;
	};

	namespace impl {
		template<class Default, class AlwaysVoid, template<class...> class Op, class... Args>
		struct detector {
			using value_t = std::false_type;
			using type = Default;
		};

		template<class Default, template<class...> class Op, class... Args>
		struct detector<Default, daw::void_t<Op<Args...>>, Op, Args...> {
			using value_t = std::true_type;
			using type = Op<Args...>;
		};

	} // namespace impl
	template<template<class...> class Op, class... Args>
	using is_detected = typename impl::detector<nonesuch, void, Op, Args...>::value_t;

	template<template<class...> class Op, class... Args>
	using detected_t = typename impl::detector<nonesuch, void, Op, Args...>::type;

	template<class Default, template<class...> class Op, class... Args>
	using detected_or = impl::detector<Default, void, Op, Args...>;

	template<template<class...> class Op, class... Args>
	constexpr bool is_detected_v = is_detected<Op, Args...>::value;

	template<class Default, template<class...> class Op, class... Args>
	using detected_or_t = typename detected_or<Default, Op, Args...>::type;

	template<class Expected, template<class...> class Op, class... Args>
	using is_detected_exact = std::is_same<Expected, detected_t<Op, Args...>>;

	template<class Expected, template<class...> class Op, class... Args>
	constexpr bool is_detected_exact_v = is_detected_exact<Expected, Op, Args...>::value;

	template<class To, template<class...> class Op, class... Args>
	using is_detected_convertible = std::is_convertible<detected_t<Op, Args...>, To>;

	template<class To, template<class...> class Op, class... Args>
	constexpr bool is_detected_convertible_v = is_detected_convertible<To, Op, Args...>::value;

	template<typename T>
	constexpr bool is_nothrow_copy_constructible_v = std::is_nothrow_copy_constructible<T>::value;

	template<typename T>
	constexpr bool is_nothrow_move_constructible_v = std::is_nothrow_move_constructible<T>::value;

	template<typename T>
	constexpr bool is_nothrow_default_constructible_v = std::is_nothrow_default_constructible<T>::value;

	template<typename T>
	constexpr bool is_destructible_v = std::is_destructible<T>::value;

	template<typename T>
	constexpr bool is_trivially_destructible_v = std::is_trivially_destructible<T>::value;

	template<typename T>
	constexpr bool is_nothrow_destructible_v = std::is_nothrow_destructible<T>::value;

	template<typename T, typename U = T>
	constexpr T exchange( T &obj, U &&new_value ) {
		T old_value = std::move( obj );
		obj = std::forward<U>( new_value );
		return old_value;
	}
} // namespace daw
