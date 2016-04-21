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

namespace daw {
	namespace range {
		namespace impl {
			template<typename Iterator>
			auto to_refvec( Iterator first, Iterator last ) {
				using value_type = typename ::std::iterator_traits<Iterator>::value_type;
				::std::vector<::std::reference_wrapper<value_type>> result;
				for( auto it = first; it != last; ++it ) {
					result.push_back( std::ref( *it ) );
				}
				return result;
			}
		}	// namespace impl

		template<typename T> struct CollectionRange;

		template<typename ValueType>
		auto make_collection_range( ) {
			return CollectionRange<ValueType>( );
		}

		template<typename Collection> auto make_collection_range( Collection && collection ) {
			using value_type = ::std::decay_t<typename Collection::value_type>;
			return CollectionRange<value_type>( ::std::forward<Collection>( collection ) );
		}

		template<typename IteratorF, typename IteratorL> auto make_collection_range( IteratorF first, IteratorL last ) {
			using value_type = ::std::decay_t<typename ::std::iterator_traits<IteratorF>::value_type>;
			return CollectionRange<value_type>( first, last );
		}

		template<typename Iterator>
		class ReferenceRange {
			using referenced_value_type = typename ::std::iterator_traits<Iterator>::value_type;
			::std::vector<::std::reference_wrapper<referenced_value_type>> m_values;
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

			ReferenceRange( iterator first, iterator last ): m_values( impl::to_refvec( first, last ) ) { }

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

			auto front( ) {
				return m_values.front( );
			}

			auto front( ) const {
				return m_values.front( );
			}

			auto back( ) {
				return m_values.back( );
			}

			auto back( ) const {
				return m_values.back( );
			}

			auto size( ) const {
				return m_values.size( );
			}

			auto operator[]( size_t pos ) {
				return m_values[pos].get( );
			}

			auto operator[]( size_t pos ) const {
				return m_values[pos].get( );
			}

			bool operator==( ReferenceRange const & other ) const {
				return ::std::equal( begin( ), end( ), other.begin( ) );
			}

			bool operator!=( ReferenceRange const & other ) const {
				return !::std::equal( begin( ), end( ), other.begin( ) );
			}

			ReferenceRange sort( ) const & {
				ReferenceRange result( *this );
				::std::sort( ::std::begin( result ), ::std::end( result ) );
				return result;
			}

			auto sort( ) && {
				auto result = make_collection_range( *this );
				::std::sort( ::std::begin( result ), ::std::end( result ) );
				return result;
			}

			template<typename UnaryPredicate>
			ReferenceRange sort( UnaryPredicate && predicate ) const {
				ReferenceRange result( *this );
				::std::sort( ::std::begin( result ), ::std::end( result ), ::std::forward<UnaryPredicate>( predicate ) );
				return result;
			}

			ReferenceRange stable_sort( ) const {
				ReferenceRange result( *this );
				::std::stable_sort( ::std::begin( result ), ::std::end( result ) );
				return result;
			}

			template<typename UnaryPredicate>
			ReferenceRange stable_sort( UnaryPredicate && predicate ) const {
				ReferenceRange result( *this );
				::std::stable_sort( ::std::begin( result ), ::std::end( result ), ::std::forward<UnaryPredicate>( predicate ) );
				return result;
			}

			ReferenceRange unique( ) const {
				ReferenceRange result( *this );
				auto & vals = result.m_values;
				vals.erase( ::std::unique( vals.begin( ), vals.end( ) ), vals.end( ) );
				return result;
			}

			template<typename UnaryPredicate>
			ReferenceRange unique( UnaryPredicate predicate ) {
				ReferenceRange result( *this );
				auto & vals = result.m_values;
				vals.erase( ::std::unique( vals.begin( ), vals.end( ), predicate ), vals.end( ) );
				return result;
			}

			template<typename Value>
			auto find( Value const & value ) const {
				return ::std::find( begin( ), end( ), value );
			}

