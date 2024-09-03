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
#include "daw_algorithm.h"
#include "daw_math.h"
#include "daw_traits.h"
#include "impl/daw_make_trait.h"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <numeric>
#include <type_traits>

namespace daw::container {
	namespace impl {
		DAW_MAKE_REQ_TRAIT( has_size_method_v, std::declval<T const &>( ).size( ) );

		namespace adl_test {}
		template<typename Container>
		constexpr bool is_nothrow_begin( ) noexcept {
			using std::begin;
			return noexcept( begin( std::declval<Container>( ) ) );
		}

		template<typename Container>
		constexpr decltype( auto )
		adl_begin( Container &&c ) noexcept( is_nothrow_begin<Container>( ) ) {
			using std::begin;
			return begin( c );
		}

		template<typename Container>
		constexpr bool is_nothrow_end( ) noexcept {
			using std::end;
			return noexcept( end( std::declval<Container>( ) ) );
		}

		template<typename Container>
		constexpr decltype( auto )
		adl_end( Container &&c ) noexcept( is_nothrow_end<Container>( ) ) {
			using std::end;
			return end( c );
		}

		template<typename Container>
		constexpr bool is_nothrow_sortable( ) noexcept {
			return noexcept( std::sort( adl_begin( std::declval<Container>( ) ),
			                            adl_end( std::declval<Container>( ) ) ) );
		}

		template<typename Container, typename Predicate>
		constexpr bool is_nothrow_sortable( ) noexcept {
			return noexcept( std::sort( adl_begin( std::declval<Container>( ) ),
			                            adl_end( std::declval<Container>( ) ),
			                            std::declval<Predicate>( ) ) );
		}

	} // namespace impl

	template<typename T, size_t N>
	constexpr size_t container_size( T const ( & )[N] ) noexcept {
		return N;
	}

	template<typename Container,
	         std::enable_if_t<impl::has_size_method_v<Container>,
	                          std::nullptr_t> = nullptr>
	constexpr size_t container_size( Container const &c ) noexcept {
		return static_cast<size_t>( c.size( ) );
	}

	template<typename Sortable>
	void sort( Sortable &container ) noexcept(
	  impl::is_nothrow_sortable<Sortable>( ) ) {

		static_assert( traits::is_sortable_container_v<Sortable> );
		using std::begin;
		using std::end;
		std::sort( begin( container ), end( container ) );
	}

	template<typename Sortable, typename Compare>
	void sort( Sortable &container, Compare &&compare ) noexcept(
	  impl::is_nothrow_sortable<Sortable, Compare>( ) ) {
		using std::begin;
		using std::end;
		using value_type = DAW_TYPEOF( *begin( container ) );

		static_assert( traits::is_sortable_container_v<Sortable> );
		static_assert(
		  traits::is_binary_predicate_v<Compare, value_type, value_type> );

		std::sort( begin( container ), end( container ), DAW_FWD( compare ) );
	}

	template<typename Container,
	         std::enable_if_t<daw::traits::is_container_like_v<Container>,
	                          std::nullptr_t> = nullptr>
	void stable_sort( Container &container ) noexcept( noexcept(
	  std::stable_sort( std::begin( container ), std::end( container ) ) ) ) {

		std::stable_sort( std::begin( container ), std::end( container ) );
	}

	template<typename Container,
	         typename Compare,
	         std::enable_if_t<daw::traits::is_container_like_v<Container>,
	                          std::nullptr_t> = nullptr>
	void stable_sort( Container &container, Compare compare ) noexcept(
	  noexcept( std::stable_sort( std::begin( container ),
	                              std::end( container ),
	                              compare ) ) ) {

		using std::begin;
		using std::end;
		using value_type = DAW_TYPEOF( *begin( container ) );

		static_assert(
		  traits::is_binary_predicate_v<Compare, value_type, value_type> );

		std::stable_sort( begin( container ), end( container ), compare );
	}

	template<typename Container,
	         typename Value,
	         std::enable_if_t<daw::traits::is_container_like_v<Container>,
	                          std::nullptr_t> = nullptr>
	decltype( auto )
	find( Container &container, Value const &value ) noexcept( noexcept(
	  std::find( std::begin( container ), std::end( container ), value ) ) ) {

		return std::find( std::begin( container ), std::end( container ), value );
	}

