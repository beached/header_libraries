// The MIT License (MIT)
//
// Copyright (c) 2013-2018 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
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
#include <cstddef>
#include <functional>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "cpp_17.h"
#include "daw_exception.h"
#include "daw_traits.h"
#include "impl/daw_math_impl.h"

namespace daw {
	template<typename Map, typename Key>
	constexpr decltype( auto ) try_get( Map &container, Key &&k ) {
		auto pos = container.find( std::forward<Key>( k ) );
		using result_t = daw::remove_cvref_t<decltype( *pos )>;
		if( pos == std::end( container ) ) {
			return std::optional<result_t>{};
		}
		return std::optional<result_t>( std::in_place, *pos );
	}

	template<typename Map, typename Key>
	constexpr decltype( auto ) try_get( Map const &container, Key &&k ) {
		auto pos = container.find( std::forward<Key>( k ) );
		using result_t = daw::remove_cvref_t<decltype( *pos )>;
		if( pos == std::end( container ) ) {
			return std::optional<result_t>{};
		}
		return std::optional<result_t>( std::in_place, *pos );
	}

	// Iterator movement functions
	namespace impl {
		template<typename Iterator, typename Iterator2, typename Iterator3,
		         typename Distance,
		         std::enable_if_t<!traits::is_random_access_iterator<Iterator>,
		                          std::nullptr_t> = nullptr>
		constexpr void safe_advance_impl( Iterator2 const first, Iterator &it,
		                                  Iterator3 const last,
		                                  Distance dist ) noexcept {

			// Move it forward towards last
			for( ; it != last and dist > 0; --dist, ++it ) {}

			// Move it backwards towards first
			for( ; it != first and dist < 0; ++dist, --it ) {}
		}

		template<typename Iterator, typename Iterator2, typename Iterator3,
		         typename Distance,
		         std::enable_if_t<traits::is_random_access_iterator<Iterator>,
		                          std::nullptr_t> = nullptr>
		constexpr void safe_advance_impl( Iterator2 const first, Iterator &it,
		                                  Iterator3 const last,
		                                  Distance dist ) noexcept {

			if( dist < 0 ) {
				auto const dist_to_first = it - first;
				if( dist_to_first < 0 ) {
					it = first;
					return;
				}
				dist = -dist;
				it -= daw::min( static_cast<ptrdiff_t>( dist_to_first ),
				                static_cast<ptrdiff_t>( dist ) );
				return;
			}
			auto const dist_to_last = last - it;
			if( dist_to_last < 0 ) {
				it = last;
				return;
			}
			it += daw::min( static_cast<ptrdiff_t>( dist_to_last ),
			                static_cast<ptrdiff_t>( dist ) );
		}
	} // namespace impl

	/// @brief Advance Iterator within the bounds of container
	/// @tparam Container Container type who's iterators are of type Iterator
	/// @tparam Iterator Iterator pointing to members of container.
	/// @param container container to set bounds on iterator
	/// @param it iterator to move.  It is undefined behavior if iterator is
	/// outside the range [std::begin(container), std::end(container)]
	/// @param distance how far to move
	template<typename Container, typename Iterator, typename Distance>
	constexpr void safe_advance( Container &container, Iterator &it,
	                             Distance distance ) noexcept {

		traits::is_container_like_test<Container>( );
		traits::is_iterator_test<Iterator>( );

		using std::begin;
		using std::end;
		auto const d = static_cast<ptrdiff_t>( distance );
		impl::safe_advance_impl( begin( container ), it, end( container ), d );
	}

	/// @brief Advance Iterator within the bounds of container
	/// @tparam Container Container type who's iterators are of type Iterator
	/// @tparam Iterator Iterator pointing to members of container.
	/// @param container container to set bounds on iterator
	/// @param it iterator to move.  It is undefined behavior if iterator is
	/// outside the range [std::begin(container), std::end(container)]
	/// @param distance how far to move
	template<typename Container, typename Iterator, typename Distance>
	constexpr void safe_advance( Container const &container, Iterator &it,
	                             Distance distance ) noexcept {

		traits::is_container_like_test<Container>( );
		traits::is_iterator_test<Iterator>( );

		using std::begin;
		using std::end;
		auto const d = static_cast<ptrdiff_t>( distance );
		impl::safe_advance_impl( begin( container ), it, end( container ), d );
	}

	/// @brief Advance iterator n steps forward but do not go past last.
	/// Undefined if it > last
	/// @tparam Iterator Type of Iterator to advance
	/// @param it iterator to advance
	/// @param last boundary for it
	/// @param n number of steps to advance forwards
	/// @return The resulting iterator advanced n steps
	template<typename Iterator, typename Distance = size_t>
	constexpr Iterator
	safe_next( Iterator it, Iterator const last,
	           Distance n = 1U ) noexcept( noexcept( daw::next( it, n ) ) ) {

		traits::is_iterator_test<Iterator>( );
		auto const d = static_cast<ptrdiff_t>( n );
		impl::safe_advance_impl( it, it, last, d );
		return it;
	}

	/// @brief Advance iterator n steps backward but do not go past first.
	/// Undefined if it < first
	/// @tparam Iterator Type of Iterator to advance
	/// @param it iterator to advance
	/// @param first boundary for it
	/// @param n number of steps to advance backwards
	/// @return The resulting iterator advanced n steps
	template<typename Iterator, typename Distance>
	constexpr Iterator
	safe_prev( Iterator it, Iterator first,
	           Distance n = 1 ) noexcept( noexcept( daw::prev( it, n ) ) ) {

		traits::is_iterator_test<Iterator>( );
		auto const d = static_cast<ptrdiff_t>( n );
		impl::safe_advance_impl( first, it, it, -d );
		return it;
	}

	/// @brief Take iterator return from begin of a container and return the
	/// result of running next with n steps
	/// @tparam Container Container type iterator will come from
	/// @param container container to get iterator from
	/// @param n how many steps to move forward from begin
	/// @return an iterator referencing a value in container n steps from begin
	template<typename Container>
	constexpr auto
	begin_at( Container &container,
	          size_t n ) noexcept( noexcept( std::begin( container ) ) )
	  -> decltype( std::begin( container ) ) {

		traits::is_container_like_test<Container>( );
		auto result = std::begin( container );
		safe_advance( container, result, static_cast<ptrdiff_t>( n ) );
		return result;
	}

	/// @brief Take iterator return from begin of a container and return the
	/// result of running next with n steps
	/// @tparam Container Container type iterator will come from
	/// @param container container to get iterator from
	/// @param n how many steps to move forward from begin
	/// @return an iterator referencing a value in container n steps from begin
	template<typename Container>
	constexpr auto
	begin_at( Container const &container,
	          size_t n ) noexcept( noexcept( std::cbegin( container ) ) )
	  -> decltype( std::cbegin( container ) ) {

		traits::is_container_like_test<Container>( );

		auto result = std::cbegin( container );
		safe_advance( container, result, static_cast<ptrdiff_t>( n ) );
		return result;
	}

	namespace algorithm {
		template<typename Lhs>
		constexpr auto const &min_item( Lhs const &lhs ) noexcept {
			return lhs;
		}

		template<
		  typename Lhs, typename... Ts,
		  std::enable_if_t<( sizeof...( Ts ) > 0 ), std::nullptr_t> = nullptr>
		constexpr auto const &min_item( Lhs const &lhs,
		                                Ts const &... ts ) noexcept {
			auto const &rhs = min_item( ts... );
			return lhs < rhs ? lhs : rhs;
		}

		template<typename Lhs>
		constexpr auto const &max_item( Lhs const &lhs ) noexcept {
			return lhs;
		}

		template<
		  typename Lhs, typename... Ts,
		  std::enable_if_t<( sizeof...( Ts ) > 0 ), std::nullptr_t> = nullptr>
		constexpr auto const &max_item( Lhs const &lhs,
		                                Ts const &... ts ) noexcept {
			auto const &rhs = max_item( ts... );
			return lhs > rhs ? lhs : rhs;
		}

