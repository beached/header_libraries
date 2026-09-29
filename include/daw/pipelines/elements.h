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
#include "daw/daw_attributes.h"
#include "daw/daw_forward_lvalue.h"
#include "daw/daw_iterator_traits.h"
#include "daw/daw_move.h"
#include "daw/iterator/daw_arrow_proxy.h"
#include "daw/pipelines/range_base.h"

#include <type_traits>

namespace daw::pipelines::pimpl {
	template<Iterator Iterator, std::size_t Index>
	struct element_iterator {
		using iterator_category = daw::iter_category_t<Iterator>;
		// Over a range of tuple values, like zip, the element can be a reference.
		// value_type must be an object type for this to be an iterator
		using reference = std::tuple_element_t<
		  Index, daw::remove_cvref_t<daw::iter_reference_t<Iterator>>>;
		using value_type = daw::remove_cvref_t<reference>;
		using const_reference = std::tuple_element_t<
		  Index, daw::remove_cvref_t<daw::iter_const_reference_t<Iterator>>>;
		using pointer = arrow_proxy<reference>;
		using const_pointer = arrow_proxy<const_reference>;
		using difference_type = daw::iter_difference_t<Iterator>;
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
		operator[]( size_type n )
		requires( RandomIterator<Iterator> )
		{
			return raw_get( std::next( m_iter, as<difference_type>( n ) ) );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr const_reference
		operator[]( size_type n ) const
		requires( RandomIterator<Iterator> )
		{
			return raw_get( std::next( m_iter, as<difference_type>( n ) ) );
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
		requires( BidirectionalIterator<Iterator> )
		{
			--m_iter;
			return *this;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr element_iterator operator--( int )
		requires( BidirectionalIterator<Iterator> )
		{
			element_iterator result = *this;
			--m_iter;
			return result;
		}

		DAW_ATTRIB_INLINE constexpr element_iterator &
		operator+=( difference_type n )
		requires( RandomIterator<Iterator> )
		{
			m_iter += n;
			return *this;
		}

		DAW_ATTRIB_INLINE constexpr element_iterator &
		operator-=( difference_type n )
		requires( RandomIterator<Iterator> )
		{
			m_iter -= n;
			return *this;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr element_iterator
		operator+( difference_type n ) const noexcept
		requires( RandomIterator<Iterator> )
		{
			element_iterator result = *this;
			result.m_iter += n;
			return result;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr element_iterator
		operator-( difference_type n ) const noexcept
		requires( RandomIterator<Iterator> )
		{
			element_iterator result = *this;
			result.m_iter -= n;
			return result;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE friend constexpr element_iterator
		operator+( difference_type n, element_iterator const &rhs ) noexcept
		requires( RandomIterator<Iterator> )
		{
			return rhs + n;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr difference_type
		operator-( element_iterator const &rhs ) const
		requires( RandomIterator<Iterator> )
		{
			return m_iter - rhs.m_iter;
		}

		// Distance to an element iterator over another iterator type, e.g. the
		// end of a range with a sentinel.  When the underlying iterators know
		// their distance, so does this view
		template<typename OtherIterator>
		requires( not std::same_as<Iterator, OtherIterator> and
		          requires( Iterator const &l, OtherIterator const &r ) { l - r; } )
		[[nodiscard]] DAW_ATTRIB_INLINE constexpr difference_type
		operator-( element_iterator<OtherIterator, Index> const &rhs ) const {
			return static_cast<difference_type>( m_iter - rhs.base( ) );
		}

		template<typename OtherIterator>
		[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
		operator==( element_iterator<OtherIterator, Index> const &rhs ) const {
			return m_iter == rhs.base( );
		}

		template<typename OtherIterator>
		[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
		operator!=( element_iterator<OtherIterator, Index> const &rhs ) const {
			return m_iter != rhs.base( );
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
	  : private pimpl::stored_range_base_t<
	      R, //
	      pimpl::element_iterator<daw::iterator_t<R>, Index>,
	      pimpl::element_iterator<daw::iterator_end_t<R>, Index>,
	      pimpl::element_iterator<daw::const_iterator_or_t<R>, Index>,
	      pimpl::element_iterator<daw::const_iterator_end_or_t<R>, Index>> {

		using base_t = pimpl::stored_range_base_t<
		  R, //
		  pimpl::element_iterator<daw::iterator_t<R>, Index>,
		  pimpl::element_iterator<daw::iterator_end_t<R>, Index>,
		  pimpl::element_iterator<daw::const_iterator_or_t<R>, Index>,
		  pimpl::element_iterator<daw::const_iterator_end_or_t<R>, Index>>;

		using typename base_t::const_iterator_first_t;
		using typename base_t::const_iterator_last_t;
		using typename base_t::iterator_first_t;
		using typename base_t::iterator_last_t;

		using value_type = daw::iter_value_t<iterator_first_t>;

		element_view( ) = default;

		explicit constexpr element_view( daw::constructible<base_t> auto &&r )
		  : base_t( DAW_FWD( r ) ) {}

		[[nodiscard]] constexpr iterator_first_t begin( ) {
			return iterator_first_t{ base_t::rbegin( ) };
		}

		[[nodiscard]] constexpr const_iterator_first_t begin( ) const
		requires( ConstRange<R> )
		{
			return const_iterator_first_t{ base_t::rbegin( ) };
		}

		[[nodiscard]] constexpr iterator_last_t end( ) {
			return iterator_last_t{ base_t::rend( ) };
		}

		[[nodiscard]] constexpr const_iterator_last_t end( ) const
		requires( ConstRange<R> )
		{
			return const_iterator_last_t{ base_t::rend( ) };
		}

		[[nodiscard]] constexpr std::size_t size( ) const
		requires( pimpl::known_size_range<R> )
		{
			return pimpl::ranges_distance<std::size_t>( base_t::get( ) );
		}
	};

	template<std::size_t Index>
	struct element_t {
		explicit element_t( ) = default;

		template<daw::Range R>
		DAW_CPP23_STATIC_CALL_OP constexpr auto
		operator( )( R &&r ) DAW_CPP23_STATIC_CALL_OP_CONST {
			return element_view<daw::remove_rvalue_ref_t<R>, Index>{
			  DAW_FWD( r ) };
		}
	};

	template<Iterator Iterator, std::size_t... Indices>
	struct elements_iterator {
		using iterator_category = daw::iter_category_t<Iterator>;
		using value_type = std::tuple<std::tuple_element_t<
		  Indices, daw::remove_cvref_t<daw::iter_reference_t<Iterator>>>...>;
		using reference = value_type;
		using const_reference = std::tuple<std::tuple_element_t<
		  Indices, daw::remove_cvref_t<daw::iter_const_reference_t<Iterator>>>...>;
		using pointer = arrow_proxy<reference>;
		using const_pointer = arrow_proxy<const_reference>;
		using difference_type = daw::iter_difference_t<Iterator>;
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
			using result_t = std::tuple<
			  std::tuple_element_t<Indices,
			                       daw::remove_cvref_t<daw::iter_reference_t<I>>>...>;
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
		operator[]( size_type n )
		requires( RandomIterator<Iterator> )
		{
			return raw_get( std::next( m_iter, as<difference_type>( n ) ) );
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr const_reference
		operator[]( size_type n ) const
		requires( RandomIterator<Iterator> )
		{
			return raw_get( std::next( m_iter, as<difference_type>( n ) ) );
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
		requires( BidirectionalIterator<Iterator> )
		{
			--m_iter;
			return *this;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr elements_iterator
		operator--( int )
		requires( BidirectionalIterator<Iterator> )
		{
			elements_iterator result = *this;
			--m_iter;
			return result;
		}

		DAW_ATTRIB_INLINE constexpr elements_iterator &
		operator+=( difference_type n )
		requires( RandomIterator<Iterator> )
		{
			m_iter += n;
			return *this;
		}

		DAW_ATTRIB_INLINE constexpr elements_iterator &
		operator-=( difference_type n )
		requires( RandomIterator<Iterator> )
		{
			m_iter -= n;
			return *this;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr elements_iterator
		operator+( difference_type n ) const noexcept
		requires( RandomIterator<Iterator> )
		{
			elements_iterator result = *this;
			result.m_iter += n;
			return result;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr elements_iterator
		operator-( difference_type n ) const noexcept
		requires( RandomIterator<Iterator> )
		{
			elements_iterator result = *this;
			result.m_iter -= n;
			return result;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE friend constexpr elements_iterator
		operator+( difference_type n, elements_iterator const &rhs ) noexcept
		requires( RandomIterator<Iterator> )
		{
			return rhs + n;
		}

		[[nodiscard]] DAW_ATTRIB_INLINE constexpr difference_type
		operator-( elements_iterator const &rhs ) const
		requires( RandomIterator<Iterator> )
		{
			return m_iter - rhs.m_iter;
		}

		// Distance to an element iterator over another iterator type, e.g. the
		// end of a range with a sentinel.  When the underlying iterators know
		// their distance, so does this view
		template<typename OtherIterator>
		requires( not std::same_as<Iterator, OtherIterator> and
		          requires( Iterator const &l, OtherIterator const &r ) { l - r; } )
		[[nodiscard]] DAW_ATTRIB_INLINE constexpr difference_type
		operator-( elements_iterator<OtherIterator, Indices...> const &rhs ) const {
			return static_cast<difference_type>( m_iter - rhs.base( ) );
		}

		template<typename OtherIterator>
		[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool operator==(
		  elements_iterator<OtherIterator, Indices...> const &rhs ) const {
			return m_iter == rhs.base( );
		}

		template<typename OtherIterator>
		[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool operator!=(
		  elements_iterator<OtherIterator, Indices...> const &rhs ) const {
			return m_iter != rhs.base( );
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
	  : private pimpl::stored_range_base_t<
	      R, //
	      pimpl::elements_iterator<daw::iterator_t<R>, Indices...>,
	      pimpl::elements_iterator<daw::iterator_end_t<R>, Indices...>,
	      pimpl::elements_iterator<daw::const_iterator_or_t<R>, Indices...>,
	      pimpl::elements_iterator<daw::const_iterator_end_or_t<R>, Indices...>> {

		using base_t = pimpl::stored_range_base_t<
		  R, //
		  pimpl::elements_iterator<daw::iterator_t<R>, Indices...>,
		  pimpl::elements_iterator<daw::iterator_end_t<R>, Indices...>,
		  pimpl::elements_iterator<daw::const_iterator_or_t<R>, Indices...>,
		  pimpl::elements_iterator<daw::const_iterator_end_or_t<R>, Indices...>>;

		using typename base_t::const_iterator_first_t;
		using typename base_t::const_iterator_last_t;
		using typename base_t::iterator_first_t;
		using typename base_t::iterator_last_t;

		using value_type = daw::iter_value_t<iterator_first_t>;

		elements_view( ) = default;

		explicit constexpr elements_view( daw::constructible<base_t> auto &&r )
		  : base_t( DAW_FWD( r ) ) {}

		[[nodiscard]] constexpr iterator_first_t begin( ) {
			return iterator_first_t{ base_t::rbegin( ) };
		}

		[[nodiscard]] constexpr const_iterator_first_t begin( ) const
		requires( ConstRange<R> )
		{
			return const_iterator_first_t{ base_t::rbegin( ) };
		}

		[[nodiscard]] constexpr iterator_last_t end( ) {
			return iterator_last_t{ base_t::rend( ) };
		}

		[[nodiscard]] constexpr const_iterator_last_t end( ) const
		requires( ConstRange<R> )
		{
			return const_iterator_last_t{ base_t::rend( ) };
		}

		[[nodiscard]] constexpr std::size_t size( ) const
		requires( pimpl::known_size_range<R> )
		{
			return pimpl::ranges_distance<std::size_t>( base_t::get( ) );
		}
	};

	template<std::size_t... Indices>
	struct elements_t {
		explicit elements_t( ) = default;

		template<daw::Range R>
		DAW_CPP23_STATIC_CALL_OP constexpr auto
		operator( )( R &&r ) DAW_CPP23_STATIC_CALL_OP_CONST {
			return elements_view<daw::remove_rvalue_ref_t<R>, Indices...>{
			  DAW_FWD( r ) };
		}
	};

} // namespace daw::pipelines::pimpl

namespace daw::pipelines {
	template<std::size_t Index>
	inline constexpr auto Element = pimpl::element_t<Index>{ };

	template<std::size_t... Indices>
	inline constexpr auto Elements = pimpl::elements_t<Indices...>{ };
} // namespace daw::pipelines