	template<typename Container,
	         typename Value,
	         std::enable_if_t<daw::traits::is_container_like_v<Container>,
	                          std::nullptr_t> = nullptr>
	decltype( auto )
	find( Container const &container, Value const &value ) noexcept( noexcept(
	  std::find( std::begin( container ), std::end( container ), value ) ) ) {

		return std::find( std::begin( container ), std::end( container ), value );
	}

	template<typename Container,
	         typename UnaryPredicate,
	         std::enable_if_t<daw::traits::is_container_like_v<Container>,
	                          std::nullptr_t> = nullptr>
	decltype( auto )
	find_if( Container &container, UnaryPredicate pred ) noexcept( noexcept(
	  std::find_if( std::begin( container ), std::end( container ), pred ) ) ) {

		static_assert(
		  traits::is_unary_predicate_v<UnaryPredicate,
		                               decltype( *std::begin( container ) )>,
		  "Compare does not satisfy the Unary Predicate concept.  See "
		  "http://en.cppreference.com/w/cpp/concept/Predicate for more "
		  "information" );

		return std::find_if( std::begin( container ), std::end( container ), pred );
	}

	template<typename Container,
	         typename Value,
	         std::enable_if_t<daw::traits::is_container_like_v<Container>,
	                          std::nullptr_t> = nullptr>
	decltype( auto )
	erase_remove( Container &container, Value const &value ) noexcept(
	  noexcept( container.erase(
	    std::remove( std::begin( container ), std::end( container ), value ),
	    std::end( container ) ) ) ) {

		return container.erase(
		  std::remove( std::begin( container ), std::end( container ), value ),
		  std::end( container ) );
	}

	template<typename Container,
	         typename UnaryPredicate,
	         std::enable_if_t<daw::traits::is_container_like_v<Container>,
	                          std::nullptr_t> = nullptr>
	decltype( auto )
	erase_remove_if( Container &container, UnaryPredicate pred ) noexcept(
	  noexcept( container.erase(
	    std::remove_if( std::begin( container ), std::end( container ), pred ),
	    std::end( container ) ) ) ) {

		static_assert(
		  traits::is_unary_predicate_v<UnaryPredicate,
		                               decltype( *std::begin( container ) )>,
		  "Compare does not satisfy the Unary Predicate concept.  See "
		  "http://en.cppreference.com/w/cpp/concept/Predicate for more "
		  "information" );

		return container.erase(
		  std::remove_if( std::begin( container ), std::end( container ), pred ),
		  std::end( container ) );
	}

	template<typename Container,
	         typename UnaryPredicate,
	         std::enable_if_t<daw::traits::is_container_like_v<Container>,
	                          std::nullptr_t> = nullptr>
	decltype( auto )
	partition( Container &container, UnaryPredicate pred ) noexcept( noexcept(
	  std::partition( std::begin( container ), std::end( container ), pred ) ) ) {

		static_assert(
		  traits::is_unary_predicate_v<UnaryPredicate,
		                               decltype( *std::begin( container ) )>,
		  "Compare does not satisfy the Unary Predicate concept.  See "
		  "http://en.cppreference.com/w/cpp/concept/Predicate for more "
		  "information" );

		return std::partition(
		  std::begin( container ), std::end( container ), pred );
	}

	template<typename Container,
	         typename UnaryPredicate,
	         std::enable_if_t<daw::traits::is_container_like_v<Container>,
	                          std::nullptr_t> = nullptr>
	decltype( auto )
	stable_partition( Container &container, UnaryPredicate pred ) noexcept(
	  noexcept( std::stable_partition( std::begin( container ),
	                                   std::end( container ),
	                                   pred ) ) ) {

		static_assert(
		  traits::is_unary_predicate_v<UnaryPredicate,
		                               decltype( *std::begin( container ) )>,
		  "Compare does not satisfy the Unary Predicate concept.  See "
		  "http://en.cppreference.com/w/cpp/concept/Predicate for more "
		  "information" );

		return std::stable_partition(
		  std::begin( container ), std::end( container ), pred );
	}

	template<typename Container,
	         typename T,
	         std::enable_if_t<daw::traits::is_container_like_v<Container>,
	                          std::nullptr_t> = nullptr>
	decltype( auto ) accumulate( Container const &container, T &&init ) noexcept(
	  noexcept( std::accumulate( std::cbegin( container ),
	                             std::end( container ),
	                             DAW_FWD( init ) ) ) ) {

		return std::accumulate(
		  std::cbegin( container ), std::cend( container ), DAW_FWD( init ) );
	}

