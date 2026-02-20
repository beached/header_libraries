// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/cpp_17.h"
#include "daw/daw_iterator_traits.h"
#include "daw/daw_move.h"
#include "daw/iterator/daw_arrow_proxy.h"

#include <type_traits>

namespace daw::pipelines::pimpl {
	template<Iterator Iterator, std::size_t Index>
	struct element_iterator {
		using iterator_category = daw::iter_category_t<Iterator>;
		using value_type =
		  std::tuple_element_t<Index, daw::iter_reference_t<Iterator>>;
		using reference = value_type;
		using const_reference =
		  std::tuple_element_t<Index, daw::iter_const_reference_t<Iterator>>;
		using pointer = arrow_proxy<reference>;
		using const_pointer = arrow_proxy<const_reference>;
		using difference_type = std::ptrdiff_t;
		using size_type = std::size_t;

	private:
		Iterator m_iter{ };

	public:
		explicit constexpr element_iterator( ) = default;
		explicit constexpr element_iterator( Iterator it )
		  : m_iter( std::move( it ) ) {}

	private:
		template<typename I>
		[[nodiscard]] DAW_ATTRIB_INLINE static constexpr decltype( auto )
		raw_get( I &&iter ) {
			return std::get<Index>( *DAW_FWD( iter ) );
		}

	public:
		[[nodiscard]] constexpr auto &base( ) {
			return m_iter;
		}

