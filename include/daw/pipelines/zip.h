// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_iterator_traits.h"
#include "daw/daw_ref_storage.h"
#include "daw/daw_traits.h"
#include "daw/pipelines/daw_concept_checker.h"
#include "daw/pipelines/view.h"

#include <cstddef>
#include <tuple>
#include <utility>

namespace daw::pipelines {
	template<Range... Ranges>
	struct zip_view;

	namespace pimpl {
		template<std::size_t...>
		struct Swizzle_t;

		struct Concat_t;

		template<typename... Ts>
		struct tuple_pair_t {
			using type = std::tuple<Ts...>;
		};

		template<typename... Ts>
		requires( sizeof...( Ts ) == 2 ) struct tuple_pair_t<Ts...> {
			using type = std::pair<Ts...>;
		};

		template<typename... Ts>
		using tuple_pair = typename tuple_pair_t<Ts...>::type;

		template<Iterator... SentinelFors>
		struct zip_iterator_end {
			using iterator_category = std::input_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = pimpl::tuple_pair<daw::iter_value_t<SentinelFors>...>;
			using reference =
			  pimpl::tuple_pair<daw::iter_reference_t<SentinelFors>...>;
			using pointer = void;
			using i_am_a_daw_zip_iterator_end_class = void;

			zip_iterator_end( ) = default;

			constexpr bool operator==( zip_iterator_end const & ) const {
				return true;
			}

			[[noreturn]] DAW_ATTRIB_NOINLINE inline value_type operator*( ) const {
				std::terminate( );
			}

			[[noreturn]] DAW_ATTRIB_NOINLINE inline zip_iterator_end &
			operator++( ) const {
				std::terminate( );
			}

			[[noreturn]] DAW_ATTRIB_NOINLINE inline zip_iterator_end
			operator++( int ) const {
				std::terminate( );
			}
		};

		template<typename ZR, typename Last, Iterator... Iterators>
		requires( requires { typename ZR::i_am_a_daw_zip_view_class; } ) //
		  struct zip_iterator {
			static_assert( sizeof...( Iterators ) > 0,
			               "Empty zip iterator is unsupported" );

			using iterator_category =
			  daw::common_iterator_category_t<daw::iter_category_t<Iterators>...>;
			static_assert( not std::same_as<void, iterator_category> );
			using iter_types_t = pimpl::tuple_pair<Iterators...>;
			using value_type = pimpl::tuple_pair<daw::iter_value_t<Iterators>...>;
			using reference = pimpl::tuple_pair<daw::iter_reference_t<Iterators>...>;
			using const_reference =
			  pimpl::tuple_pair<daw::iter_const_reference_t<Iterators>...>;
			using difference_type = std::ptrdiff_t;
			using i_am_a_daw_zip_iterator_class = void;

			static constexpr std::size_t iter_types_size_v = sizeof...( Iterators );

		private:
			ZR *m_parent{ };
			iter_types_t m_iters{ };

			static constexpr auto zip_indices = [] {
				return std::make_index_sequence<sizeof...( Iterators )>{ };
			};

			template<std::size_t... Is>
			constexpr void increment( std::index_sequence<Is...> ) {
				(void)( ( ++std::get<Is>( m_iters ) ), ... );
			}

			template<std::size_t... Is>
			constexpr void decrement( std::index_sequence<Is...> ) {
				(void)( ( --std::get<Is>( m_iters ) ), ... );
			}

			template<std::size_t... Is>
			constexpr void advance( difference_type n, std::index_sequence<Is...> ) {
				(void)( ( std::advance( std::get<Is>( m_iters ), n ), ... ) );
			}

			template<std::size_t... Is>
			[[nodiscard]] constexpr reference get_at( difference_type n,
			                                          std::index_sequence<Is...> )
			  requires( RandomIteratorTag<iterator_category> ) {
				return reference{ *std::next( std::get<Is>( m_iters ), n )... };
			}

