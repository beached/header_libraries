#pragma once
// The MIT License (MIT)
//
// Copyright (c) 2013-2016 Darrell Wright
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
			return std::find_if( std::begin( container ), std::end( container ), predicate );
		}

		template<typename Container, typename InputIterator>
		void safe_advance( Container& container, InputIterator& it, ptrdiff_t distance ) {
			if( 0 == distance ) {
				return;
			}
			auto const it_pos = std::distance( std::begin( container ), it );

			if( distance > 0 ) {
				auto const size_of_container = std::distance( std::begin( container ), std::end( container ) );
				if( size_of_container <= static_cast<ptrdiff_t>(distance + it_pos) ) {
					distance = size_of_container - it_pos;
				}
			} else if( distance + it_pos > 0 ) {
				distance = it_pos;
			}
			std::advance( it, distance );
		}

		template<typename Container>
		auto begin_at( Container& container, size_t distance ) -> decltype(std::begin( container )) {
			auto result = std::begin( container );
			safe_advance( container, result, static_cast<ptrdiff_t>(distance) );
			return result;
		}

		//////////////////////////////////////////////////////////////////////////
		/// Summary: Run func( container, position ) on each element
		/// in interval [first_inclusive, last_exclusive)
		///
		template<typename Container>
		void for_each_subset( Container& container, size_t first_inclusive, size_t last_exclusive, const std::function<void( decltype(container), size_t )> func ) {
			auto it = begin_at( container, first_inclusive );
			auto const last_it = begin_at( container, last_exclusive );

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
			auto it = begin_at( container, first_inclusive );
			auto const last_it = begin_at( container, last_exclusive );

			for( ; it != last_it; ++it ) {
				auto row = std::distance( std::begin( container ), it );
				func( *it, row );
			}
		}

		template<typename Container, typename FunctionType>
		void for_each_with_pos( Container & container, FunctionType const func ) {
			for_each_with_pos( container, 0, static_cast<size_t>(container.size( )), func );
		}

		inline boost::posix_time::ptime now( ) {
			return boost::posix_time::second_clock::local_time( );
		}


		template<class IteratorType, class ValueType, class Comp>
		IteratorType binary_search( const IteratorType first, const IteratorType last, const ValueType& value, Comp less_than ) {
			auto midpoint = []( const IteratorType& a, const IteratorType& b ) {
				daw::exception::dbg_throw_on_false( a <= b, daw::string::string_join( __func__, ": Cannot find a midpoint unless the first parameter is <= the second" ) );
				auto const mid = std::distance( a, b ) / 2;
				auto result = a;
				std::advance( result, mid );
				return result;
			};
			daw::exception::dbg_throw_on_false( first < last, daw::string::string_join( __func__, ": First position must be less than second" ) );
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
				Reverser_generic( Fwd& fwd_ ): fwd( fwd_ ) { }
				typedef std::reverse_iterator<typename Fwd::iterator> reverse_iterator;
				reverse_iterator begin( ) { return reverse_iterator( std::end( fwd ) ); }
				reverse_iterator end( ) { return reverse_iterator( std::begin( fwd ) ); }
			};

			template<class Fwd >
			struct Reverser_special {
				Fwd &fwd;
				Reverser_special( Fwd& fwd_ ): fwd( fwd_ ) { }
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

		template<typename T>
		constexpr bool is_const_v = std::is_const<T>::value;

		template<typename T>
		using is_const_t = typename std::is_const<T>::type;

		template<typename InputIt1, typename InputIt2, typename OutputIt, typename Func>
		OutputIt transform_many( InputIt1 first1, InputIt1 last1, InputIt2 first2, OutputIt first_out, Func func ) {
			static_assert(!is_const_v<decltype(*first_out)>, "Output iterator cannot point to const data");
			while( first1 != last1 ) {
				*first_out++ = func( *first1++, *first2++ );
			}
			return first_out;
		}

		template<typename InputIt1, typename InputIt2, typename InputIt3, typename OutputIt, typename Func>
		OutputIt transform_many( InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt3 first3, OutputIt first_out, Func func ) {
			static_assert(!is_const_v<decltype(*first_out)>, "Output iterator cannot point to const data");
			while( first1 != last1 ) {
				*first_out++ = func( *first1++, *first2++, *first3++ );
			}
			return first_out;
		}

		template<typename InputIt1, typename InputIt2, typename InputIt3, typename InputIt4, typename OutputIt, typename Func>
		OutputIt transform_many( InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt3 first3, InputIt4 first4, OutputIt first_out, Func func ) {
			static_assert(!is_const_v<decltype(*first_out)>, "Output iterator cannot point to const data");
			while( first1 != last1 ) {
				*first_out++ = func( *first1++, *first2++, *first3++, *first4++ );
			}
			return first_out;
		}
		template<typename InputIt1, typename InputIt2, typename InputIt3, typename InputIt4, typename InputIt5, typename OutputIt, typename Func>
		OutputIt transform_many( InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt3 first3, InputIt4 first4, InputIt4 first5, OutputIt first_out, Func func ) {
			static_assert(!is_const_v<decltype(*first_out)>, "Output iterator cannot point to const data");
			while( first1 != last1 ) {
				*first_out++ = func( *first1++, *first2++, *first3++, *first4++, *first5++ );
			}
			return first_out;
		}

		struct enum_class_hash {
			template<typename T>
			size_t operator( )( T value ) {
				return static_cast<size_t>(value);
			}
		};	// struct enum_class_hash


		template<typename T>
		T clamp( T const & value, T const & max_value ) {
			if( value > max_value ) {
				return max_value;
			}
			return value;
		}

		template<std::size_t N>
		struct tuple_functor {
			template<typename T, typename F>
			static void run( std::size_t i, T&& t, F&& f ) {
				const std::size_t I = (N - 1);
				switch( i ) {
				case I:
					std::forward<F>( f )(std::get<I>( std::forward<T>( t ) ));
					break;
				default:
					tuple_functor<I>::run( i, std::forward<T>( t ), std::forward<F>( f ) );
				}
			}
		};	// struct tuple_functor

		template<>
		struct tuple_functor<0> {
			template<typename T, typename F>
			static void run( std::size_t, T, F ) { }
		};	// struct tuple_functor

		template<typename Iterator, typename UnaryPredicate>
		auto find_last_of( Iterator first, Iterator last, UnaryPredicate pred ) {
			auto prev = last;
			while( first != last ) {
				if( !pred( *first ) ) {
					break;
				}
				prev = first;
				++first;
			}
			return prev;
		}

		template<typename Iterator, typename UnaryPredicate>
		auto find_first_of( Iterator first, Iterator last, UnaryPredicate pred ) {
			while( first != last ) {
				if( pred( *first ) ) {
					break;
				}
				++first;
			}
			return first;
		}

		template<typename Iterator, typename UnaryPredicate>
		auto find_first_range_of( Iterator first, Iterator last, UnaryPredicate pred ) {
			first = find_first_of( first, last, pred );
			last = find_last_of( first, last, pred );
			++last;
			return std::make_pair( first, last );
		}

		template<typename Iterator, typename Value>
		auto split( Iterator first, Iterator last, Value const & value ) {
			using value_type = std::decay_t<decltype(*first)>;
			std::vector<std::vector<value_type>> result;
			std::vector<value_type> temp;
			for( auto it=first; it !=last; ++it ) {
				if( value == *it ) {
					result.push_back( std::move( temp ) );
					temp = std::vector<value_type>{ };
				} else {
					temp.push_back( *it );
				}
			}
			return result;
		}

		template<typename Value, typename UnaryFunction>
		bool satisfies( Value value, UnaryFunction func ) {
			return func( value );
		}

		template<typename Value, typename UnaryFunction, typename... UnaryFunctions>
		bool satisfies( Value value, UnaryFunction func, UnaryFunctions... funcs ) {
			return func( value ) || satisfies( value, funcs... );
		}
			
		template<typename Iterator, typename UnaryFunction, typename... UnaryFunctions>
		bool satisfies( Iterator first, Iterator last, UnaryFunction func, UnaryFunctions... funcs ) {
			for( auto it=first; it != last; ++it ) {
				return satisfies( *it, func, funcs... );	
			}
		}
		
		namespace impl {
			template<typename Lower, typename Upper>
			class in_range {
				Lower m_lower;
				Upper m_upper;
			public:
				in_range( Lower lower, Upper upper ): 
						m_lower{ std::move( lower ) },
						m_upper{ std::move( upper ) } {
				
					assert( lower <= upper );
				}
			
				template<typename T>
				bool operator( )( T && value ) const {
					return m_lower <= value && value <= m_upper;
				}
			};	// in_range
		
			template<typename Value>
			class equal_to {
				Value m_value;
			public:
				equal_to( Value value ):
						m_value{ std::move( value ) } { }

				template<typename T>
				bool operator( )( T && value ) const {
					return value == m_value;
				}
			};	// equal_to

			template<typename Value>
			class less_than {
				Value m_value;
			public:
				less_than( Value value ):
						m_value{ std::move( value ) } { }

				template<typename T>
				bool operator( )( T && value ) const {
					return value == m_value;
				}
			};	// less_than

			template<typename Value>
			class greater_than {
				Value m_value;
			public:
				greater_than( Value value ):
						m_value{ std::move( value ) } { }

				template<typename T>
				bool operator( )( T && value ) const {
					return value == m_value;
				}
			};	// greater_than

			template<typename Value>
			class greater_than_or_equal_to {
				Value m_value;
			public:
				greater_than_or_equal_to( Value value ):
						m_value{ std::move( value ) } { }

				template<typename T>
				bool operatthan_or( )( T && value ) const {
					return value >= m_value;
				}
			};	// greater_than_or_equal_to

			template<typename Value>
			class less_than_or_equal_to {
				Value m_value;
			public:
				less_than_or_equal_to( Value value ):
						m_value{ std::move( value ) } { }

				template<typename T>
				bool operatthan_or( )( T && value ) const {
					return value <= m_value;
				}
			};	// less_than_or_equal_to
		}	// namespace impl

		template<typename Lower, typename Upper>
		auto in_range( Lower && lower, Upper && upper ) {
			return impl::in_range<Lower, Upper>{ std::forward<Lower>( lower ), std::forward<Upper>( upper ) };
		}

		template<typename Value>
		auto equal_to( Value && value ) {
			return impl::equal_to<Value>{ std::forward<Value>( value ) };
		}

		template<typename Value>
		auto greater_than( Value && value ) {
			return impl::greater_than<Value>{ std::forward<Value>( value ) };
		}

		template<typename Value>
		auto greater_than_or_equal_to( Value && value ) {
			return impl::greater_than_or_equal_to<Value>{ std::forward<Value>( value ) };
		}

		template<typename Value>
		auto less_than( Value && value ) {
			return impl::less_than<Value>{ std::forward<Value>( value ) };
		}

		template<typename Value>
		auto less_than_or_equal_to( Value && value ) {
			return impl::less_than_or_equal_to<Value>{ std::forward<Value>( value ) };
		}


	}	// namespace algorithm
}	// namespace daw

template<typename T>
void Unused( T&& ) { };

