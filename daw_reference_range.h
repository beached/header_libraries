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
			template<typename T>
			using cleanup_t = ::std::remove_cv_t<::std::remove_reference_t<T>>;

			template<typename T>
			struct Reference {
				using ptr_type = ::std::decay_t<::std::remove_reference_t<T>>;
				ptr_type const * ptr;
				Reference( ) = delete;
				Reference( T const & value ): ptr( &value ) { }
				~Reference( ) { }

				Reference( Reference const & ) = default;
				Reference( Reference && ) = default;
				Reference & operator=( Reference const & ) = default;
				Reference & operator=( Reference && ) = default;
				
				T const & operator*( ) const {
					return *ptr;
				}

				T const & get( ) const {
					return *ptr;
				}
			};	// struct Reference

			template<typename T>
			bool operator==( Reference<T> const & lhs, Reference<T> const & rhs ) {
				return *lhs == *rhs; 
			}

			template<typename T>
			bool operator!=( Reference<T> const & lhs, Reference<T> const & rhs ) {
				return *lhs != *rhs;
			}

			template<typename T>
			bool operator<( Reference<T> const & lhs, Reference<T> const & rhs ) {
				return *lhs < *rhs;
			}

			template<typename T>
			bool operator<=( Reference<T> const & lhs, Reference<T> const & rhs ) {
				return *lhs <= *rhs;
			}

			template<typename T>
			bool operator>( Reference<T> const & lhs, Reference<T> const & rhs ) {
				return *lhs > *rhs;
			}

			template<typename T>
			bool operator>=( Reference<T> const & lhs, Reference<T> const & rhs ) {
				return *lhs >= *rhs;
			}

			template<typename T>
			auto ref( T const * value ) {
				return Reference<T>( *value );
			}

			template<typename T, typename=void>
			auto ref( T const & value ) {
				return Reference<T>( value );
			}
			
			template<typename Iterator>
			auto to_refvec( Iterator first, Iterator last ) {
				using value_type = ::std::remove_const_t<::std::remove_reference_t<decltype(*first)>>;
				using values_type = ::std::vector<::daw::range::impl::Reference<value_type>>;
				values_type result;
				for( auto it = first; it != last; ++it ) {
					auto v = ::daw::range::impl::ref<value_type>( *it );
					result.push_back( v );
				}
				return result;
			}
			
	
		}	// namespace impl

		template<typename ValueType> struct CollectionRange;
		template<typename Iterator> class ReferenceRange;

		template<typename ValueType>
		CollectionRange<ValueType> make_collection_range( ) {
			return CollectionRange<ValueType>{ };
		}

		template<typename Container, typename = decltype(::std::begin( Container { } ))>
		auto make_collection_range( Container const & container ) {
			using ValueType = impl::cleanup_t<typename ::std::iterator_traits<typename Container::iterator>::value_type>;
			return CollectionRange<ValueType>( container );
		}

		template<typename IteratorF, typename IteratorL>
		auto make_collection_range( IteratorF first, IteratorL last ) {
			using ValueType = impl::cleanup_t<typename ::std::iterator_traits<IteratorF>::value_type>;
			return CollectionRange<ValueType>( first, last );
		}

		template<typename Iterator>
		auto make_collection_range( ReferenceRange<Iterator> const & collection ) {
			using ValueType = impl::cleanup_t<typename ::std::iterator_traits<Iterator>::value_type>;
			CollectionRange<ValueType> result;
			std::transform( ::std::begin( collection ), ::std::end( collection ), ::std::back_inserter( result ), []( auto const & rv ) {
				return rv.get( );
			} );
			return result;
		}

		template<typename Iterator>
		class ReferenceRange {
			using referenced_value_type = ::std::remove_cv_t<typename ::std::iterator_traits<Iterator>::value_type>;
			using values_type = ::std::vector<::daw::range::impl::Reference<referenced_value_type>>;
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

			template<typename Value>
			auto find( Value const & value ) const {
				return ::std::find( begin( ), end( ), value );
			}

			template<typename UnaryPredicate>
			auto find_if( UnaryPredicate predicate ) const {
				return ::std::find_if( begin( ), end( ), predicate );
			}

			auto sort( ) {
				::std::sort( begin( ), end( ) );
				return *this;
			}

			ReferenceRange sort( ) const {
				return ReferenceRange( *this ).sort( );
			}

			template<typename UnaryPredicate>
			ReferenceRange sort( UnaryPredicate predicate ) {
				::std::sort( begin( ), end( ), [&predicate]( auto const & v ) {
					return predicate( v.get( ) );
				} );
				return *this;
			}

			template<typename UnaryPredicate>
			ReferenceRange sort( UnaryPredicate predicate ) const {
				return ReferenceRange( *this ).sort( predicate );
			}

			ReferenceRange stable_sort( ) {
				::std::stable_sort( begin( ), end( ) );
				return *this;
			}

			ReferenceRange stable_sort( ) const {
				return ReferenceRange( *this ).stable_sort( );
			}

			template<typename UnaryPredicate>
			ReferenceRange stable_sort( UnaryPredicate predicate ) {
				::std::stable_sort( begin( ), end( ), [&predicate]( auto const & v ) {
					return predicate( v.get( ) );
				} );
				return *this;
			}

			template<typename UnaryPredicate>
			ReferenceRange stable_sort( UnaryPredicate predicate ) const {
				return ReferenceRange( *this ).sort( predicate );
			}

			ReferenceRange unique( ) {
				erase( ::std::unique( begin( ), end( ) ), end( ) );
				return *this;
			}

			ReferenceRange unique( ) const {
				return ReferenceRange( *this ).unique( );
			}

			template<typename UnaryPredicate>
			ReferenceRange unique( UnaryPredicate predicate ) {
				erase( ::std::unique( begin( ), end( ), [&predicate]( auto const & v ) {
					return predicate( v.get( ) );
				} ), end( ) );
				return *this;
			}

			template<typename UnaryPredicate>
			ReferenceRange unique( UnaryPredicate predicate ) const {
				return ReferenceRange( *this ).unique( predicate );
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
				return ReferenceRange( *this ).partition( predicate );
			}

			template<typename UnaryPredicate>
			auto partition_it( UnaryPredicate predicate ) {
				auto mid = ::std::partition( begin( ), end( ), predicate );
				return ::std::make_pair( mid, *this );
			}

			template<typename UnaryPredicate>
			auto partition_it( UnaryPredicate predicate ) const {
				return ReferenceRange( *this ).partition_it( predicate );
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
				return ReferenceRange( *this ).stable_partition( predicate );
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
				return ReferenceRange( *this ).partition_it( predicate );
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
				return ReferenceRange( *this ).erase( predicate );
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
			ReferenceRange for_each( Function function ) const {
				for( auto const & v : m_values ) {
					function( v.get( ) );
				}
				return *this;
			}

			ReferenceRange shuffle( ) {
				static std::random_device rd;
				static std::mt19937 g( rd( ) );
				::std::shuffle( begin( ), end( ), g );
				return *this;
			}

	
			ReferenceRange shuffle( ) const {
				return ReferenceRange( *this ).shuffle( );
			}

			template<typename UniformRandomNumberGenerator>
			ReferenceRange shuffle( UniformRandomNumberGenerator && urng ) {
				::std::shuffle( begin( ), end( ), std::forward<UniformRandomNumberGenerator>( urng ) );
				return *this;
			}

			template<typename UniformRandomNumberGenerator>
			ReferenceRange shuffle( UniformRandomNumberGenerator && urng ) const {
				return ReferenceRange( *this ).shuffle( std::forward<UniformRandomNumberGenerator>( urng ) );
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

		template<typename T>
		struct CollectionRange {
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
				return make_ref_range( *this ).sort( );
			}

			CollectionRange sort( ) && {
				::std::sort( begin( ), end( ) );
				return ::std::move( *this );
			}

			template<typename UnaryPredicate>
			auto sort( UnaryPredicate predicate ) const & {
				return make_ref_range( *this ).sort( predicate );
			}

			template<typename UnaryPredicate>
			CollectionRange sort( UnaryPredicate predicate ) && {
				::std::sort( begin( ), end( ), predicate );
				return ::std::move( *this );
			}

			auto stable_sort( ) const & {
				return make_ref_range( *this ).stable_sort( );
			}

			CollectionRange stable_sort( ) && {
				::std::stable_sort( begin( ), end( ) );
				return ::std::move( *this );
			}

			template<typename UnaryPredicate>
			auto stable_sort( UnaryPredicate predicate ) const & {
				return make_ref_range( *this ).stable_sort( predicate );
			}

			template<typename UnaryPredicate>
			CollectionRange stable_sort( UnaryPredicate predicate ) && {
				::std::stable_sort( begin( ), end( ), predicate );
				return ::std::move( *this );
			}

			auto unique( ) const & {
				return make_ref_range( *this ).unique( );
			}

			CollectionRange unique( ) && {
				m_values.erase( ::std::unique( m_values.begin( ), m_values.end( ) ), m_values.end( ) );
				return ::std::move( *this );
			}

			template<typename UnaryPredicate>
			auto unique( UnaryPredicate predicate ) const & {
				return make_ref_range( *this ).unique( predicate );
			}

			template<typename UnaryPredicate>
			CollectionRange unique( UnaryPredicate predicate ) && {
				m_values.erase( ::std::unique( m_values.begin( ), m_values.end( ), predicate ), m_values.end( ) );
				return ::std::move( *this );
			}

			template<typename UnaryPredicate>
			auto partition( UnaryPredicate predicate ) const & {
				return make_ref_range( *this ).partition( predicate );
			}

			template<typename UnaryPredicate>
			CollectionRange partition( UnaryPredicate predicate ) && {
				::std::partition( begin( ), end( ), predicate );
				return ::std::move( *this );
			}

			template<typename UnaryPredicate>
			auto stable_partition( UnaryPredicate predicate ) const & {
				return make_ref_range( *this ).stable_partition( predicate );
			}

			template<typename UnaryPredicate>
			CollectionRange stable_partition( UnaryPredicate predicate ) && {
				::std::stable_partition( begin( ), end( ), predicate );
				return ::std::move( *this );
			}

			template<typename UnaryOperator>
			CollectionRange & transform( UnaryOperator oper ) const & {
				using v_t = ::std::decay_t<decltype(oper( front( ) ))>;
				auto result = make_collection_range<v_t>( );
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
				return make_ref_range( *this ).erase( predicate );
			}

			template<typename Value>
			auto erase_where_equal_to( Value const & value ) && {
				return erase( [&value]( auto const & current_value ) {
					return value == current_value;
				} );
			}

			template<typename Value>
			auto erase_where_equal_to( Value const & value ) const & {
				return make_ref_range( *this ).erase_where_equal_to( value );
			}

			template<typename UnaryPredicate>
			auto where( UnaryPredicate predicate ) && {
				return erase( [predicate]( auto const & v ) {
					return !predicate( v );
				} );
			}

			template<typename UnaryPredicate>
			auto where( UnaryPredicate predicate ) const & {
				return make_ref_range( *this ).where( predicate );
			}

			template<typename Value>
			auto where_equal_to( Value const & value ) && {
				return where( [&value]( auto const & current_value ) {
					return value == current_value;
				} );
			}

			template<typename Value>
			auto where_equal_to( Value const & value ) const & {
				return make_ref_range( *this ).where_equal_to( value );
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
				return make_ref_range( *this ).shuffle( std::forward<UniformRandomNumberGenerator>( urng ) );
			}

			CollectionRange & shuffle( ) && {
				static std::random_device rd;
				static std::mt19937 g( rd( ) );
				return shuffle( g );
			}
			
			auto shuffle( ) const & {
				return make_ref_range( *this ).shuffle( );
			}
		};	// struct CollectionRange
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