	template<typename Container,
	         typename T,
	         typename BinaryOperation,
	         std::enable_if_t<daw::traits::is_container_like_v<Container>,
	                          std::nullptr_t> = nullptr>
	decltype( auto ) accumulate(
	  Container const &container,
	  T &&init,
	  BinaryOperation
	    oper ) noexcept( noexcept( std::accumulate( std::cbegin( container ),
	                                                std::cend( container ),
	                                                DAW_FWD( init ),
	                                                oper ) ) ) {

		static_assert( std::is_invocable_v<BinaryOperation,
		                                   T,
		                                   decltype( *std::cbegin( container ) )>,
		               "Invalid BinaryOperation" );

		return std::accumulate(
		  std::cbegin( container ), std::cend( container ), DAW_FWD( init ), oper );
	}

	template<typename Container,
	         typename UnaryOperator,
	         std::enable_if_t<daw::traits::is_container_like_v<Container>,
	                          std::nullptr_t> = nullptr>
	decltype( auto )
	transform( Container &container, UnaryOperator unary_operator ) noexcept(
	  noexcept( std::transform( std::cbegin( container ),
	                            std::cend( container ),
	                            std::begin( container ),
	                            unary_operator ) ) ) {

		static_assert(
		  std::is_invocable_v<UnaryOperator, decltype( *std::cbegin( container ) )>,
		  "UnaryOperator is not callable with the values stored in Container" );

		return std::transform( std::cbegin( container ),
		                       std::cend( container ),
		                       std::begin( container ),
		                       unary_operator );
	}

	template<typename Container,
	         typename OutputIterator,
	         typename UnaryOperator,
	         std::enable_if_t<daw::traits::is_container_like_v<Container>,
	                          std::nullptr_t> = nullptr>
	decltype( auto ) transform(
	  Container const &container,
	  OutputIterator &first_out,
	  UnaryOperator
	    unary_operator ) noexcept( noexcept( std::
	                                           transform(
	                                             std::cbegin( container ),
	                                             std::cend( container ),
	                                             first_out,
	                                             unary_operator ) ) ) {

		static_assert(
		  std::is_invocable_v<UnaryOperator, decltype( *std::cbegin( container ) )>,
		  "UnaryOperator is not callable with the values stored in Container" );

		static_assert( std::is_convertible_v<decltype( unary_operator(
		                                       *std::cbegin( container ) ) ),
		                                     decltype( *first_out )>,
		               "Output of UnaryOperator cannot be assigned to *first_out" );

		return std::transform( std::cbegin( container ),
		                       std::cend( container ),
		                       first_out,
		                       unary_operator );
	}

	template<typename Container,
	         typename OutputIterator,
	         typename UnaryOperator,
	         std::enable_if_t<daw::traits::is_container_like_v<Container>,
	                          std::nullptr_t> = nullptr>
	decltype( auto ) transform(
	  Container const &container,
	  OutputIterator &&first_out,
	  UnaryOperator
	    unary_operator ) noexcept( noexcept( std::
	                                           transform(
	                                             std::cbegin( container ),
	                                             std::cend( container ),
	                                             first_out,
	                                             unary_operator ) ) ) {

		static_assert(
		  std::is_invocable_v<UnaryOperator, decltype( *std::cbegin( container ) )>,
		  "UnaryOperator is not callable with the values stored in Container" );

		std::transform( std::cbegin( container ),
		                std::cend( container ),
		                first_out,
		                unary_operator );
	}

	template<typename Container,
	         typename UnaryOperator,
	         std::enable_if_t<daw::traits::is_container_like_v<Container>,
	                          std::nullptr_t> = nullptr>
	constexpr decltype( auto ) max_element( Container &container ) noexcept(
	  noexcept( daw::algorithm::max_element( std::cbegin( container ),
	                                         std::cend( container ) ) ) ) {

		return daw::algorithm::max_element( std::cbegin( container ),
		                                    std::cend( container ) );
	}