			template<std::size_t... Is>
			[[nodiscard]] constexpr const_reference
			get_at( difference_type n, std::index_sequence<Is...> ) const
			  requires( RandomIteratorTag<iterator_category> ) {
				return const_reference{ *std::next( std::get<Is>( m_iters ), n )... };
			}

			template<size_t... Is>
			[[nodiscard]] DAW_ATTRIB_FLATINLINE constexpr reference
			get_items( std::index_sequence<Is...> ) noexcept {
				return { *std::get<Is>( m_iters )... };
			}

			template<size_t... Is>
			[[nodiscard]] DAW_ATTRIB_FLATINLINE constexpr const_reference
			get_items( std::index_sequence<Is...> ) const noexcept {
				return { *std::get<Is>( m_iters )... };
			}
			template<std::size_t... Is>
			explicit constexpr zip_iterator( ZR *zr, std::index_sequence<Is...> )
			  : m_parent( zr )
			  , m_iters{ std::begin( std::get<Is>( zr->m_ranges ).get( ) )... } {}

		public:
			zip_iterator( ) = default;

			explicit zip_iterator( Last const & ) {}

			explicit constexpr zip_iterator( ZR *zr )
			  : zip_iterator( zr,
			                  std::make_index_sequence<sizeof...( Iterators )>{ } ) {}

			[[nodiscard]] constexpr iter_types_t &base( ) {
				return m_iters;
			}

			[[nodiscard]] constexpr bool good( ) const {
				daw_ensure( m_parent );
				return [&]<std::size_t... Is>( std::index_sequence<Is...> ) {
					return ( ( std::get<Is>( m_iters ) !=
					           std::end( std::get<Is>( m_parent->m_ranges ).get( ) ) ) and
					         ... );
				}( std::make_index_sequence<sizeof...( Iterators )>{ } );
			}

			[[nodiscard]] constexpr iter_types_t const &base( ) const {
				return m_iters;
			}

			constexpr zip_iterator &operator++( ) {
				increment( zip_indices( ) );
				return *this;
			}

			[[nodiscard]] constexpr zip_iterator operator++( int ) {
				auto tmp = *this;
				increment( zip_indices( ) );
				return tmp;
			}

			[[nodiscard]] constexpr reference operator*( ) noexcept {
				return get_items( zip_indices( ) );
			}

			[[nodiscard]] constexpr const_reference operator*( ) const noexcept {
				return get_items( zip_indices( ) );
			}

			[[nodiscard]] constexpr bool
			operator==( zip_iterator const &rhs ) const noexcept {
				return m_iters == rhs.m_iters;
			}

			[[nodiscard]] constexpr bool
			operator!=( zip_iterator const &rhs ) const noexcept {
				// we don't know if they are the same length.  Return false if any are
				// equal
				return [&]<std::size_t... Is>( std::index_sequence<Is...> ) -> bool {
					using std::get;
					auto result =
					  ( ( get<Is>( m_iters ) == get<Is>( rhs.m_iters ) ) or ... );
					return not result;
				}( std::make_index_sequence<sizeof...( Iterators )>{ } );
			}

			// clang-format off
			[[nodiscard]] constexpr auto operator<=>( zip_iterator const &rhs )
			                                            const noexcept {
				return m_iters <=> rhs.m_iters;
			}
			// clang-format on

			[[nodiscard]] constexpr friend bool operator==( zip_iterator const &lhs,
			                                                Last const & ) {
				return not lhs.good( );
			}

			[[nodiscard]] constexpr friend bool operator!=( zip_iterator const &lhs,
			                                                Last const & ) {
				return lhs.good( );
			}

			[[nodiscard]] constexpr friend bool
			operator==( Last const &, zip_iterator const &rhs ) {
				return not rhs.good( );
			}

			[[nodiscard]] constexpr friend bool
			operator!=( Last const &, zip_iterator const &rhs ) {
				return rhs.good( );
			}

