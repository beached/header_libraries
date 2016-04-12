#pragma once
// The MIT License (MIT)
//
// Copyright (c) 2013-2015 Darrell Wright
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

#include "daw_string.h"
#include "daw_exception.h"

#include <algorithm>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <functional>
#include <iterator>
#include <type_traits>
#include <vector>

namespace daw {
	namespace algorithm {
		template<typename Container, typename UnaryPredicate>
		auto find_if( Container& container, UnaryPredicate predicate ) -> decltype(std::find_if( begin( container ), end( container ), predicate )) {
			using std::begin;
			using std::end;
			return std::find_if( begin( container ), end( container ), predicate );
		}

		template<typename Container, typename InputIterator>
		void safe_advance( Container& container, InputIterator& it, ptrdiff_t distance ) {
			if( 0 == distance ) {
				return;
			}
			using std::begin;
			using std::end;
			const auto it_pos = std::distance( begin( container ), it );

			if( 0 < distance ) {
				const auto size_of_container = std::distance( begin( container ), end( container ) );
				if( size_of_container <= static_cast<ptrdiff_t>(distance + it_pos) ) {
					distance = size_of_container - it_pos;
				}
			} else if( 0 < distance + it_pos ) {
				distance = it_pos;
			}
			std::advance( it, distance );
		}

		template<typename Container>
		auto begin_at( Container& container, size_t distance ) -> decltype(std::begin( container )) {
			using std::begin;
			using std::end;
			auto result = begin( container );
			safe_advance( container, result, static_cast<ptrdiff_t>(distance) );
			return result;
		}

		//////////////////////////////////////////////////////////////////////////
		/// Summary: Run func( container, position ) on each element
		/// in interval [first_inclusive, last_exclusive)
		///
		template<typename Container>
		void for_each_subset( Container& container, size_t first_inclusive, size_t last_exclusive, const std::function<void( decltype(container), size_t )> func ) {
			using std::begin;

			auto it = begin_at( container, first_inclusive );
			const auto last_it = begin_at( container, last_exclusive );

			auto& row = first_inclusive;
			for( ; it != last_it; ++it ) {
				func( container, row++ );
			}
		}

		//////////////////////////////////////////////////////////////////////////
		/// Summary: Run func( container, position ) on each element
		/// in interval [first_inclusive, last_exclusive)
		///
		template<typename Container, typename FunctionType>
		void for_each_with_pos( Container& container, size_t first_inclusive, size_t last_exclusive, FunctionType func ) {
			using std::begin;

			auto it = begin_at( container, first_inclusive );
			const auto last_it = begin_at( container, last_exclusive );

			for( ; it != last_it; ++it ) {
				auto row = std::distance( begin( container ), it );
				auto& current_value = *it;
				func( current_value, row );
			}
		}

		template<typename Container, typename FunctionType>
		void for_each_with_pos( Container& container, const FunctionType func ) {
			for_each_with_pos( container, 0, static_cast<size_t>(container.size( )), func );
		}

		inline boost::posix_time::ptime now( ) {
			return boost::posix_time::second_clock::local_time( );
		}


		template<class IteratorType, class ValueType, class Comp>
		IteratorType binary_search( const IteratorType first, const IteratorType last, const ValueType& value, Comp less_than ) {
			auto midpoint = []( const IteratorType& a, const IteratorType& b ) {
				dbg_throw_on_false( a <= b, string_join( __func__, ": Cannot find a midpoint unless the first parameter is <= the second" ) );
				const auto mid = std::distance( a, b ) / 2;
				auto result = a;
				std::advance( result, mid );
				return result;
			};
			dbg_throw_on_false( first < last, string_join( __func__, ": First position must be less than second" ) );
			IteratorType it_first( first );
			IteratorType it_last( last );

			while( it_first < it_last ) {
				auto mid = midpoint( it_first, it_last );
				if( less_than( mid, value ) ) {
					it_first = mid;
					std::advance( it_first, 1 );
				} else if( less_than( value, mid ) ) {
					it_last = mid;
				} else {	// equal
					return mid;
				}
			}
			return last;
		}

		template<class Container, class Value>
		bool contains( const Container& container, const Value& value ) {
			using std::begin;
			using std::end;
			return std::find( begin( container ), end( container ), value ) != end( container );
		}

		template<class Container>
		auto rbegin2( Container& container ) -> decltype(container.rbegin( )) {
			return container.rbegin( );
		}

