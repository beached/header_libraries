// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/cpp_17.h"
#include "daw/daw_as.h"
#include "daw/daw_concepts.h"
#include "daw/daw_forward_like.h"
#include "daw/daw_iterator_traits.h"
#include "daw/daw_move.h"
#include "daw/daw_traits.h"
#include "daw/daw_visit.h"
#include "daw/pipelines/pipeline_traits.h"
#include "daw/pipelines/view.h"

#include <array>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

namespace daw::pipelines {
	namespace pimpl {
		struct Concat_t;
	}
	template<ForwardRange... Ranges>
	struct concat_view;

	/// Iterates the ranges passed to `concat_view` in sequence. Holds a
	/// pointer back to the view, which owns the storage for every range; the
	/// iterator only tracks its own current position within that storage so
	/// that copies (e.g. via operator++( int )) advance independently.
	template<ForwardRange... Ranges>
	struct concat_iterator {
		static_assert( sizeof...( Ranges ) > 0,
		               "concat_iterator requires at least one range specified" );
		struct concat_iterator_end {
			[[nodiscard]] constexpr bool
			operator==( concat_iterator_end const & ) const = default;

			[[nodiscard]] constexpr bool
			operator!=( concat_iterator_end const & ) const = default;
		};

		using iterator_category = common_iterator_category_t<
		  range_category_t<daw::remove_rvalue_ref_t<Ranges>>...>;
		using types_t =
		  std::tuple<daw::remove_cvrvref_t<daw::remove_rvalue_ref_t<Ranges>>...>;
		using value_type =
		  std::common_type_t<range_value_t<daw::remove_rvalue_ref_t<Ranges>>...>;
		using reference = std::common_reference_t<
		  range_reference_t<daw::remove_rvalue_ref_t<Ranges>>...>;
		using const_reference = std::common_reference_t<
		  range_const_reference_t<daw::remove_rvalue_ref_t<Ranges>>...>;
		using difference_type = std::ptrdiff_t;
		using i_am_a_daw_concat_iterator_class = void;

	private:
		friend struct concat_view<Ranges...>;

		using position_t = pimpl::variant_range_storage_t<
		  view_t<iterator_t<Ranges>, iterator_end_t<Ranges>>...>;

		concat_view<Ranges...> const *m_view = nullptr;
		std::array<position_t, sizeof...( Ranges )> m_positions{ };
		std::size_t m_index = sizeof...( Ranges );

		template<std::size_t... Is>
		[[nodiscard]] static constexpr std::array<position_t, sizeof...( Ranges )>
		make_positions( concat_view<Ranges...> const &view,
		                std::index_sequence<Is...> ) {
			return std::array<position_t, sizeof...( Ranges )>{
			  position_t( view_t{ view.m_storage[Is].template get<Is>( ).rbegin( ),
			                      view.m_storage[Is].template get<Is>( ).rend( ) },
			              daw::constant_v<Is> )... };
		}

		constexpr void skip_empty_ranges( ) {
			while( m_index < sizeof...( Ranges ) and
			       m_positions[m_index].visit( []( auto const &r ) -> bool {
				       return r.rbegin( ) == r.rend( );
			       } ) ) {
				++m_index;
			}
		}

		explicit constexpr concat_iterator( concat_view<Ranges...> const &view )
		  : m_view( &view )
		  , m_positions( make_positions(
		      view, std::make_index_sequence<sizeof...( Ranges )>{ } ) )
		  , m_index( 0 ) {
			skip_empty_ranges( );
		}

	public:
		concat_iterator( ) = default;

		constexpr concat_iterator &operator++( ) {
			m_positions[m_index].visit( []( auto &p ) {
				++p;
			} );
			skip_empty_ranges( );
			return *this;
		}

		[[nodiscard]] constexpr concat_iterator operator++( int ) {
			auto tmp = *this;
			++( *this );
			return tmp;
		}

		[[nodiscard]] constexpr explicit operator bool( ) const noexcept {
			return m_index != sizeof...( Ranges );
		}

		[[nodiscard]] constexpr reference operator*( ) {
			return m_positions[m_index].visit( []( auto &p ) -> reference {
				return *p;
			} );
		}

		[[nodiscard]] constexpr const_reference operator*( ) const {
			return m_positions[m_index].visit(
			  []( auto const &p ) -> const_reference {
				  return *p;
			  } );
		}

		[[nodiscard]] constexpr bool
		operator==( concat_iterator const &rhs ) const noexcept {
			return m_view == rhs.m_view and m_index == rhs.m_index and
			       m_positions == rhs.m_positions;
		}

		[[nodiscard]] constexpr bool
		operator!=( concat_iterator const &rhs ) const noexcept {
			return not( *this == rhs );
		}

		[[nodiscard]] constexpr bool
		operator==( concat_iterator_end const & ) const noexcept {
			return not as<bool>( *this );
		}

		[[nodiscard]] constexpr bool
		operator!=( concat_iterator_end const & ) const noexcept {
			return as<bool>( *this );
		}
	};

