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
		template<typename Iterator>
		ReferenceRange<Iterator>::ReferenceRange( iterator first, iterator last ): m_values( impl::to_refvec( first, last ) ) { }

		template<typename Iterator>
		bool ReferenceRange<Iterator>::at_end( ) const {
			return begin( ) == end( );
		}

		template<typename Iterator>
		bool ReferenceRange<Iterator>::empty( ) const {
			return !(begin( ) != end( ));
		}

		template<typename Iterator>
		auto ReferenceRange<Iterator>::begin( ) const {
			return m_values.begin( );
		}

		template<typename Iterator>
		auto ReferenceRange<Iterator>::cbegin( ) const {
			return m_values.begin( );
		}

		template<typename Iterator>
		auto ReferenceRange<Iterator>::begin( ) {
			return m_values.begin( );
		}

		template<typename Iterator>
		auto ReferenceRange<Iterator>::end( ) {
			return m_values.end( );
		}

		template<typename Iterator>
		auto ReferenceRange<Iterator>::end( ) const {
			return m_values.end( );
		}

		template<typename Iterator>
		auto ReferenceRange<Iterator>::cend( ) const {
			return m_values.end( );
		}

		template<typename Iterator>
		auto & ReferenceRange<Iterator>::front( ) {
			return m_values.front( );
		}

		template<typename Iterator>
		auto const & ReferenceRange<Iterator>::front( ) const {
			return m_values.front( );
		}

		template<typename Iterator>
		auto & ReferenceRange<Iterator>::back( ) {
			return m_values.back( );
		}

		template<typename Iterator>
		auto const & ReferenceRange<Iterator>::back( ) const {
			return m_values.back( );
		}

		template<typename Iterator>
		auto ReferenceRange<Iterator>::size( ) const {
			return m_values.size( );
		}

		template<typename Iterator>
		auto & ReferenceRange<Iterator>::operator[]( size_t pos ) {
			return m_values[pos].get( );
		}

		template<typename Iterator>
		auto const & ReferenceRange<Iterator>::operator[]( size_t pos ) const {
			return m_values[pos].get( );
		}

		template<typename Iterator>
		bool ReferenceRange<Iterator>::operator==( ReferenceRange<Iterator> const & other ) const {
			return m_values == other.m_values;
		}

		template<typename Iterator>
		bool ReferenceRange<Iterator>::operator!=( ReferenceRange<Iterator> const & other ) const {
			return m_values != other.m_values;
		}

		template<typename Iterator>
		template<typename Value>
		auto ReferenceRange<Iterator>::find( Value const & value ) const {
			return ::std::find( begin( ), end( ), value );
		}

		template<typename Iterator>
		template<typename UnaryPredicate>
		auto ReferenceRange<Iterator>::find_if( UnaryPredicate predicate ) const {
			return ::std::find_if( begin( ), end( ), predicate );
		}

		template<typename Iterator>
		auto & ReferenceRange<Iterator>::sort( ) {
			::std::sort( begin( ), end( ) );
			return *this;
		}

		template<typename Iterator>
		auto ReferenceRange<Iterator>::sort( ) const {
			return copy( ).sort( );
		}

		template<typename Iterator>
		template<typename UnaryPredicate>
		ReferenceRange<Iterator> & sort( UnaryPredicate predicate ) {
			::std::sort( begin( ), end( ), [&predicate]( auto const & v ) {
				return predicate( v.get( ) );
			} );
			return *this;
		}

		template<typename Iterator>
		template<typename UnaryPredicate>
		ReferenceRange<Iterator> & sort( UnaryPredicate predicate ) const {
			return copy( ).sort( predicate );
		}

		ReferenceRange<Iterator> stable_sort( ) {
			::std::stable_sort( begin( ), end( ) );
			return *this;
		}

		ReferenceRange<Iterator> stable_sort( ) const {
			return copy( ).stable_sort( );
		}

		template<typename UnaryPredicate>
		ReferenceRange<Iterator> stable_sort( UnaryPredicate predicate ) {
			::std::stable_sort( begin( ), end( ), [&predicate]( auto const & v ) {
				return predicate( v.get( ) );
			} );
			return *this;
		}

		template<typename UnaryPredicate>
		ReferenceRange<Iterator> stable_sort( UnaryPredicate predicate ) const {
			return copy( ).sort( predicate );
		}

		ReferenceRange<Iterator> unique( ) {
			erase( ::std::unique( begin( ), end( ) ), end( ) );
			return *this;
		}

		ReferenceRange<Iterator> unique( ) const {
			return copy( ).unique( );
		}

		template<typename UnaryPredicate>
		ReferenceRange<Iterator> unique( UnaryPredicate predicate ) {
			erase( ::std::unique( begin( ), end( ), [&predicate]( auto const & v ) {
				return predicate( v.get( ) );
			} ), end( ) );
			return *this;
		}

		template<typename UnaryPredicate>
		ReferenceRange<Iterator> unique( UnaryPredicate predicate ) const {
			return copy( ).unique( predicate );
		}

		template<typename UnaryPredicate>
		auto partition( UnaryPredicate predicate ) {
			::std::partition( begin( ), end( ), [&predicate]( auto const & v ) {
				return predicate( v.get( ) );
			} );
			return *this;
		}

		template<typename UnaryPredicate>
		auto partition( UnaryPredicate predicate ) const {
			return copy( ).partition( predicate );
		}

		template<typename UnaryPredicate>
		auto partition_it( UnaryPredicate predicate ) {
			auto mid = ::std::partition( begin( ), end( ), predicate );
			return ::std::make_pair( mid, *this );
		}

		template<typename UnaryPredicate>
		auto partition_it( UnaryPredicate predicate ) const {
			return copy( ).partition_it( predicate );
		}

		template<typename UnaryPredicate>
		auto stable_partition( UnaryPredicate predicate ) {
			::std::stable_partition( m_values.begin( ), m_values.end( ), [&predicate]( auto const & v ) {
				return predicate( v.get( ) );
			} );
			return *this;
		}

		template<typename UnaryPredicate>
		auto stable_partition( UnaryPredicate predicate ) const {
			return copy( ).stable_partition( predicate );
		}

		template<typename UnaryPredicate>
		auto stable_partition_it( UnaryPredicate predicate ) {
			auto mid = ::std::stable_partition( begin( ), end( ), [&predicate]( auto const & v ) {
				return predicate( v.get( ) );
			} );
			return ::std::make_pair( mid, *this );
		}

		template<typename UnaryPredicate>
		auto stable_partition_it( UnaryPredicate predicate ) const {
			return copy( ).partition_it( predicate );
		}

		template<typename T>
		auto accumulate( T && init ) const {
			return ::std::accumulate( begin( ), end( ), ::std::forward<T>( init ) );
		}

		template<typename T, typename BinaryOperator>
		auto accumulate( T && init, BinaryOperator oper ) const {
			return ::std::accumulate( begin( ), end( ), ::std::forward<T>( init ), [&oper]( auto const & a, auto const & b ) {
				return oper( a.get( ), b.get( ) );
			} );
		}

		template<typename UnaryOperator>
		auto transform( UnaryOperator oper ) const {	// TODO verify result shouldn't be ref range
			using v_t = decltype(oper( *begin( ) ));
			auto result = make_collection_range<v_t>( );
			::std::transform( begin( ), end( ), ::std::back_inserter( result ), [&oper]( auto const & v ) {
				return oper( v.get( ) );
			} );
			return result;
		}

		template<typename Value>
		bool contains( Value const & value ) const {
			return ::std::find( begin( ), end( ), value ) != end( );
		}

		template<typename Value, typename UnaryPredicate>
		bool contains( Value const & value, UnaryPredicate predicate ) const {
			auto pred2 = [&value, &predicate]( Value const & val ) {
				return predicate( value, val );
			};
			return ::std::find_if( begin( ), end( ), pred2 ) != end( );
		}

	public:
		template<typename UnaryPredicate>
		auto erase( UnaryPredicate predicate ) {
			m_values.erase( ::std::remove_if( m_values.begin( ), m_values.end( ), [&predicate]( auto const & v ) {
				return predicate( v.get( ) );
			} ), m_values.end( ) );
			return *this;
		}

		template<typename UnaryPredicate>
		auto erase( UnaryPredicate predicate ) const {
			return copy( ).erase( predicate );
		}

		template<typename Value>
		auto erase_where_equal_to( Value const & value ) const {
			return erase( [&value]( auto const & current_value ) {
				return value == current_value;
			} );
		}

		template<typename Value>
		auto erase_where_equal_to( Value const & value ) {
			return erase( [&value]( auto const & current_value ) {
				return value == current_value;
			} );
		}

		template<typename UnaryPredicate>
		auto where( UnaryPredicate predicate ) const {
			return erase( [predicate]( auto const & v ) {
				return !predicate( v );
			} );
		}

		template<typename UnaryPredicate>
		auto where( UnaryPredicate predicate ) {
			return erase( [predicate]( auto const & v ) {
				return !predicate( v );
			} );
		}

		template<typename Value>
		auto where_equal_to( Value const & value ) const {
			return where( [&value]( auto const & current_value ) {
				return value == current_value;
			} );
		}

		template<typename Value>
		auto where_equal_to( Value const & value ) {
			return where( [&value]( auto const & current_value ) {
				return value == current_value;
			} );
		}

		template<typename Container>
		auto as( ) const {
			Container result;
			for( auto const & v : *this ) {
				result.push_back( v.get( ) );
			}
			return result;
		}

		auto as_vector( ) const {
			return as<std::vector<referenced_value_type>>( );
		}

		template<typename Function>
		ReferenceRange<Iterator> for_each( Function function ) const {
			for( auto const & v : m_values ) {
				function( v.get( ) );
			}
			return *this;
		}

		ReferenceRange<Iterator> shuffle( ) {
			static std::random_device rd;
			static std::mt19937 g( rd( ) );
			::std::shuffle( begin( ), end( ), g );
			return *this;
		}


		ReferenceRange<Iterator> shuffle( ) const {
			return copy( ).shuffle( );
		}

		template<typename UniformRandomNumberGenerator>
		ReferenceRange<Iterator> shuffle( UniformRandomNumberGenerator && urng ) {
			::std::shuffle( begin( ), end( ), std::forward<UniformRandomNumberGenerator>( urng ) );
			return *this;
		}

		template<typename UniformRandomNumberGenerator>
		ReferenceRange<Iterator> shuffle( UniformRandomNumberGenerator && urng ) const {
			return copy( ).shuffle( std::forward<UniformRandomNumberGenerator>( urng ) );
		}
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