	template<typename Container,
	         typename OutputIterator,
	         typename UnaryOperator,
	         std::enable_if_t<daw::traits::is_container_like_v<Container>,
	                          std::nullptr_t> = nullptr>
	constexpr decltype( auto ) max_element( Container const &container ) noexcept(
	  noexcept( daw::algorithm::max_element( std::cbegin( container ),
	                                         std::cend( container ) ) ) ) {

		return daw::algorithm::max_element( std::cbegin( container ),
		                                    std::cend( container ) );
	}

	template<typename Container,
	         typename Compare,
	         std::enable_if_t<daw::traits::is_container_like_v<Container>,
	                          std::nullptr_t> = nullptr>
	constexpr decltype( auto )
	max_element( Container &container, Compare compare ) noexcept(
	  noexcept( daw::algorithm::max_element( std::begin( container ),
	                                         std::end( container ),
	                                         compare ) ) ) {
		using std::begin;
		using std::end;
		using value_type = DAW_TYPEOF( *begin( container ) );

		static_assert(
		  traits::is_binary_predicate_v<Compare, value_type, value_type> );

		return daw::algorithm::max_element(
		  begin( container ), end( container ), compare );
	}

	template<typename Container,
	         typename Compare,
	         std::enable_if_t<daw::traits::is_container_like_v<Container>,
	                          std::nullptr_t> = nullptr>
	constexpr decltype( auto )
	max_element( Container const &container, Compare compare ) noexcept(
	  noexcept( daw::algorithm::max_element( std::begin( container ),
	                                         std::end( container ),
	                                         compare ) ) ) {

		using std::begin;
		using std::end;
		using value_type = DAW_TYPEOF( *begin( container ) );

		static_assert(
		  traits::is_binary_predicate_v<Compare, value_type, value_type> );

		return daw::algorithm::max_element(
		  begin( container ), end( container ), compare );
	}

	template<typename Container,
	         typename Value,
	         std::enable_if_t<
	           daw::traits::is_container_like_v<Container> &&
	             !traits::is_unary_predicate_v<
	               Value,
	               decltype( *std::cbegin( std::declval<Container>( ) ) )>,
	           std::nullptr_t> = nullptr>
	constexpr bool contains( Container const &container, Value &&value ) noexcept(
	  noexcept( std::find( std::cbegin( container ),
	                       std::cend( container ),
	                       value ) != std::cend( container ) ) ) {

		return daw::algorithm::find( std::cbegin( container ),
		                             std::cend( container ),
		                             DAW_FWD( value ) ) != std::cend( container );
	}

	template<typename Container,
	         typename UnaryPredicate,
	         std::enable_if_t<
	           daw::traits::is_container_like_v<Container> &&
	             traits::is_unary_predicate_v<
	               UnaryPredicate,
	               decltype( *std::cbegin( std::declval<Container>( ) ) )>,
	           std::nullptr_t> = nullptr>
	constexpr bool
	contains( Container const &container, UnaryPredicate pred ) noexcept(
	  noexcept( std::find_if( std::cbegin( container ),
	                          std::cend( container ),
	                          pred ) != std::cend( container ) ) ) {

		return daw::algorithm::find_if( std::cbegin( container ),
		                                std::cend( container ),
		                                pred ) != std::cend( container );
	}

	template<typename Container,
	         typename OutputIterator,
	         std::enable_if_t<daw::traits::is_container_like_v<Container>,
	                          std::nullptr_t> = nullptr>
	constexpr void copy( Container const &source, OutputIterator destination ) {
		auto src = std::cbegin( source );
		auto const last = std::cend( source );

		while( src != last ) {
			*destination = *src;
			++destination;
			++src;
		}
	}

	template<typename Container,
	         typename OutputIterator,
	         std::enable_if_t<daw::traits::is_container_like_v<Container>,
	                          std::nullptr_t> = nullptr>
	constexpr void
	copy_n( Container const &source, size_t count, OutputIterator destination ) {
		auto src = std::cbegin( source );
		count = ( daw::min )( count, daw::size( source ) );
		for( size_t n = 0; n < count; ++n ) {
			*destination = *src;
			++destination;
			++src;
		}
	}

