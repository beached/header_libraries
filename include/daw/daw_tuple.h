// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_library
//

#pragma once

#include "daw_apply.h"
#include "daw_move.h"
#include "daw_traits.h"
#include "traits/daw_traits_conditional.h"

#include <cstddef>
#include <type_traits>
#include <utility>

#if defined( __GNUC__ )
#define DAW_TUPLESPECIALCLASS class
#else
#define DAW_TUPLESPECIALCLASS struct
#endif

namespace daw::impl {
	inline namespace {
		template<typename T>
		struct tuple_tag {
			using type = T;
		};

		template<std::size_t, typename...>
		struct tuple_storage;

		struct tuple_storage_empty {};

		template<std::size_t N, typename T, bool is_empty>
		struct tuple_value {
			T m_value;

			constexpr T &get( daw::index_constant<N> ) &noexcept {
				return m_value;
			}

			constexpr T const &get( daw::index_constant<N> ) const &noexcept {
				return m_value;
			}

			constexpr T &&get( daw::index_constant<N> ) &&noexcept {
				return std::move( m_value );
			}

			constexpr T const &&get( daw::index_constant<N> ) const &&noexcept {
				return std::move( m_value );
			}
		};

		template<std::size_t N, typename T>
		struct tuple_value<N, T, true> : T {

			constexpr T &get( daw::index_constant<N> ) &noexcept {
				return *this;
			}

			constexpr T const &get( daw::index_constant<N> ) const &noexcept {
				return *this;
			}

			constexpr T &&get( daw::index_constant<N> ) &&noexcept {
				return std::move( *this );
			}

			constexpr T const &&get( daw::index_constant<N> ) const &&noexcept {
				return std::move( *this );
			}
		};

		template<std::size_t N, typename T>
		struct tuple_storage<N, T> : tuple_value<N, T, std::is_empty_v<T>> {
			using base_value = tuple_value<N, T, std::is_empty_v<T>>;
			using base_value::get;

			static constexpr tuple_tag<T> get_type( daw::index_constant<N> ) {
				return { };
			}

			constexpr tuple_storage( ) = default;

			template<typename U,
			         std::enable_if_t<
			           (not std::is_same_v<tuple_storage, daw::remove_cvref_t<U>>),
			           std::nullptr_t> = nullptr>
			constexpr tuple_storage( U &&value )
			  : base_value{ DAW_FWD( value ) } {}
		};

		template<std::size_t N, typename T, typename T1, typename... Ts>
		struct tuple_storage<N, T, T1, Ts...>
		  : tuple_value<N, T, std::is_empty_v<T>>, tuple_storage<N + 1, T1, Ts...> {

			using base_value = tuple_value<N, T, std::is_empty_v<T>>;
			using base_storage = tuple_storage<N + 1, T1, Ts...>;
			using base_storage::get;
			using base_storage::get_type;
			using base_value::get;

			static constexpr tuple_tag<T> get_type( daw::index_constant<N> ) {
				return { };
			}

			constexpr tuple_storage( ) = default;

			template<
			  typename U, typename... Us,
			  std::enable_if_t<( sizeof...( Us ) > 0 ), std::nullptr_t> = nullptr>
			constexpr tuple_storage( U &&value, Us &&...us )
			  : base_value{ DAW_FWD( value ) }
			  , base_storage{ DAW_FWD( us )... } {}
		};
	} // namespace
} // namespace daw::impl

namespace daw {
	template<typename... Ts>
	class tuple : private conditional_t<( sizeof...( Ts ) > 0 ),
	                                    impl::tuple_storage<0, Ts...>,
	                                    impl::tuple_storage_empty> {
		using base_type =
		  conditional_t<( sizeof...( Ts ) > 0 ), impl::tuple_storage<0, Ts...>,
		                impl::tuple_storage_empty>;
		using base_type::base_type;

		template<typename T>
		friend DAW_TUPLESPECIALCLASS ::std::tuple_size;

		template<std::size_t I, class T>
		friend DAW_TUPLESPECIALCLASS ::std::tuple_element;

		template<typename Predicate, std::size_t... Is, typename... Us>
		constexpr bool and_all( Predicate p, std::index_sequence<Is...>,
		                        tuple<Us...> const &rhs ) const {
			(void)p;
			return ( p( get<Is>( ), rhs.template get<Is>( ) ) and ... );
		}

		template<typename Predicate, std::size_t... Is, typename... Us>
		constexpr bool or_all( Predicate p, std::index_sequence<Is...>,
		                       tuple<Us...> const &rhs ) const {
			(void)p;
			return ( p( get<Is>( ), rhs.template get<Is>( ) ) or ... );
		}

	public:
		static constexpr std::size_t tuple_size = sizeof...( Ts );

		template<std::size_t N>
		constexpr decltype( auto ) get( ) const & {
			return base_type::get( daw::index_constant_v<N> );
		}

		template<std::size_t N>
		constexpr decltype( auto ) get( ) & {
			return base_type::get( daw::index_constant_v<N> );
		}

		template<std::size_t N>
		constexpr decltype( auto ) get( ) && {
			return base_type::get( daw::index_constant_v<N> );
		}

