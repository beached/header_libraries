#pragma once
// The MIT License (MIT)
//
// Copyright (c) 2014-2015 Darrell Wright
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


#include <algorithm>
#include <numeric>

namespace daw {
	namespace algorithm {
		template<typename Container>
		Container& sort( Container& container ) {
			std::sort( std::begin( container ), std::end( container ) );
			return container;
		}

		template<typename Container, typename UnaryPredicate>
		Container& sort( Container& container, UnaryPredicate pred ) {
			std::sort( std::begin( container ), std::end( container ), pred );
			return container;
		}

		template<typename Container>
		Container& stable_sort( Container& container ) {
			std::stable_sort( std::begin( container ), std::end( container ) );
			return container;
		}

		template<typename Container, typename UnaryPredicate>
		Container& stable_sort( Container& container, UnaryPredicate pred ) {
			std::stable_sort( std::begin( container ), std::end( container ), pred );
			return container;
		}

		template<typename Container, typename Value>
		auto find( Container const & container, Value const & value ) -> decltype(end( container )) {
			return std::find( std::begin( container ), std::end( container ), value );
		}

		template<typename Container, typename Value>
		bool binary_search( Container const & container, Value const & value ) {
			return std::binary_search( std::begin( container ), std::end( container ), value );
		}

		template<typename Container, typename UnaryPredicate>
		auto find_if( Container const & container, UnaryPredicate pred ) -> decltype(end( container )) {
			return std::find_if( std::begin( container ), std::end( container ), pred );
		}

		template<typename Container, typename Value>
		auto erase_remove( Container& container, Value const & value ) -> decltype(container.erase( std::end( container ), std::end( container ) )) {
			return container.erase( std::remove( std::begin( container ), std::end( container ), value ), std::end( container ) );
		}

		template<typename Container, typename UnaryPredicate>
		auto erase_remove_if( Container& container, UnaryPredicate pred ) -> decltype(container.erase( std::end( container ), std::end( container ) )) {
			return container.erase( std::remove_if( std::begin( container ), std::end( container ), pred ), std::end( container ) );
		}

		template<typename Container, typename UnaryPredicate>
		auto partition( Container& container, UnaryPredicate pred ) -> decltype(begin( container )) {
			return std::partition( std::begin( container ), std::end( container ), pred );
		}

		template<typename Container, typename UnaryPredicate>
		auto stable_partition( Container& container, UnaryPredicate pred ) -> decltype(begin( container )) {
			return std::stable_partition( std::begin( container ), std::end( container ), pred );
		}

		template<typename Container, typename T>
		T accumulate( Container const & container, T init ) {
			return std::accumulate( std::begin( container ), std::end( container ), std::move( init ) );
		}

		template<typename Container, typename T, typename BinaryOperator>
		T accumulate( Container const & container, T init, BinaryOperator oper ) {
			return std::accumulate( std::begin( container ), std::end( container ), std::move( init ), oper );
		}

		template<typename ContainerIn, typename UnaryOperator>
		auto map( ContainerIn const & in, UnaryOperator oper ) -> std::vector < decltype(oper( std::declval<typename std::iterator_traits<decltype(std::begin( in ))>::value_type>( ) )) > {
			using result_t = std::vector < decltype(oper( std::declval<typename std::iterator_traits<decltype(std::begin( in ))>::value_type>( ) )) > ;
			result_t result;
			std::transform( std::begin( in ), std::end( in ), std::back_inserter( result ), oper );
			return result;
		}

		template<typename Container, typename Value>
		bool contains( Container const & container, Value const & value ) {
			return std::find( std::begin( container ), std::end( container ), value ) != std::end( container );
		}

		template<typename Container, typename Value, typename UnaryPredicate>
		bool contains( Container const & container, Value const & value, UnaryPredicate pred ) {
			auto pred2 = [&value, &pred]( Value const & val ) {
				return pred( value, val );
			};
			return std::find_if( std::begin( container ), std::end( container ), pred2 ) != std::end( container );
		}

		template<typename ContainerType, typename Predicate>
		auto where( ContainerType& container, Predicate pred ) -> std::vector < std::reference_wrapper<typename ContainerType::value_type> > {
			using ValueType = typename ContainerType::value_type;
			std::vector<std::reference_wrapper<ValueType>> result;

			std::copy_if( std::begin( container ), std::end( container ), std::back_inserter( result ), pred );

			return result;
		}
	}	// namespace algorithm
}	// namespace daw
