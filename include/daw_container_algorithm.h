// The MIT License (MIT)
//
// Copyright (c) 2017 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <algorithm>
#include <numeric>

#include "daw_traits.h"

namespace daw {
	namespace container {
		template<typename Container,
		         std::enable_if_t<daw::traits::is_container_like_v<Container>, std::nullptr_t> = nullptr>
		void
		sort( Container &container ) noexcept( noexcept( std::sort( std::begin( container ), std::end( container ) ) ) ) {

			std::sort( std::begin( container ), std::end( container ) );
		}

		template<typename Container, typename Compare,
		         std::enable_if_t<daw::traits::is_container_like_v<Container>, std::nullptr_t> = nullptr>
		void sort( Container &container,
		           Compare compare ) noexcept( noexcept( std::sort( std::begin( container ), std::end( container ),
		                                                            compare ) ) ) {
			static_assert(
			  daw::is_binary_predicate_v<Compare, decltype( *std::begin( container ) ), decltype( *std::begin( container ) )>,
			  "Compare does not satisfy the Binary Predicate concept.  See "
			  "http://en.cppreference.com/w/cpp/concept/BinaryPredicate for more information" );

			std::sort( std::begin( container ), std::end( container ), compare );
		}

		template<typename Container,
		         std::enable_if_t<daw::traits::is_container_like_v<Container>, std::nullptr_t> = nullptr>
		void stable_sort( Container &container ) noexcept( noexcept( std::stable_sort( std::begin( container ),
		                                                                               std::end( container ) ) ) ) {

			std::stable_sort( std::begin( container ), std::end( container ) );
		}

		template<typename Container, typename Compare,
		         std::enable_if_t<daw::traits::is_container_like_v<Container>, std::nullptr_t> = nullptr>
		void stable_sort( Container &container,
		                  Compare compare ) noexcept( noexcept( std::stable_sort( std::begin( container ),
		                                                                          std::end( container ), compare ) ) ) {

			static_assert(
			  daw::is_binary_predicate_v<Compare, decltype( *std::begin( container ) ), decltype( *std::end( container ) )>,
			  "Compare does not satisfy the Binary Predicate concept.  See "
			  "http://en.cppreference.com/w/cpp/concept/BinaryPredicate for more information" );

			std::stable_sort( std::begin( container ), std::end( container ), compare );
		}

		template<typename Container, typename Value,
		         std::enable_if_t<daw::traits::is_container_like_v<Container>, std::nullptr_t> = nullptr>
		decltype( auto ) find( Container &container,
		                       Value const &value ) noexcept( noexcept( std::find( std::begin( container ),
		                                                                           std::end( container ), value ) ) ) {

			return std::find( std::begin( container ), std::end( container ), value );
		}

		template<typename Container, typename Value,
		         std::enable_if_t<daw::traits::is_container_like_v<Container>, std::nullptr_t> = nullptr>
		decltype( auto ) find( Container const &container,
		                       Value const &value ) noexcept( noexcept( std::find( std::begin( container ),
		                                                                           std::end( container ), value ) ) ) {

			return std::find( std::begin( container ), std::end( container ), value );
		}

		template<typename Container, typename UnaryPredicate,
		         std::enable_if_t<daw::traits::is_container_like_v<Container>, std::nullptr_t> = nullptr>
		decltype( auto ) find_if( Container &container, UnaryPredicate pred ) noexcept(
		  noexcept( std::find_if( std::begin( container ), std::end( container ), pred ) ) ) {

			static_assert( daw::is_unary_predicate_v<UnaryPredicate, decltype( *std::begin( container ) )>,
			               "Compare does not satisfy the Unary Predicate concept.  See "
			               "http://en.cppreference.com/w/cpp/concept/Predicate for more information" );

			return std::find_if( std::begin( container ), std::end( container ), pred );
		}

		template<typename Container, typename UnaryPredicate,
		         std::enable_if_t<daw::traits::is_container_like_v<Container>, std::nullptr_t> = nullptr>
		decltype( auto ) find_if( Container const &container, UnaryPredicate pred ) noexcept(
		  noexcept( std::find_if( std::begin( container ), std::end( container ), pred ) ) ) {

			static_assert( daw::is_unary_predicate_v<UnaryPredicate, decltype( *std::begin( container ) )>,
			               "Compare does not satisfy the Unary Predicate concept.  See "
			               "http://en.cppreference.com/w/cpp/concept/Predicate for more information" );

			return std::find_if( std::begin( container ), std::end( container ), pred );
		}

