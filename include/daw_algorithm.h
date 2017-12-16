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
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "cpp_17.h"
#include "daw_exception.h"
#include "daw_traits.h"

namespace daw {
	template<typename T, typename U>
	constexpr std::common_type_t<T, U> min( T const &lhs, U const &rhs ) noexcept {
		if( lhs <= rhs ) {
			return lhs;
		}
		return rhs;
	}

	template<typename T, typename U>
	constexpr std::common_type_t<T, U> max( T const &lhs, U const &rhs ) noexcept {
		if( lhs >= rhs ) {
			return lhs;
		}
		return rhs;
	}

	namespace algorithm {
		namespace impl {
			template<typename Iterator1, typename Iterator2>
			constexpr auto dist( Iterator1 first, Iterator2 last, std::random_access_iterator_tag ) noexcept {
				return last - first;
			}

			template<typename Iterator1, typename Iterator2, typename Tag>
			auto dist( Iterator1 first, Iterator2 last, Tag ) noexcept {
				return std::distance( first, last );
			}

			template<typename Iterator, typename Distance, typename ItTag>
			constexpr void advance( Iterator &first, Distance n, ItTag ) noexcept {
				if( static_cast<ptrdiff_t>( n ) < 0 ) {
					for( ; n < 0; ++n ) {
						--first;
					}
					return;
				}
				for( ; n > 0; --n ) {
					++first;
				}
			}

			template<typename Iterator, typename Distance>
			constexpr void advance( Iterator &first, Distance n, std::random_access_iterator_tag ) noexcept {
				first += static_cast<ptrdiff_t>( n );
			}
		} // namespace impl

		template<typename Iterator, typename Distance>
		constexpr void advance( Iterator &it, Distance n ) noexcept {
			impl::advance( it, static_cast<typename std::iterator_traits<Iterator>::difference_type>( n ),
			               typename std::iterator_traits<Iterator>::iterator_category{} );
		}

