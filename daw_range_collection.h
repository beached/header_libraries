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
#include "daw_range_common.h"
#include "daw_range_reference.h"

namespace daw {
	namespace range {
		template<typename Iterator> class ReferenceRange;

		template<typename T>
		struct CollectionRange {
			using is_range_collection = std::true_type;
			using value_type = ::std::decay_t<T>;
			using values_type = ::std::vector<value_type>;
		private:
			values_type m_values;
		public:
			using reference = typename values_type::reference;
			using const_reference = typename values_type::const_reference;
			using iterator = typename values_type::iterator;
			using const_iterator = typename values_type::const_iterator;
			using difference_type = typename ::std::iterator_traits<iterator>::difference_type;

			CollectionRange( ) = default;

			template<typename Collection>
			CollectionRange( Collection const & collection ): m_values( impl::to_vector( collection ) ) { }
			
			template<typename IteratorF, typename IteratorL>
			CollectionRange( IteratorF first, IteratorL last ): m_values( impl::to_vector( first, last ) ) { }

			CollectionRange( CollectionRange const & ) = default;
			CollectionRange( CollectionRange && ) = default;
			~CollectionRange( ) = default;
			CollectionRange & operator=( CollectionRange const & ) = default;
			CollectionRange & operator=( CollectionRange && ) = default;

			bool at_end( ) const {
				return begin( ) == end( );
			}

			bool empty( ) const {
				return !(begin( ) != end( ));
			}

			auto begin( ) {
				return m_values.begin( );
			}

			auto end( ) {
				return m_values.end( );
			}

			auto begin( ) const {
				return m_values.begin( );
			}

			auto end( ) const {
				return m_values.end( );
			}

			auto cbegin( ) const {
				return m_values.cbegin( );
			}

			auto cend( ) const {
				return m_values.cend( );
			}

			auto & front( ) {
				return m_values.front( );
			}

			auto const & front( ) const {
				return m_values.front( );
			}

			auto & back( ) {
				return m_values.back( );
			}

			auto const & back( ) const {
				return m_values.back( );
			}

			auto size( ) const {
				return m_values.size( );
			}

			auto & operator[]( size_t pos ) {
				return m_values[pos].get( );
			}

			auto const & operator[]( size_t pos ) const {
				return m_values[pos].get( );
			}

			bool operator==( CollectionRange const & other ) const {
				return ::std::equal( begin( ), end( ), other.begin( ) );
			}

			bool operator!=( CollectionRange const & other ) const {
				return !::std::equal( begin( ), end( ), other.begin( ) );
			}

			template<typename... Args>
			void push_back( Args &&... args ) {
				m_values.push_back( std::forward<Args>( args )... );
			}

			template<typename Value>
			auto find( Value const & value ) const {
				return ::std::find( begin( ), end( ), value );
			}

			template<typename UnaryPredicate>
			auto find_if( UnaryPredicate predicate ) const {
				return ::std::find_if( begin( ), end( ), predicate );
			}

			template<typename U>
			auto accumulate( U && init ) const {
				return ::std::accumulate( begin( ), end( ), ::std::forward<U>( init ) );
			}