			// bidirectional iterator interface
			constexpr zip_iterator &operator--( )
			  requires( BidirectionalIteratorTag<iterator_category> ) {
				decrement( zip_indices( ) );
				return *this;
			}

			[[nodiscard]] constexpr zip_iterator operator--( int )
			  requires( BidirectionalIteratorTag<iterator_category> ) {
				auto tmp = *this;
				decrement( zip_indices( ) );
				return tmp;
			}

			// random access iterator interface
			[[nodiscard]] constexpr reference operator[]( difference_type n )
			  requires( RandomIteratorTag<iterator_category> ) {
				return get_at( n, zip_indices( ) );
			}

			[[nodiscard]] constexpr reference operator[]( difference_type n ) const
			  requires( RandomIteratorTag<iterator_category> ) {
				return get_at( n, zip_indices( ) );
			}

			constexpr zip_iterator &operator+=( difference_type n )
			  requires( RandomIteratorTag<iterator_category> ) {
				advance( n, zip_indices( ) );
				return *this;
			}

			constexpr zip_iterator &operator-=( difference_type n )
			  requires( RandomIteratorTag<iterator_category> ) {
				advance( -n, zip_indices( ) );
				return *this;
			}

			friend constexpr zip_iterator operator+( zip_iterator lhs,
			                                         difference_type n )
			  requires( RandomIteratorTag<iterator_category> ) {
				lhs += n;
				return lhs;
			}

			friend constexpr zip_iterator operator+( difference_type n,
			                                         zip_iterator rhs )
			  requires( RandomIteratorTag<iterator_category> ) {
				rhs += n;
				return rhs;
			}

			friend constexpr zip_iterator operator-( zip_iterator lhs,
			                                         difference_type n )
			  requires( RandomIteratorTag<iterator_category> ) {
				lhs -= n;
				return lhs;
			}

			friend constexpr zip_iterator operator-( difference_type n,
			                                         zip_iterator rhs )
			  requires( RandomIteratorTag<iterator_category> ) {
				rhs -= n;
				return rhs;
			}

			constexpr difference_type operator-( zip_iterator const &rhs ) const
			  requires( RandomIteratorTag<iterator_category> ) {
				return std::get<0>( m_iters ) - std::get<0>( rhs.m_iters );
			}
		};
	} // namespace pimpl

	template<Range... Ranges>
	struct zip_view {
		using i_am_a_daw_zip_view_class = void;
		using ranges_t = std::tuple<daw::ref_storage<Ranges>...>;
		using end_value_t = pimpl::tuple_pair<daw::range_value_t<Ranges>...>;
		using last_iterator =
		  pimpl::zip_iterator_end<daw::iterator_end_t<Ranges>...>;
		using const_last_iterator =
		  pimpl::zip_iterator_end<daw::const_iterator_end_t<Ranges>...>;
		using iterator =
		  pimpl::zip_iterator<zip_view, last_iterator, daw::iterator_t<Ranges>...>;
		using const_iterator =
		  pimpl::zip_iterator<zip_view const, const_last_iterator,
		                      daw::const_iterator_t<Ranges>...>;

		static constexpr std::size_t range_count = sizeof...( Ranges );

		template<typename ZR, typename, Iterator...>
		requires( requires { typename ZR::i_am_a_daw_zip_view_class; } ) //
		  friend struct pimpl::zip_iterator;

	private:
		ranges_t m_ranges{ };
		friend struct pimpl::Concat_t;

	public:
		explicit zip_view( ) = default;

		explicit constexpr zip_view( Ranges... rs )
		  : m_ranges{ DAW_FWD( rs )... } {}

		[[nodiscard]] constexpr iterator begin( ) {
			return iterator{ this };
		}

		[[nodiscard]] constexpr const_iterator begin( ) const {
			return const_iterator{ this };
		}

		[[nodiscard]] constexpr last_iterator end( ) {
			return last_iterator{ };
		}