		template<typename Container, typename Iterator>
		constexpr void safe_advance( Container &container, Iterator &it,
		                             typename std::iterator_traits<Iterator>::difference_type distance ) noexcept {
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
		constexpr Iterator next( Iterator it, typename std::iterator_traits<Iterator>::difference_type n = 1 ) noexcept {
			impl::advance( it, n, typename std::iterator_traits<Iterator>::iterator_category{} );
			return it;
		}

		template<typename Iterator>
		constexpr Iterator prev( Iterator it, typename std::iterator_traits<Iterator>::difference_type n = 1 ) noexcept {
			impl::advance( it, -n, typename std::iterator_traits<Iterator>::iterator_category{} );
			return it;
		}

		template<typename Iterator>
		constexpr Iterator safe_next( Iterator it, Iterator last,
		                              typename std::iterator_traits<Iterator>::difference_type n = 1 ) noexcept {
			return daw::algorithm::next( it, daw::min( n, std::distance( it, last ) ) );
		}

		template<typename Iterator>
		constexpr Iterator safe_prev( Iterator it, Iterator first, size_t n = 1 ) noexcept {
			return daw::algorithm::prev( it, daw::min( n, std::distance( first, it ) ) );
		}

		template<typename Container>
		constexpr auto begin_at( Container &container, size_t distance ) -> decltype( std::begin( container ) ) {
			auto result = std::begin( container );
			safe_advance( container, result, static_cast<ptrdiff_t>( distance ) );
			return result;
		}

		template<typename Lhs>
		constexpr auto const &min_item( Lhs const &lhs ) noexcept {
			return lhs;
		}

		template<typename Lhs, typename... Ts, std::enable_if_t<( sizeof...( Ts ) > 0 ), std::nullptr_t> = nullptr>
		constexpr auto const &min_item( Lhs const &lhs, Ts const &... ts ) noexcept {
			auto const &rhs = min_item( ts... );
			return lhs < rhs ? lhs : rhs;
		}

		template<typename Lhs>
		constexpr auto const &max_item( Lhs const &lhs ) noexcept {
			return lhs;
		}

		template<typename Lhs, typename... Ts, std::enable_if_t<( sizeof...( Ts ) > 0 ), std::nullptr_t> = nullptr>
		constexpr auto const &max_item( Lhs const &lhs, Ts const &... ts ) noexcept {
			auto const &rhs = max_item( ts... );
			return lhs > rhs ? lhs : rhs;
		}

		template<typename IteratorType, typename ValueType, typename Comp>
		IteratorType binary_search( const IteratorType first, const IteratorType last, const ValueType &value,
		                            Comp less_than ) {
			auto midpoint = []( const IteratorType &a, const IteratorType &b ) {
				daw::exception::dbg_throw_on_false( a <= b,
				                                    " Cannot find a midpoint unless the first parameter is <= the second" );

				auto const mid = std::distance( a, b ) / 2;
				auto result = a;
				std::advance( result, mid );
				return result;
			};
			daw::exception::dbg_throw_on_false( first < last, ": First position must be less than second" );
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
				explicit Reverser_generic( Fwd &fwd_ )
				  : fwd( fwd_ ) {}
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
				explicit Reverser_special( Fwd &fwd_ )
				  : fwd( fwd_ ) {}
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
			static_assert( daw::is_unary_predicate_v<UnaryPredicate, decltype( *first )>,
			               "Compare does not satisfy the Unary Predicate concept.  See "
			               "http://en.cppreference.com/w/cpp/concept/Predicate for more information" );

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
			static_assert( daw::is_unary_predicate_v<UnaryPredicate, decltype( *first )>,
			               "Compare does not satisfy the Unary Predicate concept.  See "
			               "http://en.cppreference.com/w/cpp/concept/Predicate for more information" );

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
			static_assert( daw::is_unary_predicate_v<UnaryPredicate, decltype( *first )>,
			               "Compare does not satisfy the Unary Predicate concept.  See "
			               "http://en.cppreference.com/w/cpp/concept/Predicate for more information" );

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

		/// Returns true if any function returns true for the value
		///
		/// @param value Argument to UnaryPredicate
		/// @param func A UnaryPredicate that returns true/false
		/// @return The result of func
		template<typename Value, typename UnaryPredicate>
		constexpr bool satisfies_one( Value value, UnaryPredicate func ) noexcept( noexcept( func( value ) ) ) {
			static_assert( is_unary_predicate_v<UnaryPredicate, decltype( value )>,
			               "UnaryPredicate must take one value and return a bool e.g. func( value ) must be valid" );
			return func( value );
		}

		/// Returns true if any function returns true for the value
		///
		/// @param value Argument to UnaryPredicate(s)
		/// @param func A UnaryPredicate that returns true/false
		/// @param funcs UnaryPredicates that return true/false
		/// @return True if any of the func/funcs return true(e.g. like OR)
		template<typename Value, typename UnaryPredicate, typename... UnaryPredicates>
		constexpr bool satisfies_one( Value value, UnaryPredicate func, UnaryPredicates... funcs ) {

			static_assert( is_unary_predicate_v<UnaryPredicate, decltype( value )>,
			               "UnaryPredicate must take one value and return a bool e.g. func( value ) must be valid" );

			return func( value ) || satisfies_one( value, funcs... );
		}

		/// Returns true if any function returns true for any value in range
		///
		/// @param first iterator pointing to the beginning of the range inclusively
		/// @param last iterator pointing to the end of the range exclusively
		/// @param func A UnaryPredicate that returns true/false
		/// @param funcs UnaryPredicates that return true/false
		/// @return True if any of the func/funcs return true(e.g. like OR) for any value in range
		template<typename Iterator, typename LastType, typename UnaryPredicate, typename... UnaryPredicates,
		         std::enable_if_t<(daw::is_dereferenceable_v<LastType> &&
		                           daw::is_equality_comparable_v<daw::traits::deref_t<LastType>>),
		                          std::nullptr_t> = nullptr>
		constexpr bool
		satisfies_one( Iterator first, LastType last, UnaryPredicate func,
		               UnaryPredicates... funcs ) noexcept( noexcept( satisfies_one( *first, func, funcs... ) ) ) {

			static_assert( is_unary_predicate_v<UnaryPredicate, decltype( *first )>,
			               "UnaryPredicate must take one value and return a bool e.g. func( *first ) must be valid" );

			while( first != last ) {
				if( satisfies_one( *first, func, funcs... ) ) {
					return true;
				}
				++first;
			}
			return false;
		}

		/// Returns true if function returns true for the value
		///
		/// @param value Argument to UnaryPredicate
		/// @param func A UnaryPredicate that returns true/false
		/// @return The result of func
		template<typename Value, typename UnaryPredicate>
		constexpr bool satisfies_all( Value value, UnaryPredicate func ) noexcept( noexcept( func( value ) ) ) {
			static_assert( is_unary_predicate_v<UnaryPredicate, decltype( value )>,
			               "UnaryPredicate must take one value and return a bool e.g. func( value ) must be valid" );
			return func( value );
		}

		/// Returns true if all function(s) returns true for the value
		///
		/// @param value Argument to UnaryPredicate(s)
		/// @param func A UnaryPredicate that returns true/false
		/// @param funcs UnaryPredicates that return true/false
		/// @return True if any of the func/funcs return true(e.g. like OR)
		template<typename Value, typename UnaryPredicate, typename... UnaryPredicates>
		constexpr bool satisfies_all( Value value, UnaryPredicate func, UnaryPredicates... funcs ) {

			static_assert( is_unary_predicate_v<UnaryPredicate, decltype( value )>,
			               "UnaryPredicate must take one value and return a bool e.g. func( value ) must be valid" );

			return func( value ) && satisfies_all( value, funcs... );
		}

		/// Returns true if all function(s) returns true for all values in range
		///
		/// @param first iterator pointing to the beginning of the range inclusively
		/// @param last iterator pointing to the end of the range exclusively
		/// @param func A UnaryPredicate that returns true/false
		/// @param funcs UnaryPredicates that return true/false
		/// @return True if any of the func/funcs return true(e.g. like OR) for any value in range
		template<typename Iterator, typename LastType, typename UnaryPredicate, typename... UnaryPredicates,
		         std::enable_if_t<(daw::is_dereferenceable_v<LastType> &&
		                           daw::is_equality_comparable_v<daw::traits::deref_t<LastType>>),
		                          std::nullptr_t> = nullptr>
		constexpr bool
		satisfies_all( Iterator first, LastType last, UnaryPredicate func,
		               UnaryPredicates... funcs ) noexcept( noexcept( satisfies_one( *first, func, funcs... ) ) ) {

			static_assert( is_unary_predicate_v<UnaryPredicate, decltype( *first )>,
			               "UnaryPredicate must take one value and return a bool e.g. func( *first ) must be valid" );

			while( first != last ) {
				if( !satisfies_all( *first, func, funcs... ) ) {
					return false;
				}
				++first;
			}
			return true;
		}

		namespace impl {
			template<typename Lower, typename Upper>
			class in_range {
				Lower m_lower;
				Upper m_upper;

			public:
				constexpr in_range( Lower lower, Upper upper )
				  : m_lower{std::move( lower )}
				  , m_upper{std::move( upper )} {
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
				constexpr equal_to( Value value )
				  : m_value{std::move( value )} {}

				template<typename T>
				constexpr bool operator( )( T &&value ) const {
					return value == m_value;
				}
			}; // equal_to

			template<typename Value>
			class less_than {
				Value m_value;

			public:
				constexpr less_than( Value value )
				  : m_value{std::move( value )} {}

				template<typename T>
				constexpr bool operator( )( T &&value ) const {
					return value < m_value;
				}
			}; // less_than

			template<typename Value>
			class greater_than {
				Value m_value;

			public:
				constexpr greater_than( Value value )
				  : m_value{std::move( value )} {}

				template<typename T>
				constexpr bool operator( )( T &&value ) const {
					return value > m_value;
				}
			}; // greater_than

			template<typename Value>
			class greater_than_or_equal_to {
				Value m_value;

			public:
				constexpr greater_than_or_equal_to( Value value )
				  : m_value{std::move( value )} {}

				template<typename T>
				constexpr bool operator( )( T &&value ) const {
					return value >= m_value;
				}
			}; // greater_than_or_equal_to

			template<typename Value>
			class less_than_or_equal_to {
				Value m_value;

			public:
				constexpr less_than_or_equal_to( Value value )
				  : m_value{std::move( value )} {}

				template<typename T>
				constexpr bool operator( )( T &&value ) const {
					return value <= m_value;
				}
			}; // less_than_or_equal_to
		}    // namespace impl

		/// Returns a callable that returns true if the value passed is in the range [Lower, Upper]
		///
		/// @param lower The lowest value allowed
		/// @param upper The largest value allowed
		/// @returns true if value passed to operator( ) is within the range [lower, upper]
		template<typename Lower, typename Upper>
		constexpr auto in_range( Lower &&lower, Upper &&upper ) {
			return impl::in_range<Lower, Upper>{std::forward<Lower>( lower ), std::forward<Upper>( upper )};
		}

		/// Returns a callable that returns true if value passed is equal to value construct with
		///
		/// @param value Value to test with
		/// @returns True if value passed to operator( ) is equal to value constructed with
		template<typename Value>
		constexpr auto equal_to( Value &&value ) {
			return impl::equal_to<Value>{std::forward<Value>( value )};
		}

		/// Returns a callable that returns true if value passed is greater than the value constructed with
		///
		/// @param value Value to test with
		/// @returns True if value passed to operator( ) is greater than to value constructed with
		template<typename Value>
		constexpr auto greater_than( Value &&value ) {
			return impl::greater_than<Value>{std::forward<Value>( value )};
		}

		/// Returns a callable that returns true if value passed is greater than or equal to the value constructed with
		///
		/// @param value Value to test with
		/// @returns True if value passed to operator( ) is greater than or equal to to value constructed with
		template<typename Value>
		constexpr auto greater_than_or_equal_to( Value &&value ) {
			return impl::greater_than_or_equal_to<Value>{std::forward<Value>( value )};
		}

		/// Returns a callable that returns true if value passed is less than the value constructed with
		///
		/// @param value Value to test with
		/// @returns True if value passed to operator( ) is less than to value constructed with
		template<typename Value>
		constexpr auto less_than( Value &&value ) {
			return impl::less_than<Value>{std::forward<Value>( value )};
		}

		/// Returns a callable that returns true if value passed is less than or equal to the value constructed with
		///
		/// @param value Value to test with
		/// @returns True if value passed to operator( ) is less than or equal to to value constructed with
		template<typename Value>
		constexpr auto less_than_or_equal_to( Value &&value ) {
			return impl::less_than_or_equal_to<Value>{std::forward<Value>( value )};
		}

		/// Returns true if the first range [first1, last1) is lexigraphically less than the second range [first2, last2)
		///
		/// @tparam InputIterator1 Iterator type for start of range 1
		/// @tparam LastType1 Type for value at end of range 1
		/// @tparam InputIterator2 Iterator type for start of range 2
		/// @tparam LastType2 Type for value at end of range 2
		/// @tparam Compare Type for comp callback that fullfills Compare concept
		/// @param first1 Start of range 1
		/// @param last1 End of range 1
		/// @param first2 Start of range 1
		/// @param last2 End of Range 1
		/// @param comp Comparison function that returns true if value1 < value2
		/// @return true of range 1 is lexigraphically less than range 2 using supplied comparison
		template<typename InputIterator1, typename LastType1, typename InputIterator2, typename LastType2, typename Compare>
		constexpr bool lexicographical_compare( InputIterator1 first1, LastType1 last1, InputIterator2 first2,
		                                        LastType2 last2,
		                                        Compare comp ) noexcept( noexcept( comp( *first1, *first2 ) !=
		                                                                           comp( *first2, *first1 ) ) ) {

			static_assert( is_binary_predicate_v<Compare, decltype( *first1 ), decltype( *first2 )>,
			               "Compare must satisfy the compare concept, which includes BinaryPredicate.  See "
			               "http://en.cppreference.com/w/cpp/concept/Compare" );

			while( ( first1 != last1 ) && ( first2 != last2 ) ) {
				if( comp( *first1, *first2 ) ) {
					return true;
				}
				if( comp( *first2, *first1 ) ) {
					return false;
				}
				++first1;
				++first2;
			}
			return ( first1 == last1 ) && ( first2 != last2 );
		}

		/// Returns true if the first range [first1, last1) is lexigraphically less than the second range [first2, last2)
		///
		/// @tparam InputIterator1 Iterator type for start of range 1
		/// @tparam LastType1 Type for value at end of range 1
		/// @tparam InputIterator2 Iterator type for start of range 2
		/// @tparam LastType2 Type for value at end of range 2
		/// @param first1 Start of range 1
		/// @param last1 End of range 1
		/// @param first2 Start of range 1
		/// @param last2 End of Range 1
		/// @return true of range 1 is lexigraphically less than range 2
		template<typename InputIterator1, typename LastType1, typename InputIterator2, typename LastType2>
		constexpr bool
		lexicographical_compare( InputIterator1 first1, LastType1 last1, InputIterator2 first2,
		                         LastType2 last2 ) noexcept( noexcept( ( *first1 < *first2 ) != ( *first2 < *first1 ) ) ) {

			while( ( first1 != last1 ) && ( first2 != last2 ) ) {
				if( *first1 < *first2 ) {
					return true;
				}
				if( *first2 < *first1 ) {
					return false;
				}
				++first1;
				++first2;
			}
			return ( first1 == last1 ) && ( first2 != last2 );
		}

		/// Apply the TransformFunction on the value referenced by range [first, last) when the predicate returns true for
		/// that value
		///
		/// @tparam InputIterator Type of Iterator for start of range
		/// @tparam LastType Type for representing end of range
		/// @tparam OutputIterator Iterator for output range
		/// @tparam UnaryPredicate A unary predicate that takes the dereferenced InputIterator as an arugment
		/// @tparam TransformFunction Takes the dereferenced InputIterator as an arugment and returns a value assignable to
		/// the dereferenced OutputIterator
		/// @param first first item in range [first, last)
		/// @param last last item in range [first, last)
		/// @param first_out Output iterator written to when predicate returns true
		/// @param pred predicate to determine if a transform should happen
		/// @param trans transform function to convert from input range to output range
		/// @return The end of the output range
		template<typename InputIterator, typename LastType, typename OutputIterator, typename UnaryPredicate,
		         typename TransformFunction>
		constexpr OutputIterator transform_if( InputIterator first, LastType const last, OutputIterator first_out,
		                                       UnaryPredicate pred, TransformFunction trans ) {

			static_assert( is_iterator_v<InputIterator>, "first is does not satisfy the requirements of an Iterator" );
			static_assert( is_iterator_v<OutputIterator>, "first_out is does not satisfy the requirements of an Iterator" );
			static_assert( is_unary_predicate_v<UnaryPredicate, decltype( *first )>,
			               "pred does not satisfy the Unary Predicate concept.  See "
			               "http://en.cppreference.com/w/cpp/concept/Predicate for more information" );

			static_assert( is_callable_v<TransformFunction, decltype( *first )>,
			               "TransformFunction does not accept a single argument of the dereferenced type of first" );

			static_assert( is_assignable_v<decltype( *first_out ), decltype( trans( *first ) )>,
			               "Result of trans( *first ) must be assignable to *first_out" );

			while( first != last ) {
				if( pred( *first ) ) {
					*first_out = trans( *first );
				}
				++first;
			}
			return first_out;
		}

		/// Run the transform function unary_op on n elements of range started by first
		///
		/// @tparam InputIterator input range iterator type
		/// @tparam OutputIterator output range iterator type
		/// @tparam UnaryOperation callable that takes the dereferenced value from input range and is assignable to the
		/// dereferenced value of output range
		/// @param first first element in input range [first, first + count)
		/// @param first_out first element in output range [first_out, first_out + count)
		/// @param count number of items to process
		/// @param unary_op callable that transforms items from input range to items of output range
		/// @return last item in output range
		template<typename InputIterator, typename OutputIterator, typename UnaryOperation>
		constexpr OutputIterator
		transform_n( InputIterator first, OutputIterator first_out, size_t count,
		             UnaryOperation unary_op ) noexcept( noexcept( *first_out = unary_op( *first ) ) ) {

			static_assert( is_callable_v<UnaryOperation, decltype( *first )>,
			               "UnaryOperation does not accept a single argument of the dereferenced type of first" );

			static_assert( is_assignable_v<decltype( *first_out ), decltype( unary_op( *first ) )>,
			               "Result of unary_op( *first ) must be assignable to *first_out" );

			while( count-- > 0 ) {
				*first_out = unary_op( *first );
				++first;
				++first_out;
			}
			return first_out;
		}

		/// Transform range [first, last) and output to range [first_out, first_out + std::distance( first, last ))
		///
		/// @tparam InputIterator input range iterator type
		/// @tparam LastType type of Iterator marking end of input range
		/// @tparam OutputIterator output range iterator type
		/// @tparam UnaryOperation callable that takes the dereferenced value from input range and is assignable to the
		/// dereferenced value of output range
		/// @param first first element in input range [first, first + count)
		/// @param last end of input range
		/// @param first_out first element in output range [first_out, first_out + count)
		/// @param unary_op callable that transforms items from input range to items of output range
		/// @return last item in output range
		template<typename InputIterator, typename LastType, typename OutputIterator, typename UnaryOperation>
		constexpr OutputIterator
		transform( InputIterator first, LastType last, OutputIterator first_out,
		           UnaryOperation unary_op ) noexcept( noexcept( *first_out = unary_op( *first ) ) ) {

			static_assert( is_callable_v<UnaryOperation, decltype( *first )>,
			               "UnaryOperation does not accept a single argument of the dereferenced type of first" );

			static_assert( is_assignable_v<decltype( *first_out ), decltype( unary_op( *first ) )>,
			               "Result of unary_op( *first ) must be assignable to *first_out" );

			while( first != last ) {
				*first_out = unary_op( *first );
				++first;
				++first_out;
			}
			return first_out;
		}

		/// Transform input range [first, last) to output range [first_out, first_out + std::distance(first, last)).
		///
		/// @tparam InputIterator type of Iterator of input range
		/// @tparam LastType type of Iterator marking end of input range
		/// @tparam OutputIterator type of iterator for output range
		/// @tparam BinaryOperation a callable that takes the type of the dereference input range iterator and the output
		/// range as argument, returning the next position in output range
		/// @param first start of input range
		/// @param last end of input range
		/// @param first_out first item in output range
		/// @param binary_op transformation function
		/// @return end of output range written to
		template<typename InputIterator, typename LastType, typename OutputIterator, typename BinaryOperation>
		constexpr OutputIterator
		transform_it( InputIterator first, LastType last, OutputIterator first_out,
		              BinaryOperation binary_op ) noexcept( noexcept( first_out = binary_op( *first++, first_out ) ) ) {

			while( first != last ) {
				first_out = binary_op( *first, first_out );
				++first;
			}
			return first_out;
		}

		/// Copy input range [first, last) to output range [first_out, first_out + std::distance( first, last ))
		///
		/// @tparam InputIterator type of Iterator of input range
		/// @tparam LastType type of Iterator marking end of input range
		/// @tparam OutputIterator type of iterator for output range
		/// @param first start of input range
		/// @param last end of input range
		/// @param first_out first item in output range
		/// @return end of output range written to
		template<typename InputIterator, typename LastType, typename OutputIterator>
		constexpr OutputIterator copy( InputIterator first, LastType last,
		                               OutputIterator first_out ) noexcept( noexcept( *first_out = *first ) ) {
			while( first != last ) {
				*first_out = *first;
				++first;
				++first_out;
			}
			return first_out;
		}

		/// Copy input range [first, last) to output range [first_out, first_out + count)
		///
		/// @tparam InputIterator type of Iterator of input range
		/// @tparam OutputIterator type of iterator for output range
		/// @param first start of input range
		/// @param first_out first item in output range
		/// @param count number of items to copy
		/// @return end of output range written to
		template<typename InputIterator, typename OutputIterator>
		constexpr OutputIterator copy_n( InputIterator first, OutputIterator first_out,
		                                 size_t count ) noexcept( noexcept( *first_out = *first ) ) {
			while( count-- > 0 ) {
				*first_out = *first;
				++first;
				++first_out;
			}
			return first_out;
		}

		/// Move values from input range [first, last) to output range [first_out, std::distance( first, last))
		///
		/// @tparam InputIterator type of Iterator of input range
		/// @tparam LastType type of Iterator marking end of input range
		/// @tparam OutputIterator type of iterator for output range
		/// @param first start of input range
		/// @param last end of input range
		/// @param first_out first item in output range
		/// @return end of output range written to
		template<typename InputIterator, typename LastType, typename OutputIterator>
		constexpr OutputIterator move( InputIterator first, LastType const last,
		                               OutputIterator first_out ) noexcept( noexcept( *first_out = std::move( *first ) ) ) {

			while( first != last ) {
				*first_out = std::move( *first );
				++first;
				++first_out;
			}
			return first_out;
		}

		/// Move values from input range [first, last) to output range [first_out, first_out + count)
		///
		/// @tparam InputIterator type of Iterator of input range
		/// @tparam OutputIterator type of iterator for output range
		/// @param first start of input range
		/// @param first_out first item in output range
		/// @param count number of items to move
		/// @return end of output range written to
		template<typename InputIterator, typename OutputIterator>
		constexpr OutputIterator move_n( InputIterator first, OutputIterator first_out,
		                                 size_t count ) noexcept( noexcept( *first_out = std::move( *first ) ) ) {

			while( count-- > 0 ) {
				*first_out = std::move( *first );
				++first;
				++first_out;
			}
			return first_out;
		}

		/// Determine if two ranges [first1, last1) and [first2, first2 + std::distance( first1, last1 )) are equal
		///
		/// @tparam InputIterator1 type of Iterator of first input range
		/// @tparam LastType type of Iterator marking end of first input range
		/// @tparam InputIterator2 type of Iterator of second input range
		/// @param first1 start of first input range
		/// @param last1 end of first input range
		/// @param first2 start of second input range
		/// @return true if both ranges are equal
		template<typename InputIterator1, typename LastType, typename InputIterator2>
		constexpr bool equal( InputIterator1 first1, LastType last1,
		                      InputIterator2 first2 ) noexcept( noexcept( *first1 == *first2 ) ) {

			while( ( first1 != last1 ) && ( *first1 == *first2 ) ) {
				++first1;
				++first2;
			}
			return !( first1 != last1 );
		}

		/// Determine if two ranges [first1, last1) and [first2, last2)
		///
		/// @tparam InputIterator1 type of Iterator of first input range
		/// @tparam LastType1 type of Iterator marking end of first input range
		/// @tparam InputIterator2 type of Iterator of second input range
		/// @tparam LastType2 type of Iterator marking end of second input range
		/// @param first1 start of first input range
		/// @param last1 end of first input range
		/// @param first2 start of second input range
		/// @param last2 end of second input range
		/// @return true if both ranges are equal
		template<typename InputIterator1, typename LastType1, typename InputIterator2, typename LastType2>
		constexpr bool equal( InputIterator1 first1, LastType1 last1, InputIterator2 first2,
		                      LastType2 last2 ) noexcept( noexcept( *first1 == *first2 ) ) {

			while( ( first1 != last1 ) && ( first2 != last2 ) && ( *first1 == *first2 ) ) {
				++first1;
				++first2;
			}

			return !( first1 != last1 ) && !( first2 != last2 );
		}

		/// Determine if two ranges [first1, last1) and [first2, last2) using pred
		///
		/// @tparam InputIterator1 type of Iterator of first input range
		/// @tparam LastType1 type of Iterator marking end of first input range
		/// @tparam InputIterator2 type of Iterator of second input range
		/// @tparam LastType2 type of Iterator marking end of second input range
		/// @tparam Compare type of predicate fullfilling Compare concept
		/// @param first1 start of first input range
		/// @param last1 end of first input range
		/// @param first2 start of second input range
		/// @param last2 end of second input range
		/// @param pred predicate to determine equality of elements
		/// @return true if both ranges are equal
		template<typename InputIterator1, typename LastType1, typename InputIterator2, typename LastType2, typename Compare>
		constexpr bool equal( InputIterator1 first1, LastType1 last1, InputIterator2 first2, LastType2 last2,
		                      Compare pred ) noexcept( noexcept( pred( *first1, *first2 ) ) ) {

			while( ( first1 != last1 ) && ( first2 != last2 ) && pred( *first1, *first2 ) ) {
				++first1;
				++first2;
			}
			return !( first1 != last1 ) && !( first2 != last2 );
		}

		/// constexpr version of std::swap
		///
		/// @tparam T first type to swap
		/// @tparam U second type to swap
		/// @param t first value to swap
		/// @param u second value to swap
		template<typename T, typename U,
		         std::enable_if_t<is_convertible_v<T, U> && is_convertible_v<U, T> && is_nothrow_move_assignable_v<T> &&
		                            is_nothrow_move_assignable_v<U>,
		                          std::nullptr_t> = nullptr>
		constexpr void swapper( T &t, U &u ) noexcept {
			T tmp = std::move( t );
			t = std::move( u );
			u = std::move( tmp );
		}

		/// Performs a left rotation on a range of elements.
		///
		/// @tparam ForwardIterator type of Iterator for items in range
		/// @tparam LastType type that is equal to ForwardIterator when end of range reached
		/// @param first first item in range
		/// @param middle middle of range, first item in new range
		/// @param last last item in range
		template<typename ForwardIterator, typename LastType>
		constexpr void rotate( ForwardIterator first, ForwardIterator middle,
		                       LastType last ) noexcept( noexcept( swapper( *first, *middle ) ) ) {

			ForwardIterator tmp = middle;
			while( first != tmp ) {
				daw::algorithm::swapper( *first, *tmp );
				++first;
				++tmp;
				if( tmp == last ) {
					tmp = middle;
				} else if( first == middle ) {
					middle = tmp;
				}
			}
		}

		/// Returns an iterator pointing to the first element in the range [first, last) that is greater than value, or last if no such element is found.
		///
		/// @tparam RandomIterator Iteratot type pointing to range
		/// @tparam T a value comparable to the dereferenced RandomIterator
		/// @param first first item in range
		/// @param last end of range
		/// @param value value to compare to
		/// @return position of first element greater than value or last
		template<typename RandomIterator, typename T>
		constexpr RandomIterator upper_bound( RandomIterator first, RandomIterator last, T const &value ) noexcept {
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
		constexpr void insertion_sort( RandomIterator1 first, RandomIterator2 const last ) noexcept {
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

			static_assert(
			  is_unary_predicate_v<UnaryOperation, decltype( *first )>,
			  "UnaryOperation must take one value of the dereferenced type of first e.g. unary_op( *first ) must be valid" );

			while( first != last ) {
				*first_out++ = unary_op( *first++ );
			}
		}

		template<typename T, typename RandomIterator, typename RandomIteratorLast, typename BinaryOperation>
		constexpr T reduce( RandomIterator first, RandomIteratorLast last, T init,
		                    BinaryOperation binary_op ) noexcept( noexcept( init = binary_op( init, *first++ ) ) ) {

			static_assert( is_binary_predicate_v<BinaryOperation, T, decltype( *first )>,
			               "BinaryOperation passed to reduce must take two values referenced by first. e.g binary_op( "
			               "init, *first) ) "
			               "must be valid" );

			static_assert( is_convertible_v<decltype( binary_op( init, *first++ ) ), T>,
			               "Result of BinaryOperation must be convertable to type of value referenced by RandomIterator. "
			               "e.g. *first = binary_op( *first, *(first + 1) ) must be valid." );

			while( first != last ) {
				init = binary_op( init, *first++ );
			}
			return std::move( init );
		}

		template<typename InputIterator1, typename InputIterator1Last, typename InputIterator2, typename T,
		         typename ReduceFunction, typename MapFunction>
		constexpr T
		map_reduce( InputIterator1 first1, InputIterator1Last last1, InputIterator2 first2, T init,
		            ReduceFunction reduce_func,
		            MapFunction map_func ) noexcept( noexcept( reduce_func( init, map_func( *first1, *first2 ) ) ) ) {

			static_assert( is_binary_predicate_v<MapFunction, decltype( *first1 ), decltype( *first2 )>,
			               "BinaryOperation map_func passed take two values referenced by first. e.g map_func( *first1, "
			               "*first2 ) must be valid" );

			static_assert( is_binary_predicate_v<ReduceFunction, T, decltype( map_func( *first1, *first2 ) )>,
			               "BinaryOperation reduce_func must take two values referenced by first. e.g reduce_func( init, "
			               "map_func( *first1, *first2 ) ) must be valid" );

			while( first1 != last1 ) {
				init = reduce_func( init, map_func( *first1, *first2 ) );
				++first1;
				++first2;
			}
			return init;
		}

		template<class ForwardIt1, class ForwardIt2>
		constexpr ForwardIt1 search( ForwardIt1 first, ForwardIt1 last, ForwardIt2 s_first,
		                             ForwardIt2 s_last ) noexcept( noexcept( *first == *s_first ) ) {
			for( ;; ++first ) {
				ForwardIt1 it = first;
				for( ForwardIt2 s_it = s_first;; ++it, ++s_it ) {
					if( s_it == s_last ) {
						return first;
					}
					if( it == last ) {
						return last;
					}
					if( !( *it == *s_it ) ) {
						break;
					}
				}
			}
		}

		template<class ForwardIt1, class ForwardIt2, class BinaryPredicate>
		constexpr ForwardIt1 search( ForwardIt1 first, ForwardIt1 last, ForwardIt2 s_first, ForwardIt2 s_last,
		                             BinaryPredicate p ) noexcept( noexcept( !p( *first, *s_first ) ) ) {
			for( ;; ++first ) {
				ForwardIt1 it = first;
				for( ForwardIt2 s_it = s_first;; ++it, ++s_it ) {
					if( s_it == s_last ) {
						return first;
					}
					if( it == last ) {
						return last;
					}
					if( !p( *it, *s_it ) ) {
						break;
					}
				}
			}
		}

		template<typename InputIterator, typename T>
		constexpr T accumulate( InputIterator first, InputIterator last, T init ) noexcept {
			for( ; first != last; ++first ) {
				init = init + *first;
			}
			return init;
		}

		template<typename InputIterator, typename T, typename BinaryOperation>
		constexpr T accumulate( InputIterator first, InputIterator last, T init,
		                        BinaryOperation op ) noexcept( noexcept( op( init, *first ) ) ) {

			for( ; first != last; ++first ) {
				init = op( init, *first );
			}
			return init;
		}

		template<typename ForwardIterator, typename LastType, typename Compare>
		constexpr auto minmax_element( ForwardIterator first, LastType last,
		                               Compare comp ) noexcept( noexcept( comp( *first, *first ) ) ) {

			static_assert( daw::is_binary_predicate_v<Compare, decltype( *first ), decltype( *first )>,
			               "Compare must satisfy the compare concept, which includes BinaryPredicate.  See "
			               "http://en.cppreference.com/w/cpp/concept/Compare" );

			struct {
				ForwardIterator min_element;
				ForwardIterator max_element;
			} result{first, first};

			if( first == last ) {
				return result;
			}
			if( ++first == last ) {
				return result;
			}
			if( comp( *first, *result.min_element ) ) {
				result.min_element = first;
			} else {
				result.max_element = first;
			}

			while( ++first != last ) {
				auto i = first;
				if( ++first == last ) {
					if( comp( *i, *result.min_element ) ) {
						result.min_element = i;
					} else if( !( comp( *i, *result.max_element ) ) ) {
						result.max_element = i;
					}
					break;
				} else {
					if( comp( *first, *i ) ) {
						if( comp( *first, *result.min_element ) ) {
							result.min_element = first;
						}
						if( !( comp( *i, *result.max_element ) ) ) {
							result.max_element = i;
						}
					} else {
						if( comp( *i, *result.min_element ) ) {
							result.min_element = i;
						}
						if( !( comp( *first, *result.max_element ) ) ) {
							result.max_element = first;
						}
					}
				}
			}
			return result;
		}

		template<typename ForwardIterator, typename LastType>
		constexpr decltype( auto )
		minmax_element( ForwardIterator first,
		                LastType last ) noexcept( noexcept( minmax_element( first, last, std::less<>{} ) ) ) {

			return daw::algorithm::minmax_element( first, last, std::less<>{} );
		}
	} // namespace algorithm
} // namespace daw