			template<typename U, typename BinaryOperator>
			auto accumulate( U && init, BinaryOperator oper ) const {
				return ::std::accumulate( begin( ), end( ), ::std::forward<U>( init ), oper );
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

			auto sort( ) const & {
				return make_range_reference( *this ).sort( );
			}

			CollectionRange sort( ) && {
				::std::sort( begin( ), end( ) );
				return ::std::move( *this );
			}

			template<typename UnaryPredicate>
			auto sort( UnaryPredicate predicate ) const & {
				return make_range_reference( *this ).sort( predicate );
			}

			template<typename UnaryPredicate>
			CollectionRange sort( UnaryPredicate predicate ) && {
				::std::sort( begin( ), end( ), predicate );
				return ::std::move( *this );
			}

			auto stable_sort( ) const & {
				return make_range_reference( *this ).stable_sort( );
			}

			CollectionRange stable_sort( ) && {
				::std::stable_sort( begin( ), end( ) );
				return ::std::move( *this );
			}

			template<typename UnaryPredicate>
			auto stable_sort( UnaryPredicate predicate ) const & {
				return make_range_reference( *this ).stable_sort( predicate );
			}

			template<typename UnaryPredicate>
			CollectionRange stable_sort( UnaryPredicate predicate ) && {
				::std::stable_sort( begin( ), end( ), predicate );
				return ::std::move( *this );
			}

			auto unique( ) const & {
				return make_range_reference( *this ).unique( );
			}

			CollectionRange unique( ) && {
				m_values.erase( ::std::unique( m_values.begin( ), m_values.end( ) ), m_values.end( ) );
				return ::std::move( *this );
			}

			template<typename UnaryPredicate>
			auto unique( UnaryPredicate predicate ) const & {
				return make_range_reference( *this ).unique( predicate );
			}

			template<typename UnaryPredicate>
			CollectionRange unique( UnaryPredicate predicate ) && {
				m_values.erase( ::std::unique( m_values.begin( ), m_values.end( ), predicate ), m_values.end( ) );
				return ::std::move( *this );
			}

			template<typename UnaryPredicate>
			auto partition( UnaryPredicate predicate ) const & {
				return make_range_reference( *this ).partition( predicate );
			}

			template<typename UnaryPredicate>
			CollectionRange partition( UnaryPredicate predicate ) && {
				::std::partition( begin( ), end( ), predicate );
				return ::std::move( *this );
			}

			template<typename UnaryPredicate>
			auto stable_partition( UnaryPredicate predicate ) const & {
				return make_range_reference( *this ).stable_partition( predicate );
			}

			template<typename UnaryPredicate>
			CollectionRange stable_partition( UnaryPredicate predicate ) && {
				::std::stable_partition( begin( ), end( ), predicate );
				return ::std::move( *this );
			}

			template<typename UnaryOperator>
			CollectionRange & transform( UnaryOperator oper ) const & {
				using v_t = ::std::decay_t<decltype(oper( front( ) ))>;
				auto result = CollectionRange<v_t>( );
				::std::transform( ::std::begin( m_values ), ::std::end( m_values ), ::std::back_inserter( result ), oper );
				return result;
			}

			template<typename UnaryPredicate>
			CollectionRange erase( UnaryPredicate predicate ) && {
				m_values.erase( ::std::remove_if( m_values.begin( ), m_values.end( ), predicate ), m_values.end( ) );
				return ::std::move( *this );
			}

			template<typename UnaryPredicate>
			auto erase( UnaryPredicate predicate ) const & {
				return make_range_reference( *this ).erase( predicate );
			}

			template<typename Value>
			auto erase_where_equal_to( Value const & value ) && {
				return erase( [&value]( auto const & current_value ) {
					return value == current_value;
				} );
			}

			template<typename Value>
			auto erase_where_equal_to( Value const & value ) const & {
				return make_range_reference( *this ).erase_where_equal_to( value );
			}

			template<typename UnaryPredicate>
			auto where( UnaryPredicate predicate ) && {
				return erase( [predicate]( auto const & v ) {
					return !predicate( v );
				} );
			}

			template<typename UnaryPredicate>
			auto where( UnaryPredicate predicate ) const & {
				return make_range_reference( *this ).where( predicate );
			}

			template<typename Value>
			auto where_equal_to( Value const & value ) && {
				return where( [&value]( auto const & current_value ) {
					return value == current_value;
				} );
			}

			template<typename Value>
			auto where_equal_to( Value const & value ) const & {
				return make_range_reference( *this ).where_equal_to( value );
			}

			template<typename Container>
			auto as( ) const {
				Container result;
				::std::transform( begin( ), end( ), ::std::back_inserter( result ), []( auto const & rv ) {
					return rv.get( );
				} );
				return result;
			}

			auto as_vector( ) const {
				return as<values_type>( );
			}

			template<typename Function>
			CollectionRange & for_each( Function function ) const & {
				for( auto const & v : m_values ) {
					function( v );
				}
				return *this;
			}

			template<typename Function>
			CollectionRange & for_each( Function function ) & {
				for( auto & v : m_values ) {
					function( v );
				}
				return *this;
			}


			template<typename Function>
			CollectionRange for_each( Function function ) && {
				for( auto & v : m_values ) {
					function( v );
				}
				return ::std::move( *this );
			}

			template<typename UniformRandomNumberGenerator>
			CollectionRange shuffle( UniformRandomNumberGenerator && urng ) && {
				::std::shuffle( begin( ), end( ), std::forward<UniformRandomNumberGenerator>( urng ) );
				return ::std::move( *this );
			}

			template<typename UniformRandomNumberGenerator>
			auto shuffle( UniformRandomNumberGenerator && urng ) const & {
				return make_range_reference( *this ).shuffle( std::forward<UniformRandomNumberGenerator>( urng ) );
			}

			CollectionRange & shuffle( ) && {
				static std::random_device rd;
				static std::mt19937 g( rd( ) );
				return shuffle( g );
			}
			
			auto shuffle( ) const & {
				return make_range_reference( *this ).shuffle( );
			}
		};	// struct CollectionRange