		namespace impl {
			template<typename ForwardIterator>
			constexpr ForwardIterator midpoint( ForwardIterator a,
			                                    ForwardIterator b ) {

				traits::is_forward_access_iterator_test<ForwardIterator>( );

				daw::exception::precondition_check(
				  a <= b,
				  " Cannot find a midpoint unless the first parameter is <= the "
				  "second" );

				return daw::next( a, daw::distance( a, b ) / 2 );
			}
		} // namespace impl

		template<typename ForwardIterator, typename Value,
		         typename Compare = std::less<>>
		constexpr ForwardIterator lower_bound( ForwardIterator first,
		                                       ForwardIterator last, Value &&value,
		                                       Compare cmp = {} ) {
			// Precondition checks
			traits::is_forward_access_iterator_test<ForwardIterator>( );
			traits::is_input_iterator_test<ForwardIterator>( );
			traits::is_predicate_test<Compare, decltype( *first ), Value>( );

			while( first != last ) {
				auto mid = std::next( first, std::distance( first, last ) / 2 );
				if( cmp( *mid, value ) ) {
					first = next( mid );
				} else {
					last = mid;
				}
			}
			return first;
		}

		template<typename InputIterator, typename UnaryPredicate>
		constexpr InputIterator find_if( InputIterator first, InputIterator last,
		                                 UnaryPredicate &&unary_predicate ) {
			traits::is_input_iterator_test<InputIterator>( );
			traits::is_unary_predicate_test<UnaryPredicate, decltype( *first )>( );

			while( first != last ) {
				if( unary_predicate( *first ) ) {
					return first;
				}
				std::advance( first, 1 );
			}
			return last;
		}

		template<typename InputIterator, typename UnaryPredicate>
		constexpr InputIterator find_if_not( InputIterator first,
		                                     InputIterator last,
		                                     UnaryPredicate &&unary_predicate ) {
			traits::is_input_iterator_test<InputIterator>( );
			traits::is_unary_predicate_test<UnaryPredicate, decltype( *first )>( );

			while( first != last ) {
				if( !unary_predicate( *first ) ) {
					return first;
				}
				std::advance( first, 1 );
			}
			return last;
		}

		template<typename ForwardIterator, typename UnaryPredicate>
		constexpr ForwardIterator partition( ForwardIterator first,
		                                     ForwardIterator last,
		                                     UnaryPredicate &&unary_predicate ) {
			// Precondition checks
			traits::is_forward_access_iterator_test<ForwardIterator>( );
			traits::is_input_iterator_test<ForwardIterator>( );
			traits::is_unary_predicate_test<UnaryPredicate, decltype( *first )>( );

			first = std::find_if_not( first, last, unary_predicate );
			if( first == last ) {
				return first;
			}

			for( auto it = std::next( first ); it != last; std::advance( it ) ) {
				if( unary_predicate( *it ) ) {
					daw::iter_swap( it, first );
					std::advance( first, 1 );
				}
			}
			return first;
		}

		template<typename ForwardIterator, typename Value,
		         typename Predicate = std::less<>>
		constexpr ForwardIterator
		binary_search( ForwardIterator first, ForwardIterator const last,
		               Value &&value, Predicate less_than = Predicate{} ) {

			traits::is_forward_access_iterator_test<ForwardIterator>( );
			traits::is_input_iterator_test<ForwardIterator>( );
			traits::is_predicate_test<Predicate, decltype( *first ), Value>( );

			exception::precondition_check(
			  first < last, ": First position must be less than second" );

			auto it_last = last;

			while( first < it_last ) {
				auto const mid = impl::midpoint( first, it_last );
				if( less_than( *mid, value ) ) {
					first = mid;
					daw::advance( first, 1 );
				} else if( less_than( value, *mid ) ) {
					it_last = mid;
				} else { // equal
					return mid;
				}
			}
			return last;
		}

		template<typename Container>
		constexpr auto
		rbegin2( Container &container ) noexcept( noexcept( container.rbegin( ) ) )
		  -> decltype( container.rbegin( ) ) {

			traits::is_container_like_test<Container>( );
			return container.rbegin( );
		}

		template<typename Container>
		constexpr auto crbegin2( Container &container ) noexcept(
		  noexcept( container.crbegin( ) ) ) -> decltype( container.crbegin( ) ) {

			traits::is_container_like_test<Container>( );
			return container.crbegin( );
		}

		template<typename Container>
		constexpr auto
		rend2( Container &container ) noexcept( noexcept( container.rend( ) ) )
		  -> decltype( container.rend( ) ) {

			traits::is_container_like_test<Container>( );
			return container.rend( );
		}

		template<typename Container>
		constexpr auto
		crend2( Container &container ) noexcept( noexcept( container.crend( ) ) )
		  -> decltype( container.crend( ) ) {

			traits::is_container_like_test<Container>( );
			return container.crend( );
		}

		template<typename Value>
		[[deprecated( "use vector::data( ) member" )]] inline Value const *
		to_array( std::vector<Value> const &values ) {

			return values.data( );
		}

		template<typename Value>
		[[deprecated( "use vector::data( ) member" )]] inline Value *
		to_array( std::vector<Value> &values ) {

			return values.data( );
		}

		template<typename Ptr>
		[[deprecated]] constexpr bool is_null_ptr( Ptr const *const ptr ) noexcept {
			return nullptr == ptr;
		}

		/// @brief Reverser eg for( auto item: reverse( container ) ) { }
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
			auto reverse_impl( Fwd &fwd, long )
			  -> decltype( Reverser_generic<Fwd>( fwd ) ) {
				return Reverser_generic<Fwd>( fwd );
			}

			template<typename Fwd>
			auto reverse_impl( Fwd &fwd, int )
			  -> decltype( fwd.rbegin( ), Reverser_special<Fwd>( fwd ) ) {
				return Reverser_special<Fwd>( fwd );
			}
		} // namespace details

		template<typename Fwd>
		auto reverse( Fwd &&fwd )
		  -> decltype( details::reverse_impl( fwd, int( 0 ) ) ) {
			static_assert( !is_rvalue_reference_v<Fwd &&>,
			               "Cannot pass rvalue_reference to reverse()" );
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
			return are_equal( v1, v2 ) and are_equal( others... );
		}

		template<typename ForwardIterator>
		constexpr std::pair<ForwardIterator, ForwardIterator>
		slide( ForwardIterator first, ForwardIterator last,
		       ForwardIterator target ) {

			traits::is_forward_access_iterator_test<ForwardIterator>( );
			traits::is_input_iterator_test<ForwardIterator>( );

			if( target < first ) {
				return std::make_pair( target, std::rotate( target, first, last ) );
			}

			if( last < target ) {
				return std::make_pair( std::rotate( first, last, target ), target );
			}

			return std::make_pair( first, last );
		}

		template<typename ForwardIterator, typename Predicate>
		std::pair<ForwardIterator, ForwardIterator>
		gather( ForwardIterator first, ForwardIterator last, ForwardIterator target,
		        Predicate predicate ) {

			traits::is_forward_access_iterator_test<ForwardIterator>( );
			traits::is_input_iterator_test<ForwardIterator>( );

			auto start =
			  std::stable_partition( first, target, std::not1( predicate ) );

			auto finish = std::stable_partition( target, last, predicate );

			return std::make_pair( start, finish );
		}

		template<typename InputIt1, typename InputIt2, typename OutputIt,
		         typename Func>
		constexpr OutputIt transform_many( InputIt1 first1, InputIt1 last1,
		                                   InputIt2 first2, OutputIt first_out,
		                                   Func func ) {

			traits::is_input_iterator_test<InputIt1>( );
			traits::is_input_iterator_test<InputIt2>( );
			traits::is_output_iterator_test<OutputIt,
			                                decltype( func( *first1, *first2 ) )>( );

			while( first1 != last1 ) {
				*first_out = func( *first1, *first2 );
				first1 = daw::next( first1 );
				first2 = daw::next( first2 );
				first_out = daw::next( first_out );
			}
			return first_out;
		}