		[[nodiscard]] constexpr const_last_iterator end( ) const {
			return const_last_iterator{ };
		}

		template<Range... NewRanges>
		[[nodiscard]] constexpr auto append_ranges( NewRanges &&...nranges ) const {
			return std::apply(
			  [&]<typename... CRs>( CRs const &...current_ranges ) {
				  return zip_view<typename CRs::type..., NewRanges...>{
				    current_ranges.get( )..., DAW_FWD( nranges )... };
			  },
			  m_ranges );
		}

		template<Range...>
		friend struct zip_view;

		template<Range... NewRanges>
		[[nodiscard]] constexpr auto
		append_zip_view( zip_view<NewRanges...> const &zv ) const {
			return std::apply(
			  [&]( auto const &...current_ranges ) {
				  return std::apply(
				    [&]<typename... NRs>( NRs const &...next_ranges ) {
					    return zip_view<Ranges..., NewRanges...>{
					      current_ranges.get( )..., next_ranges.get( )... };
				    },
				    zv.m_ranges );
			  },
			  m_ranges );
		}

		template<std::size_t... Indices>
		constexpr auto swizzle( ) && {
			static_assert( std::max( { Indices... } ) < range_count,
			               "Swizzle - Index that is beyond the number "
			               "of zipped ranges" );

			return zip_view{ std::move( std::get<Indices>( m_ranges ) ).get( )... };
		}

		template<std::size_t... Indices>
		constexpr auto swizzle( ) const & {
			static_assert( std::max( { Indices... } ) < range_count,
			               "Swizzle - Index that is beyond the number "
			               "of zipped ranges" );

			return zip_view{ std::get<Indices>( m_ranges ).get( )... };
		}

		template<std::size_t... Indices>
		constexpr auto swizzle( ) & {
			return as_const( *this ).template swizzle<Indices...>( );
		}

		template<std::size_t... Indices>
		constexpr auto swizzle( ) const && {
			return as_const( *this ).template swizzle<Indices...>( );
		}
	};

	template<Range... Ranges>
	zip_view( Ranges &&... ) -> zip_view<Ranges...>;

	struct Zip_t {
		explicit Zip_t( ) = default;
		[[nodiscard]] DAW_CPP23_STATIC_CALL_OP constexpr auto
		operator( )( ) DAW_CPP23_STATIC_CALL_OP_CONST {
			return []( Range auto &&r, Range auto &&...ranges ) {
				return zip_view( DAW_FWD( r ), DAW_FWD( ranges )... );
			};
		}

		[[nodiscard]] DAW_CPP23_STATIC_CALL_OP constexpr auto
		operator( )( Range auto &&r,
		             Range auto &&...ranges ) DAW_CPP23_STATIC_CALL_OP_CONST {
			return zip_view( DAW_FWD( r ), DAW_FWD( ranges )... );
		}
	};

	inline constexpr auto Zip = Zip_t( );

	/// Zip any number of containers and then the containers in the current
	/// pipeline.  If the Range passed is a zip_view or tuple<Ranges> it will
	/// merge them
	template<Range... Ranges>
	[[nodiscard]] constexpr auto ZipMore( Ranges &&...rs ) {
		return [v = zip_view{ DAW_FWD( rs )... }]<typename R>( R &&r ) {
			if constexpr( requires {
				              typename daw::remove_cvref_t<
				                R>::i_am_a_daw_zip_view_class;
			              } ) {
				return v.append_zip_view( DAW_FWD( r ) );
			} else if constexpr( daw::is_tuple_like_v<R> ) {
				return std::apply(
				  [&]( auto &&...next_ranges ) {
					  return v.append_ranges( DAW_FWD( next_ranges )... );
				  },
				  DAW_FWD( r ) );
			} else {
				return v.append_ranges( DAW_FWD( r ) );
			}
		};
	}
} // namespace daw::pipelines
