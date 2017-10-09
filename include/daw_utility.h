// The MIT License (MIT)
//
// Copyright (c) 2014-2017 Darrell Wright
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

#ifdef max
#undef max
#endif // max

#include <cmath>
#include <functional>
#include <initializer_list>
#include <limits>
#include <memory>
#include <random>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

#include "daw_exception.h"
#include "daw_string_view.h"
#include "daw_traits.h"

namespace daw {
	namespace impl {
		template<typename ResultType, typename... ArgTypes>
		struct make_function_pointer_impl {
			using type = typename std::add_pointer<ResultType( ArgTypes... )>::type;
		};

		template<typename ResultType, typename ClassType, typename... ArgTypes>
		struct make_pointer_to_member_function_impl {
			using type = ResultType ( ClassType::* )( ArgTypes... );
		};

		template<typename ResultType, typename ClassType, typename... ArgTypes>
		struct make_pointer_to_volatile_member_function_impl {
			using type = ResultType ( ClassType::* )( ArgTypes... ) volatile;
		};

		template<typename ResultType, typename ClassType, typename... ArgTypes>
		struct make_pointer_to_const_member_function_impl {
			using type = ResultType ( ClassType::* )( ArgTypes... ) const;
		};

		template<typename ResultType, typename ClassType, typename... ArgTypes>
		struct make_pointer_to_const_volatile_member_function_impl {
			using type = ResultType ( ClassType::* )( ArgTypes... ) const volatile;
		};
	} // namespace impl

	template<typename ResultType, typename... ArgTypes>
	using function_pointer_t = typename impl::make_function_pointer_impl<ResultType, ArgTypes...>::type;

	template<typename ResultType, typename ClassType, typename... ArgTypes>
	using pointer_to_member_function_t =
	  typename impl::make_pointer_to_member_function_impl<ResultType, ClassType, ArgTypes...>::type;

	template<typename ResultType, typename ClassType, typename... ArgTypes>
	using pointer_to_volatile_member_function_t =
	  typename impl::make_pointer_to_volatile_member_function_impl<ResultType, ClassType, ArgTypes...>::type;

	template<typename ResultType, typename ClassType, typename... ArgTypes>
	using pointer_to_const_member_function_t =
	  typename impl::make_pointer_to_const_member_function_impl<ResultType, ClassType, ArgTypes...>::type;

	template<typename ResultType, typename ClassType, typename... ArgTypes>
	using pointer_to_const_volatile_member_function_t =
	  typename impl::make_pointer_to_const_volatile_member_function_impl<ResultType, ClassType, ArgTypes...>::type;

	namespace impl {
		template<typename T>
		class EqualToImpl final {
			T m_value;

		public:
			EqualToImpl( ) = delete;
			~EqualToImpl( ) = default;
			constexpr EqualToImpl( EqualToImpl const & ) = default;
			constexpr EqualToImpl &operator=( EqualToImpl const & ) = default;
			constexpr EqualToImpl( EqualToImpl && ) noexcept = default;
			constexpr EqualToImpl &operator=( EqualToImpl && ) noexcept = default;

			constexpr EqualToImpl( T value ) : m_value( std::move( value ) ) {}

			constexpr bool operator( )( T const &value ) noexcept {
				return m_value == value;
			}
		}; // class EqualToImpl
	}    // namespace impl
	template<typename T>
	constexpr impl::EqualToImpl<T> equal_to( T value ) {
		return impl::EqualToImpl<T>( std::move( value ) );
	}

	template<typename T>
	class equal_to_last final {
		T *m_value;

	public:
		~equal_to_last( ) = default;
		constexpr equal_to_last( equal_to_last const & ) noexcept = default;
		constexpr equal_to_last( equal_to_last && ) noexcept = default;
		constexpr equal_to_last &operator=( equal_to_last const & ) noexcept = default;
		constexpr equal_to_last &operator=( equal_to_last && ) noexcept = default;

		constexpr equal_to_last( ) noexcept : m_value( nullptr ) {}

		bool operator( )( T const &value ) noexcept {
			bool result = false;
			if( m_value ) {
				result = *m_value == value;
			}
			m_value = const_cast<T *>( &value );
			return result;
		}
	}; // class equal_to_last

	namespace impl {
		template<typename Function>
		class NotImpl final {
			Function m_function;

		public:
			NotImpl( Function func ) : m_function( func ) {}
			~NotImpl( ) = default;
			NotImpl( NotImpl && ) noexcept = default;
			NotImpl &operator=( NotImpl const & ) = default;
			NotImpl &operator=( NotImpl && ) noexcept = default;

			template<typename... Args>
			bool operator( )( Args &&... args ) {
				return !m_function( std::forward<Args>( args )... );
			}
		}; // class NotImpl
	}    // namespace impl