		template<std::size_t N>
		constexpr decltype( auto ) get( ) const && {
			return base_type::get( daw::index_constant_v<N> );
		}

		template<typename... Vs>
		friend constexpr bool operator==( tuple const &lhs,
		                                  tuple<Vs...> const &rhs ) {
			if constexpr( tuple_size == tuple<Vs...>::tuple_size ) {
				return lhs.and_all(
				  []( auto const &l, auto const &r ) {
					  return l == r;
				  },
				  std::make_index_sequence<sizeof...( Vs )>{ }, rhs );
			} else {
				return false;
			}
		}

		template<typename... Vs>
		friend constexpr bool operator!=( tuple const &lhs,
		                                  tuple<Vs...> const &rhs ) {
			if constexpr( tuple_size == tuple<Vs...>::tuple_size ) {
				return lhs.or_all(
				  []( auto const &l, auto const &r ) {
					  return l != r;
				  },
				  std::make_index_sequence<sizeof...( Vs )>{ }, rhs );
			} else {
				return true;
			}
		}
	};

	template<typename... Ts>
	tuple( Ts... ) -> tuple<Ts...>;

	template<std::size_t N, typename... Ts>
	constexpr decltype( auto ) get( tuple<Ts...> const &tp ) {
		return tp.template get<N>( );
	}

	template<std::size_t N, typename... Ts>
	constexpr decltype( auto ) get( tuple<Ts...> &tp ) {
		return tp.template get<N>( );
	}

	template<std::size_t N, typename... Ts>
	constexpr decltype( auto ) get( tuple<Ts...> &&tp ) {
		return tp.template get<N>( );
	}

	template<typename... Ts>
	constexpr tuple<Ts &&...> forward_as_tuple( Ts &&...ts ) noexcept {
		return { DAW_FWD( ts )... };
	}

	template<typename... Ts>
	constexpr tuple<Ts &...> tie( Ts &...ts ) noexcept {
		return { ts... };
	}

	namespace make_tuple_details {
		template<class T>
		struct unwrap_refwrapper {
			using type = T;
		};

		template<class T>
		struct unwrap_refwrapper<std::reference_wrapper<T>> {
			using type = T &;
		};

		template<class T>
		using unwrap_decay_t =
		  typename unwrap_refwrapper<typename std::decay<T>::type>::type;
	} // namespace make_tuple_details

	template<typename... Ts>
	constexpr tuple<make_tuple_details::unwrap_decay_t<Ts>...>
	make_tuple( Ts &&...args ) {
		return { DAW_FWD( args )... };
	}
} // namespace daw

namespace std {
	template<typename... Ts>
	DAW_TUPLESPECIALCLASS tuple_size<::daw::tuple<Ts...>>
	  : public daw::index_constant<::daw::tuple<Ts...>::tuple_size>{ };

	template<std::size_t I, typename... Ts>
	DAW_TUPLESPECIALCLASS tuple_element<I, ::daw::tuple<Ts...>> {
	public:
		using type = typename decltype( ::daw::tuple<Ts...>::get_type(
		  daw::index_constant<I>{ } ) )::type;
	};
} // namespace std

namespace daw {
	namespace tuple_apply_detail {
		template<typename F, typename Tuple, std::size_t... Is>
		inline constexpr decltype( auto ) apply_impl( F &&f, Tuple &&t,
		                                              std::index_sequence<Is...> ) {
			return daw::invoke( DAW_FWD( f ), daw::get<Is>( DAW_FWD( t ) )... );
		}
	} // namespace tuple_apply_detail

	template<typename F, typename... Ts>
	inline constexpr decltype( auto ) apply( F &&f, tuple<Ts...> &t ) {
		return tuple_apply_detail::apply_impl(
		  DAW_FWD( f ), t,
		  std::make_index_sequence<std::tuple_size_v<tuple<Ts...>>>{ } );
	}

	template<typename F, typename... Ts>
	inline constexpr decltype( auto ) apply( F &&f, tuple<Ts...> const &t ) {
		return tuple_apply_detail::apply_impl(
		  DAW_FWD( f ), t,
		  std::make_index_sequence<std::tuple_size_v<tuple<Ts...>>>{ } );
	}

	template<typename F, typename... Ts>
	inline constexpr decltype( auto ) apply( F &&f, tuple<Ts...> &&t ) {
		return tuple_apply_detail::apply_impl(
		  DAW_FWD( f ), DAW_FWD( t ),
		  std::make_index_sequence<
		    std::tuple_size_v<std::remove_reference_t<tuple<Ts...>>>>{ } );
	}

	template<typename F, typename... Ts>
	inline constexpr decltype( auto ) apply( F &&f, tuple<Ts...> const &&t ) {
		return tuple_apply_detail::apply_impl(
		  DAW_FWD( f ), DAW_FWD( t ),
		  std::make_index_sequence<
		    std::tuple_size_v<std::remove_reference_t<tuple<Ts...>>>>{ } );
	}
} // namespace daw

#ifdef DAW_TUPLESPECIALCLASS
#undef DAW_TUPLESPECIALCLASS
#endif