		namespace impl {
			template<typename ValueType>
			CollectionRange<ValueType> make_range_collection( ) {
				return CollectionRange<ValueType>{ };
			}

			template<typename Container, typename=void>
			auto make_range_collection( Container const & container ) {
				using ValueType = impl::cleanup_t<typename ::std::iterator_traits<typename Container::iterator>::value_type>;
				return CollectionRange<ValueType>( container );
			}

			template<typename IteratorF, typename IteratorL>
			auto make_range_collection( IteratorF first, IteratorL last ) {
				using ValueType = impl::cleanup_t<typename ::std::iterator_traits<IteratorF>::value_type>;
				return CollectionRange<ValueType>( first, last );
			}

			template<typename Iterator>
			auto make_range_collection( ::daw::range::ReferenceRange<Iterator> const & collection ) {
				using ValueType = impl::cleanup_t<typename ::std::iterator_traits<Iterator>::value_type>;
				CollectionRange<ValueType> result;
				std::transform( ::std::begin( collection ), ::std::end( collection ), ::std::back_inserter( result ), []( auto const & rv ) {
					return rv.get( );
				} );
				return result;
			}
		}	// namespace impl

		template<typename Arg, typename... Args>
		auto make_range_collection( Arg && arg, Args&&... args ) {
			return ::daw::range::impl::make_range_collection( ::std::forward<Arg>( arg ), ::std::forward<Args>( args )... );	
		}

		namespace details {
			template<typename T>
			std::false_type is_range_collection_impl( T const &, long );

			template<typename T>
			auto is_range_collection_impl( T const & value, int ) -> typename T::is_range_collection;
		}   // namespace details

		template<typename T>
		struct is_range_collection: decltype(details::is_range_collection_impl( std::declval<T const &>( ), 1 )) {};

		template<typename T> using is_range_collection_t = typename is_range_collection<T>::type;
		template<typename T> constexpr bool is_range_collection_v = is_range_collection<T>::value;
 

	}	// namespace range
}	// namespace daw

template<typename T>
::std::ostream & operator<<( ::std::ostream& os, ::daw::range::CollectionRange<T> const & rng ) {
	os << "{";
	if( !rng.empty( ) ) {
		for( auto it = rng.cbegin( ); it != rng.cend( ); ++it ) {
			os << " " << *it;
		}
	}
	os << " }";
	return os;
}