	template<typename Container,
	         typename OutputIterator,
	         typename UnaryPredicate,
	         std::enable_if_t<daw::traits::is_container_like_v<Container>,
	                          std::nullptr_t> = nullptr>
	constexpr OutputIterator copy_if( Container const &source,
	                                  OutputIterator destination,
	                                  UnaryPredicate pred ) {
		static_assert(
		  traits::is_unary_predicate_v<UnaryPredicate,
		                               decltype( *std::begin( source ) )>,
		  "Compare does not satisfy the Unary Predicate concept.  See "
		  "http://en.cppreference.com/w/cpp/concept/Predicate for more "
		  "information" );

		auto src = std::cbegin( source );
		auto const last = std::cend( source );

		while( src != last ) {
			if( pred( *src ) ) {
				*destination = *src;
				++destination;
			}
			++src;
		}
		return destination;
	}

	template<typename Container,
	         typename OutputIterator,
	         typename UnaryPredicate,
	         std::enable_if_t<daw::traits::is_container_like_v<Container>,
	                          std::nullptr_t> = nullptr>
	constexpr void
	copy_n( Container const &source, size_t count, OutputIterator destination ) {
		static_assert(
		  traits::is_unary_predicate_v<UnaryPredicate,
		                               decltype( *std::begin( source ) )>,
		  "Compare does not satisfy the Unary Predicate concept.  See "
		  "http://en.cppreference.com/w/cpp/concept/Predicate for more "
		  "information" );

		auto src = std::cbegin( source );
		count = ( daw::min )( count, daw::size( source ) );
		for( size_t n = 0; n < count; ++n ) {
			if( pred( *src ) ) {
				*destination = *src;
				++destination;
				++src;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	/// Summary: Run func( container, position ) on each element
	/// in interval [first_inclusive, last_exclusive)
	///
	template<typename Container, typename Function>
	constexpr void for_each_with_pos(
	  Container &container,
	  size_t const first_inclusive,
	  size_t last_exclusive,
	  Function func ) noexcept( noexcept( func( *std::begin( container ),
	                                            std::declval<size_t>( ) ) ) ) {

		using value_t =
		  std::decay_t<typename std::iterator_traits<decltype( std::begin(
		    container ) )>::value_type>;
		static_assert( std::is_invocable_v<Function, value_t, size_t>,
		               "Supplied function does not satisfy requirements of "
		               "taking arguments of type (value_t, size_t)" );

		auto it = daw::begin_at( container, first_inclusive );
		for( size_t row = first_inclusive; row < last_exclusive; ++row ) {
			func( *it, row );
			++it;
		}
	}

	template<typename Container, typename Function>
	constexpr void
	for_each_with_pos( Container &container, Function func ) noexcept(
	  noexcept( func( *std::begin( container ), std::declval<size_t>( ) ) ) ) {
		using value_t =
		  std::decay_t<typename std::iterator_traits<decltype( std::begin(
		    container ) )>::value_type>;
		static_assert( std::is_invocable_v<Function, value_t, size_t>,
		               "Supplied function does not satisfy requirements of "
		               "taking arguments of type (value_t, size_t)" );
		for_each_with_pos( container, 0, container_size( container ), func );
	}

	/// @brief Summary: Run func( container, position ) on each element in
	/// interval [first_inclusive, last_exclusive)
	/// @tparam Container range of items to iterate over
	/// @tparam Function A callable that can take as arguments the container and
	/// an index
	/// @param container range of items
	/// @param first index of first item in range to work on
	/// @param last end of range
	/// @param func callable to pass container and position too
	template<typename Container, typename Function>
	constexpr void for_each_subset(
	  Container &container,
	  size_t first,
	  size_t const last,
	  Function func ) noexcept( traits::is_nothrow_callable_v<Function,
	                                                          Container,
	                                                          size_t> ) {

		static_assert( traits::has_integer_subscript_v<Container>,
		               "Container is required to have a subscript operator that "
		               "takes integral types" );
		static_assert( std::is_invocable_v<Function, Container, size_t>,
		               "Supplied function does not satisfy requirements of "
		               "taking arguments of type (Container, size_t)" );

		for( ; first < last; ++first ) {
			func( container, first );
		}
	}

	/// @brief insert the contents of one container into another
	/// @tparam Source range_t type that has std::begin/std::end being value
	/// @tparam Destination Container that must have insert( iter, range_begin,
	/// range_end ) as a member along with std::end
	/// @param source range holding data to copy to destination
	/// @param destination a container to append to
	template<typename Source, typename Destination>
	constexpr void append( Source const &source, Destination &destination ) {
		destination.insert(
		  std::end( destination ), std::begin( source ), std::end( source ) );
	}
} // namespace daw::container