	template<typename Function>
	impl::NotImpl<Function> Not( Function func ) {
		return impl::NotImpl<Function>( func );
	}

	// For generic types that are functors, delegate to its 'operator()'
	template<typename T>
	struct function_traits : public function_traits<decltype( &T::operator( ) )> {};

	// for pointers to member function(const version)
	template<typename ClassType, typename ReturnType, typename... Args>
	struct function_traits<ReturnType ( ClassType::* )( Args... ) const> {
		static constexpr size_t arity = sizeof...( Args );
		using type = std::function<ReturnType( Args... )>;
		using arg_types = std::tuple<Args...>;
		using result_type = ReturnType;
	};

	// for pointers to member function
	template<typename ClassType, typename ReturnType, typename... Args>
	struct function_traits<ReturnType ( ClassType::* )( Args... )> {
		static constexpr size_t arity = sizeof...( Args );
		using type = std::function<ReturnType( Args... )>;
		using arg_types = std::tuple<Args...>;
		using result_type = ReturnType;
	};

	// for function pointers
	template<typename ReturnType, typename... Args>
	struct function_traits<ReturnType ( * )( Args... )> {
		static constexpr size_t arity = sizeof...( Args );
		using type = std::function<ReturnType( Args... )>;
		using arg_types = std::tuple<Args...>;
		using result_type = ReturnType;
	};
	template<typename F>
	using function_traits_t = typename function_traits<F>::type;

	template<typename L>
	constexpr function_traits_t<L> make_function( L l ) noexcept {
		return static_cast<function_traits_t<L>>( l );
	}

	// handles bind & multiple function call operator()'s
	template<typename ReturnType, typename... Args, class T>
	auto make_function( T &&t ) -> std::function<decltype( ReturnType( t( std::declval<Args>( )... ) ) )( Args... )> {
		return {std::forward<T>( t )};
	}

	// handles explicit overloads
	template<typename ReturnType, typename... Args>
	std::function<ReturnType( Args... )> make_function( ReturnType ( *p )( Args... ) ) {
		return {p};
	}

	// handles explicit overloads
	template<typename ReturnType, typename... Args, typename ClassType>
	std::function<ReturnType( Args... )> make_function( ReturnType ( ClassType::*p )( Args... ) ) {
		return {p};
	}

	//*****************
	// Strip const/volatile/reference from types
	template<typename ReturnType, typename... Args, class T>
	auto make_root_function( T &&t ) -> std::function<decltype(
	  daw::triats::root_type_t<ReturnType>( t( std::declval<daw::traits::root_type_t<Args>>( )... ) ) )( Args... )> {
		return {std::forward<T>( t )};
	}

	// handles explicit overloads
	template<typename ReturnType, typename... Args>
	std::function<daw::traits::root_type_t<ReturnType>( daw::traits::root_type_t<Args>... )>
	make_function( ReturnType ( *p )( Args... ) ) {
		return {p};
	}

	// handles explicit overloads
	template<typename ReturnType, typename... Args, typename ClassType>
	std::function<daw::traits::root_type_t<ReturnType>( daw::traits::root_type_t<Args>... )>
	make_function( ReturnType ( ClassType::*p )( Args... ) ) {
		return {p};
	}
	//*****************

	// handles explicit overloads
	template<typename ReturnType, typename... Args, typename ClassType>
	std::function<ReturnType( Args... )> make_std_function( ReturnType ( ClassType::*p )( Args... ) ) {
		return {p};
	}

	template<typename T>
	T copy( T const &value ) {
		return value;
	}

	template<typename T>
	std::vector<T> copy_vector( std::vector<T> const &container, size_t num_items ) {
		daw::exception::daw_throw_on_false( num_items <= container.size( ),
		                                    "Cannot copy more items than are in container" );
		std::vector<T> result;
		result.reserve( num_items );

		std::copy( std::begin( container ), std::next( std::begin( container ), static_cast<intmax_t>( num_items ) ),
		           std::back_inserter( result ) );

		return result;
	}

	template<class T, class U>
	constexpr T round_to_nearest( T const value, U const rnd_by ) noexcept {
		static_assert( is_arithmetic_v<T>, "First template parameter must be an arithmetic type" );
		static_assert( is_floating_point_v<U>, "Second template parameter must be a floating point type" );
		auto const rnd = std::round( static_cast<U>( value ) / rnd_by );
		return static_cast<T>( rnd * rnd_by );
	}

	template<class T, class U>
	constexpr T floor_by( T const value, U const rnd_by ) noexcept {
		static_assert( is_arithmetic_v<T>, "First template parameter must be an arithmetic type" );
		static_assert( is_floating_point_v<U>, "Second template parameter must be a floating point type" );
		auto const rnd = std::floor( static_cast<U>( value ) / rnd_by );
		return static_cast<T>( rnd * rnd_by );
	}