		[[nodiscard]] constexpr auto const &base( ) const {
			return m_iter;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr reference
		operator[]( size_type n ) requires( RandomIterator<Iterator> ) {
			return raw_get( std::next( m_iter, static_cast<difference_type>( n ) ) );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr const_reference
		operator[]( size_type n ) const requires( RandomIterator<Iterator> ) {
			return raw_get( std::next( m_iter, static_cast<difference_type>( n ) ) );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr reference operator*( ) {
			return raw_get( m_iter );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr const_reference
		operator*( ) const {
			return raw_get( m_iter );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr pointer operator->( ) {
			return pointer( raw_get( m_iter ) );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr const_pointer
		operator->( ) const {
			return const_pointer( raw_get( m_iter ) );
		}

		DAW_ATTRIB_INLINE constexpr element_iterator &operator++( ) {
			++m_iter;
			return *this;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr element_iterator
		operator++( int ) {
			element_iterator result = *this;
			++m_iter;
			return result;
		}

		DAW_ATTRIB_INLINE constexpr element_iterator &operator--( )
		  requires( BidirectionalIterator<Iterator> ) {
			--m_iter;
			return *this;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr element_iterator operator--( int )
		  requires( BidirectionalIterator<Iterator> ) {
			element_iterator result = *this;
			--m_iter;
			return result;
		}

		DAW_ATTRIB_INLINE constexpr element_iterator &
		operator+=( difference_type n ) requires( RandomIterator<Iterator> ) {
			m_iter += n;
			return *this;
		}

		DAW_ATTRIB_INLINE constexpr element_iterator &
		operator-=( difference_type n ) requires( RandomIterator<Iterator> ) {
			m_iter -= n;
			return *this;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr element_iterator
		operator+( difference_type n ) const noexcept
		  requires( RandomIterator<Iterator> ) {
			element_iterator result = *this;
			m_iter += n;
			return result;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr element_iterator
		operator-( difference_type n ) const noexcept
		  requires( RandomIterator<Iterator> ) {
			element_iterator result = *this;
			m_iter -= n;
			return result;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr difference_type
		operator-( element_iterator const &rhs )
		  requires( RandomIterator<Iterator> ) {
			return m_iter - rhs.m_iter;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr friend bool
		operator==( element_iterator const &lhs, element_iterator const &rhs ) {
			return lhs.m_iter == rhs.m_iter;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr friend bool
		operator!=( element_iterator const &lhs, element_iterator const &rhs ) {
			return lhs.m_iter != rhs.m_iter;
		}

		// clang-format off
		[[nodiscard]] DAW_ATTRIB_INLINE constexpr friend auto
		operator<=>( element_iterator const &lhs, element_iterator const &rhs )
		  requires( RandomIterator<Iterator> ) {
			return lhs.m_iter <=> rhs.m_iter;
		}
		// clang-format on
	};

	template<Range R, std::size_t Index>
	struct element_view
	  : range_base_t<
	      element_iterator<daw::iterator_t<std::remove_reference_t<R>>, Index>,
	      element_iterator<daw::iterator_end_t<std::remove_reference_t<R>>,
	                       Index>> {
		using range_t = std::remove_reference_t<R>;
		using daw_range_base_t =
		  range_base_t<element_iterator<daw::iterator_t<range_t>, Index>,
		               element_iterator<daw::iterator_end_t<range_t>, Index>>;

		using typename daw_range_base_t::iterator_first_t;
		using typename daw_range_base_t::iterator_last_t;

		using value_type = daw::iter_value_t<iterator_first_t>;

		iterator_first_t m_first = iterator_first_t{ };
		iterator_last_t m_last = iterator_last_t{ };

		explicit element_view( ) = default;

		template<daw::Range U>
		explicit constexpr element_view( U &&r )
		  : m_first( std::begin( daw::forward_lvalue<U>( r ) ) )
		  , m_last( std::end( daw::forward_lvalue<U>( r ) ) ) {}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr iterator_first_t begin( ) const {
			return m_first;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr iterator_last_t end( ) const {
			return m_last;
		}
	};

	template<std::size_t Index>
	struct element_t {
		explicit element_t( ) = default;

		template<daw::Range R>
		DAW_CPP23_STATIC_CALL_OP constexpr auto
		operator( )( R &&r ) DAW_CPP23_STATIC_CALL_OP_CONST {
			return element_view<daw::remove_rvalue_ref_t<R>, Index>{
			  daw::forward_lvalue<R>( r ) };
		}
	};

	template<Iterator Iterator, std::size_t... Indices>
	struct elements_iterator {
		using iterator_category = daw::iter_category_t<Iterator>;
		using value_type = std::tuple<
		  std::tuple_element_t<Indices, daw::iter_reference_t<Iterator>>...>;
		using reference = value_type;
		using const_reference = std::tuple<
		  std::tuple_element_t<Indices, daw::iter_const_reference_t<Iterator>>...>;
		using pointer = arrow_proxy<reference>;
		using const_pointer = arrow_proxy<const_reference>;
		using difference_type = std::ptrdiff_t;
		using size_type = std::size_t;

	private:
		Iterator m_iter{ };

	public:
		explicit constexpr elements_iterator( ) = default;
		explicit constexpr elements_iterator( Iterator it )
		  : m_iter( std::move( it ) ) {}

	private:
		template<typename I>
		[[nodiscard]] DAW_ATTRIB_INLINE static constexpr auto raw_get( I &&iter ) {
			using result_t =
			  std::tuple<std::tuple_element_t<Indices, daw::iter_reference_t<I>>...>;
			auto &&r = *DAW_FWD( iter );
			return result_t{ std::get<Indices>( r )... };
		}

	public:
		[[nodiscard]] constexpr auto &base( ) {
			return m_iter;
		}

		[[nodiscard]] constexpr auto const &base( ) const {
			return m_iter;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr reference
		operator[]( size_type n ) requires( RandomIterator<Iterator> ) {
			return raw_get( std::next( m_iter, static_cast<difference_type>( n ) ) );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr const_reference
		operator[]( size_type n ) const requires( RandomIterator<Iterator> ) {
			return raw_get( std::next( m_iter, static_cast<difference_type>( n ) ) );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr reference operator*( ) {
			return raw_get( m_iter );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr const_reference
		operator*( ) const {
			return raw_get( m_iter );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr pointer operator->( ) {
			return pointer( raw_get( m_iter ) );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr const_pointer
		operator->( ) const {
			return const_pointer( raw_get( m_iter ) );
		}

		DAW_ATTRIB_INLINE constexpr elements_iterator &operator++( ) {
			++m_iter;
			return *this;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr elements_iterator
		operator++( int ) {
			elements_iterator result = *this;
			++m_iter;
			return result;
		}

		DAW_ATTRIB_INLINE constexpr elements_iterator &operator--( )
		  requires( BidirectionalIterator<Iterator> ) {
			--m_iter;
			return *this;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr elements_iterator
		operator--( int ) requires( BidirectionalIterator<Iterator> ) {
			elements_iterator result = *this;
			--m_iter;
			return result;
		}

		DAW_ATTRIB_INLINE constexpr elements_iterator &
		operator+=( difference_type n ) requires( RandomIterator<Iterator> ) {
			m_iter += n;
			return *this;
		}

		DAW_ATTRIB_INLINE constexpr elements_iterator &
		operator-=( difference_type n ) requires( RandomIterator<Iterator> ) {
			m_iter -= n;
			return *this;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr elements_iterator
		operator+( difference_type n ) const noexcept
		  requires( RandomIterator<Iterator> ) {
			elements_iterator result = *this;
			m_iter += n;
			return result;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr elements_iterator
		operator-( difference_type n ) const noexcept
		  requires( RandomIterator<Iterator> ) {
			elements_iterator result = *this;
			m_iter -= n;
			return result;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr difference_type
		operator-( elements_iterator const &rhs )
		  requires( RandomIterator<Iterator> ) {
			return m_iter - rhs.m_iter;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr friend bool
		operator==( elements_iterator const &lhs, elements_iterator const &rhs ) {
			return lhs.m_iter == rhs.m_iter;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr friend bool
		operator!=( elements_iterator const &lhs, elements_iterator const &rhs ) {
			return lhs.m_iter != rhs.m_iter;
		}

		// clang-format off
		[[nodiscard]] DAW_ATTRIB_INLINE constexpr friend auto
		operator<=>( elements_iterator const &lhs, elements_iterator const &rhs )
		  requires( RandomIterator<Iterator> ) {
			return lhs.m_iter <=> rhs.m_iter;
		}
		// clang-format on
	};

	template<Range R, std::size_t... Indices>
	struct elements_view
	  : range_base_t<
	      elements_iterator<daw::iterator_t<std::remove_reference_t<R>>,
	                        Indices...>,
	      elements_iterator<daw::iterator_end_t<std::remove_reference_t<R>>,
	                        Indices...>> {
		using range_t = std::remove_reference_t<R>;
		using daw_range_base_t =
		  range_base_t<elements_iterator<daw::iterator_t<range_t>, Indices...>,
		               elements_iterator<daw::iterator_end_t<range_t>, Indices...>>;

		using typename daw_range_base_t::iterator_first_t;
		using typename daw_range_base_t::iterator_last_t;

		using value_type = daw::iter_value_t<iterator_first_t>;

		iterator_first_t m_first = iterator_first_t{ };
		iterator_last_t m_last = iterator_last_t{ };

		explicit elements_view( ) = default;

		template<daw::Range U>
		explicit constexpr elements_view( U &&r )
		  : m_first( std::begin( daw::forward_lvalue<U>( r ) ) )
		  , m_last( std::end( daw::forward_lvalue<U>( r ) ) ) {}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr iterator_first_t begin( ) const {
			return m_first;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr iterator_last_t end( ) const {
			return m_last;
		}
	};

	template<std::size_t... Indices>
	struct elements_t {
		explicit elements_t( ) = default;

		template<daw::Range R>
		DAW_CPP23_STATIC_CALL_OP constexpr auto
		operator( )( R &&r ) DAW_CPP23_STATIC_CALL_OP_CONST {
			return elements_view<daw::remove_rvalue_ref_t<R>, Indices...>{
			  daw::forward_lvalue<R>( r ) };
		}
	};

} // namespace daw::pipelines::pimpl

namespace daw::pipelines {
	template<std::size_t Index>
	inline constexpr auto Element = pimpl::element_t<Index>{ };

	template<std::size_t... Indices>
	inline constexpr auto Elements = pimpl::elements_t<Indices...>{ };
} // namespace daw::pipelines
