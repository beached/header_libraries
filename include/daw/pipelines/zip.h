// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_iterator_traits.h"
#include "daw/daw_traits.h"
#include "range.h"

#include <cstddef>
#include <tuple>
#include <utility>

namespace daw::pipelines {
	namespace pimpl {
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
	} // namespace pimpl
	template<Iterator... Iterators>
	struct zip_iterator {
		static_assert( sizeof...( Iterators ) > 0,
		               "Empty zip iterator is unsupported" );

		using iterator_category =
		  daw::common_iterator_category_t<iter_category_t<Iterators>...>;
		static_assert( not std::same_as<void, iterator_category> );
		using types_t = pimpl::tuple_pair<Iterators...>;
		static constexpr std::size_t types_size_v = sizeof...( Iterators );
		using value_type = pimpl::tuple_pair<daw::iter_value_t<Iterators>...>;
		using reference = pimpl::tuple_pair<daw::iter_reference_t<Iterators>...>;
		using const_reference =
		  pimpl::tuple_pair<daw::iter_const_reference_t<Iterators>...>;
		using difference_type = std::ptrdiff_t;
		using i_am_a_daw_zip_iterator_class = void;

	private:
		types_t m_iters;

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
		constexpr reference get_at( difference_type n, std::index_sequence<Is...> )
		  requires( RandomIteratorTag<iterator_category> ) {
			return reference{ *std::next( std::get<Is>( m_iters ), n )... };
		}

		template<std::size_t... Is>
		constexpr const_reference get_at( difference_type n,
		                                  std::index_sequence<Is...> ) const
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

	public:
		explicit zip_iterator( ) = default;

		explicit constexpr zip_iterator( Iterators const &...its )
		  : m_iters( std::move( its )... ) {}

		[[nodiscard]] constexpr types_t &base( ) {
			return m_iters;
		}

		[[nodiscard]] constexpr types_t const &base( ) const {
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

		// bidirectional iterator interface
		constexpr zip_iterator &operator--( )
		  requires( BidirectionalIteratorTag<iterator_category> ) {
			decrement( m_iters );
			return *this;
		}

		[[nodiscard]] constexpr zip_iterator operator--( int )
		  requires( BidirectionalIteratorTag<iterator_category> ) {
			auto tmp = *this;
			decrement( m_iters );
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

		constexpr zip_iterator operator+( difference_type n ) const
		  requires( RandomIteratorTag<iterator_category> ) {
			auto result = *this;
			result.advance( n, zip_indices( ) );
			return result;
		}

		constexpr zip_iterator operator-( difference_type n ) const
		  requires( RandomIteratorTag<iterator_category> ) {
			auto result = *this;
			result.advance( -n, zip_indices( ) );
			return result;
		}

		constexpr difference_type operator-( zip_iterator const &rhs ) const
		  requires( RandomIteratorTag<iterator_category> ) {
			return std::get<0>( m_iters ) - std::get<0>( rhs.m_iters );
		}

		[[nodiscard]] constexpr bool operator<( zip_iterator const &rhs )
		  requires( RandomIteratorTag<iterator_category> ) {
			return m_iters < rhs.m_iters;
		}

		[[nodiscard]] constexpr bool operator<=( zip_iterator const &rhs )
		  requires( RandomIteratorTag<iterator_category> ) {
			return m_iters <= rhs.m_iters;
		}

		[[nodiscard]] constexpr bool operator>( zip_iterator const &rhs )
		  requires( RandomIteratorTag<iterator_category> ) {
			return m_iters > rhs.m_iters;
		}

		[[nodiscard]] constexpr bool operator>=( zip_iterator const &rhs )
		  requires( RandomIteratorTag<iterator_category> ) {
			return m_iters >= rhs.m_iters;
		}
	};
	template<Iterator... Iterators>
	zip_iterator( Iterators... ) -> zip_iterator<Iterators...>;

	template<Range... Ranges>
	struct zip_view {
		using value_type = daw::iter_value_t<zip_iterator<iterator_t<Ranges>...>>;
		using iterator = zip_iterator<iterator_t<Ranges>...>;

		iterator m_first = iterator{ };
		iterator m_last = iterator{ };

		explicit zip_view( ) = default;

		template<Range... Rs>
		explicit constexpr zip_view( Rs &&...rs )
		  : m_first( ( std::begin( DAW_FWD( rs ) ) )... )
		  , m_last( ( std::end( DAW_FWD( rs ) ) )... ) {}

		[[nodiscard]] constexpr iterator begin( ) const {
			return m_first;
		}

		[[nodiscard]] constexpr iterator end( ) const {
			return m_last;
		}
	};

	template<Range... Ranges>
	zip_view( Ranges &&... ) -> zip_view<Ranges...>;

	/// Zip any number of containers and then the containers in the current
	/// pipeline.  If the Range passed is a zip_view or tuple<Ranges> it will
	/// merge them
	template<Range... Ranges>
	[[nodiscard]] constexpr auto ZipMore( Ranges &&...rs ) {
		return [=]<Range R>( R &&r ) {
			if constexpr( requires( R ) {
				              typename iterator_t<R>::i_am_a_daw_zip_iterator_class;
			              } ) {
				// zip_view
				auto tp_first = std::begin( DAW_FWD( r ) ).base( );
				auto tp_last = std::end( DAW_FWD( r ) ).base( );
				static_assert( std::tuple_size_v<decltype( tp_first )> ==
				                 std::tuple_size_v<decltype( tp_last )>,
				               "There is a bug in zip_view.  The begin( ) and end( ) "
				               "should have the same size" );
				return [&]<std::size_t... Is>( std::index_sequence<Is...> ) {
					return zip_view( rs...,
					                 range_t{ std::get<Is>( std::move( tp_first ) ),
					                          std::get<Is>( std::move( tp_last ) ) }... );
				}( std::make_index_sequence<
				         std::tuple_size_v<decltype( tp_first )>>{ } );
			} else if constexpr( daw::is_tuple_like_v<range_value_t<R>> ) {
				// tuple like.
				return [&]<std::size_t... Is>( std::index_sequence<Is...> ) {
					return zip_view( rs..., std::get<Is>( DAW_FWD( r ) )... );
				}( std::make_index_sequence<std::tuple_size_v<DAW_TYPEOF( r )>>{ } );
			} else {
				return zip_view( rs..., DAW_FWD( r ) );
			}
		};
	}
} // namespace daw::pipelines