	template<class T, class U>
	constexpr T ceil_by( T const value, U const rnd_by ) noexcept {
		static_assert( is_arithmetic_v<T>, "First template parameter must be an arithmetic type" );
		static_assert( is_floating_point_v<U>, "Second template parameter must be a floating point type" );
		auto const rnd = std::ceil( static_cast<U>( value ) / rnd_by );
		return static_cast<T>( rnd * rnd_by );
	}

	constexpr bool is_space( char chr ) noexcept {
		return 32 == chr;
	}

	template<typename Iterator1, typename Iterator2, typename Pred>
	std::vector<Iterator1> find_all_where( Iterator1 first, Iterator2 const last, Pred predicate ) {
		std::vector<Iterator1> results;
		for( ; first != last; ++first ) {
			if( predicate( *first ) ) {
				results.push_back( first );
			}
		}
		return results;
	}

	template<typename T, typename Pred>
	decltype( auto ) find_all_where( T const &values, Pred predicate ) {
		return find_all_where( std::cbegin( values ), std::cend( values ), predicate );
	}

	constexpr char AsciiUpper( char chr ) noexcept {
		return chr & ~static_cast<char>( 32 );
	}

	constexpr char AsciiLower( char chr ) noexcept {
		return chr | static_cast<char>( 32 );
	}

	template<typename CharType, typename Traits, typename Allocator>
	auto AsciiUpper( std::basic_string<CharType, Traits, Allocator> str ) noexcept {
		for( auto &chr : str ) {
			chr = AsciiUpper( chr );
		}
		return str;
	}

	template<typename CharType, typename Traits, typename Allocator>
	auto AsciiLower( std::basic_string<CharType, Traits, Allocator> str ) noexcept {
		for( auto &chr : str ) {
			chr = AsciiLower( chr );
		}
		return str;
	}

	template<typename Iterator>
	constexpr bool equal_nc( Iterator first, Iterator last, daw::string_view upper_value ) noexcept {
		if( static_cast<size_t>( std::distance( first, last ) ) != upper_value.size( ) ) {
			return false;
		}
		for( auto c : upper_value ) {
			if( c != daw::AsciiUpper( *first ) ) {
				return false;
			}
			++first;
		}
		return true;
	}

	namespace details {
		template<typename T>
		struct RunIfValid {
			::std::weak_ptr<T> m_link;
			RunIfValid( std::weak_ptr<T> w_ptr ) : m_link( w_ptr ) {}

			template<typename Function>
			bool operator( )( Function func ) {
				if( auto s_ptr = m_link.lock( ) ) {
					func( s_ptr );
					return true;
				} else {
					return false;
				}
			}
		};
	} // namespace details

	template<typename T>
	auto RunIfValid(::std::weak_ptr<T> w_ptr ) {
		return details::RunIfValid<T>( w_ptr );
	}

	template<typename T>
	auto copy_ptr_value( T const *const original ) {
		using result_t = daw::traits::root_type_t<T>;
		return new result_t{*original};
	}

	// Acts like a reference, but has a strong no-null guarantee
	// Non-owning
	template<typename T>
	class not_null {
		T *m_ptr;

	public:
		not_null( ) = delete;
		~not_null( ) = default;
		constexpr not_null( not_null const & ) noexcept = default;
		constexpr not_null( not_null && ) noexcept = default;
		constexpr not_null &operator=( not_null const & ) noexcept = default;
		constexpr not_null &operator=( not_null && ) noexcept = default;

		constexpr not_null( T *ptr ) : m_ptr{ptr} {
			daw::exception::daw_throw_on_null( ptr, "ptr cannot be null" );
		}

		explicit constexpr operator bool( ) const noexcept {
			return true;
		}

		constexpr T *operator->( ) noexcept {
			return m_ptr;
		}

		constexpr T const *operator->( ) const noexcept {
			return m_ptr;
		}

		constexpr T *get( ) noexcept {
			return m_ptr;
		}

		constexpr T const *get( ) const noexcept {
			return m_ptr;
		}

		friend constexpr bool operator==( not_null const &lhs, not_null const &rhs ) noexcept {
			return lhs.m_ptr == rhs.m_ptr;
		}

		friend constexpr bool operator!=( not_null const &lhs, not_null const &rhs ) noexcept {
			return lhs.m_ptr != rhs.m_ptr;
		}
	}; // not_null

	template<typename Arg, typename... Args>
	auto make_initializer_list( Arg &&arg, Args &&... args ) {
		return std::initializer_list<Arg>{std::forward<Arg>( arg ), std::forward<Args>( args )...};
	}

	template<typename Container, typename... Args>
	decltype( auto ) append( Container &container, Args &&... args ) {
		return container.insert( container.end( ), make_initializer_list( std::forward<Args>( args )... ) );
	}