		template<typename Container, typename Value,
		         std::enable_if_t<daw::traits::is_container_like_v<Container>, std::nullptr_t> = nullptr>
		decltype( auto ) erase_remove( Container &container, Value const &value ) noexcept( noexcept( container.erase(
		  std::remove( std::begin( container ), std::end( container ), value ), std::end( container ) ) ) ) {

			static_assert( daw::is_detected_v<decltype( container.erase( std::begin( container ), std::end( container ) ) )>,
			               "Container must have erase method taking two iterators" );

			return container.erase( std::remove( std::begin( container ), std::end( container ), value ),
			                        std::end( container ) );
		}

		template<typename Container, typename UnaryPredicate,
		         std::enable_if_t<daw::traits::is_container_like_v<Container>, std::nullptr_t> = nullptr>
		decltype( auto ) erase_remove_if( Container &container, UnaryPredicate pred ) noexcept( noexcept( container.erase(
		  std::remove_if( std::begin( container ), std::end( container ), pred ), std::end( container ) ) ) ) {

			static_assert( daw::is_unary_predicate_v<UnaryPredicate, decltype( *std::begin( container ) )>,
			               "Compare does not satisfy the Unary Predicate concept.  See "
			               "http://en.cppreference.com/w/cpp/concept/Predicate for more information" );

			return container.erase( std::remove_if( std::begin( container ), std::end( container ), pred ),
			                        std::end( container ) );
		}

		template<typename Container, typename UnaryPredicate,
		         std::enable_if_t<daw::traits::is_container_like_v<Container>, std::nullptr_t> = nullptr>
		decltype( auto ) partition( Container &container, UnaryPredicate pred ) noexcept(
		  noexcept( std::partition( std::begin( container ), std::end( container ), pred ) ) ) {

			static_assert( daw::is_unary_predicate_v<UnaryPredicate, decltype( *std::begin( container ) )>,
			               "Compare does not satisfy the Unary Predicate concept.  See "
			               "http://en.cppreference.com/w/cpp/concept/Predicate for more information" );

			return std::partition( std::begin( container ), std::end( container ), pred );
		}

		template<typename Container, typename UnaryPredicate,
		         std::enable_if_t<daw::traits::is_container_like_v<Container>, std::nullptr_t> = nullptr>
		decltype( auto ) stable_partition( Container &container, UnaryPredicate pred ) noexcept(
		  noexcept( std::stable_partition( std::begin( container ), std::end( container ), pred ) ) ) {

			static_assert( daw::is_unary_predicate_v<UnaryPredicate, decltype( *std::begin( container ) )>,
			               "Compare does not satisfy the Unary Predicate concept.  See "
			               "http://en.cppreference.com/w/cpp/concept/Predicate for more information" );

			return std::stable_partition( std::begin( container ), std::end( container ), pred );
		}

		template<typename Container, typename T,
		         std::enable_if_t<daw::traits::is_container_like_v<Container>, std::nullptr_t> = nullptr>
		decltype( auto ) accumulate( Container const &container, T &&init ) noexcept(
		  noexcept( std::accumulate( std::cbegin( container ), std::end( container ), std::forward<T>( init ) ) ) ) {

			return std::accumulate( std::cbegin( container ), std::cend( container ), std::forward<T>( init ) );
		}

		template<typename Container, typename T, typename BinaryOperation,
		         std::enable_if_t<daw::traits::is_container_like_v<Container>, std::nullptr_t> = nullptr>
		decltype( auto ) accumulate( Container const &container, T &&init, BinaryOperation oper ) noexcept(
		  noexcept( std::accumulate( std::cbegin( container ), std::cend( container ), std::forward<T>( init ), oper ) ) ) {

			static_assert( daw::is_callable_v<BinaryOperation, T, decltype( *std::cbegin( container ) )>,
			               "Invalid BinaryOperation" );

			return std::accumulate( std::cbegin( container ), std::cend( container ), std::forward<T>( init ), oper );
		}

		template<typename Container, typename UnaryOperator,
		         std::enable_if_t<daw::traits::is_container_like_v<Container>, std::nullptr_t> = nullptr>
		decltype( auto ) transform( Container &container, UnaryOperator unary_operator ) noexcept( noexcept(
		  std::transform( std::cbegin( container ), std::cend( container ), std::begin( container ), unary_operator ) ) ) {

			static_assert( daw::is_callable_v<UnaryOperator, decltype( *std::cbegin( container ) )>,
			               "UnaryOperator is not callable with the values stored in Container" );

			return std::transform( std::cbegin( container ), std::cend( container ), std::begin( container ),
			                       unary_operator );
		}

