// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_iterator_traits.h"
#include "daw/daw_move.h"
#include "daw/daw_traits.h"
#include "daw/daw_visit.h"
#include "pipeline_traits.h"
#include "range.h"

#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

namespace daw::pipelines {
	template<Range... Ranges>
	struct concat_view {
		static_assert( sizeof...( Ranges ) > 0,
		               "concat_view requires at least one iterator specified" );
		using iterator_category =
		  common_iterator_category_t<std::forward_iterator_tag,
		                             range_category_t<Ranges>...>;
		static_assert( not std::same_as<void, iterator_category> );
		using types_t = std::tuple<Ranges...>;
		using value_type = std::common_type_t<range_value_t<Ranges>...>;
		using reference = std::common_reference_t<range_reference_t<Ranges>...>;
		using const_reference =
		  std::common_reference_t<range_const_reference_t<Ranges>...>;
		using difference_type = std::ptrdiff_t;
		using i_am_a_daw_concat_view_class = void;

	private:
		using range_variant_t =
		  std::variant<std::pair<iterator_t<Ranges>, iterator_t<Ranges>>...>;

		std::array<range_variant_t, sizeof...( Ranges )> m_ranges;
		std::size_t m_index = 0;

		template<std::size_t... Is>
		static constexpr std::array<range_variant_t, sizeof...( Ranges )>
		get_iter_pair( std::index_sequence<Is...>,
		               std::tuple<Ranges...> &&ranges ) {
			return std::array{
			  range_variant_t( std::in_place_index_t<Is>{ },
			                   std::begin( std::get<Is>( std::move( ranges ) ) ),
			                   std::end( std::get<Is>( std::move( ranges ) ) ) )... };
		}

	public:
		explicit concat_view( ) = default;

		explicit constexpr concat_view( Ranges... ranges )
		  : m_ranges(
		      get_iter_pair( std::make_index_sequence<sizeof...( Ranges )>{ },
		                     std::tuple{ std::move( ranges )... } ) ) {}

		concat_view begin( ) const {
			return *this;
		}

		concat_view end( ) const {
			auto result = *this;
			result.m_index = m_ranges.size( ) - 1;
			for( auto &r : result.m_ranges ) {
				daw::visit_nt( r, []( auto &p ) {
					p.first = p.second;
				} );
			}
			return result;
		}

		constexpr concat_view &operator++( ) {
			daw::visit_nt( m_ranges[m_index], [&]( auto &p ) {
				++p.first;
				if( p.first == p.second ) {
					++m_index;
				}
			} );
			return *this;
		}

		[[nodiscard]] constexpr concat_view operator++( int ) {
			auto tmp = *this;
			++( *this );
			return tmp;
		}

		[[nodiscard]] constexpr reference operator*( ) noexcept {
			return daw::visit_nt( m_ranges[m_index], []( auto &p ) -> reference {
				return *p.first;
			} );
		}

		[[nodiscard]] constexpr const_reference operator*( ) const noexcept {
			return daw::visit_nt( m_ranges[m_index], []( auto &p ) -> reference {
				return *p.first;
			} );
		}

		[[nodiscard]] constexpr bool
		operator==( concat_view const &rhs ) const noexcept {
			return m_ranges == rhs.m_ranges;
		}

		[[nodiscard]] constexpr bool
		operator!=( concat_view const &rhs ) const noexcept {
			return m_ranges != rhs.m_ranges;
		}
	};
	template<typename... Ranges>
	concat_view( Ranges... ) -> concat_view<Ranges...>;

	namespace pimpl {
		struct Concat_t {
			template<typename R>
			[[nodiscard]] DAW_CPP23_STATIC_CALL_OP constexpr auto
			operator( )( R &&r ) DAW_CPP23_STATIC_CALL_OP_CONST {
				using std::get;
				if constexpr( requires {
					              typename iterator_t<R>::i_am_a_daw_zip_iterator_class;
				              } ) {
					return [&]<std::size_t... Is>( std::index_sequence<Is...> ) {
						auto tp_first = std::begin( r ).get_tuple( );
						auto tp_last = std::end( r ).get_tuple( );
						return concat_view{ range_t{ get<Is>( std::move( tp_first ) ),
						                             get<Is>( std::move( tp_last ) ) }... };
					}( std::make_index_sequence<
					         daw::remove_cvref_t<iterator_t<R>>::types_size_v>{ } );
				} else {
					static_assert( daw::is_tuple_like_v<daw::remove_cvref_t<R>> );
					return [&]<std::size_t... Is>( std::index_sequence<Is...> ) {
						static_assert(
						  ( Range<std::tuple_element_t<Is, daw::remove_cvref_t<R>>> and
						    ... ),
						  "Concat when passed a tuple expects a tuple of Ranges" );
						return concat_view{
						  range_t{ std::begin( get<Is>( DAW_FWD( r ) ) ),
						           std::end( get<Is>( DAW_FWD( r ) ) ) }... };
					}( std::make_index_sequence<
					         daw::remove_cvref_t<R>::types_size_v>{ } );
				}
			}
		};
	} // namespace pimpl
	inline constexpr auto Concat = pimpl::Concat_t{ };
} // namespace daw::pipelines