		template<class Container>
		auto crbegin2( Container& container ) -> decltype(container.crbegin( )) {
			return container.crbegin( );
		}

		template<class Container>
		auto rend2( Container& container ) -> decltype(container.rend( )) {
			return container.rend( );
		}

		template<class Container>
		auto crend2( Container& container ) -> decltype(container.crend( )) {
			return container.crend( );
		}

		template<class Value>
		inline const Value* to_array( const std::vector<Value>& values ) {
			return &(values[0]);
		}

		template<class Value>
		inline Value* to_array( std::vector<Value>& values ) {
			return &(values[0]);
		}

		template<typename Ptr>
		inline bool is_null_ptr( Ptr const * const ptr ) {
			return nullptr == ptr;
		}


		/// Reverser eg for( auto item: reverse( container ) ) { }
		namespace details {
			template<class Fwd>
			struct Reverser_generic {
				Fwd &fwd;
				Reverser_generic( Fwd& fwd_ ) : fwd( fwd_ ) { }
				typedef std::reverse_iterator<typename Fwd::iterator> reverse_iterator;
				reverse_iterator begin( ) { return reverse_iterator( std::end( fwd ) ); }
				reverse_iterator end( ) { return reverse_iterator( std::begin( fwd ) ); }
			};

			template<class Fwd >
			struct Reverser_special {
				Fwd &fwd;
				Reverser_special( Fwd& fwd_ ) : fwd( fwd_ ) { }
				auto begin( ) -> decltype(fwd.rbegin( )) { return fwd.rbegin( ); }
				auto end( ) ->decltype(fwd.rbegin( )) { return fwd.rend( ); }
			};

			template<class Fwd>
			auto reverse_impl( Fwd& fwd, long ) -> decltype(Reverser_generic<Fwd>( fwd )) {
				return Reverser_generic<Fwd>( fwd );
			}

			template<class Fwd>
			auto reverse_impl( Fwd& fwd, int ) -> decltype(fwd.rbegin( ), Reverser_special<Fwd>( fwd )) {
				return Reverser_special<Fwd>( fwd );
			}
		}	// namespace details

		template<class Fwd>
		auto reverse( Fwd&& fwd ) -> decltype(details::reverse_impl( fwd, int( 0 ) )) {
			static_assert(!(std::is_rvalue_reference<Fwd&&>::value), "Cannot pass rvalue_reference to reverse()");
			return details::reverse_impl( fwd, int( 0 ) );
		}

		template<class ValueType>
		bool are_equal( ValueType ) {
			return true;
		}

		template<class ValueType>
		bool are_equal( ValueType v1, ValueType v2 ) {
			return v1 == v2;
		}

		template<class ValueType, class ...Values>
		bool are_equal( ValueType v1, ValueType v2, Values... others ) {
			return are_equal( v1, v2 ) && are_equal( others... );
		}


		template<typename Iterator>
		std::pair<Iterator, Iterator> slide( Iterator first, Iterator last, Iterator target ) {
			if( target < first ) {
				return std::make_pair( target, std::rotate( target, first, last ) );
			}

			if( last < target ) {
				return std::make_pair( std::rotate( first, last, target ), target );
			}

			return std::make_pair( first, last );
		}

		template<typename Iterator, typename Predicate>
		std::pair<Iterator, Iterator> gather( Iterator first, Iterator last, Iterator target, Predicate predicate ) {
			auto start = std::stable_partition( first, target, std::not1( predicate ) );
			auto finish = std::stable_partition( target, last, predicate );
			return std::make_pair( start, finish );
		}


// 		template<typename InputIt1, typename InputIt2, typename OutputIt, typename UnaryOperation>
// 		OutputIt transform_many( InputIt1 first1, InputIt1 last1, InputIt2 first2, OutputIt d_first, UnaryOperation unary_op ) {
// 			while( first1 != last1 ) {
// 				*d_first++ = unary_op( *first1++, *first2++ );
// 			}
// 			return d_first;
// 		}

		template<typename InputIt1, typename OutputIt, typename UnaryOperation, typename ...InputIt2>
		OutputIt transform_many( InputIt1 first1, InputIt1 last1, InputIt2... additional_inputs, OutputIt d_first, UnaryOperation unary_op ) {
			while( first1 != last1 ) {
				*d_first++ = unary_op( *first1++, (*additional_inputs++)... );
			}
			return d_first;
		}

	}	// namespace algorithm
}	// namespace daw

template<typename T>
void Unused( T&& ) { };