	template<typename Container, typename Item>
	constexpr bool contains( Container const &container, Item const &item ) noexcept {
		return std::find( std::cbegin( container ), std::cend( container ), item ) != std::cend( container );
	}

	template<typename Container, typename Item>
	constexpr decltype( auto ) index_of( Container const &container, Item const &item ) noexcept {
		auto const pos = std::find( std::begin( container ), std::end( container ), item );
		return std::distance( std::begin( container ), pos );
	}

	constexpr auto or_all( ) noexcept {
		return false;
	}

	template<typename Value>
	constexpr Value or_all( Value value ) noexcept {
		return value;
	}

	template<typename Value, typename... T>
	constexpr auto or_all( Value value, T... values ) noexcept {
		return value | or_all( values... );
	}

	template<typename Value>
	constexpr size_t bitcount( Value value ) noexcept {
		size_t result = 0;
		while( value ) {
			result += static_cast<size_t>( value & ~static_cast<Value>( 0b1 ) );
			value >>= 1;
		}
		return result;
	}

	template<typename Value, typename... T>
	constexpr size_t bitcount( Value value, T... values ) noexcept {
		return bitcount( value ) + bitcount( values... );
	};

	template<typename IntegerDest, typename IntegerSource>
	constexpr bool can_fit( IntegerSource const value ) noexcept {
		static_assert( is_integral_v<IntegerDest>, "Must supply an integral type" );
		static_assert( is_integral_v<IntegerSource>, "Must supply an integral type" );
		if( value >= 0 ) {
			return value <= std::numeric_limits<IntegerDest>::max( );
		} else if( std::numeric_limits<IntegerDest>::is_signed ) {
			return value >= std::numeric_limits<IntegerDest>::min( );
		} else {
			return false;
		}
	}

	constexpr void breakpoint( ) noexcept {
		;
	}

	template<typename T, std::enable_if_t<!is_floating_point_v<T>, std::nullptr_t> = nullptr>
	constexpr bool nearly_equal( T const &a, T const &b ) noexcept {
		return a == b;
	}

	template<typename T, std::enable_if_t<is_floating_point_v<T>, std::nullptr_t> = nullptr>
	constexpr bool nearly_equal( T const &a, T const &b ) noexcept {
		// Code from http://floating-point-gui.de/errors/comparison/
		auto absA = std::abs( a );
		auto absB = std::abs( b );
		auto diff = std::abs( a - b );

		if( a == b ) { // shortcut, handles infinities
			return true;
		}
		if( a == 0 || b == 0 || diff < std::numeric_limits<T>::min_exponent ) {
			// a or b is zero or both are extremely close to it
			// 			// relative error is less meaningful here
			return diff < ( std::numeric_limits<T>::epsilon( ) * std::numeric_limits<T>::min_exponent );
		}
		// use relative error
		return diff / std::min( ( absA + absB ), std::numeric_limits<T>::max( ) ) < std::numeric_limits<T>::epsilon( );
	}

	template<typename Iterator>
	constexpr std::reverse_iterator<Iterator> make_reverse_iterator( Iterator i ) {
		return std::reverse_iterator<Iterator>( std::move( i ) );
	}

	template<typename T, typename Iterator>
	void fill_random( T const min_value, T const max_value, Iterator first, Iterator last ) {
		std::random_device rnd_device;
		// Specify the engine and distribution.
		std::mt19937 mersenne_engine{rnd_device( )};
		std::uniform_int_distribution<T> dist{min_value, max_value};

		std::generate( first, last, [&]( ) { return dist( mersenne_engine ); } );
	}

	template<typename T, typename RndType = T>
	std::vector<T> generate_random_data( size_t count, RndType min_value = std::numeric_limits<T>::min( ),
	                                     RndType max_value = std::numeric_limits<T>::max( ) ) {
		std::vector<T> result;
		result.resize( count );
		fill_random( min_value, max_value, result.begin( ), result.end( ) );
		return result;
	}

	template<typename Iterator>
	constexpr std::move_iterator<Iterator> make_move_iterator( Iterator i ) {
		return std::move_iterator<Iterator>( i );
	}

	template<typename Iterator1, typename Iterator2, typename OutputIterator>
	constexpr OutputIterator cxpr_copy( Iterator1 first_in, Iterator2 const last_in, OutputIterator first_out ) {
		while( first_in != last_in ) {
			*first_out++ = *first_in++;
		}
		return first_out;
	}

	template<typename Iterator1, typename Iterator2, typename OutputIterator>
	constexpr OutputIterator cxpr_move( Iterator1 first_in, Iterator2 const last_in, OutputIterator first_out ) {
		while( first_in != last_in ) {
			*first_out++ = std::move( *first_in++ );
		}
		return first_out;
	}

} // namespace daw

template<typename... Ts>
constexpr void Unused( Ts &&... ) noexcept {}
