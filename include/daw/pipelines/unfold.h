// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_attributes.h"
#include "daw/daw_concepts.h"
#include "daw/daw_move.h"
#include "daw/daw_ref_storage.h"
#include "daw/pipelines/range_base.h"

#include <concepts>
#include <cstddef>
#include <exception>
#include <functional>
#include <iterator>
#include <memory>
#include <optional>
#include <type_traits>
#include <utility>

namespace daw::pipelines {
	/// The end of an unfold.  Like sized_iterator_end, it models Iterator only
	/// so that views requiring Iterator<ILast> accept it; it is never
	/// dereferenced or advanced
	template<typename T>
	struct unfold_iterator_end {
		using iterator_category = std::input_iterator_tag;
		using value_type = T;
		using reference = T const &;
		using pointer = T const *;
		using difference_type = std::ptrdiff_t;

		unfold_iterator_end( ) = default;

		[[nodiscard]] constexpr bool
		operator==( unfold_iterator_end const & ) const {
			return true;
		}

		[[noreturn]] DAW_ATTRIB_NOINLINE reference operator*( ) const {
			std::terminate( );
		}

		[[noreturn]] DAW_ATTRIB_NOINLINE unfold_iterator_end &operator++( ) {
			std::terminate( );
		}

		[[noreturn]] DAW_ATTRIB_NOINLINE unfold_iterator_end operator++( int ) {
			std::terminate( );
		}
	};

	/// Fn: State -> std::optional<std::pair<T, State>>
	/// nullopt ends the range, otherwise first is the element and second the
	/// next state
	template<typename State, typename Fn>
	struct unfold_iterator {
		using step_type = std::invoke_result_t<Fn const &, State const &>;
		using iterator_category = std::input_iterator_tag;
		using value_type = typename step_type::value_type::first_type;
		using reference = value_type const &;
		using pointer = value_type const *;
		using difference_type = std::ptrdiff_t;

	private:
		Fn const *m_fn = nullptr;
		step_type m_step{ };

	public:
		unfold_iterator( ) = default;

		explicit constexpr unfold_iterator( Fn const &fn, State const &seed )
		  : m_fn( std::addressof( fn ) )
		  , m_step( std::invoke( fn, seed ) ) {}

		[[nodiscard]] constexpr reference operator*( ) const {
			return m_step->first;
		}

		[[nodiscard]] constexpr pointer operator->( ) const {
			return std::addressof( m_step->first );
		}

		constexpr unfold_iterator &operator++( ) {
			// Move the next state out before overwriting the step that holds it
			State next = std::move( m_step->second );
			m_step = std::invoke( *m_fn, std::as_const( next ) );
			return *this;
		}

		constexpr unfold_iterator operator++( int )
		requires( std::copy_constructible<unfold_iterator> )
		{
			auto result = *this;
			operator++( );
			return result;
		}

		constexpr void operator++( int )
		requires( not std::copy_constructible<unfold_iterator> )
		{
			operator++( );
		}

		/// Two iterators are equal when both are exhausted.  When the element and
		/// state are comparable, iterators at the same step also compare equal
		[[nodiscard]] constexpr bool
		operator==( unfold_iterator const &rhs ) const {
			if constexpr( std::equality_comparable<step_type> ) {
				return m_step == rhs.m_step;
			} else {
				return not m_step and not rhs.m_step;
			}
		}

		[[nodiscard]] constexpr bool
		operator==( unfold_iterator_end<value_type> const & ) const {
			return not m_step.has_value( );
		}
	};

	template<typename State, typename Fn>
	struct unfold_view
	  : private pimpl::range_base_t<
	      unfold_iterator<std::remove_cvref_t<State>, std::remove_cvref_t<Fn>>,
	      unfold_iterator_end<typename unfold_iterator<
	        std::remove_cvref_t<State>,
	        std::remove_cvref_t<Fn>>::value_type>> {
		using state_type = std::remove_cvref_t<State>;
		using function_type = std::remove_cvref_t<Fn>;
		using iterator = unfold_iterator<state_type, function_type>;
		using value_type = typename iterator::value_type;
		using const_iterator = iterator;
		using iterator_last = unfold_iterator_end<value_type>;
		using const_iterator_last = iterator_last;

	private:
		using seed_storage_t = daw::ref_storage<State>;
		using function_storage_t = daw::ref_storage<Fn>;
		seed_storage_t m_seed;
		DAW_NO_UNIQUE_ADDRESS function_storage_t m_fn;

	public:
		unfold_view( ) = default;

		explicit constexpr unfold_view(
		  daw::constructible<seed_storage_t> auto &&seed,
		  daw::constructible<function_storage_t> auto &&fn )
		  : m_seed( DAW_FWD( seed ) )
		  , m_fn( DAW_FWD( fn ) ) {}

		[[nodiscard]] constexpr iterator begin( ) const {
			return iterator{ m_fn.get( ), m_seed.get( ) };
		}

		[[nodiscard]] constexpr iterator_last end( ) const {
			return iterator_last{ };
		}
	};
	template<typename State, typename Fn>
	unfold_view( State &&, Fn && ) -> unfold_view<State, Fn>;

	namespace pimpl {
		template<typename Fn>
		struct Unfold_t {
			using storage_t = daw::ref_storage<Fn>;
			DAW_NO_UNIQUE_ADDRESS storage_t m_fn;

			explicit constexpr Unfold_t( daw::constructible<storage_t> auto &&fn )
			  : m_fn( DAW_FWD( fn ) ) {}

			template<typename State>
			[[nodiscard]] constexpr auto operator( )( State &&state ) const & {
				return unfold_view( DAW_FWD( state ), m_fn.get( ) );
			}

			template<typename State>
			[[nodiscard]] constexpr auto operator( )( State &&state ) && {
				if constexpr( storage_t::is_owned ) {
					return unfold_view( DAW_FWD( state ), std::move( m_fn ).get( ) );
				} else {
					return unfold_view( DAW_FWD( state ), m_fn.get( ) );
				}
			}
		};
		template<typename Fn>
		Unfold_t( Fn && ) -> Unfold_t<Fn>;
	} // namespace pimpl
	[[nodiscard]] constexpr auto Unfold( auto &&fn ) {
		return pimpl::Unfold_t{ DAW_FWD( fn ) };
	}
} // namespace daw::pipelines
