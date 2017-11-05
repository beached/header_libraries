// The MIT License (MIT)
//
// Copyright (c) 2013-2017 Darrell Wright
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
#include <functional>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "cpp_17.h"
#include "daw_exception.h"
#include "daw_string.h"
#include "daw_traits.h"

namespace daw {
	namespace algorithm {
		namespace impl {
			template<typename Container>
			using has_size_method = decltype( std::declval<Container>( ).size( ) );
		}

		template<typename T, size_t N>
		constexpr size_t container_size( T const ( & )[N] ) noexcept {
			return N;
		}

		template<typename Container,
		         std::enable_if_t<daw::is_detected_v<impl::has_size_method, Container>, std::nullptr_t> = nullptr>
		constexpr size_t container_size( Container const &c ) noexcept {
			return static_cast<size_t>( c.size( ) );
		}

		template<typename Container, typename UnaryPredicate>
		auto find_if( Container &container, UnaryPredicate predicate )
		  -> decltype( std::find_if( begin( container ), end( container ), predicate ) ) {
			return std::find_if( std::begin( container ), std::end( container ), predicate );
		}
		namespace impl {
			template<typename Iterator1, typename Iterator2>
			constexpr auto dist( Iterator1 first, Iterator2 last, std::random_access_iterator_tag ) noexcept {
				return last - first;
			}

			template<typename Iterator1, typename Iterator2, typename Tag>
			auto dist( Iterator1 first, Iterator2 last, Tag ) noexcept {
				return std::distance( first, last );
			}

			template<typename Iterator, typename Distance>
			constexpr void advance( Iterator &first, Distance n, std::random_access_iterator_tag ) noexcept {
				first += n;
			}

			template<typename Iterator, typename Distance, typename Tag>
			void advance( Iterator &first, Distance n, Tag ) noexcept {
				std::advance( first, n );
			}
		} // namespace impl

		template<typename Container, typename Iterator>
		constexpr void safe_advance( Container &container, Iterator &it, ptrdiff_t distance ) noexcept {
			if( 0 == distance ) {
				return;
			}
			using it_cat = typename std::iterator_traits<decltype( std::cbegin( container ) )>::iterator_category;
			auto const it_pos = impl::dist( std::cbegin( container ), it, it_cat{} );

			if( distance > 0 ) {
				auto const size_of_container = impl::dist( std::cbegin( container ), std::cend( container ), it_cat{} );
				if( size_of_container <= static_cast<ptrdiff_t>( distance + it_pos ) ) {
					distance = size_of_container - it_pos;
				}
			} else if( distance + it_pos > 0 ) {
				distance = it_pos;
			}
			impl::advance( it, distance, it_cat{} );
		}

		template<typename RandomIterator1, typename RandomIterator2>
		constexpr auto distance( RandomIterator1 first, RandomIterator2 last ) noexcept {
			return last - first;
		}

		template<typename Iterator>
		constexpr Iterator safe_next( Iterator it, Iterator last, size_t n ) noexcept {
			return std::next( it, std::min( n, static_cast<size_t>( std::distance( it, last ) ) ) );
		}

		template<typename RandomIterator>
		constexpr RandomIterator next( RandomIterator it,
		                               typename std::iterator_traits<RandomIterator>::difference_type n = 1 ) noexcept {
			return it + n;
		}

		template<typename Container>
		constexpr auto begin_at( Container &container, size_t distance ) -> decltype( std::begin( container ) ) {
			auto result = std::begin( container );
			safe_advance( container, result, static_cast<ptrdiff_t>( distance ) );
			return result;
		}

		//////////////////////////////////////////////////////////////////////////
		/// Summary: Run func( container, position ) on each element
		/// in interval [first_inclusive, last_exclusive)
		///
		template<typename Container, typename Function>
		constexpr void for_each_subset( Container &container, size_t const first_inclusive, size_t last_exclusive,
		                                Function func ) noexcept( noexcept( func( container, std::declval<size_t>( ) ) ) ) {
			static_assert(
			  daw::is_callable_v<Function, Container, size_t>,
			  "Supplied function does not satisfy requirements of taking arguments of type (Container, size_t)" );

			for( size_t row = first_inclusive; row < last_exclusive; ++row ) {
				func( container, row );
			}
		}