		template<typename Container, typename OutputIterator, typename UnaryOperator,
		         std::enable_if_t<daw::traits::is_container_like_v<Container>, std::nullptr_t> = nullptr>
		decltype( auto )
		transform( Container const &container, OutputIterator &first_out, UnaryOperator unary_operator ) noexcept(
		  noexcept( std::transform( std::cbegin( container ), std::cend( container ), first_out, unary_operator ) ) ) {

			static_assert( daw::is_callable_v<UnaryOperator, decltype( *std::cbegin( container ) )>,
			               "UnaryOperator is not callable with the values stored in Container" );

			static_assert(
			  daw::is_convertible_v<decltype( unary_operator( *std::cbegin( container ) ) ), decltype( *first_out )>,
			  "Output of UnaryOperator cannot be assigned to *first_out" );

			return std::transform( std::cbegin( container ), std::cend( container ), first_out, unary_operator );
		}

		template<typename Container, typename OutputIterator, typename UnaryOperator,
		         std::enable_if_t<daw::traits::is_container_like_v<Container>, std::nullptr_t> = nullptr>
		decltype( auto )
		transform( Container const &container, OutputIterator &&first_out, UnaryOperator unary_operator ) noexcept(
		  noexcept( std::transform( std::cbegin( container ), std::cend( container ), first_out, unary_operator ) ) ) {

			static_assert( daw::is_callable_v<UnaryOperator, decltype( *std::cbegin( container ) )>,
			               "UnaryOperator is not callable with the values stored in Container" );

			std::transform( std::cbegin( container ), std::cend( container ), first_out, unary_operator );
		}

		template<typename Container, typename UnaryOperator,
		         std::enable_if_t<daw::traits::is_container_like_v<Container>, std::nullptr_t> = nullptr>
		decltype( auto ) max_element( Container &container ) noexcept(
		  noexcept( std::max_element( std::cbegin( container ), std::cend( container ) ) ) ) {

			return std::max_element( std::cbegin( container ), std::cend( container ) );
		}

		template<typename Container, typename OutputIterator, typename UnaryOperator,
		         std::enable_if_t<daw::traits::is_container_like_v<Container>, std::nullptr_t> = nullptr>
		decltype( auto ) max_element( Container const &container ) noexcept(
		  noexcept( std::max_element( std::cbegin( container ), std::cend( container ) ) ) ) {

			return std::max_element( std::cbegin( container ), std::cend( container ) );
		}

		template<typename Container, typename Compare,
		         std::enable_if_t<daw::traits::is_container_like_v<Container>, std::nullptr_t> = nullptr>
		decltype( auto ) max_element( Container &container, Compare compare ) noexcept(
		  noexcept( std::max_element( std::begin( container ), std::end( container ), compare ) ) ) {

			static_assert(
			  daw::is_binary_predicate_v<Compare, decltype( *std::begin( container ) ), decltype( *std::end( container ) )>,
			  "Compare does not satisfy the Binary Predicate concept.  See "
			  "http://en.cppreference.com/w/cpp/concept/BinaryPredicate for more information" );

			return std::max_element( std::begin( container ), std::end( container ), compare );
		}

		template<typename Container, typename Compare,
		         std::enable_if_t<daw::traits::is_container_like_v<Container>, std::nullptr_t> = nullptr>
		decltype( auto ) max_element( Container const &container, Compare compare ) noexcept(
		  noexcept( std::max_element( std::begin( container ), std::end( container ), compare ) ) ) {

			static_assert(
			  daw::is_binary_predicate_v<Compare, decltype( *std::begin( container ) ), decltype( *std::end( container ) )>,
			  "Compare does not satisfy the Binary Predicate concept.  See "
			  "http://en.cppreference.com/w/cpp/concept/BinaryPredicate for more information" );

			return std::max_element( std::begin( container ), std::end( container ), compare );
		}

		template<
		  typename Container, typename Value,
		  std::enable_if_t<daw::traits::is_container_like_v<Container> &&
		                     !daw::is_unary_predicate_v<Value, decltype( *std::cbegin( std::declval<Container>( ) ) )>,
		                   std::nullptr_t> = nullptr>
		bool contains( Container const &container,
		               Value &&value ) noexcept( noexcept( std::find( std::cbegin( container ), std::cend( container ),
		                                                              value ) != std::cend( container ) ) ) {

			return std::find( std::cbegin( container ), std::cend( container ), std::forward<Value>( value ) ) !=
			       std::cend( container );
		}

		template<typename Container, typename UnaryPredicate,
		         std::enable_if_t<
		           daw::traits::is_container_like_v<Container> &&
		             daw::is_unary_predicate_v<UnaryPredicate, decltype( *std::cbegin( std::declval<Container>( ) ) )>,
		           std::nullptr_t> = nullptr>
		bool contains( Container const &container, UnaryPredicate pred ) noexcept(
		  noexcept( std::find_if( std::cbegin( container ), std::cend( container ), pred ) != std::cend( container ) ) ) {

			return std::find_if( std::cbegin( container ), std::cend( container ), pred ) != std::cend( container );
		}
	} // namespace container
} // namespace daw
