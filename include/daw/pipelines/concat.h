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
#include "daw/iterator/daw_checked_iterator_proxy.h"
#include "daw/pipelines/daw_concept_checker.h"
#include "daw/pipelines/pipeline_traits.h"
#include "daw/pipelines/view.h"

#include <array>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

namespace daw::pipelines {
	template<ForwardRange... Ranges>
	struct concat_view;

	namespace pimpl {
		struct Concat_t;

		template<typename CV, ForwardRange... Ranges>
		struct concat_iterator;

		template<ForwardIterator... SentinelFors>
		struct concat_iterator_end {
			using iterator_category = std::input_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = std::common_type_t<iter_value_t<SentinelFors>...>;
			using reference =
			  std::common_reference_t<iter_reference_t<SentinelFors>...>;
			using pointer = void;
			using i_am_a_daw_concat_iterator_end_class = void;

			concat_iterator_end( ) = default;

			constexpr bool operator==( concat_iterator_end const & ) const {
				return true;
			}

			[[noreturn]] DAW_ATTRIB_NOINLINE inline value_type operator*( ) const {
				std::terminate( );
			}

			[[noreturn]] DAW_ATTRIB_NOINLINE inline concat_iterator_end &
			operator++( ) const {
				std::terminate( );
			}

			[[noreturn]] DAW_ATTRIB_NOINLINE inline concat_iterator_end
			operator++( int ) const {
				std::terminate( );
			}
		};

		/// Iterates the ranges passed to `concat_view` in sequence. Holds a
		/// pointer back to the view, which owns the storage for every range; the
		/// iterator only tracks its own current position within that storage so
		/// that copies (e.g. via operator++( int )) advance independently.
		template<typename CV, ForwardRange... Ranges>
		struct concat_iterator {
			using iterator_category = std::forward_iterator_tag;

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
			using last_iterator =
			  pimpl::concat_iterator_end<daw::iterator_end_t<Ranges>...>;
			using last_const_iterator =
			  pimpl::concat_iterator_end<daw::const_iterator_end_t<Ranges>...>;
			using position_t = variant_range_storage_t<
			  view_t<iterator_t<Ranges>, iterator_end_t<Ranges>>...>;

			using cv_t = CV;
			using cv_pointer_t = cv_t *;
			using cv_const_pointer_t = cv_t const *;
			using positions_t = std::array<position_t, cv_t::range_size>;

			cv_pointer_t m_view{ };
			positions_t m_positions = positions_t{ };
			std::size_t m_index = cv_t::range_size;

			template<std::size_t... Is>
			[[nodiscard]] static constexpr positions_t
			make_positions( cv_const_pointer_t view, std::index_sequence<Is...> ) {
				return positions_t{
				  position_t( view_t{ view->m_storage[Is].template get<Is>( ).rbegin( ),
				                      view->m_storage[Is].template get<Is>( ).rend( ) },
				              daw::constant_v<Is> )... };
			}

			constexpr void skip_empty_ranges( ) {
				while( m_index < cv_t::range_size and
				       m_positions[m_index].visit( []( auto const &r ) -> bool {
					       return r.rbegin( ) == r.rend( );
				       } ) ) {
					++m_index;
				}
			}

		public:
			explicit constexpr concat_iterator( cv_pointer_t view )
			  : m_view( view )
			  , m_positions( make_positions(
			      view, std::make_index_sequence<cv_t::range_size>{ } ) )
			  , m_index( 0 ) {
				skip_empty_ranges( );
			}

			concat_iterator( ) = default;

			constexpr concat_iterator &operator++( ) {
				m_positions[m_index].visit( []( auto &p ) {
					(void)++p;
				} );
				skip_empty_ranges( );
				return *this;
			}

			[[nodiscard]] constexpr concat_iterator operator++( int ) {
				auto tmp = *this;
				++( *this );
				return tmp;
			}

			[[nodiscard]] constexpr bool good( ) const noexcept {
				return m_index != cv_t::range_size;
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
			operator==( last_iterator const & ) const noexcept {
				return not good( );
			}
		};
	} // namespace pimpl

	template<ForwardRange... Ranges>
	struct concat_view
	  : pimpl::range_base_t<
	      pimpl::concat_iterator<concat_view<Ranges...>, Ranges...>,
	      pimpl::concat_iterator_end<daw::iterator_end_t<Ranges>...>> {
		static constexpr std::size_t range_size = sizeof...( Ranges );
		static_assert( range_size > 0,
		               "concat_view requires at least one iterator specified" );

		friend struct daw::pipelines::pimpl::Concat_t;
		using i_am_a_daw_concat_view_class = void;

	private:
		using types_t =
		  std::tuple<daw::remove_cvrvref_t<daw::remove_rvalue_ref_t<Ranges>>...>;
		using value_type =
		  std::common_type_t<range_value_t<daw::remove_rvalue_ref_t<Ranges>>...>;
		using reference = std::common_reference_t<
		  range_reference_t<daw::remove_rvalue_ref_t<Ranges>>...>;
		using const_reference = std::common_reference_t<
		  range_const_reference_t<daw::remove_rvalue_ref_t<Ranges>>...>;
		using difference_type = std::ptrdiff_t;
		using position_t = pimpl::variant_range_storage_t<
		  view_t<iterator_t<Ranges>, iterator_end_t<Ranges>>...>;

		using iterator = pimpl::concat_iterator<concat_view, Ranges...>;
		friend iterator;

		using const_iterator = pimpl::concat_iterator<concat_view const, Ranges...>;
		friend const_iterator;

		using last_iterator =
		  pimpl::concat_iterator_end<daw::iterator_end_t<Ranges>...>;
		using last_const_iterator =
		  pimpl::concat_iterator_end<daw::const_iterator_end_t<Ranges>...>;

		using storage_t = pimpl::variant_range_storage_t<Ranges...>;

		// mutable so that begin( ) can stay const (needed to model Range) while
		// still handing out mutable iterators into owned/referenced storage.
		mutable std::array<storage_t, range_size> m_storage{ };

		template<std::size_t... Is>
		explicit constexpr concat_view( std::index_sequence<Is...>,
		                                Ranges... ranges )
		  : m_storage{ storage_t{ DAW_FWD( ranges ), daw::constant_v<Is> }... } {}

	public:
		explicit concat_view( ) = default;

		explicit constexpr concat_view( Ranges... ranges )
		  : concat_view( std::make_index_sequence<range_size>{ },
		                 DAW_FWD( ranges )... ) {}

		[[nodiscard]] constexpr iterator begin( ) noexcept {
			return iterator{ this };
		}
		[[nodiscard]] constexpr const_iterator begin( ) const noexcept {
			return const_iterator{ this };
		}

		[[nodiscard]] constexpr last_iterator end( ) noexcept {
			return last_iterator{ };
		}

		[[nodiscard]] constexpr last_const_iterator end( ) const noexcept {
			return last_const_iterator{ };
		}

		[[nodiscard]] constexpr bool operator==( concat_view const &rhs ) const {
			for( std::size_t n = 0; n < m_storage.size( ); ++n ) {
				if( m_storage[n] != rhs.m_storage[n] ) {
					return false;
				}
			}
			return true;
		}
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
						return concat_view{ std::get<Is>( r.m_ranges ).get( )... };
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