		//////////////////////////////////////////////////////////////////////////
		/// Summary: Run func( container, position ) on each element
		/// in interval [first_inclusive, last_exclusive)
		///
		template<typename Container, typename Function>
		constexpr void for_each_with_pos( Container &container, size_t const first_inclusive, size_t last_exclusive,
		                                  Function func ) noexcept( noexcept( func( *std::begin( container ),
		                                                                            std::declval<size_t>( ) ) ) ) {

			using value_t = std::decay_t<typename std::iterator_traits<decltype( std::begin( container ) )>::value_type>;
			static_assert( daw::is_callable_v<Function, value_t, size_t>,
			               "Supplied function does not satisfy requirements of taking arguments of type (value_t, size_t)" );

			auto it = begin_at( container, first_inclusive );
			for( size_t row = first_inclusive; row < last_exclusive; ++row ) {
				func( *it++, row );
			}
		}

		template<typename Container, typename Function>
		constexpr void for_each_with_pos( Container &container,
		                                  Function func ) noexcept( noexcept( func( *std::begin( container ),
		                                                                            std::declval<size_t>( ) ) ) ) {
			using value_t = std::decay_t<typename std::iterator_traits<decltype( std::begin( container ) )>::value_type>;
			static_assert( daw::is_callable_v<Function, value_t, size_t>,
			               "Supplied function does not satisfy requirements of taking arguments of type (value_t, size_t)" );
			for_each_with_pos( container, 0, container_size( container ), func );
		}

		template<typename IteratorType, typename ValueType, typename Comp>
		IteratorType binary_search( const IteratorType first, const IteratorType last, const ValueType &value,
		                            Comp less_than ) {
			auto midpoint = []( const IteratorType &a, const IteratorType &b ) {
				daw::exception::dbg_throw_on_false(
				  a <= b, daw::string::string_join( __func__,
				                                    ": Cannot find a midpoint unless the first parameter is <= the second" ) );
				auto const mid = std::distance( a, b ) / 2;
				auto result = a;
				std::advance( result, mid );
				return result;
			};
			daw::exception::dbg_throw_on_false(
			  first < last, daw::string::string_join( __func__, ": First position must be less than second" ) );
			IteratorType it_first( first );
			IteratorType it_last( last );

			while( it_first < it_last ) {
				auto mid = midpoint( it_first, it_last );
				if( less_than( mid, value ) ) {
					it_first = mid;
					std::advance( it_first, 1 );
				} else if( less_than( value, mid ) ) {
					it_last = mid;
				} else { // equal
					return mid;
				}
			}
			return last;
		}

		template<typename Container>
		auto rbegin2( Container &container ) -> decltype( container.rbegin( ) ) {
			return container.rbegin( );
		}

		template<typename Container>
		auto crbegin2( Container &container ) -> decltype( container.crbegin( ) ) {
			return container.crbegin( );
		}

		template<typename Container>
		auto rend2( Container &container ) -> decltype( container.rend( ) ) {
			return container.rend( );
		}

		template<typename Container>
		auto crend2( Container &container ) -> decltype( container.crend( ) ) {
			return container.crend( );
		}

		template<typename Value>
		inline const Value *to_array( const std::vector<Value> &values ) {
			return &( values[0] );
		}

		template<typename Value>
		inline Value *to_array( std::vector<Value> &values ) {
			return &( values[0] );
		}

		template<typename Ptr>
		inline bool is_null_ptr( Ptr const *const ptr ) {
			return nullptr == ptr;
		}