		template<typename InputIt1, typename InputIt2, typename InputIt3,
		         typename OutputIt, typename Func>
		constexpr OutputIt transform_many( InputIt1 first1, InputIt1 last1,
		                                   InputIt2 first2, InputIt3 first3,
		                                   OutputIt first_out, Func func ) {

			traits::is_input_iterator_test<InputIt1>( );
			traits::is_input_iterator_test<InputIt2>( );
			traits::is_input_iterator_test<InputIt3>( );
			traits::is_output_iterator_test<
			  OutputIt, decltype( func( *first1, *first2, *first3 ) )>( );

			while( first1 != last1 ) {
				*first_out = func( *first1, *first2, *first3 );
				first1 = daw::next( first1 );
				first2 = daw::next( first2 );
				first3 = daw::next( first3 );
				first_out = daw::next( first_out );
			}
			return first_out;
		}

		template<typename InputIt1, typename InputIt2, typename InputIt3,
		         typename InputIt4, typename OutputIt, typename Func>
		constexpr OutputIt transform_many( InputIt1 first1, InputIt1 last1,
		                                   InputIt2 first2, InputIt3 first3,
		                                   InputIt4 first4, OutputIt first_out,
		                                   Func func ) {

			traits::is_input_iterator_test<InputIt1>( );
			traits::is_input_iterator_test<InputIt2>( );
			traits::is_input_iterator_test<InputIt3>( );
			traits::is_input_iterator_test<InputIt4>( );
			traits::is_output_iterator_test<
			  OutputIt, decltype( func( *first1, *first2, *first3, *first4 ) )>( );

			while( first1 != last1 ) {
				*first_out = func( *first1, *first2, *first3, *first4 );
				first1 = daw::next( first1 );
				first2 = daw::next( first2 );
				first3 = daw::next( first3 );
				first4 = daw::next( first4 );
				first_out = daw::next( first_out );
			}
			return first_out;
		}
		template<typename InputIt1, typename InputIt2, typename InputIt3,
		         typename InputIt4, typename InputIt5, typename OutputIt,
		         typename Func>
		constexpr OutputIt transform_many( InputIt1 first1, InputIt1 last1,
		                                   InputIt2 first2, InputIt3 first3,
		                                   InputIt4 first4, InputIt4 first5,
		                                   OutputIt first_out, Func func ) {

			traits::is_input_iterator_test<InputIt1>( );
			traits::is_input_iterator_test<InputIt2>( );
			traits::is_input_iterator_test<InputIt3>( );
			traits::is_input_iterator_test<InputIt4>( );
			traits::is_input_iterator_test<InputIt5>( );
			traits::is_output_iterator_test<OutputIt,
			                                decltype( func( *first1, *first2, *first3,
			                                                *first4, *first5 ) )>( );

			while( first1 != last1 ) {
				*first_out = func( *first1, *first2, *first3, *first4, *first5 );
				first1 = daw::next( first1 );
				first2 = daw::next( first2 );
				first3 = daw::next( first3 );
				first4 = daw::next( first4 );
				first5 = daw::next( first5 );
				first_out = daw::next( first_out );
			}
			return first_out;
		}