			template<typename UnaryPredicate>
			auto find_if( UnaryPredicate predicate ) const {
				return ::std::find_if( begin( ), end( ), predicate );
			}

			template<typename UnaryPredicate>
			auto partition( UnaryPredicate predicate ) {
				ReferenceRange result( *this );
				return ::std::partition( ::std::begin( result ), ::std::end( result ), predicate );
			}

			template<typename UnaryPredicate>
			auto stable_partition( UnaryPredicate predicate ) const {
				ReferenceRange result( *this );
				return ::std::stable_partition( ::std::begin( result ), ::std::end( result ), predicate );
			}

			template<typename T>
			auto accumulate( T && init ) const {
				return ::std::accumulate( begin( ), end( ), ::std::forward<T>( init ) );
			}

			template<typename T, typename BinaryOperator>
			auto accumulate( T && init, BinaryOperator oper ) const {
				return ::std::accumulate( begin( ), end( ), ::std::forward<T>( init ), oper );
			}

			template<typename UnaryOperator>
			auto transform( UnaryOperator oper ) const {	// TODO verify result shouldn't be ref range
				using v_t = decltype(oper( *begin( ) ));
				auto result = make_collection_range<v_t>( );
				::std::transform( begin( ), end( ), ::std::back_inserter( result ), oper );
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

		private:
			template<typename UnaryPredicate>
			auto erase_impl( UnaryPredicate predicate ) {
				m_values.erase( ::std::remove_if( m_values.begin( ), m_values.end( ), predicate ), m_values.end( ) );
				return *this;
			}

		public:
			template<typename UnaryPredicate>
			auto erase( UnaryPredicate predicate ) const {
				return ReferenceRange( *this ).erase_impl( predicate );
			}

			template<typename Value>
			auto erase_where_equal_to( Value const & value ) const {
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

			template<typename Value>
			auto where_equal_to( Value const & value ) const {
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
			ReferenceRange for_each( Function function ) const {
				for( auto const & v : m_values ) {
					function( v.get( ) );
				}
				return *this;
			}

			
			ReferenceRange shuffle( ) const {
				static std::random_device rd;
				static std::mt19937 g( rd( ) );
				ReferenceRange result( *this );
				::std::shuffle( result.begin( ), result.end( ), g );
				return result;
			}

			template<typename UniformRandomNumberGenerator>
			ReferenceRange shuffle( UniformRandomNumberGenerator && urng ) const {
				ReferenceRange result( *this );
				::std::shuffle( result.begin( ), result.end( ), std::forward<UniformRandomNumberGenerator>( urng ) );
				return result;
			}
		};	// class ReferenceRange

		template<typename Iterator>
		::std::ostream & operator<<( ::std::ostream& os, ReferenceRange<Iterator> const & rng ) {
			os << "{";
			if( !rng.empty( ) ) {
				for( auto it = rng.cbegin( ); it != rng.cend( ); ++it ) {
					os << " " << it->get( );
				}
			}
			os << " }";
			return os;
		}

		

		template<typename Container, typename ::std::enable_if<daw::traits::is_container_not_string<Container>::value, long>::type = 0>
		auto make_ref_range( Container const & container ) {
			using iterator = decltype(::std::begin( container ));
			return ReferenceRange<iterator>( ::std::begin( container ), ::std::end( container ) );
		}

		template<typename Iterator>
		auto make_ref_range( ReferenceRange<Iterator> const & container ) {
			return ReferenceRange<Iterator>( container );
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

		template<typename T>
		struct CollectionRange {
			using value_type = ::std::decay_t<T>;
			using values_type = ::std::vector<value_type>;
		private:
			values_type m_values;
			CollectionRange( ) = default;

			template<typename Collection>
			CollectionRange( Collection const & collection ): m_values( impl::to_vector( collection ) ) { }
			
			template<typename IteratorF, typename IteratorL>
			CollectionRange( IteratorF first, IteratorL last ): m_values( impl::to_vector( first, last ) ) { }


		public:
			using reference = typename values_type::reference;
			using const_reference = typename values_type::const_reference;
			using iterator = typename values_type::iterator;
			using const_iterator = typename values_type::const_iterator;
			using difference_type = typename ::std::iterator_traits<iterator>::difference_type;

			template<typename Collection>
			friend auto make_collection_range( Collection && collection );
			
			template<typename ValueType>
			friend auto make_collection_range( );

			template<typename IteratorF, typename IteratorL>
			friend auto make_collection_range( IteratorF, IteratorL );

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

			auto front( ) {
				return m_values.front( );
			}

			auto front( ) const {
				return m_values.front( );
			}

			auto back( ) {
				return m_values.back( );
			}

			auto back( ) const {
				return m_values.back( );
			}

			auto size( ) const {
				return m_values.size( );
			}

			auto operator[]( size_t pos ) {
				return m_values[pos].get( );
			}

			auto operator[]( size_t pos ) const {
				return m_values[pos].get( );
			}

			bool operator==( CollectionRange const & other ) const {
				return ::std::equal( begin( ), end( ), other.begin( ) );
			}

			bool operator!=( CollectionRange const & other ) const {
				return !::std::equal( begin( ), end( ), other.begin( ) );
			}

			template<typename... Args>
			auto push_back( Args &&... args ) {
				return m_values.push_back( std::forward<Args>( args )... );
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
			auto accumulate( U && init ) {
				return ::std::accumulate( begin( ), end( ), ::std::forward<U>( init ) );
			}

			template<typename U, typename BinaryOperator>
			auto accumulate( U && init, BinaryOperator oper ) {
				return ::std::accumulate( begin( ), end( ), ::std::forward<U>( init ), oper );
			}

			template<typename Value>
			bool contains( Value const & value ) const {
				return ::std::find( begin( ), end( ), value ) != end( );
			}

			template<typename Value, typename UnaryPredicate>
			bool contains( Value const & value, UnaryPredicate predicate ) {
				auto pred2 = [&value, &predicate]( Value const & val ) {
					return predicate( value, val );
				};
				return ::std::find_if( begin( ), end( ), pred2 ) != end( );
			}


			auto sort( ) & {
				auto result = make_ref_range( *this );
				::std::sort( ::std::begin( result ), ::std::end( result ) );
				return result;
			}

			CollectionRange sort( ) && {
				::std::sort( begin( ), end( ) );
				return ::std::move( *this );
			}

			template<typename UnaryPredicate>
			auto sort( UnaryPredicate predicate ) & {
				auto result = make_ref_range( *this );
				::std::sort( ::std::begin( result ), ::std::end( result ), ::std::forward<UnaryPredicate>( predicate ) );
				return result;
			}

			template<typename UnaryPredicate>
			CollectionRange sort( UnaryPredicate predicate ) && {
				::std::sort( begin( ), end( ), predicate );
				return ::std::move( *this );
			}

			auto stable_sort( ) & {
				auto result = make_ref_range( *this );
				::std::stable_sort( ::std::begin( result ), ::std::end( result ) );
				return result;
			}

			CollectionRange stable_sort( ) && {
				::std::stable_sort( begin( ), end( ) );
				return ::std::move( *this );
			}

			template<typename UnaryPredicate>
			auto stable_sort( UnaryPredicate predicate ) & {
				auto result = make_ref_range( *this );
				::std::stable_sort( ::std::begin( result ), ::std::end( result ), ::std::forward<UnaryPredicate>( predicate ) );
				return result;
			}

			template<typename UnaryPredicate>
			CollectionRange stable_sort( UnaryPredicate predicate ) && {
				::std::stable_sort( begin( ), end( ), predicate );
				return ::std::move( *this );
			}

			auto unique( ) & {
				auto result = make_ref_range( *this );
				auto & vals = result.m_values;
				vals.erase( ::std::unique( vals.begin( ), vals.end( ) ), vals.end( ) );
				return result;
			}

			CollectionRange unique( ) && {
				m_values.erase( ::std::unique( m_values.begin( ), m_values.end( ) ), m_values.end( ) );
				return ::std::move( *this );
			}

			template<typename UnaryPredicate>
			auto unique( UnaryPredicate predicate ) & {
				auto result = make_ref_range( *this );
				auto & vals = result.m_values;
				vals.erase( ::std::unique( vals.begin( ), vals.end( ), predicate ), vals.end( ) );
				return result;
			}

			template<typename UnaryPredicate>
			CollectionRange unique( UnaryPredicate predicate ) && {
				m_values.erase( ::std::unique( m_values.begin( ), m_values.end( ), predicate ), m_values.end( ) );
				return ::std::move( *this );
			}

			template<typename UnaryPredicate>
			auto partition( UnaryPredicate predicate ) & {
				auto result = make_ref_range( *this );
				::std::partition( ::std::begin( result ), ::std::end( result ), predicate );
				return result;
			}

			template<typename UnaryPredicate>
			CollectionRange partition( UnaryPredicate predicate ) && {
				::std::partition( begin( ), end( ), predicate );
				return std::move( *this );
			}

			template<typename UnaryPredicate>
			auto stable_partition( UnaryPredicate predicate ) & {
				auto result = make_ref_range( *this );
				::std::stable_partition( ::std::begin( result ), ::std::end( result ), predicate );
				return result;
			}

			template<typename UnaryPredicate>
			CollectionRange stable_partition( UnaryPredicate predicate ) && {
				::std::stable_partition( begin( ), end( ), predicate );
				return std::move( *this );
			}

			template<typename UnaryOperator>
			CollectionRange transform( UnaryOperator oper ) {
				using v_t = ::std::decay_t<decltype(oper( front( ) ))>;
				auto result = make_collection_range<v_t>( );
				::std::transform( ::std::begin( m_values ), ::std::end( m_values ), ::std::back_inserter( result ), oper );
				return result;
			}

		private:
			template<typename UnaryPredicate>
			auto erase_impl( UnaryPredicate predicate ) {
				m_values.erase( ::std::remove_if( m_values.begin( ), m_values.end( ), predicate ), m_values.end( ) );
				return *this;
			}

		public:
			// TODO: get erase into reference range
			template<typename UnaryPredicate>
			CollectionRange erase( UnaryPredicate predicate ) && {
				erase_impl( predicate );
				return std::move( *this );
			}

			template<typename Value>
			auto erase_where_equal_to( Value const & value ) && {
				return erase( [&value]( auto const & current_value ) {
					return value == current_value;
				} );
			}

			template<typename UnaryPredicate>
			auto where( UnaryPredicate predicate ) && {
				return erase( [predicate]( auto const & v ) {
					return !predicate( v );
				} );
			}

			template<typename Value>
			auto where_equal_to( Value const & value ) && {
				return where( [&value]( auto const & current_value ) {
					return value == current_value;
				} );
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
			CollectionRange for_each( Function function ) const {
				for( auto const & v : m_values ) {
					function( v.get( ) );
				}
				return *this;
			}

			CollectionRange shuffle( ) && {
				static std::random_device rd;
				static std::mt19937 g( rd( ) );
				CollectionRange result( *this );
				::std::shuffle( result.begin( ), result.end( ), g );
				return result;
			}

			template<typename  UniformRandomNumberGenerator>
			CollectionRange shuffle( UniformRandomNumberGenerator && urng ) && {
				CollectionRange result( *this );
				::std::shuffle( result.begin( ), result.end( ), std::forward<UniformRandomNumberGenerator>( urng ) );
				return result;
			}
		};	// struct CollectionRange
	}	// namespace range
}	// namespace daw