		/// Reverser eg for( auto item: reverse( container ) ) { }
		namespace details {
			template<typename Fwd>
			struct Reverser_generic {
				Fwd &fwd;
				Reverser_generic( Fwd &fwd_ ) : fwd( fwd_ ) {}
				typedef std::reverse_iterator<typename Fwd::iterator> reverse_iterator;
				reverse_iterator begin( ) {
					return reverse_iterator( std::end( fwd ) );
				}
				reverse_iterator end( ) {
					return reverse_iterator( std::begin( fwd ) );
				}
			};

			template<typename Fwd>
			struct Reverser_special {
				Fwd &fwd;
				Reverser_special( Fwd &fwd_ ) : fwd( fwd_ ) {}
				auto begin( ) -> decltype( fwd.rbegin( ) ) {
					return fwd.rbegin( );
				}
				auto end( ) -> decltype( fwd.rbegin( ) ) {
					return fwd.rend( );
				}
			};

			template<typename Fwd>
			auto reverse_impl( Fwd &fwd, long ) -> decltype( Reverser_generic<Fwd>( fwd ) ) {
				return Reverser_generic<Fwd>( fwd );
			}

			template<typename Fwd>
			auto reverse_impl( Fwd &fwd, int ) -> decltype( fwd.rbegin( ), Reverser_special<Fwd>( fwd ) ) {
				return Reverser_special<Fwd>( fwd );
			}
		} // namespace details

		template<typename Fwd>
		auto reverse( Fwd &&fwd ) -> decltype( details::reverse_impl( fwd, int( 0 ) ) ) {
			static_assert( !is_rvalue_reference_v<Fwd &&>, "Cannot pass rvalue_reference to reverse()" );
			return details::reverse_impl( fwd, int( 0 ) );
		}

		template<typename ValueType>
		constexpr bool are_equal( ValueType ) noexcept {
			return true;
		}

		template<typename ValueType>
		constexpr bool are_equal( ValueType v1, ValueType v2 ) {
			return v1 == v2;
		}

		template<typename ValueType, typename... Values>
		constexpr bool are_equal( ValueType v1, ValueType v2, Values... others ) {
			return are_equal( v1, v2 ) && are_equal( others... );
		}