		template<typename T>
		constexpr T clamp( T &&value, T &&max_value ) noexcept {
			if( value > max_value ) {
				return std::forward<T>( max_value );
			}
			return std::forward<T>( value );
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
					tuple_functor<I>::run( i, std::forward<T>( t ),
					                       std::forward<F>( f ) );
				}
			}
		}; // struct tuple_functor

		template<>
		struct tuple_functor<0> {
			template<typename T, typename F>
			constexpr static void run( std::size_t, T, F ) {}
		}; // struct tuple_functor

		template<typename ForwardIterator, typename UnaryPredicate>
		constexpr auto find_last_of( ForwardIterator first,
		                             ForwardIterator const last,
		                             UnaryPredicate pred ) {

			traits::is_forward_access_iterator_test<ForwardIterator>( );
			traits::is_unary_predicate_test<UnaryPredicate, decltype( *first )>( );

			auto prev = last;
			while( first != last ) {
				if( !pred( *first ) ) {
					break;
				}
				prev = first;
				first = daw::next( first );
			}
			return prev;
		}

		template<typename ForwardIterator, typename UnaryPredicate>
		constexpr auto find_first_of( ForwardIterator first, ForwardIterator last,
		                              UnaryPredicate pred ) {

			traits::is_forward_access_iterator_test<ForwardIterator>( );
			traits::is_unary_predicate_test<UnaryPredicate, decltype( *first )>( );

			while( first != last ) {
				if( pred( *first ) ) {
					break;
				}
				first = daw::next( first );
			}
			return first;
		}

		template<typename Iterator, typename UnaryPredicate>
		constexpr auto find_first_range_of( Iterator first, Iterator const last,
		                                    UnaryPredicate pred ) {

			traits::is_unary_predicate_test<UnaryPredicate, decltype( *first )>( );

			first = find_first_of( first, last, pred );
			auto second = find_last_of( first, last, pred );
			if( second != last ) {
				second = daw::next( second );
			}
			return std::make_pair( first, second );
		}

		template<typename Iterator, typename Value>
		auto split( Iterator first, Iterator last, Value const &value ) {

			traits::is_iterator_test<Iterator>( );
			using value_type = daw::traits::root_type_t<decltype( *first )>;

			auto result = std::vector<std::vector<value_type>>( );
			auto temp = std::vector<value_type>( );

			for( auto it = first; it != last; ++it ) {
				if( value == *it ) {
					result.push_back( std::move( temp ) );
					temp.clear( ); // TODO: Not sure if necessary, need to check
				} else {
					temp.push_back( *it );
				}
			}
			return result;
		}

		/// @brief Returns true if any function returns true for the value
		/// @param value Argument to UnaryPredicate
		/// @param func A UnaryPredicate that returns true/false
		/// @return The result of func
		template<typename Value, typename UnaryPredicate>
		constexpr bool
		satisfies_one( Value value,
		               UnaryPredicate func ) noexcept( noexcept( func( value ) ) ) {

			traits::is_unary_predicate_test<UnaryPredicate, Value>( );

			return func( value );
		}

		/// @brief Returns true if any function returns true for the value
		/// @param value Argument to UnaryPredicate(s)
		/// @param func A UnaryPredicate that returns true/false
		/// @param funcs UnaryPredicates that return true/false
		/// @return True if any of the func/funcs return true(e.g. like OR)
		template<typename Value, typename UnaryPredicate,
		         typename... UnaryPredicates>
		constexpr bool satisfies_one( Value value, UnaryPredicate func,
		                              UnaryPredicates... funcs ) {

			traits::is_unary_predicate_test<UnaryPredicate, Value>( );

			return func( value ) or satisfies_one( value, funcs... );
		}

		/// @brief Returns true if any function returns true for any value in range
		/// @param first iterator pointing to the beginning of the range inclusively
		/// @param last iterator pointing to the end of the range exclusively
		/// @param func A UnaryPredicate that returns true/false
		/// @param funcs UnaryPredicates that return true/false
		/// @return True if any of the func/funcs return true(e.g. like OR) for any
		/// value in range
		template<
		  typename Iterator, typename Iterator2, typename UnaryPredicate,
		  typename... UnaryPredicates,
		  std::enable_if_t<all_true_v<traits::is_dereferenceable_v<Iterator2>,
		                              traits::is_equality_comparable_v<
		                                daw::traits::deref_t<Iterator2>>>,
		                   std::nullptr_t> = nullptr>
		constexpr bool satisfies_one(
		  Iterator first, Iterator2 last, UnaryPredicate func,
		  UnaryPredicates... funcs ) noexcept( noexcept( satisfies_one( *first,
		                                                                func,
		                                                                funcs... ) ) ) {

			traits::is_iterator_test<Iterator>( );
			traits::is_unary_predicate_test<UnaryPredicate, decltype( *first )>( );

			while( first != last ) {
				if( satisfies_one( *first, func, funcs... ) ) {
					return true;
				}
				first = daw::next( first );
			}
			return false;
		}

		/// @brief Returns true if function returns true for the value
		/// @param value Argument to UnaryPredicate
		/// @param func A UnaryPredicate that returns true/false
		/// @return The result of func
		template<typename Value, typename UnaryPredicate>
		constexpr bool satisfies_all(
		  Value &&value,
		  UnaryPredicate &&func ) noexcept( noexcept( func( value ) ) ) {

			traits::is_unary_predicate_test<UnaryPredicate, Value>( );

			return std::forward<UnaryPredicate>( func )(
			  std::forward<Value>( value ) );
		}

		/// @brief Returns true if all function(s) returns true for the value
		/// @param value Argument to UnaryPredicate(s)
		/// @param func A UnaryPredicate that returns true/false
		/// @param funcs UnaryPredicates that return true/false
		/// @return True if any of the func/funcs return true(e.g. like OR)
		template<typename Value, typename UnaryPredicate,
		         typename... UnaryPredicates>
		constexpr bool satisfies_all( Value value, UnaryPredicate &&func,
		                              UnaryPredicates &&... funcs ) {

			traits::is_unary_predicate_test<UnaryPredicate, Value>( );

			auto const result = std::forward<UnaryPredicate>( func )( value );
			return result and
			       satisfies_all( std::move( value ),
			                      std::forward<UnaryPredicates>( funcs )... );
		}

		/// @brief Returns true if all function(s) returns true for all values in
		/// range
		/// @param first iterator pointing to the beginning of the range inclusively
		/// @param last iterator pointing to the end of the range exclusively
		/// @param func A UnaryPredicate that returns true/false
		/// @param funcs UnaryPredicates that return true/false
		/// @return True if any of the func/funcs return true(e.g. like OR) for any
		/// value in range
		template<
		  typename Iterator, typename Iterator2, typename UnaryPredicate,
		  typename... UnaryPredicates,
		  std::enable_if_t<all_true_v<traits::is_dereferenceable_v<Iterator2>,
		                              traits::is_equality_comparable_v<
		                                daw::traits::deref_t<Iterator2>>>,
		                   std::nullptr_t> = nullptr>
		constexpr bool satisfies_all(
		  Iterator first, Iterator2 last, UnaryPredicate func,
		  UnaryPredicates... funcs ) noexcept( noexcept( satisfies_one( *first,
		                                                                func,
		                                                                funcs... ) ) ) {

			traits::is_unary_predicate_test<UnaryPredicate, decltype( *first )>( );

			while( first != last ) {
				if( !satisfies_all( *first, func, funcs... ) ) {
					return false;
				}
				first = daw::next( first );
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
						daw::exception::daw_throw<std::exception>( );
					};
				}

				template<typename T>
				constexpr bool operator( )( T &&value ) const {
					return m_lower <= value and value <= m_upper;
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

		/// @brief Returns a callable that returns true if the value passed is in
		/// the range [Lower, Upper]
		/// @param lower The lowest value allowed
		/// @param upper The largest value allowed
		/// @returns true if value passed to operator( ) is within the range [lower,
		/// upper]
		template<typename Lower, typename Upper>
		constexpr auto in_range( Lower &&lower, Upper &&upper ) {
			return impl::in_range<Lower, Upper>{std::forward<Lower>( lower ),
			                                    std::forward<Upper>( upper )};
		}

		/// @brief Returns a callable that returns true if value passed is equal to
		/// value construct with
		/// @param value Value to test with
		/// @returns True if value passed to operator( ) is equal to value
		/// constructed with
		template<typename Value>
		constexpr auto equal_to( Value &&value ) {
			return impl::equal_to<Value>{std::forward<Value>( value )};
		}

		/// @brief Returns a callable that returns true if value passed is greater
		/// than the value constructed with
		/// @param value Value to test with
		/// @returns True if value passed to operator( ) is greater than to value
		/// constructed with
		template<typename Value>
		constexpr auto greater_than( Value &&value ) {
			return impl::greater_than<Value>{std::forward<Value>( value )};
		}

		/// @brief Returns a callable that returns true if value passed is greater
		/// than or equal to the value constructed with
		/// @param value Value to test with
		/// @returns True if value passed to operator( ) is greater than or equal to
		/// to value constructed with
		template<typename Value>
		constexpr auto greater_than_or_equal_to( Value &&value ) {
			return impl::greater_than_or_equal_to<Value>{
			  std::forward<Value>( value )};
		}

		/// @brief Returns a callable that returns true if value passed is less than
		/// the value constructed with
		/// @param value Value to test with
		/// @returns True if value passed to operator( ) is less than to value
		/// constructed with
		template<typename Value>
		constexpr auto less_than( Value &&value ) {
			return impl::less_than<Value>{std::forward<Value>( value )};
		}

		/// @brief Returns a callable that returns true if value passed is less than
		/// or equal to the value constructed with
		/// @param value Value to test with
		/// @returns True if value passed to operator( ) is less than or equal to to
		/// value constructed with
		template<typename Value>
		constexpr auto less_than_or_equal_to( Value &&value ) {
			return impl::less_than_or_equal_to<Value>{std::forward<Value>( value )};
		}

		/// @brief Returns true if the first range [first1, last1) is
		/// lexigraphically less than the second range [first2, last2)
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
		/// @return true of range 1 is lexigraphically less than range 2 using
		/// supplied comparison
		template<typename InputIterator1, typename LastType1,
		         typename InputIterator2, typename LastType2, typename Compare>
		constexpr bool lexicographical_compare(
		  InputIterator1 first1, LastType1 last1, InputIterator2 first2,
		  LastType2 last2,
		  Compare comp ) noexcept( noexcept( comp( *first1, *first2 ) !=
		                                     comp( *first2, *first1 ) ) ) {

			traits::is_input_iterator_test<InputIterator1>( );
			traits::is_input_iterator_test<InputIterator2>( );
			traits::is_compare_test<Compare, decltype( *first1 ),
			                        decltype( *first2 )>( );
			traits::is_compare_test<Compare, decltype( *first2 ),
			                        decltype( *first1 )>( );

			while( ( first1 != last1 ) and ( first2 != last2 ) ) {
				if( comp( *first1, *first2 ) ) {
					return true;
				}
				if( comp( *first2, *first1 ) ) {
					return false;
				}
				first1 = daw::next( first1 );
				first2 = daw::next( first2 );
			}
			return ( first1 == last1 ) and ( first2 != last2 );
		}

		/// @brief Returns true if the first range [first1, last1) is
		/// lexigraphically less than the second range [first2, last2)
		/// @tparam InputIterator1 Iterator type for start of range 1
		/// @tparam LastType1 Type for value at end of range 1
		/// @tparam InputIterator2 Iterator type for start of range 2
		/// @tparam LastType2 Type for value at end of range 2
		/// @param first1 Start of range 1
		/// @param last1 End of range 1
		/// @param first2 Start of range 1
		/// @param last2 End of Range 1
		/// @return true of range 1 is lexigraphically less than range 2
		template<typename InputIterator1, typename LastType1,
		         typename InputIterator2, typename LastType2>
		constexpr bool lexicographical_compare(
		  InputIterator1 first1, LastType1 last1, InputIterator2 first2,
		  LastType2 last2 ) noexcept( noexcept( ( *first1 < *first2 ) !=
		                                        ( *first2 < *first1 ) ) ) {

			traits::is_input_iterator_test<InputIterator1>( );
			traits::is_input_iterator_test<InputIterator2>( );

			while( ( first1 != last1 ) and ( first2 != last2 ) ) {
				if( *first1 < *first2 ) {
					return true;
				}
				if( *first2 < *first1 ) {
					return false;
				}
				first1 = daw::next( first1 );
				first2 = daw::next( first2 );
			}
			return ( first1 == last1 ) and ( first2 != last2 );
		}

		/// @brief Apply the TransformFunction on the value referenced by range
		/// [first, last) when the predicate returns true for that value
		/// @tparam ForwardIterator Type of Iterator for start of range
		/// @tparam LastType Type for representing end of range
		/// @tparam OutputIterator Iterator for output range
		/// @tparam UnaryPredicate A unary predicate that takes the dereferenced
		/// InputIterator as an arugment
		/// @tparam TransformFunction Takes the dereferenced InputIterator as an
		/// arugment and returns a value assignable to the dereferenced
		/// OutputIterator
		/// @param first first item in range [first, last)
		/// @param last last item in range [first, last)
		/// @param first_out Output iterator written to when predicate returns true
		/// @param pred predicate to determine if a transform should happen
		/// @param trans transform function to convert from input range to output
		/// range
		/// @return The end of the output range
		template<typename ForwardIterator, typename LastType,
		         typename OutputIterator, typename UnaryPredicate,
		         typename TransformFunction>
		constexpr OutputIterator
		transform_if( ForwardIterator first, LastType const last,
		              OutputIterator first_out, UnaryPredicate pred,
		              TransformFunction trans ) {

			traits::is_forward_access_iterator_test<ForwardIterator>( );
			traits::is_input_iterator_test<ForwardIterator>( );
			traits::is_unary_predicate_test<UnaryPredicate, decltype( *first )>( );
			traits::is_output_iterator_test<OutputIterator,
			                                decltype( pred( *first ) )>( );

			static_assert(
			  traits::is_callable_v<TransformFunction, decltype( *first )>,
			  "TransformFunction does not accept a single argument of "
			  "the dereferenced type of first" );

			while( first != last ) {
				if( pred( *first ) ) {
					*first_out = trans( *first );
				}
				first = daw::next( first );
			}
			return first_out;
		}

		/// @brief Run the transform function unary_op on n elements of range
		/// started by first
		/// @tparam InputIterator input range iterator type
		/// @tparam OutputIterator output range iterator type
		/// @tparam UnaryOperation callable that takes the dereferenced value from
		/// input range and is assignable to the dereferenced value of output range
		/// @param first first element in input range [first, first + count)
		/// @param first_out first element in output range [first_out, first_out +
		/// count)
		/// @param count number of items to process
		/// @param unary_op callable that transforms items from input range to items
		/// of output range
		/// @return last item in output range
		template<typename InputIterator, typename OutputIterator,
		         typename UnaryOperation>
		constexpr OutputIterator transform_n(
		  InputIterator first, OutputIterator first_out, size_t count,
		  UnaryOperation unary_op ) noexcept( noexcept( *first_out =
		                                                  unary_op( *first ) ) ) {

			traits::is_input_iterator_test<InputIterator>( );
			static_assert( traits::is_callable_v<UnaryOperation, decltype( *first )>,
			               "UnaryOperation does not accept a single argument of the "
			               "dereferenced type of first" );

			traits::is_output_iterator_test<OutputIterator,
			                                decltype( unary_op( *first ) )>( );

			while( count-- > 0 ) {
				*first_out = unary_op( *first );
				first = daw::next( first );
				first_out = daw::next( first_out );
			}
			return first_out;
		}

		/// @brief Transform range [first, last) and output to range [first_out,
		/// first_out + std::distance( first, last ))
		/// @tparam InputIterator input range iterator type
		/// @tparam LastType type of Iterator marking end of input range
		/// @tparam OutputIterator output range iterator type
		/// @tparam UnaryOperation callable that takes the dereferenced value from
		/// input range and is assignable to the dereferenced value of output range
		/// @param first first element in input range [first, first + count)
		/// @param last end of input range
		/// @param first_out first element in output range [first_out, first_out +
		/// count)
		/// @param unary_op callable that transforms items from input range to items
		/// of output range
		/// @return last item in output range
		template<typename InputIterator, typename LastType, typename OutputIterator,
		         typename UnaryOperation>
		constexpr OutputIterator transform(
		  InputIterator first, LastType last, OutputIterator first_out,
		  UnaryOperation unary_op ) noexcept( noexcept( *first_out =
		                                                  unary_op( *first ) ) ) {

			traits::is_input_iterator_test<InputIterator>( );

			static_assert( traits::is_callable_v<UnaryOperation, decltype( *first )>,
			               "UnaryOperation does not accept a single argument of the "
			               "dereferenced type of first" );

			traits::is_output_iterator_test<OutputIterator,
			                                decltype( unary_op( *first ) )>( );

			while( first != last ) {
				*first_out = unary_op( *first );
				first = daw::next( first );
				first_out = daw::next( first_out );
			}
			return first_out;
		}

		/// @brief Transform input range [first, last) to output range [first_out,
		/// first_out + std::distance(first, last)).
		/// @tparam InputIterator type of Iterator of input range
		/// @tparam LastType type of Iterator marking end of input range
		/// @tparam OutputIterator type of iterator for output range
		/// @tparam BinaryOperation a callable that takes the type of the
		/// dereference input range iterator and the output range as argument,
		/// returning the next position in output range
		/// @param first start of input range
		/// @param last end of input range
		/// @param first_out first item in output range
		/// @param binary_op transformation function
		/// @return end of output range written to
		template<typename InputIterator, typename LastType, typename OutputIterator,
		         typename BinaryOperation>
		constexpr OutputIterator transform_it(
		  InputIterator first, LastType last, OutputIterator first_out,
		  BinaryOperation
		    binary_op ) noexcept( noexcept( first_out = binary_op( *first++,
		                                                           first_out ) ) ) {
			traits::is_input_iterator_test<InputIterator>( );
			traits::is_iterator_test<OutputIterator>( ); // binary_op sets the value
			                                             // so we cannot test if is
			                                             // output iterator

			while( first != last ) {
				first_out = binary_op( *first, first_out );
				first = daw::next( first );
			}
			return first_out;
		}

		/// @brief Copy input range [first, last) to output range [first_out,
		/// first_out + std::distance( first, last ))
		/// @tparam InputIterator type of Iterator of input range
		/// @tparam LastType type of Iterator marking end of input range
		/// @tparam OutputIterator type of iterator for output range
		/// @param first start of input range
		/// @param last end of input range
		/// @param first_out first item in output range
		/// @return end of output range written to
		template<typename InputIterator, typename LastType, typename OutputIterator>
		constexpr OutputIterator copy(
		  InputIterator first, LastType last,
		  OutputIterator first_out ) noexcept( noexcept( *first_out = *first ) ) {

			traits::is_input_iterator_test<InputIterator>( );
			traits::is_output_iterator_test<OutputIterator, decltype( *first )>( );

			while( first != last ) {
				*first_out = *first;
				first = daw::next( first );
				first_out = daw::next( first_out );
			}
			return first_out;
		}

		/// @brief Copy input range [first, last) to output range [first_out,
		/// first_out + count)
		/// @tparam InputIterator type of Iterator of input range
		/// @tparam OutputIterator type of iterator for output range
		/// @param first start of input range
		/// @param first_out first item in output range
		/// @param count number of items to copy
		/// @return end of output range written to
		template<typename InputIterator, typename OutputIterator>
		constexpr OutputIterator
		copy_n( InputIterator first, OutputIterator first_out,
		        size_t count ) noexcept( noexcept( *first_out = *first ) ) {

			traits::is_input_iterator_test<InputIterator>( );
			traits::is_output_iterator_test<OutputIterator, decltype( *first )>( );

			while( count-- > 0 ) {
				*first_out = *first;
				first = daw::next( first );
				first_out = daw::next( first_out );
			}
			return first_out;
		}

		/// @brief Move values from input range [first, last) to output range
		/// [first_out, std::distance( first, last))
		///
		/// @tparam InputIterator type of Iterator of input range
		/// @tparam LastType type of Iterator marking end of input range
		/// @tparam OutputIterator type of iterator for output range
		/// @param first start of input range
		/// @param last end of input range
		/// @param first_out first item in output range
		/// @return end of output range written to
		template<typename InputIterator, typename LastType, typename OutputIterator>
		constexpr OutputIterator move(
		  InputIterator first, LastType const last,
		  OutputIterator first_out ) noexcept( noexcept( *first_out =
		                                                   std::move( *first ) ) ) {

			traits::is_input_iterator_test<InputIterator>( );
			traits::is_output_iterator_test<OutputIterator,
			                                decltype( std::move( *first ) )>( );

			while( first != last ) {
				*first_out = std::move( *first );
				first = daw::next( first );
				first_out = daw::next( first_out );
			}
			return first_out;
		}

		/// @brief Move values from input range [first, last) to output range
		/// [first_out, first_out + count)
		/// @tparam InputIterator type of Iterator of input range
		/// @tparam OutputIterator type of iterator for output range
		/// @param first start of input range
		/// @param first_out first item in output range
		/// @param count number of items to move
		/// @return end of output range written to
		template<typename InputIterator, typename OutputIterator>
		constexpr OutputIterator move_n(
		  InputIterator first, OutputIterator first_out,
		  size_t count ) noexcept( noexcept( *first_out = std::move( *first ) ) ) {

			traits::is_input_iterator_test<InputIterator>( );
			traits::is_output_iterator_test<OutputIterator,
			                                decltype( std::move( *first ) )>( );

			while( count-- > 0 ) {
				*first_out = std::move( *first );
				first = daw::next( first );
				first_out = daw::next( first_out );
			}
			return first_out;
		}

		/// @brief Determine if two ranges [first1, last1) and [first2, first2 +
		/// std::distance( first1, last1 )) are equal
		/// @tparam InputIterator1 type of Iterator of first input range
		/// @tparam LastType type of Iterator marking end of first input range
		/// @tparam InputIterator2 type of Iterator of second input range
		/// @param first1 start of first input range
		/// @param last1 end of first input range
		/// @param first2 start of second input range
		/// @return true if both ranges are equal
		template<typename InputIterator1, typename LastType,
		         typename InputIterator2>
		constexpr bool
		equal( InputIterator1 first1, LastType last1,
		       InputIterator2 first2 ) noexcept( noexcept( *first1 == *first2 ) ) {

			traits::is_input_iterator_test<InputIterator1>( );
			traits::is_input_iterator_test<InputIterator2>( );

			while( ( first1 != last1 ) and ( *first1 == *first2 ) ) {
				first1 = daw::next( first1 );
				first2 = daw::next( first2 );
			}
			return !( first1 != last1 );
		}

		/// @brief Determine if two ranges [first1, last1) and [first2, last2)
		/// @tparam InputIterator1 type of Iterator of first input range
		/// @tparam LastType1 type of Iterator marking end of first input range
		/// @tparam InputIterator2 type of Iterator of second input range
		/// @tparam LastType2 type of Iterator marking end of second input range
		/// @param first1 start of first input range
		/// @param last1 end of first input range
		/// @param first2 start of second input range
		/// @param last2 end of second input range
		/// @return true if both ranges are equal
		template<typename InputIterator1, typename LastType1,
		         typename InputIterator2, typename LastType2>
		constexpr bool
		equal( InputIterator1 first1, LastType1 last1, InputIterator2 first2,
		       LastType2 last2 ) noexcept( noexcept( *first1 == *first2 ) ) {

			traits::is_input_iterator_test<InputIterator1>( );
			traits::is_input_iterator_test<InputIterator2>( );

			while( ( first1 != last1 ) and ( first2 != last2 ) and
			       ( *first1 == *first2 ) ) {
				first1 = daw::next( first1 );
				first2 = daw::next( first2 );
			}

			return !( first1 != last1 ) and !( first2 != last2 );
		}

		/// @brief Determine if two ranges [first1, last1) and [first2, last2) using
		/// pred
		/// @tparam InputIterator1 type of Iterator of first input range
		/// @tparam LastType1 type of Iterator marking end of first input range
		/// @tparam InputIterator2 type of Iterator of second input range
		/// @tparam LastType2 type of Iterator marking end of second input range
		/// @tparam Compare type of predicate fullfilling Compare concept
		/// @param first1 start of first input range
		/// @param last1 end of first input range
		/// @param first2 start of second input range
		/// @param last2 end of second input range
		/// @param comp predicate to determine equality of elements
		/// @return true if both ranges are equal
		template<typename InputIterator1, typename LastType1,
		         typename InputIterator2, typename LastType2, typename Compare>
		constexpr bool
		equal( InputIterator1 first1, LastType1 last1, InputIterator2 first2,
		       LastType2 last2,
		       Compare comp ) noexcept( noexcept( comp( *first1, *first2 ) ) ) {

			traits::is_input_iterator_test<InputIterator1>( );
			traits::is_input_iterator_test<InputIterator2>( );
			traits::is_compare_test<Compare, decltype( *first1 ),
			                        decltype( *first2 )>( );

			while( ( first1 != last1 ) and ( first2 != last2 ) and
			       comp( *first1, *first2 ) ) {
				first1 = daw::next( first1 );
				first2 = daw::next( first2 );
			}
			return !( first1 != last1 ) and !( first2 != last2 );
		}

		/// @brief constexpr version of std::swap
		/// @tparam T first type to swap
		/// @tparam U second type to swap
		/// @param lhs first value to swap
		/// @param rhs second value to swap
		template<typename T, typename U,
		         std::enable_if_t<
		           all_true_v<is_convertible_v<T, U>, is_convertible_v<U, T>>,
		           std::nullptr_t> = nullptr>
		constexpr void swapper( T &lhs, U &rhs ) noexcept(
		  is_nothrow_move_assignable_v<T> and is_nothrow_move_assignable_v<U> ) {

			auto tmp = std::move( lhs );
			lhs = std::move( rhs );
			rhs = std::move( tmp );
		}

		/// @brief Performs a left rotation on a range of elements.
		/// @tparam ForwardIterator type of Iterator for items in range
		/// @tparam LastType type that is equal to ForwardIterator when end of range
		/// reached
		/// @param first first item in range
		/// @param middle middle of range, first item in new range
		/// @param last last item in range
		template<typename ForwardIterator, typename LastType>
		constexpr void
		rotate( ForwardIterator first, ForwardIterator middle,
		        LastType last ) noexcept( noexcept( swapper( *first, *middle ) ) ) {

			traits::is_forward_access_iterator_test<ForwardIterator>( );
			traits::is_inout_iterator_test<ForwardIterator>( );

			ForwardIterator tmp = middle;
			while( first != tmp ) {
				daw::algorithm::swapper( *first, *tmp );
				first = daw::next( first );
				tmp = daw::next( tmp );
				if( tmp == last ) {
					tmp = middle;
				} else if( first == middle ) {
					middle = tmp;
				}
			}
		}

		/// @brief Returns an iterator pointing to the first element in the range
		/// [first, last) that is greater than value, or last if no such element is
		/// found.
		/// @tparam ForwardIterator Iteratot type pointing to range
		/// @tparam T a value comparable to the dereferenced RandomIterator
		/// @param first first item in range
		/// @param last end of range
		/// @param value value to compare to
		/// @return position of first element greater than value or last
		template<typename ForwardIterator, typename T>
		constexpr ForwardIterator upper_bound(
		  ForwardIterator first, ForwardIterator last,
		  T const &value ) noexcept( noexcept( daw::advance( first, 1 ) ) and
		                             noexcept( ++first ) and
		                             noexcept( daw::distance( first, last ) ) ) {

			traits::is_forward_access_iterator_test<ForwardIterator>( );
			traits::is_input_iterator_test<ForwardIterator>( );

			auto count = daw::distance( first, last );
			while( count > 0 ) {
				auto it = first;
				auto step = count / 2;
				daw::advance( it, step );
				if( !( value < *it ) ) {
					first = daw::next( it );
					it = first;
					count -= step + 1;
				} else {
					count = step;
				}
			}
			return first;
		}

		template<typename RandomIterator, typename Compare = std::less<>>
		constexpr void nth_element(
		  RandomIterator first, RandomIterator nth, RandomIterator const last,
		  Compare comp = Compare{} ) noexcept( noexcept( comp( *first, *nth ) ) &&
		                                       noexcept( swapper( *first,
		                                                          *nth ) ) ) {

			traits::is_random_access_iterator_test<RandomIterator>( );
			traits::is_inout_iterator_test<RandomIterator>( );

			traits::is_compare_test<Compare, decltype( *first ), decltype( *nth )>( );

			if( !( first != last ) ) {
				return;
			}
			while( first != nth ) {
				auto min_idx = first;
				auto j = daw::next( first );
				while( j != last ) {
					if( comp( *j, *min_idx ) ) {
						min_idx = j;
						swapper( *first, *min_idx );
					}
					j = daw::next( j );
				}
				first = daw::next( first );
			}
		}

		/* TODO: remove
		template<typename RandomIterator1, typename RandomIterator2, typename
		Compare> constexpr void quick_sort( RandomIterator1 first, RandomIterator2
		const last, Compare comp ) noexcept {

		  static_assert( traits::is_compare_v<Compare, decltype( *first )>,
		                 "Compare function does not meet the requirements of the
		Compare concept. " "http://en.cppreference.com/w/cpp/concept/Compare" );

		  auto const N = daw::algorithm::distance( first, last );
		  if( N <= 1 ) {
		    return;
		  }
		  auto const pivot = daw::algorithm::next( first, N / 2 );
		  daw::algorithm::nth_element( first, pivot, last, comp );
		  daw::algorithm::quick_sort( first, pivot, comp );
		  daw::algorithm::quick_sort( pivot, last, comp );
		}

		template<typename RandomIterator1, typename RandomIterator2>
		constexpr void insertion_sort( RandomIterator1 first, RandomIterator2 const
		last ) noexcept { for( auto i = first; i != last; ++i ) {
		    daw::algorithm::rotate( daw::algorithm::upper_bound( first, i, *i ), i,
		daw::algorithm::next( i ) );
		  }
		}
		*/

		/// @brief Examines the range [first, last) and finds the largest range
		/// beginning at first in which the elements are sorted in ascending order.
		/// @tparam ForwardIterator Iterator type used to hold range
		/// @param first first item in range
		/// @param last	end of range
		/// @return	ForwardIterator with the last sorted item
		template<typename ForwardIterator>
		constexpr ForwardIterator
		is_sorted_until( ForwardIterator first, ForwardIterator last ) noexcept(
		  noexcept( *first < daw::next( *first ) ) ) {

			traits::is_forward_access_iterator_test<ForwardIterator>( );
			traits::is_input_iterator_test<ForwardIterator>( );

			if( !( first != last ) ) {
				return last;
			}
			auto next_it = daw::next( first );
			while( next_it != last ) {
				if( *next_it < *first ) {
					return next_it;
				}
				first = next_it;
				next_it = daw::next( next_it );
			}
			return last;
		}

		/// @brief Examines the range [first, last) and finds the largest range
		/// beginning at first in which the elements are sorted in ascending order.
		/// @tparam ForwardIterator Iterator type used to hold range
		/// @tparam Compare Comparision function object type that satifies the
		/// Compare concept
		/// @param first first item in range
		/// @param last	end of range
		/// @param comp comparision function object
		/// @return	ForwardIterator with the last sorted item
		template<typename ForwardIterator, typename Compare>
		constexpr ForwardIterator is_sorted_until(
		  ForwardIterator first, ForwardIterator last,
		  Compare comp ) noexcept( noexcept( comp( *first, *first ) ) ) {

			traits::is_forward_access_iterator_test<ForwardIterator>( );
			traits::is_input_iterator_test<ForwardIterator>( );
			traits::is_compare_test<Compare, decltype( *first )>( );

			if( !( first != last ) ) {
				return last;
			}
			auto next_it = daw::next( first );
			while( next_it != last ) {
				if( comp( *next_it, *first ) ) {
					return next_it;
				}
				first = next_it;
				next_it = daw::next( next_it );
			}
			return last;
		}

		template<typename ForwardIterator, typename LastType>
		constexpr bool is_sorted( ForwardIterator first, LastType last ) noexcept(
		  noexcept( *first < *daw::next( first ) and *first < *last and
		            first != last ) and
		  noexcept( daw::next( first ) ) and noexcept( ++first ) ) {

			traits::is_forward_access_iterator_test<ForwardIterator>( );
			traits::is_input_iterator_test<ForwardIterator>( );

			if( !( first != last ) ) {
				return true;
			}
			auto next_it = daw::next( first );
			while( ( next_it != last ) and !( *next_it < *first ) ) {
				first = daw::next( first );
				next_it = daw::next( next_it );
			}
			return !( next_it != last );
		}

		template<typename ForwardIterator, typename LastType, typename Compare>
		constexpr bool
		is_sorted( ForwardIterator first, LastType last,
		           Compare comp ) noexcept( noexcept( comp( *first, *first ) ) ) {

			traits::is_forward_access_iterator_test<ForwardIterator>( );
			traits::is_input_iterator_test<ForwardIterator>( );
			traits::is_compare_test<Compare, decltype( *first )>( );

			if( !( first != last ) ) {
				return true;
			}
			auto next_it = daw::next( first );
			while( ( next_it != last ) and !( *next_it < *first ) ) {
				first = daw::next( first );
				next_it = daw::next( next_it );
			}
			return !( next_it != last );
		}

		template<typename ForwardIterator, typename T>
		constexpr void fill_n( ForwardIterator first, size_t count,
		                       T &&value ) noexcept {

			traits::is_forward_access_iterator_test<ForwardIterator>( );

			for( size_t n = 0; n < count; ++n ) {
				*first = value;
				first = daw::next( first );
			}
		}

		template<typename InputIterator, typename OutputIterator,
		         typename UnaryOperation>
		constexpr void
		map( InputIterator first, InputIterator const last,
		     OutputIterator first_out,
		     UnaryOperation unary_op ) noexcept( noexcept( *daw::next( first_out ) =
		                                                     unary_op( *daw::next(
		                                                       first ) ) ) ) {

			traits::is_input_iterator_test<InputIterator>( );
			traits::is_output_iterator_test<OutputIterator>( );
			traits::is_unary_predicate_test<UnaryOperation, decltype( *first )>( );

			while( first != last ) {
				*first_out = unary_op( *first );
				first = daw::next( first );
				first_out = daw::next( first_out );
			}
		}

		template<typename InputIterator1, typename InputIterator2,
		         typename OutputIterator, typename BinaryOperation>
		constexpr void
		map( InputIterator1 first1, InputIterator1 const last1,
		     InputIterator2 first2, OutputIterator first_out,
		     BinaryOperation
		       Binary_op ) noexcept( noexcept( *daw::next( first_out ) =
		                                         binary_op(
		                                           *daw::next( first1 ),
		                                           *daw::next( first2 ) ) ) ) {

			traits::is_input_iterator_test<InputIterator1>( );
			traits::is_input_iterator_test<InputIterator2>( );
			traits::is_output_iterator_test<OutputIterator>( );
			traits::is_binary_predicate_test<BinaryOperation, decltype( *first1 ),
			                                 decltype( *first2 )>( );

			while( first1 != last1 ) {
				*first_out = unary_op( *first1, *first2 );
				first1 = daw::next( first1 );
				first2 = daw::next( first2 );
				first_out = daw::next( first_out );
			}
		}

		template<typename T, typename RandomIterator, typename RandomIteratorLast,
		         typename BinaryOperation>
		constexpr T reduce(
		  RandomIterator first, RandomIteratorLast last, T init,
		  BinaryOperation
		    binary_op ) noexcept( noexcept( init = binary_op( init, *first++ ) ) ) {

			static_assert(
			  traits::is_binary_predicate_v<BinaryOperation, T, decltype( *first )>,
			  "BinaryOperation passed to reduce must take two values referenced by "
			  "first. e.g binary_op( "
			  "init, *first) ) "
			  "must be valid" );

			static_assert(
			  is_convertible_v<decltype( binary_op( init, *first++ ) ), T>,
			  "Result of BinaryOperation must be convertable to type of value "
			  "referenced by RandomIterator. "
			  "e.g. *first = binary_op( *first, *(first + 1) ) must be valid." );

			while( first != last ) {
				init = binary_op( init, *first );
				first = daw::next( first );
			}
			return std::move( init );
		}

		template<typename InputIterator1, typename InputIterator1Last,
		         typename InputIterator2, typename T, typename ReduceFunction,
		         typename MapFunction>
		constexpr T map_reduce(
		  InputIterator1 first1, InputIterator1Last last1, InputIterator2 first2,
		  T init, ReduceFunction reduce_func,
		  MapFunction
		    map_func ) noexcept( noexcept( reduce_func( init,
		                                                map_func( *first1,
		                                                          *first2 ) ) ) ) {

			static_assert( traits::is_iterator_v<InputIterator1>,
			               "Iterator1 passed to rotate does not meet the "
			               "requirements of the Iterator concept "
			               "http://en.cppreference.com/w/cpp/concept/Iterator" );

			static_assert( traits::is_iterator_v<InputIterator2>,
			               "Iterator2 passed to rotate does not meet the "
			               "requirements of the Iterator concept "
			               "http://en.cppreference.com/w/cpp/concept/Iterator" );

			static_assert(
			  traits::is_binary_predicate_v<MapFunction, decltype( *first1 ),
			                                decltype( *first2 )>,
			  "BinaryOperation map_func passed take two values "
			  "referenced by first. e.g map_func( *first1, "
			  "*first2 ) must be valid" );

			static_assert(
			  traits::is_binary_predicate_v<ReduceFunction, T,
			                                decltype( map_func( *first1, *first2 ) )>,
			  "BinaryOperation reduce_func must take two values referenced by first. "
			  "e.g reduce_func( init, "
			  "map_func( *first1, *first2 ) ) must be valid" );

			while( first1 != last1 ) {
				init = reduce_func( init, map_func( *first1, *first2 ) );
				++first1;
				++first2;
			}
			return init;
		}

		template<class ForwardIterator1, class ForwardIterator2>
		constexpr ForwardIterator1 search(
		  ForwardIterator1 first, ForwardIterator1 last, ForwardIterator2 s_first,
		  ForwardIterator2 s_last ) noexcept( noexcept( *first == *s_first ) ) {

			static_assert(
			  traits::is_forward_access_iterator_v<ForwardIterator1>,
			  "ForwardIterator1 passed to rotate does not meet the requirements of "
			  "the ForwardIterator concept "
			  "http://en.cppreference.com/w/cpp/concept/ForwardIterator" );

			static_assert(
			  traits::is_forward_access_iterator_v<ForwardIterator2>,
			  "ForwardIterator2 passed to rotate does not meet the requirements of "
			  "the ForwardIterator concept "
			  "http://en.cppreference.com/w/cpp/concept/ForwardIterator" );

			for( ;; ++first ) {
				ForwardIterator1 it = first;
				for( ForwardIterator2 s_it = s_first;; ++it, ++s_it ) {
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

		template<typename ForwardIterator1, class ForwardIterator2, class Compare>
		constexpr ForwardIterator1
		search( ForwardIterator1 first, ForwardIterator1 last,
		        ForwardIterator2 s_first, ForwardIterator2 s_last,
		        Compare comp ) noexcept( noexcept( !comp( *first, *s_first ) ) ) {

			static_assert(
			  traits::is_forward_access_iterator_v<ForwardIterator1>,
			  "ForwardIterator1 passed to rotate does not meet the requirements of "
			  "the ForwardIterator concept "
			  "http://en.cppreference.com/w/cpp/concept/ForwardIterator" );

			static_assert(
			  traits::is_forward_access_iterator_v<ForwardIterator2>,
			  "ForwardIterator2 passed to rotate does not meet the requirements of "
			  "the ForwardIterator concept "
			  "http://en.cppreference.com/w/cpp/concept/ForwardIterator" );

			static_assert(
			  traits::is_compare_v<Compare, decltype( *first ), decltype( *s_first )>,
			  "Compare function does not meet the requirements of the Compare "
			  "concept. "
			  "http://en.cppreference.com/w/cpp/concept/Compare" );

			for( ;; ++first ) {
				ForwardIterator1 it = first;
				for( ForwardIterator2 s_it = s_first;; ++it, ++s_it ) {
					if( s_it == s_last ) {
						return first;
					}
					if( it == last ) {
						return last;
					}
					if( !comp( *it, *s_it ) ) {
						break;
					}
				}
			}
		}

		template<typename InputIterator, typename T>
		constexpr T accumulate( InputIterator first, InputIterator last,
		                        T init ) noexcept {
			for( ; first != last; ++first ) {
				init = init + *first;
			}
			return init;
		}

		template<typename InputIterator, typename LastType, typename T,
		         typename BinaryOperation>
		constexpr T accumulate(
		  InputIterator first, LastType last, T init,
		  BinaryOperation binary_op ) noexcept( noexcept( binary_op( init,
		                                                             *first ) ) ) {

			static_assert( traits::is_iterator_v<InputIterator>,
			               "Iterator passed to rotate does not meet the requirements "
			               "of the Iterator concept "
			               "http://en.cppreference.com/w/cpp/concept/Iterator" );

			while( first != last ) {
				init = binary_op( init, *first );
				++first;
			}
			return init;
		}

		/// @brief return the min and max of two items sorted
		///	@tparam T of items to evaluate
		/// @param a item 1
		/// @param b item 2
		/// @return a std::pair<T, T> that has the first member holding min(a, b)
		/// and second max(a, b)
		template<typename T>
		constexpr std::pair<T, T> minmax_item( T const &a, T const &b ) noexcept {
			if( b < a ) {
				return std::pair<T, T>{b, a};
			}
			return std::pair<T, T>{a, b};
		}

		/// @brief return the min and max of two items sorted
		///	@tparam T of items to evaluate
		/// @tparam Compare predicate that returns true if lhs < rhs
		/// @param a item 1
		/// @param b item 2
		/// @param comp comparison predicate
		/// @return a std::pair<T, T> that has the first member holding min(a, b)
		/// and second max(a, b)
		template<typename T, typename Compare>
		constexpr std::pair<T, T> minmax_item( T a, T b, Compare comp ) noexcept {
			static_assert( traits::is_compare_v<Compare, T>,
			               "Compare function does not meet the requirements of the "
			               "Compare concept. "
			               "http://en.cppreference.com/w/cpp/concept/Compare" );
			if( comp( b, a ) ) {
				return std::pair<T, T>{std::move( b ), std::move( a )};
			}
			return std::pair<T, T>{std::move( a ), std::move( b )};
		}

		template<typename ForwardIterator, typename LastType, typename Compare>
		constexpr auto minmax_element(
		  ForwardIterator first, LastType last,
		  Compare comp ) noexcept( noexcept( comp( *first, *first ) ) ) {

			traits::is_forward_access_iterator_test<ForwardIterator>( );
			traits::is_input_iterator_test<ForwardIterator>( );
			traits::is_compare_test<Compare, decltype( *first )>( );

			struct {
				ForwardIterator min_element;
				ForwardIterator max_element;
			} result{first, first};

			if( !( first != last ) ) {
				return result;
			}
			first = daw::next( first );
			if( !( first != last ) ) {
				return result;
			}
			if( comp( *first, *result.min_element ) ) {
				result.min_element = first;
			} else {
				result.max_element = first;
			}

			first = daw::next( first );
			while( first != last ) {
				auto i = first;
				first = daw::next( first );
				if( !( first != last ) ) {
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
				first = daw::next( first );
			}
			return result;
		}

		template<typename ForwardIterator, typename LastType>
		constexpr decltype( auto )
		minmax_element( ForwardIterator first, LastType last ) noexcept(
		  noexcept( minmax_element( first, last, std::less<>{} ) ) ) {

			static_assert(
			  traits::is_forward_access_iterator_v<ForwardIterator>,
			  "ForwardIterator passed to rotate does not meet the requirements of "
			  "the ForwardIterator concept "
			  "http://en.cppreference.com/w/cpp/concept/ForwardIterator" );

			return daw::algorithm::minmax_element( first, last, std::less<>{} );
		}

		template<typename InputIterator1, typename LastType1,
		         typename InputIterator2, typename LastType2,
		         typename OutputIterator>
		constexpr OutputIterator set_intersection(
		  InputIterator1 first1, LastType1 last1, InputIterator2 first2,
		  LastType2 last2,
		  OutputIterator d_first ) noexcept( noexcept( *first2 < *first1 ) &&
		                                     noexcept( *first1 < *first2 ) ) {

			while( first1 != last1 and first2 != last2 ) {
				if( *first1 < *first2 ) {
					++first1;
				} else {
					if( !( *first2 < *first1 ) ) {
						*d_first++ = *first1++;
					}
					++first2;
				}
			}
			return d_first;
		}
	} // namespace algorithm
} // namespace daw
