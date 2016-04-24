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

#pragma once

#include <iterator>
#include <algorithm>
#include <numeric>
#include <vector>
#include <random>
#include <iostream>
#include "daw_traits.h"
#include "daw_algorithm.h"
#include "daw_reference.h"
#include "daw_range_collection.h"

namespace daw {
	namespace range {
		namespace impl {
		template<typename ValueType> struct CollectionRange;

		template<typename Iterator>
		class ReferenceRange {
			using referenced_value_type = ::std::remove_cv_t<typename ::std::iterator_traits<Iterator>::value_type>;
			using values_type = ::std::vector<::daw::Reference<referenced_value_type>>;
			values_type mutable m_values;
		public:
			using value_type = decltype(m_values.front( ).get( ));
			using reference = decltype(m_values.front( ));
			using const_reference = const reference;
			using iterator = Iterator;
			using const_iterator = const iterator;
			using difference_type = typename ::std::iterator_traits<decltype(m_values.begin( ))>::difference_type;

			ReferenceRange( ) = default;
			ReferenceRange( ReferenceRange const & ) = default;
			ReferenceRange( ReferenceRange && ) = default;
			~ReferenceRange( ) = default;
			ReferenceRange & operator=( ReferenceRange const & ) = default;
			ReferenceRange & operator=( ReferenceRange && ) = default;

			ReferenceRange( iterator first, iterator last );

			bool at_end( ) const;
			bool empty( ) const;
			auto begin( );
			auto begin( ) const;
			auto cbegin( ) const;
			auto end( );
			auto end( ) const;
			auto cend( ) const;
			auto front( );
			auto front( ) const;
			auto back( );
			auto back( ) const;
			auto size( ) const;
			auto & operator[]( size_t pos );
			auto & operator[]( size_t pos ) const;
			bool operator==( ReferenceRange const & other ) const;
			bool operator!=( ReferenceRange const & other ) const;
			template<typename Value> auto find( Value const & value ) const;
			template<typename UnaryPredicate> auto find_if( UnaryPredicate predicate ) const;
			ReferenceRange copy( ) const;
			ReferenceRange & sort( );
			ReferenceRange sort( ) const;
			template<typename UnaryPredicate> ReferenceRange & sort( UnaryPredicate predicate );
			template<typename UnaryPredicate> ReferenceRange sort( UnaryPredicate predicate ) const;
			ReferenceRange & stable_sort( );
			ReferenceRange stable_sort( ) const;
			template<typename UnaryPredicate> ReferenceRange & stable_sort( UnaryPredicate predicate );
			template<typename UnaryPredicate> ReferenceRange stable_sort( UnaryPredicate predicate ) const;
			ReferenceRange & unique( );
			ReferenceRange unique( ) const;
			template<typename UnaryPredicate> ReferenceRange & unique( UnaryPredicate predicate );
			template<typename UnaryPredicate> ReferenceRange unique( UnaryPredicate predicate ) const;
			template<typename UnaryPredicate> ReferenceRange &  partition( UnaryPredicate predicate );
			template<typename UnaryPredicate> ReferenceRange partition( UnaryPredicate predicate ) const;
			template<typename UnaryPredicate> ReferenceRange & stable_partition( UnaryPredicate predicate );
			template<typename UnaryPredicate> ReferenceRnage stable_partition( UnaryPredicate predicate ) const;
			template<typename UnaryPredicate> auto partition_it( UnaryPredicate predicate );
			template<typename UnaryPredicate> auto partition_it( UnaryPredicate predicate ) const;
			template<typename UnaryPredicate> auto stable_partition_it( UnaryPredicate predicate );
			template<typename UnaryPredicate> auto stable_partition_it( UnaryPredicate predicate ) const;
			template<typename T> auto accumulate( T && init ) const;
			template<typename T, typename BinaryOperator> auto accumulate( T && init, BinaryOperator oper ) const;
			template<typename UnaryOperator> auto transform( UnaryOperator oper ) const;
			template<typename Value> bool contains( Value const & value ) const;
			template<typename Value, typename UnaryPredicate> bool contains( Value const & value, UnaryPredicate predicate ) const;
			template<typename UnaryPredicate> ReferenceRange & erase( UnaryPredicate predicate );
			template<typename UnaryPredicate> ReferenceRange erase( UnaryPredicate predicate ) const;
			template<typename Value> ReferenceRange erase_where_equal_to( Value const & value ) const;
			template<typename Value> ReferenceRange & erase_where_equal_to( Value const & value );
			template<typename UnaryPredicate> ReferenceRange where( UnaryPredicate predicate ) const;
			template<typename UnaryPredicate> ReferencceRange & where( UnaryPredicate predicate );
			template<typename Value> ReferenceRange where_equal_to( Value const & value ) const;
			template<typename Value> ReferenceRange & where_equal_to( Value const & value );
			template<typename Container> Container as( ) const;
			auto as_vector( ) const;
			template<typename Function>
			ReferenceRange const & for_each( Function function ) const;
			ReferenceRange & shuffle( );
			ReferenceRange shuffle( ) const;
			template<typename UniformRandomNumberGenerator>	ReferenceRange & shuffle( UniformRandomNumberGenerator && urng );
			template<typename UniformRandomNumberGenerator> ReferenceRange shuffle( UniformRandomNumberGenerator && urng ) const;
		};	// class ReferenceRange

		template<typename Container, typename = void>
		auto make_ref_range( Container & container ) {
			using iterator = ::std::remove_const_t<decltype(::std::begin( container ))>;
			return ReferenceRange<iterator>( ::std::begin( container ), ::std::end( container ) );
		}

		template<typename IteratorF, typename IteratorL>
		auto make_ref_range( IteratorF first, IteratorL last ) {
			using iterator = typename ::std::remove_const_t<IteratorF>;
			return ReferenceRange<iterator>( first, last );
		}

		template<typename Iterator>
		auto make_ref_range( ::daw::range::ReferenceRange<Iterator> const & container ) {
			using iterator = typename ::std::remove_const_t<Iterator>;
			return ReferenceRange<iterator>( container.begin( ), container.end( ) );
		}

		namespace impl {
			template<typename Collection>
			auto to_vector( Collection const & collection ) {
				using value_type = ::std::decay_t<typename Collection::value_type>;
				::std::vector<value_type> result;
				::std::copy( ::std::begin( collection ), ::std::end( collection ), ::std::back_inserter( result ) );
				return result;
			}

			template<typename IteratorF, typename IteratorL>
			auto to_vector( IteratorF first, IteratorL last ) {
				using value_type = typename ::std::iterator_traits<IteratorF>::value_type;
				::std::vector<value_type> result;
				::std::copy( first, last, ::std::back_inserter( result ) );
				return result;
			}

		}	// namespace impl
	}	// namespace range
}	// namespace daw

template<typename Iterator>
::std::ostream & operator<<( ::std::ostream& os, ::daw::range::ReferenceRange<Iterator> const & rng ) {
	os << "{";
	if( !rng.empty( ) ) {
		for( auto it = rng.cbegin( ); it != rng.cend( ); ++it ) {
			os << " " << it->get( );
		}
	}
	os << " }";
	return os;
}

#include "daw_range_reference_impl.h"