	template<ForwardRange... Ranges>
	struct concat_view
	  : pimpl::range_base_t<
	      concat_iterator<Ranges...>,
	      typename concat_iterator<Ranges...>::concat_iterator_end> {
		static_assert( sizeof...( Ranges ) > 0,
		               "concat_view requires at least one iterator specified" );

		friend struct daw::pipelines::pimpl::Concat_t;
		using iterator = concat_iterator<Ranges...>;
		using iterator_category = typename iterator::iterator_category;
		using types_t = typename iterator::types_t;
		using value_type = typename iterator::value_type;
		using reference = typename iterator::reference;
		using const_reference = typename iterator::const_reference;
		using difference_type = std::ptrdiff_t;
		using i_am_a_daw_concat_view_class = void;

	private:
		friend struct concat_iterator<Ranges...>;

		using storage_t = pimpl::variant_range_storage_t<Ranges...>;

		// mutable so that begin( ) can stay const (needed to model Range) while
		// still handing out mutable iterators into owned/referenced storage.
		mutable std::array<storage_t, sizeof...( Ranges )> m_storage{ };

		template<std::size_t... Is>
		explicit constexpr concat_view( std::index_sequence<Is...>,
		                                Ranges... ranges )
		  : m_storage{ storage_t{ DAW_FWD( ranges ), daw::constant_v<Is> }... } {}

	public:
		explicit concat_view( ) = default;

		explicit constexpr concat_view( Ranges... ranges )
		  : concat_view( std::make_index_sequence<sizeof...( Ranges )>{ },
		                 DAW_FWD( ranges )... ) {}

		[[nodiscard]] constexpr iterator begin( ) const noexcept {
			return iterator{ *this };
		}

		[[nodiscard]] static constexpr typename iterator::concat_iterator_end
		end( ) noexcept {
			return { };
		}

		[[nodiscard]] constexpr bool operator==( concat_view const &rhs ) const {
			for( std::size_t n = 0; n < m_storage.size( ); ++n ) {
				if( m_storage[n] != rhs.m_storage[n] ) {
					return false;
				}
			}
			return true;
		}

		[[nodiscard]] constexpr bool
		operator!=( concat_view const &rhs ) const = default;
	};
	namespace pimpl {
		template<typename>
		consteval bool is_tuple_of_fwd_ranges( ) {
			return false;
		}

		template<typename Tuple>
		requires( daw::is_tuple_like_v<Tuple> ) //
		  consteval bool is_tuple_of_fwd_ranges( ) {
			return []<std::size_t... Is>( std::index_sequence<Is...> ) {
				return ( ForwardRange<std::tuple_element_t<Is, Tuple>> and ... );
			}( std::make_index_sequence<std::tuple_size_v<Tuple>>{ } );
		}
	} // namespace pimpl
	template<typename... Ranges>
	concat_view( Ranges &&... )
	  -> concat_view<daw::remove_rvalue_ref_t<Ranges>...>;

	namespace pimpl {
		struct Concat_t {
			template<typename R, ForwardRange... Rs>
			[[nodiscard]] DAW_CPP23_STATIC_CALL_OP constexpr auto
			operator( )( R &&r, Rs &&...rs ) DAW_CPP23_STATIC_CALL_OP_CONST {
				using std::get;
				if constexpr( sizeof...( Rs ) == 0 and requires {
					              typename daw::remove_cvref_t<
					                R>::i_am_a_daw_zip_view_class;
				              } ) {
					return [&]<std::size_t... Is>( std::index_sequence<Is...> ) {
						return concat_view{
						  get_range_ref( std::get<Is>( r.m_ranges ) )... };
					}( std::make_index_sequence<daw::remove_cvref_t<R>::range_count>{ } );
				} else if constexpr( ( sizeof...( Rs ) == 0 ) and
				                     daw::is_tuple_like_v<daw::remove_cvref_t<R>> and
				                     pimpl::is_tuple_of_fwd_ranges<
				                       daw::remove_cvref_t<R>>( ) ) {
					static_assert( sizeof...( Rs ) == 0 );
					return [&]<std::size_t... Is>( std::index_sequence<Is...> ) {
						static_assert(
						  ( Range<std::tuple_element_t<Is, daw::remove_cvref_t<R>>> and
						    ... ),
						  "Concat when passed a tuple expects a tuple of Ranges" );
						return concat_view{ std::get<Is>( DAW_FWD( r ) )... };
					}( std::make_index_sequence<
					         std::tuple_size_v<daw::remove_cvref_t<R>>>{ } );
				} else {
					static_assert( ForwardRange<R> );
					return concat_view<R, Rs...>{ DAW_FWD( r ), DAW_FWD( rs )... };
				}
			}
		};
	} // namespace pimpl
	/// Make a Range that is a zip iterator, or who's value is a tuple of Ranges
	/// act as if it was one range. e.g tuple<vector<int>,
	/// vector<int>>{{1,2,3},{4,5,6}} would output {1,2,3,4,5,6}
	inline constexpr auto Concat = pimpl::Concat_t{ };
} // namespace daw::pipelines
