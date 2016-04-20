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

			ReferenceRange( Iterator First, Iterator Last ): m_values( impl::to_refvec( First, Last ) ) { }

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

			ReferenceRange sort( ) {
				ReferenceRange result( *this );
				::std::sort( ::std::begin( result ), ::std::end( result ) );
				return result;
			}

			template<typename UnaryPredicate>
			ReferenceRange sort( UnaryPredicate && predicate ) {
				ReferenceRange result( *this );
				::std::sort( ::std::begin( result ), ::std::end( result ), ::std::forward<UnaryPredicate>( predicate ) );
				return result;
			}

			ReferenceRange stable_sort( ) {
				ReferenceRange result( *this );
				::std::stable_sort( ::std::begin( result ), ::std::end( result ) );
				return result;
			}

			template<typename UnaryPredicate>
			ReferenceRange stable_sort( UnaryPredicate && predicate ) {
				ReferenceRange result( *this );
				::std::stable_sort( ::std::begin( result ), ::std::end( result ), ::std::forward<UnaryPredicate>( predicate ) );
				return result;
			}

			ReferenceRange unique( ) {
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
			auto stable_partition( UnaryPredicate predicate ) {
				ReferenceRange result( *this );
				return ::std::stable_partition( ::std::begin( result ), ::std::end( result ), predicate );
			}

			template<typename T>
			auto accumulate( T && init ) {
				return ::std::accumulate( begin( ), end( ), ::std::forward<T>( init ) );
			}

			template<typename T, typename BinaryOperator>
			auto accumulate( T && init, BinaryOperator oper ) {
				return ::std::accumulate( begin( ), end( ), ::std::forward<T>( init ), oper );
			}

			template<typename UnaryOperator>
			auto map( UnaryOperator oper ) {	// TODO verify result shouldn't be ref range
				using v_t = decltype(oper( *begin( ) ));
				using result_t = ::std::vector<v_t>;
				result_t result;
				::std::transform( begin( ), end( ), ::std::back_inserter( result ), oper );
				return result;
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

		private:
			template<typename UnaryPredicate>
			auto erase_impl( UnaryPredicate predicate ) {
				m_values.erase( ::std::remove_if( m_values.begin( ), m_values.end( ), predicate ), m_values.end( ) );
				return *this;
			}

		public:
			template<typename UnaryPredicate>
			auto erase( UnaryPredicate predicate ) {
				return ReferenceRange( *this ).erase_impl( predicate );
			}

			template<typename Value>
			auto erase_where_equal_to( Value const & value ) {
				return erase( [&value]( auto const & current_value ) {
					return value == current_value;
				} );
			}

			template<typename UnaryPredicate>
			auto where( UnaryPredicate predicate ) {
				return erase( [predicate]( auto const & v ) {
					return !predicate( v );
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
			ReferenceRange for_each( Function function ) const {
				for( auto const & v : m_values ) {
					function( v.get( ) );
				}
				return *this;
			}

			
			ReferenceRange shuffle( ) {
				static std::random_device rd;
				static std::mt19937 g( rd( ) );
				ReferenceRange result( *this );
				::std::shuffle( result.begin( ), result.end( ), g );
				return result;
			}

			template<typename  UniformRandomNumberGenerator>
			ReferenceRange shuffle( UniformRandomNumberGenerator && urng ) {
				ReferenceRange result( *this );
				::std::shuffle( result.begin( ), result.end( ), std::forward<UniformRandomNumberGenerator>( urng ) );
				return result;
			}
		};	// class ReferenceRange

		template<typename Container, typename ::std::enable_if<daw::traits::is_container_not_string<Container>::value, long>::type = 0>
		auto make_ref_range( Container & container ) {
			using iterator = decltype(::std::begin( container ));
			return ReferenceRange<iterator>( ::std::begin( container ), ::std::end( container ) );
		}

		template<typename Container, typename ::std::enable_if<daw::traits::is_container_not_string<Container>::value, long>::type = 0>
		auto make_cref_range( Container const & container ) {
			using iterator = decltype(::std::begin( container ));
			return ReferenceRange<iterator>( ::std::begin( container ), ::std::end( container ) );
		}

		template<typename Iterator>
		struct Range {
			using value_type = typename ::std::iterator_traits<Iterator>::value_type;
			using reference = typename ::std::iterator_traits<Iterator>::reference;
			using const_reference = const reference;
			using iterator = Iterator;
			using const_iterator = const iterator;
			using difference_type = typename ::std::iterator_traits<Iterator>::difference_type;
		private:
			iterator m_begin;
			iterator m_end;
		public:
			Range( ) = default;
			Range( Range const & ) = default;
			Range( Range && ) = default;
			~Range( ) = default;
			Range & operator=( Range const & ) = default;
			Range & operator=( Range && ) = default;

			Range( Iterator First, Iterator Last ):
				m_begin( First ),
				m_end( Last ) { }

			Range& move_next( ) {
				assert( m_begin != m_end );
				++m_begin;
				return *this;
			}

			Range& move_back( ) {
				--m_begin;
				return *this;
			}

			Range& move_back( Iterator start ) {
				assert( m_begin > start );
				--m_begin;
				return *this;
			}

			bool at_end( ) const {
				return !(m_begin != m_end);
			}

			bool empty( ) const {
				return !(m_begin != m_end);
			}

			iterator begin( ) {
				return m_begin;
			}

			void advance( difference_type n ) {
				::std::advance( m_begin, n );
			}

			void set_begin( iterator i ) {
				m_begin = i;
			}

			iterator end( ) {
				return m_end;
			}

			void set_end( iterator i ) {
				m_end = i;
			}

			void set( iterator First, iterator Last ) {
				m_begin = First;
				m_end = Last;
			}

			const_iterator begin( ) const {
				return m_begin;
			}

			const_iterator end( ) const {
				return m_end;
			}

			const_iterator cbegin( ) const {
				return m_begin;
			}

			const_iterator cend( ) const {
				return m_end;
			}

			reference front( ) {
				return *m_begin;
			}

			const_reference front( ) const {
				return *m_begin;
			}

			reference back( ) {
				auto it = m_begin;
				::std::advance( it, size( ) - 1 );
				return *it;
			}

			const_reference back( ) const {
				auto it = m_begin;
				::std::advance( it, size( ) - 1 );
				return *it;
			}

			reference operator*( ) {
				return *m_begin;
			}

			const_reference operator*( ) const {
				return *m_begin;
			}

			size_t size( ) const {
				return static_cast<size_t>(::std::distance( m_begin, m_end ));
			}

			reference operator[]( size_t pos ) {
				return *(m_begin + pos);
			}

			const_reference operator[]( size_t pos ) const {
				return *(m_begin + pos);
			}

			bool operator==( Range const & other ) const {
				return ::std::equal( m_begin, m_end, other.m_begin );
			}

			bool operator!=( Range const & other ) const {
				return !::std::equal( m_begin, m_end, other.m_begin );
			}

			auto sort( ) {
				return make_ref_range( *this ).sort( );
			}

			template<typename UnaryPredicate>
			auto sort( UnaryPredicate predicate ) {
				return make_ref_range( *this ).sort( predicate );
			}

			auto unique( ) {
				return make_ref_range( *this ).unique( );
			}

			template<typename UnaryPredicate>
			auto unique( UnaryPredicate predicate ) {
				return make_ref_range( *this ).unique( predicate );
			}

			auto stable_sort( ) {
				return make_ref_range( *this ).stable_sort( );
			}

			template<typename UnaryPredicate>
			auto stable_sort( UnaryPredicate predicate ) {
				return make_ref_range( *this ).stable_sort( predicate );
			}

			template<typename Value>
			iterator find( Value const & value ) const {
				return ::std::find( m_begin, m_end, value );
			}

			template<typename UnaryPredicate>
			iterator find_if( UnaryPredicate predicate ) const {
				return ::std::find_if( m_begin, m_end, predicate );
			}

			template<typename Value>
			auto where_equal_to( Value const & value ) {
				return make_ref_range( *this ).where_equal_to( value );
			}

			template<typename UnaryPredicate>
			auto where( UnaryPredicate predicate ) {
				return make_ref_range( *this ).where( predicate );
			}

			template<typename Value>
			auto erase_where_equal_to( Value const & value ) {
				return make_ref_range( *this ).erase_equal_to( value );
			}

			template<typename UnaryPredicate>
			auto erase( UnaryPredicate predicate ) {
				return make_ref_range( *this ).erase( predicate );
			}

			template<typename UnaryPredicate>
			auto partition( UnaryPredicate predicate ) {
				return make_ref_range( *this ).partition( predicate );
			}

			template<typename UnaryPredicate>
			auto stable_partition( UnaryPredicate predicate ) {
				return make_ref_range( *this ).stable_partition( predicate );
			}

			template<typename T>
			T accumulate( T && init ) {
				return ::std::accumulate( m_begin, m_end, ::std::forward<T>( init ) );
			}

			template<typename T, typename BinaryOperator>
			T accumulate( T && init, BinaryOperator oper ) {
				return ::std::accumulate( m_begin, m_end, ::std::forward<T>( init ), oper );
			}

			template<typename UnaryOperator>
			auto map( UnaryOperator oper ) {	// TODO verify result shouldn't be ref range
				using v_t = decltype(oper( *begin( ) ));
				using result_t = ::std::vector<v_t>;
				result_t result;
				::std::transform( begin( ), end( ), ::std::back_inserter( result ), oper );
				return result;
			}

			template<typename Value>
			bool contains( Value const & value ) const {
				return ::std::find( m_begin, m_end, value ) != m_end;
			}

			template<typename Value, typename UnaryPredicate>
			bool contains( Value const & value, UnaryPredicate predicate ) {
				auto predicate2 = [&value, &predicate]( Value const & val ) {
					return predicate( value, val );
				};
				return ::std::find_if( m_begin, m_end, predicate2 ) != m_end;
			}

			auto shuffle( ) {
				return make_ref_range( *this ).shuffle( );
			}

			template<typename  UniformRandomNumberGenerator>
			auto shuffle( UniformRandomNumberGenerator && urng ) {
				return make_ref_range( *this ).shuffle( std::forward<UniformRandomNumberGenerator>( urng ) );
			}

		};	// struct Range

		template<typename Iterator>
		Range<Iterator> make_range( Iterator first, Iterator last ) {
			return Range<Iterator> { first, last };
		}

		template<typename Container, typename ::std::enable_if<daw::traits::is_container_not_string<Container>::value, long>::type = 0>
		auto make_range( Container & container ) {
			using Iterator = decltype(::std::begin( container ));
			return Range<Iterator>( ::std::begin( container ), ::std::end( container ) );
		}

		template<typename Container, typename ::std::enable_if<daw::traits::is_container_not_string<Container>::value, long>::type = 0>
		auto make_crange( Container const & container ) {
			using Iterator = decltype(::std::begin( container ));
			return Range<Iterator>( ::std::begin( container ), ::std::end( container ) );
		}

		template<typename Iterator>
		void safe_advance( Range<Iterator> & range, typename ::std::iterator_traits<Iterator>::difference_type count ) {
			assert( 0 <= count );
			if( ::std::distance( range.begin( ), range.end( ) ) >= count ) {
				range.advance( count );
			} else {
				range.set_begin( range.end( ) );
			}
		}

		template<typename Iterator>
		bool contains( Range<Iterator> const & range, typename ::std::iterator_traits<Iterator>::value_type const & key ) {
			return ::std::find( range.begin( ), range.end( ), key ) != range.end( );
		}

		template<typename Iterator>
		bool at_end( Range<Iterator> const & range ) {
			return range.begin( ) == range.end( );
		}
	}	// namespace range
}	// namespace daw