		template<typename Iterator>
		constexpr std::pair<Iterator, Iterator> slide( Iterator first, Iterator last, Iterator target ) {
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

		template<typename InputIt1, typename InputIt2, typename OutputIt, typename Func>
		constexpr OutputIt transform_many( InputIt1 first1, InputIt1 last1, InputIt2 first2, OutputIt first_out,
		                                   Func func ) {
			static_assert( !is_const_v<decltype( *first_out )>, "Output iterator cannot point to const data" );
			while( first1 != last1 ) {
				*first_out++ = func( *first1++, *first2++ );
			}
			return first_out;
		}

		template<typename InputIt1, typename InputIt2, typename InputIt3, typename OutputIt, typename Func>
		constexpr OutputIt transform_many( InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt3 first3,
		                                   OutputIt first_out, Func func ) {
			static_assert( !is_const_v<decltype( *first_out )>, "Output iterator cannot point to const data" );
			while( first1 != last1 ) {
				*first_out++ = func( *first1++, *first2++, *first3++ );
			}
			return first_out;
		}

		template<typename InputIt1, typename InputIt2, typename InputIt3, typename InputIt4, typename OutputIt,
		         typename Func>
		constexpr OutputIt transform_many( InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt3 first3,
		                                   InputIt4 first4, OutputIt first_out, Func func ) {
			static_assert( !is_const_v<decltype( *first_out )>, "Output iterator cannot point to const data" );
			while( first1 != last1 ) {
				*first_out++ = func( *first1++, *first2++, *first3++, *first4++ );
			}
			return first_out;
		}
		template<typename InputIt1, typename InputIt2, typename InputIt3, typename InputIt4, typename InputIt5,
		         typename OutputIt, typename Func>
		constexpr OutputIt transform_many( InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt3 first3,
		                                   InputIt4 first4, InputIt4 first5, OutputIt first_out, Func func ) {
			static_assert( !is_const_v<decltype( *first_out )>, "Output iterator cannot point to const data" );
			while( first1 != last1 ) {
				*first_out++ = func( *first1++, *first2++, *first3++, *first4++, *first5++ );
			}
			return first_out;
		}

		struct enum_class_hash {
			template<typename T>
			constexpr size_t operator( )( T value ) {
				return static_cast<size_t>( value );
			}
		}; // struct enum_class_hash

		template<typename T>
		constexpr T clamp( T const &value, T const &max_value ) noexcept {
			if( value > max_value ) {
				return max_value;
			}
			return value;
		}

		template<std::size_t N>
		struct tuple_functor {
			template<typename T, typename F>
			static void run( std::size_t i, T &&t, F &&f ) {
				constexpr const std::size_t I = ( N - 1 );
				switch( i ) {
				case I:
					std::forward<F>( f )( std::get<I>( std::forward<T>( t ) ) );
					break;
				default:
					tuple_functor<I>::run( i, std::forward<T>( t ), std::forward<F>( f ) );
				}
			}
		}; // struct tuple_functor

		template<>
		struct tuple_functor<0> {
			template<typename T, typename F>
			constexpr static void run( std::size_t, T, F ) {}
		}; // struct tuple_functor

		template<typename Iterator, typename UnaryPredicate>
		constexpr auto find_last_of( Iterator first, Iterator last, UnaryPredicate pred ) {
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
		constexpr auto find_first_of( Iterator first, Iterator last, UnaryPredicate pred ) {
			while( first != last ) {
				if( pred( *first ) ) {
					break;
				}
				++first;
			}
			return first;
		}

		template<typename Iterator, typename UnaryPredicate>
		constexpr auto find_first_range_of( Iterator first, Iterator last, UnaryPredicate pred ) {
			first = find_first_of( first, last, pred );
			last = find_last_of( first, last, pred );
			++last;
			return std::make_pair( first, last );
		}

		template<typename Iterator, typename Value>
		auto split( Iterator first, Iterator last, Value const &value ) {
			using value_type = daw::traits::root_type_t<decltype( *first )>;
			std::vector<std::vector<value_type>> result;
			std::vector<value_type> temp;
			for( auto it = first; it != last; ++it ) {
				if( value == *it ) {
					result.push_back( std::move( temp ) );
					temp = std::vector<value_type>{};
				} else {
					temp.push_back( *it );
				}
			}
			return result;
		}

		template<typename Value, typename UnaryFunction>
		constexpr bool satisfies_one( Value value, UnaryFunction func ) {
			return func( value );
		}

		template<typename Value, typename UnaryFunction, typename... UnaryFunctions>
		constexpr bool satisfies_one( Value value, UnaryFunction func, UnaryFunctions... funcs ) {
			return func( value ) || satisfies_one( value, funcs... );
		}

		template<typename Iterator, typename UnaryFunction, typename... UnaryFunctions>
		constexpr bool satisfies_one( Iterator first, Iterator last, UnaryFunction func, UnaryFunctions... funcs ) {
			for( auto it = first; it != last; ++it ) {
				return satisfies_one( *it, func, funcs... );
			}
		}

		template<typename Value, typename UnaryFunction>
		constexpr bool satisfies_all( Value value, UnaryFunction func ) {
			return func( value );
		}

		template<typename Value, typename UnaryFunction, typename... UnaryFunctions>
		constexpr bool satisfies_all( Value value, UnaryFunction func, UnaryFunctions... funcs ) {
			return func( value ) && satisfies_all( value, funcs... );
		}

		template<typename Iterator, typename UnaryFunction, typename... UnaryFunctions>
		constexpr bool satisfies_all( Iterator first, Iterator last, UnaryFunction func, UnaryFunctions... funcs ) {
			for( auto it = first; it != last; ++it ) {
				return satisfies_all( *it, func, funcs... );
			}
		}

		namespace impl {
			template<typename Lower, typename Upper>
			class in_range {
				Lower m_lower;
				Upper m_upper;

			public:
				constexpr in_range( Lower lower, Upper upper ) : m_lower{std::move( lower )}, m_upper{std::move( upper )} {
					if( lower > upper ) {
						throw std::exception{};
					};
				}

				template<typename T>
				constexpr bool operator( )( T &&value ) const {
					return m_lower <= value && value <= m_upper;
				}
			}; // in_range

			template<typename Value>
			class equal_to {
				Value m_value;

			public:
				constexpr equal_to( Value value ) : m_value{std::move( value )} {}

				template<typename T>
				constexpr bool operator( )( T &&value ) const {
					return value == m_value;
				}
			}; // equal_to

			template<typename Value>
			class less_than {
				Value m_value;

			public:
				constexpr less_than( Value value ) : m_value{std::move( value )} {}

				template<typename T>
				constexpr bool operator( )( T &&value ) const {
					return value < m_value;
				}
			}; // less_than

			template<typename Value>
			class greater_than {
				Value m_value;

			public:
				constexpr greater_than( Value value ) : m_value{std::move( value )} {}

				template<typename T>
				constexpr bool operator( )( T &&value ) const {
					return value > m_value;
				}
			}; // greater_than

			template<typename Value>
			class greater_than_or_equal_to {
				Value m_value;

			public:
				constexpr greater_than_or_equal_to( Value value ) : m_value{std::move( value )} {}

				template<typename T>
				constexpr bool operator( )( T &&value ) const {
					return value >= m_value;
				}
			}; // greater_than_or_equal_to

			template<typename Value>
			class less_than_or_equal_to {
				Value m_value;

			public:
				constexpr less_than_or_equal_to( Value value ) : m_value{std::move( value )} {}

				template<typename T>
				constexpr bool operator( )( T &&value ) const {
					return value <= m_value;
				}
			}; // less_than_or_equal_to
		}    // namespace impl

		template<typename Lower, typename Upper>
		constexpr auto in_range( Lower &&lower, Upper &&upper ) {
			return impl::in_range<Lower, Upper>{std::forward<Lower>( lower ), std::forward<Upper>( upper )};
		}

		template<typename Value>
		constexpr auto equal_to( Value &&value ) {
			return impl::equal_to<Value>{std::forward<Value>( value )};
		}

		template<typename Value>
		constexpr auto greater_than( Value &&value ) {
			return impl::greater_than<Value>{std::forward<Value>( value )};
		}

		template<typename Value>
		constexpr auto greater_than_or_equal_to( Value &&value ) {
			return impl::greater_than_or_equal_to<Value>{std::forward<Value>( value )};
		}

		template<typename Value>
		constexpr auto less_than( Value &&value ) {
			return impl::less_than<Value>{std::forward<Value>( value )};
		}

		template<typename Value>
		constexpr auto less_than_or_equal_to( Value &&value ) {
			return impl::less_than_or_equal_to<Value>{std::forward<Value>( value )};
		}

		template<typename ForwardIterator1, typename ForwardIterator2>
		constexpr auto lexicographical_compare( ForwardIterator1 first1, ForwardIterator1 const last1,
		                                        ForwardIterator2 first2, ForwardIterator2 const last2 ) {
			decltype( *first1 - *first2 ) tmp;
			for( ; first1 != last1 && first2 != last2; ++first1, ++first2 ) {
				if( ( tmp = *first1 - *first2 ) != 0 ) {
					return tmp;
				}
			}
			if( first1 == last1 ) {
				if( first2 == last2 ) {
					return 0;
				}
				return 1;
			} else if( first2 == last2 ) {
				return -1;
			}
			throw std::logic_error( "This case should have been handled within the loop" );
		}

		template<typename InputIteratorFirst, typename InputIteratorLast, typename OutputIterator, typename Predicate,
		         typename TransformFunction>
		constexpr auto transform_if( InputIteratorFirst first, InputIteratorLast const last, OutputIterator out_it,
		                             Predicate pred, TransformFunction trans ) {
			while( first != last ) {
				if( pred( *first ) ) {
					*out_it = trans( *first );
				}
				++first;
			}
			return out_it;
		}

		template<typename InputIterator, typename OutputIterator, typename UnaryOperation>
		constexpr OutputIterator
		transform_n( InputIterator first_in, OutputIterator first_out, size_t count,
		             UnaryOperation unary_op ) noexcept( noexcept( *first_out = unary_op( *first_in ) ) ) {

			for( size_t n = 0; n < count; ++n ) {
				*first_out++ = unary_op( *first_in++ );
			}
			return first_out;
		}

		template<typename InputIterator1, typename InputIterator2, typename OutputIterator>
		constexpr void copy( InputIterator1 first_in, InputIterator2 const last_in, OutputIterator first_out ) noexcept {
			while( first_in != last_in ) {
				*first_out++ = *first_in++;
			}
		}

		template<typename InputIterator, typename OutputIterator>
		constexpr void copy_n( InputIterator first_in, OutputIterator first_out, size_t const count ) noexcept {
			for( size_t n = 0; n < count; ++n ) {
				*first_out++ = *first_in++;
			}
		}

		template<typename InputIterator1, typename InputIterator2, typename InputIterator3>
		constexpr bool equal( InputIterator1 first1, InputIterator2 last1,
		                      InputIterator3 first2 ) noexcept( noexcept( *first1 == *first2 ) ) {
			while( first1 != last1 ) {
				if( !( *first1++ == *first2++ ) ) {
					return false;
				}
			}
			return true;
		}

		template<typename InputIterator1, typename InputIterator2, typename InputIterator3, typename InputIterator4>
		constexpr bool equal( InputIterator1 first1, InputIterator2 last1, InputIterator3 first2,
		                      InputIterator4 last2 ) noexcept( noexcept( *first1 == *first2 ) ) {
			while( first1 != last1 && first2 != last2 ) {
				if( !( *first1++ == *first2++ ) ) {
					return false;
				}
			}
			return first1 == last1 && first2 == last2;
		}

		template<typename InputIterator1, typename InputIterator2, typename InputIterator3, typename InputIterator4,
		         typename BinaryPredicate>
		constexpr bool equal( InputIterator1 first1, InputIterator2 last1, InputIterator3 first2, InputIterator4 last2,
		                      BinaryPredicate pred ) noexcept( noexcept( pred( *first1, *first2 ) ) ) {
			while( first1 != last1 && first2 != last2 ) {
				if( !( pred( *first1, *first2 ) ) ) {
					++first1;
					++first2;
					return false;
				}
			}
			return first1 == last1 && first2 == last2;
		}

		template<typename T, typename U,
		         std::enable_if_t<is_convertible_v<T, U> && is_convertible_v<U, T>, std::nullptr_t> = nullptr>
		constexpr void swapper( T &t, U &u ) noexcept {
			T tmp = std::move( t );
			t = std::move( u );
			u = std::move( tmp );
		}

		template<typename ForwardIterator>
		constexpr void rotate( ForwardIterator first, ForwardIterator middle,
		                       ForwardIterator last ) noexcept( noexcept( std::swap( *first, *middle ) ) ) {

			ForwardIterator tmp = middle;
			while( first != tmp ) {
				daw::algorithm::swapper( *first++, *tmp++ );
				if( tmp == last ) {
					tmp = middle;
				} else if( first == middle ) {
					middle = tmp;
				}
			}
		}

		template<typename ForwardIterator, typename T>
		constexpr ForwardIterator upper_bound( ForwardIterator first, ForwardIterator const last,
		                                       T const &value ) noexcept {

			auto count = distance( first, last );
			while( count > 0 ) {
				auto it = first;
				auto step = count / 2;
				impl::advance( it, step, std::random_access_iterator_tag{} );
				if( !( value < *it ) ) {
					first = ++it;
					count -= step + 1;
				} else {
					count = step;
				}
			}
			return first;
		}

		template<typename RandomIterator1, typename RandomIterator2, typename RandomIterator3,
		         typename Compare = std::less<>>
		constexpr void nth_element( RandomIterator1 first, RandomIterator2 nth, RandomIterator3 const last,
		                            Compare cmp = Compare{} ) noexcept {
			for( auto i = first; i != nth; ++i ) {
				auto min_idx = i;
				for( auto j = daw::algorithm::next( i ); j != last; ++j ) {
					if( cmp( *j, *min_idx ) ) {
						min_idx = j;
						daw::algorithm::swapper( *i, *min_idx );
					}
				}
			}
		}

		template<typename RandomIterator1, typename RandomIterator2, typename Compare = std::less<>>
		constexpr void quick_sort( RandomIterator1 first, RandomIterator2 const last, Compare cmp = Compare{} ) noexcept {
			auto const N = daw::algorithm::distance( first, last );
			if( N <= 1 ) {
				return;
			}
			auto const pivot = daw::algorithm::next( first, N / 2 );
			daw::algorithm::nth_element( first, pivot, last, cmp );
			daw::algorithm::quick_sort( first, pivot, cmp );
			daw::algorithm::quick_sort( pivot, last, cmp );
		}

		template<typename RandomIterator1, typename RandomIterator2>
		constexpr void sort( RandomIterator1 first, RandomIterator2 const last ) noexcept {
			for( auto i = first; i != last; ++i ) {
				daw::algorithm::rotate( daw::algorithm::upper_bound( first, i, *i ), i, daw::algorithm::next( i ) );
			}
		}

		template<typename ForwardIterator1, typename ForwardIterator2>
		constexpr ForwardIterator1 is_sorted_until( ForwardIterator1 first, ForwardIterator2 const last ) noexcept {
			static_assert( daw::is_convertible_v<ForwardIterator2, ForwardIterator1>,
			               "Must be able to convert last to first" );
			if( first != last ) {
				auto next_it = first;
				while( ++next_it != last ) {
					if( *next_it < *first ) {
						return next_it;
					}
					first = next_it;
				}
			}
			return last;
		}

		template<typename ForwardIterator1, typename ForwardIterator2>
		constexpr bool is_sorted( ForwardIterator1 first, ForwardIterator2 const last ) noexcept {
			return daw::algorithm::is_sorted_until( first, last ) == last;
		}

		template<typename ForwardIterator, typename T>
		constexpr void fill_n( ForwardIterator first, size_t count, T const &value ) noexcept {
			for( size_t n = 0; n < count; ++n ) {
				*first++ = value;
			}
		}

		template<typename RandomIterator, typename RandomOutputIterator, typename UnaryOperation>
		constexpr void map( RandomIterator first, RandomIterator const last, RandomOutputIterator first_out,
		                    UnaryOperation unary_op ) noexcept( noexcept( *first_out++ = unary_op( *first++ ) ) ) {

			while( first != last ) {
				*first_out++ = unary_op( *first++ );
			}
		}

		template<typename T, typename RandomIterator, typename BinaryOperation>
		constexpr T reduce( RandomIterator first, RandomIterator const last, T init,
		                    BinaryOperation binary_op ) noexcept( noexcept( init = binary_op( init, *first++ ) ) ) {

			static_assert( is_binary_predicate_v<BinaryOperation, T, decltype( *first )>,
			               "BinaryOperation passed to reduce must take two values referenced by first. e.g binary_op( "
			               "init, *first) ) "
			               "must be valid" );

			static_assert(
			  is_convertible_v<decltype( binary_op( init, *first++ ) ), T >,
			  "Result of BinaryOperation must be convertable to type of value referenced by RandomIterator. "
			  "e.g. *first = binary_op( *first, *(first + 1) ) must be valid." );

				while( first != last ) {
				init = binary_op( init, *first++ );
				}
			  return std::move( init );
		}
	} // namespace algorithm
} // namespace daw
