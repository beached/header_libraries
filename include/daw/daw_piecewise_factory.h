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
#include "daw_exception.h"
#include "daw_move.h"
#include "daw_overload.h"
#include "daw_remove_cvref.h"
#include "daw_traits.h"
#include "daw_utility.h"
#include "daw_visit.h"
#include "traits/daw_traits_conditional.h"

#include <functional>
#include <optional>
#include <tuple>
#include <variant>

namespace daw {
	namespace impl {
		template<typename T, typename U>
		daw::remove_cvref_t<T> remove_layer_func( std::variant<T, U> );

		template<typename T>
		daw::remove_cvref_t<T> remove_layer_func( T );

		template<typename T>
		struct process_args_t {
			constexpr T operator( )( T const &value ) const {
				return value;
			}
			constexpr T operator( )( T &&value ) const {
				return std::move( value );
			}

			template<
			  typename Callable,
			  std::enable_if_t<
			    std::is_convertible_v<
			      decltype( std::declval<daw::remove_cvref_t<Callable>>( )( ) ), T>,
			    std::nullptr_t> = nullptr>
			constexpr T operator( )( Callable &&f ) const {
				return DAW_FWD( f )( );
			}
			T operator( )( std::monostate ) const {
				daw::exception::daw_throw<std::invalid_argument>(
				  "Value cannot be empty" );
			}
		};

		template<typename T>
		constexpr T &&get_value( T &&value ) {
			return DAW_FWD( value );
		}

		template<typename... Args>
		constexpr auto get_value( std::variant<Args...> const &value ) {
			using T = decltype( remove_layer_func( value ) );
			return daw::visit_nt( value, process_args_t<T>{ } );
		}

		template<typename... Args>
		constexpr auto get_value( std::variant<Args...> &&value ) {
			using T = decltype( remove_layer_func( value ) );
			return daw::visit_nt( std::move( value ), process_args_t<T>{ } );
		}

		template<size_t N, typename... Args>
		constexpr auto process_args( std::tuple<Args...> const &tp ) {
			return get_value( *std::get<N>( tp ) );
		}

		template<size_t N, typename... Args>
		constexpr auto process_args( std::tuple<Args...> &&tp ) {
			return get_value( *std::get<N>( std::move( tp ) ) );
		}

		template<typename T, typename... Args, std::size_t... I>
		constexpr T piecewise_applier_impl( std::tuple<Args...> const &tp,
		                                    std::index_sequence<I...> ) {

			return construct_a<T>( process_args<I>( tp )... );
		}

		template<typename T, typename... Args, std::size_t... I>
		constexpr T piecewise_applier_impl( std::tuple<Args...> &&tp,
		                                    std::index_sequence<I...> ) {

			return construct_a<T>( process_args<I>( std::move( tp ) )... );
		}

		template<
		  size_t N, bool, typename... Args, typename Value,
		  std::enable_if_t<( N >= sizeof...( Args ) ), std::nullptr_t> = nullptr>
		void set_tuple( size_t, std::tuple<Args...> &, Value && ) {
			daw::exception::daw_throw<std::out_of_range>( );
		}

		template<
		  size_t N, bool use_late, typename... Args, typename Value,
		  std::enable_if_t<( N < sizeof...( Args ) ), std::nullptr_t> = nullptr>
		constexpr void set_tuple( size_t index, std::tuple<Args...> &tp,
		                          Value &&value ) {
			if( N != index ) {
				set_tuple<N + 1, use_late>( index, tp, DAW_FWD( value ) );
				return;
			}
			using tp_val_t = decltype( std::get<N>( tp ) );
			using value_t =
			  decltype( remove_layer_func( std::declval<tp_val_t>( ) ) );

			auto const setter = daw::overload(
			  [&]( value_t const &v ) {
				  std::get<N>( tp ) = value_t{ v };
			  },
			  [&]( value_t &&v ) {
				  std::get<N>( tp ) = value_t{ std::move( v ) };
			  },
			  [&]( std::function<value_t( )> const &f ) {
				  std::get<N>( tp ) = value_t{ f( ) };
			  },
			  [&]( std::function<value_t( )> &&f ) {
				  std::get<N>( tp ) = value_t{ std::move( f )( ) };
			  },
			  []( ... ) noexcept {} );

			setter( DAW_FWD( value ) );
		}

		template<typename T, typename... Args>
		constexpr T piecewise_applier( std::tuple<Args...> const &tp ) {
			return piecewise_applier_impl<T>(
			  tp, std::make_index_sequence<sizeof...( Args )>{ } );
		}

		template<typename T, typename... Args>
		constexpr T piecewise_applier( std::tuple<Args...> &&tp ) {
			return piecewise_applier_impl<T>(
			  std::move( tp ), std::make_index_sequence<sizeof...( Args )>{ } );
		}

		template<typename T, bool use_late, typename... Args>
		struct piecewise_factory_impl_t {
		protected:
			using ttype_t =
			  conditional_t<use_late,
			                std::tuple<std::optional<
			                  std::variant<Args, std::function<Args( )>>>...>,
			                std::tuple<std::optional<Args>...>>;

			ttype_t m_args;

		public:
			constexpr T operator( )( ) const & {
				return piecewise_applier<T>( m_args );
			}

			constexpr T operator( )( ) & {
				return piecewise_applier<T>( m_args );
			}

			constexpr T operator( )( ) && {
				return piecewise_applier<T>( std::move( m_args ) );
			}

		private:
			template<size_t N>
			using arg_at_t = std::tuple_element_t<N, std::tuple<Args...>>;

		public:
			template<size_t N>
			constexpr void set( arg_at_t<N> const &value ) {

				std::get<N>( m_args ) = value;
			}

			template<size_t N>
			constexpr void set( arg_at_t<N> &&value ) {

				std::get<N>( m_args ) = std::move( value );
			}

			template<typename Value>
			constexpr void set( size_t index, Value &&value ) {
				set_tuple<0, use_late>( index, m_args, DAW_FWD( value ) );
			}
		};
	} // namespace impl

	template<typename T, typename... Args>
	struct piecewise_factory_late_t
	  : private impl::piecewise_factory_impl_t<T, true, Args...> {

		using impl::piecewise_factory_impl_t<T, true,
		                                     Args...>::piecewise_factory_impl_t;
		using impl::piecewise_factory_impl_t<T, true, Args...>::set;
		using impl::piecewise_factory_impl_t<T, true, Args...>::operator( );

		template<size_t N, typename Callable,
		         std::enable_if_t<
		           std::is_convertible_v<decltype( std::declval<Callable>( )( ) ),
		                                 traits::nth_type<N, Args...>>,
		           std::nullptr_t> = nullptr>
		void set( Callable &&f ) {
			std::function<traits::nth_type<N, Args...>( )> func = DAW_FWD( f );
			std::get<N>( this->m_args ) = std::move( func );
		}
	};

	template<typename T, typename... Args>
	struct piecewise_factory_t
	  : private impl::piecewise_factory_impl_t<T, false, Args...> {

		using impl::piecewise_factory_impl_t<T, false,
		                                     Args...>::piecewise_factory_impl_t;
		using impl::piecewise_factory_impl_t<T, false, Args...>::set;
		using impl::piecewise_factory_impl_t<T, false, Args...>::operator( );
	};
} // namespace daw
