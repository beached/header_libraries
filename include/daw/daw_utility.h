// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "cpp_17.h"
#include "daw_algorithm.h"
#include "daw_construct_a.h"
#include "daw_exception.h"
#include "daw_move.h"
#include "daw_traits.h"

#include <ciso646>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <functional>
#include <initializer_list>
#include <limits>
#include <memory>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <vector>

template<typename... Ts>
inline constexpr void Unused( Ts &&... ) noexcept {}

namespace daw {
	namespace utility_details {
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
	} // namespace utility_details

	template<typename ResultType, typename... ArgTypes>
	using function_pointer_t =
	  typename utility_details::make_function_pointer_impl<ResultType,
	                                                       ArgTypes...>::type;

	template<typename ResultType, typename ClassType, typename... ArgTypes>
	using pointer_to_member_function_t =
	  typename utility_details::make_pointer_to_member_function_impl<
	    ResultType, ClassType, ArgTypes...>::type;

	template<typename ResultType, typename ClassType, typename... ArgTypes>
	using pointer_to_volatile_member_function_t =
	  typename utility_details::make_pointer_to_volatile_member_function_impl<
	    ResultType, ClassType, ArgTypes...>::type;

	template<typename ResultType, typename ClassType, typename... ArgTypes>
	using pointer_to_const_member_function_t =
	  typename utility_details::make_pointer_to_const_member_function_impl<
	    ResultType, ClassType, ArgTypes...>::type;

	template<typename ResultType, typename ClassType, typename... ArgTypes>
	using pointer_to_const_volatile_member_function_t = typename utility_details::
	  make_pointer_to_const_volatile_member_function_impl<ResultType, ClassType,
	                                                      ArgTypes...>::type;

	namespace utility_details {
		template<typename T>
		class EqualToImpl {
			T m_value;

		public:
			inline constexpr EqualToImpl( T value ) noexcept(
			  std::is_nothrow_copy_constructible_v<T> )
			  : m_value( DAW_MOVE( value ) ) {}

			[[nodiscard]] inline constexpr bool
			operator( )( T const &value ) noexcept {
				return m_value == value;
			}
		}; // class EqualToImpl
	}    // namespace utility_details
	template<typename T>
	inline constexpr utility_details::EqualToImpl<T> equal_to( T value ) {
		return utility_details::EqualToImpl<T>( DAW_MOVE( value ) );
	}

	template<typename T>
	class equal_to_last {
		T *m_value = nullptr;

	public:
		inline constexpr equal_to_last( ) noexcept = default;

		inline bool operator( )( T const &value ) noexcept {
			bool result = false;
			if( m_value ) {
				result = *m_value == value;
			}
			m_value = const_cast<T *>( &value );
			return result;
		}
	}; // class equal_to_last

	namespace utility_details {
		template<typename Function>
		class NotImpl {
			Function m_function;

		public:
			inline constexpr NotImpl( Function func ) noexcept(
			  std::is_nothrow_move_constructible_v<Function> )
			  : m_function( DAW_MOVE( func ) ) {}

			template<typename... Args>
			[[nodiscard]] inline constexpr bool operator( )( Args &&...args ) {
				return !m_function( DAW_FWD2( Args, args )... );
			}
		}; // class NotImpl
	}    // namespace utility_details

	template<typename Function>
	[[nodiscard]] inline constexpr utility_details::NotImpl<Function>
	Not( Function func ) {
		return utility_details::NotImpl<Function>( func );
	}

	// For generic types that are functors, delegate to its 'operator()'

	template<typename T>
	struct function_traits : public function_traits<decltype( &T{ }( ) )> {};

	// for pointers to member function(const version)
	template<typename ClassType, typename ReturnType, typename... Args>
	struct function_traits<ReturnType ( ClassType::* )( Args... ) const> {
		static constexpr std::size_t arity = sizeof...( Args );
		using type = std::function<ReturnType( Args... )>;
		using arg_types = std::tuple<Args...>;
		using result_type = ReturnType;
	};

	// for pointers to member function
	template<typename ClassType, typename ReturnType, typename... Args>
	struct function_traits<ReturnType ( ClassType::* )( Args... )> {
		static constexpr std::size_t arity = sizeof...( Args );
		using type = std::function<ReturnType( Args... )>;
		using arg_types = std::tuple<Args...>;
		using result_type = ReturnType;
	};

	// for function pointers
	template<typename ReturnType, typename... Args>
	struct function_traits<ReturnType ( * )( Args... )> {
		static constexpr std::size_t arity = sizeof...( Args );
		using type = std::function<ReturnType( Args... )>;
		using root_type = std::function<daw::traits::root_type_t<ReturnType>(
		  daw::traits::root_type_t<Args>... )>;
		using arg_types = std::tuple<Args...>;
		using result_type = ReturnType;
	};
	template<typename F>
	using function_traits_t = typename function_traits<F>::type;

	template<typename L>
	[[nodiscard]] constexpr function_traits_t<L> make_function( L l ) noexcept {
		return static_cast<function_traits_t<L>>( l );
	}

	template<typename L>
	[[nodiscard]] constexpr auto make_root_function( L l ) noexcept {
		return static_cast<typename function_traits<L>::root_type>( l );
	}

	// handles bind & multiple function call operator()'s
	template<typename ReturnType, typename... Args, class T>
	[[nodiscard]] auto make_function( T &&t ) -> std::function<
	  decltype( ReturnType( t( std::declval<Args>( )... ) ) )( Args... )> {
		return { DAW_FWD2( T, t ) };
	}

	// handles explicit overloads
	template<typename ReturnType, typename... Args>
	[[nodiscard]] std::function<ReturnType( Args... )>
	make_function( ReturnType ( *p )( Args... ) ) {
		return { p };
	}

	// handles explicit overloads
	template<typename ReturnType, typename... Args, typename ClassType>
	[[nodiscard]] std::function<ReturnType( Args... )>
	make_function( ReturnType ( ClassType::*p )( Args... ) ) {
		return { p };
	}

	//*****************
	// Strip const/volatile/reference from types
	template<typename ReturnType, typename... Args, class T>
	[[nodiscard]] auto make_root_function( T &&t )
	  -> std::function<decltype( daw::traits::root_type_t<ReturnType>(
	    t( std::declval<daw::traits::root_type_t<Args>>( )... ) ) )( Args... )> {
		return { DAW_FWD2( T, t ) };
	}

	// handles explicit overloads
	template<typename ReturnType, typename... Args>
	[[nodiscard]] std::function<
	  daw::traits::root_type_t<ReturnType>( daw::traits::root_type_t<Args>... )>
	make_function( ReturnType ( *p )( Args... ) ) {
		return { p };
	}

	// handles explicit overloads
	template<typename ReturnType, typename... Args, typename ClassType>
	[[nodiscard]] std::function<
	  daw::traits::root_type_t<ReturnType>( daw::traits::root_type_t<Args>... )>
	make_function( ReturnType ( ClassType::*p )( Args... ) ) {
		return { p };
	}
	//*****************

	// handles explicit overloads
	template<typename ReturnType, typename... Args, typename ClassType>
	[[nodiscard]] std::function<ReturnType( Args... )>
	make_std_function( ReturnType ( ClassType::*p )( Args... ) ) {
		return { p };
	}

	template<typename T>
	[[nodiscard]] constexpr T
	copy( T &&value ) noexcept( std::is_nothrow_copy_constructible<T>::value ) {
		return value;
	}

	template<template<class, class> class Vector, typename T, typename A>
	[[nodiscard]] constexpr Vector<T, A>
	copy_vector( Vector<T, A> const &container, std::size_t num_items ) {
		daw::exception::daw_throw_on_false(
		  num_items <= container.size( ),
		  "Cannot copy more items than are in container" );
		return Vector<T, A>(
		  container.begin( ),
		  std::next( container.begin( ),
		             static_cast<std::ptrdiff_t>( num_items ) ) );
	}

	[[nodiscard]] inline constexpr bool is_space( char chr ) noexcept {
		return ' ' == chr;
	}

	template<typename Iterator1, typename Iterator2, typename Pred>
	[[nodiscard]] std::vector<Iterator1>
	find_all_where( Iterator1 first, Iterator2 const last, Pred predicate ) {
		std::vector<Iterator1> results{ };
		std::copy_if( first, last, std::back_inserter( results ), predicate );
		return results;
	}

	template<typename T, typename Pred>
	[[nodiscard]] auto find_all_where( T const &values, Pred predicate ) {
		return find_all_where( std::cbegin( values ), std::cend( values ),
		                       predicate );
	}

	[[nodiscard]] constexpr char AsciiUpper( char chr ) noexcept {
		if( 'a' <= chr and chr <= 'z' ) {
			return static_cast<char>(
			  static_cast<unsigned>( static_cast<unsigned char>( chr ) ) & ~32U );
		}
		return chr;
	}

	[[nodiscard]] constexpr char AsciiLower( char chr ) noexcept {
		if( 'A' <= chr and chr <= 'Z' ) {
			return static_cast<char>( static_cast<unsigned char>(
			  static_cast<unsigned>( static_cast<unsigned char>( chr ) ) | 32U ) );
		}
		return chr;
	}

	template<typename CharType, typename Traits, typename Allocator>
	[[nodiscard]] constexpr auto
	AsciiUpper( std::basic_string<CharType, Traits, Allocator> str ) noexcept {
		daw::algorithm::map(
		  str.cbegin( ), str.cend( ), str.begin( ),
		  []( CharType c ) noexcept { return AsciiUpper( c ); } );
		return DAW_MOVE( str );
	}

	template<typename CharType, typename Traits, typename Allocator>
	[[nodiscard]] constexpr auto
	AsciiLower( std::basic_string<CharType, Traits, Allocator> str ) noexcept {
		daw::algorithm::map(
		  str.cbegin( ), str.cend( ), str.begin( ),
		  []( CharType c ) noexcept { return AsciiLower( c ); } );
		return DAW_MOVE( str );
	}

	namespace utility_details {
		template<typename T>
		struct RunIfValid {
			std::weak_ptr<T> m_link;
			RunIfValid( std::weak_ptr<T> w_ptr )
			  : m_link( w_ptr ) {}

			template<typename Function>
			[[nodiscard]] bool operator( )( Function func ) {
				if( auto s_ptr = m_link.lock( ) ) {
					func( s_ptr );
					return true;
				} else {
					return false;
				}
			}
		};
	} // namespace utility_details

	template<typename T>
	[[nodiscard]] auto RunIfValid( std::weak_ptr<T> w_ptr ) {
		return utility_details::RunIfValid<T>( w_ptr );
	}

	template<typename T>
	[[nodiscard]] auto copy_ptr_value( T const *const original ) {
		using result_t = daw::traits::root_type_t<T>;
		return new result_t( *original );
	}

	template<typename T>
	using non_owning_ptr = T;

	template<typename Arg, typename... Args>
	[[nodiscard]] auto make_initializer_list( Arg &&arg, Args &&...args ) {
		return std::initializer_list<Arg>{ DAW_FWD2( Arg, arg ),
		                                   DAW_FWD2( Args, args )... };
	}

	template<typename Container, typename... Args>
	[[nodiscard]] constexpr auto append( Container &container, Args &&...args ) {
		return container.insert( container.end( ), { DAW_FWD( args )... } );
	}

	template<typename Container, typename Item>
	[[nodiscard]] constexpr bool contains( Container const &container,
	                                       Item const &item ) noexcept {
		return daw::algorithm::find( std::cbegin( container ),
		                             std::cend( container ),
		                             item ) != std::cend( container );
	}

	template<typename Container, typename Item>
	[[nodiscard]] constexpr auto index_of( Container const &container,
	                                       Item const &item ) noexcept {
		auto const pos = daw::algorithm::find( std::begin( container ),
		                                       std::end( container ), item );
		return std::distance( std::begin( container ), pos );
	}

	[[nodiscard]] constexpr auto or_all( ) noexcept {
		return false;
	}

	template<typename Value>
	[[nodiscard]] constexpr Value or_all( Value value ) noexcept {
		return value;
	}

	template<typename Value, typename... T>
	[[nodiscard]] constexpr decltype( auto ) or_all( Value value,
	                                                 T... values ) noexcept {
		return value | ( values | ... );
	}

	template<typename Value>
	[[nodiscard]] constexpr std::size_t bitcount( Value value ) noexcept {
		size_t result = 0;
		while( value ) {
			result += static_cast<size_t>( value & ~static_cast<Value>( 0b1 ) );
			value >>= 1;
		}
		return result;
	}

	template<typename Value, typename... T>
	[[nodiscard]] constexpr std::size_t bitcount( Value value,
	                                              T... values ) noexcept {
		return bitcount( value ) + bitcount( values... );
	}

	template<typename IntegerDest, typename IntegerSource>
	[[nodiscard]] constexpr bool can_fit( IntegerSource const value ) noexcept {
		static_assert( std::is_integral_v<IntegerDest>,
		               "Must supply an integral type" );
		static_assert( std::is_integral_v<IntegerSource>,
		               "Must supply an integral type" );
		if( value >= 0 ) {
			return value <= ( std::numeric_limits<IntegerDest>::max )( );
		} else if( std::numeric_limits<IntegerDest>::is_signed ) {
			return value >= ( std::numeric_limits<IntegerDest>::min )( );
		} else {
			return false;
		}
	}

	constexpr void breakpoint( ) noexcept {
		int const x = 0;
		Unused( x );
	}

	template<typename Iterator1, typename Iterator2, typename OutputIterator>
	constexpr OutputIterator cxpr_copy( Iterator1 first_in,
	                                    Iterator2 const last_in,
	                                    OutputIterator first_out ) {
		while( first_in != last_in ) {
			*first_out = *first_in;
			++first_out;
			++first_in;
		}
		return first_out;
	}

	template<typename Iterator1, typename Iterator2, typename OutputIterator>
	constexpr OutputIterator cxpr_move( Iterator1 first_in,
	                                    Iterator2 const last_in,
	                                    OutputIterator first_out ) {
		while( first_in != last_in ) {
			*first_out = DAW_MOVE( *first_in );
			++first_out;
			++first_in;
		}
		return first_out;
	}

	namespace utility_details {
		[[nodiscard]] constexpr char get_nibble( uint8_t c ) noexcept {
			c &= 0x0F;
			if( c < 10 ) {
				return static_cast<char>( '0' + c );
			}
			return static_cast<char>( 'A' + ( c - 10 ) );
		}

		[[nodiscard]] constexpr char get_lc_nibble( uint8_t c ) noexcept {
			c &= 0x0F;
			if( c < 10 ) {
				return static_cast<char>( '0' + c );
			}
			return static_cast<char>( 'a' + ( c - 10 ) );
		}
	} // namespace utility_details

	template<typename OutputIterator>
	[[nodiscard]] constexpr OutputIterator hex( char c,
	                                            OutputIterator it_out ) noexcept {
		uint8_t n = static_cast<uint8_t>( c );
		*it_out = utility_details::get_nibble( n >> 4 );
		++it_out;
		*it_out = utility_details::get_nibble( n & 0x0F );
		++it_out;
		return it_out;
	}

	template<typename OutputIterator>
	[[nodiscard]] constexpr OutputIterator
	hex_lc( char c, OutputIterator it_out ) noexcept {
		auto n = static_cast<std::uint8_t>( c );
		*it_out = utility_details::get_lc_nibble( n >> 4 );
		++it_out;
		*it_out = utility_details::get_lc_nibble( n & 0x0F );
		++it_out;
		return it_out;
	}

	template<typename T, typename OutputIterator,
	         std::enable_if_t<std::is_integral_v<T>, std::nullptr_t> = nullptr>
	[[nodiscard]] constexpr OutputIterator hex( T const &val,
	                                            OutputIterator it_out ) noexcept {
		for( std::size_t n = sizeof( T ); n > 0; --n ) {
			it_out = hex( static_cast<char>( val >> ( 8 * ( n - 1 ) ) ), it_out );
		}
		return it_out;
	}

	template<
	  typename T, typename OutputIterator,
	  std::enable_if_t<not std::is_integral_v<T>, std::nullptr_t> = nullptr>
	[[nodiscard]] OutputIterator hex( T const &val,
	                                  OutputIterator it_out ) noexcept {
		auto chr_ptr = reinterpret_cast<char const *>( &val );
		for( std::size_t n = 0; n < sizeof( T ); ++n ) {
			it_out = hex( *chr_ptr, it_out );
			++it_out;
			++chr_ptr;
		}
		return it_out;
	}

	template<typename ForwardIterator1, typename ForwardIterator2,
	         typename OutputIterator,
	         std::enable_if_t<std::is_integral_v<typename std::iterator_traits<
	                            ForwardIterator1>::value_type>,
	                          std::nullptr_t> = nullptr>
	[[nodiscard]] constexpr OutputIterator
	hex( ForwardIterator1 first_in, ForwardIterator2 const last_in,
	     OutputIterator first_out ) noexcept {
		for( ; first_in != last_in; ++first_in ) {
			auto const val = *first_in;
			first_out = hex( val, first_out );
		}
		return first_out;
	}

	template<typename OutputIterator>
	[[nodiscard]] constexpr OutputIterator
	hex( char const *str, std::size_t len, OutputIterator first_out ) noexcept {
		for( std::size_t n = 0; n < len; ++n ) {
			first_out = hex( str[n], first_out );
		}
		return first_out;
	}

	template<size_t N, typename OutputIterator>
	[[nodiscard]] constexpr OutputIterator
	hex( char const ( &str )[N], OutputIterator first_out ) noexcept {
		return hex( str, N - 1, first_out );
	}

	template<
	  typename ForwardIterator1, typename ForwardIterator2,
	  typename OutputIterator,
	  std::enable_if_t<not std::is_integral_v<typename std::iterator_traits<
	                     ForwardIterator1>::value_type>,
	                   std::nullptr_t> = nullptr>
	[[nodiscard]] OutputIterator hex( ForwardIterator1 first_in,
	                                  ForwardIterator2 const last_in,
	                                  OutputIterator first_out ) noexcept {
		for( ; first_in != last_in; ++first_in ) {
			first_out = hex( *first_out, first_out );
		}
		return first_out;
	}

	template<typename Iterator>
	[[nodiscard]] constexpr auto make_range( Iterator first,
	                                         Iterator last ) noexcept {
		struct range_t {
			using iterator = Iterator;
			using difference_type =
			  typename std::iterator_traits<iterator>::difference_type;

			iterator m_first;
			iterator m_last;

			constexpr iterator begin( ) {
				return m_first;
			}

			constexpr iterator begin( ) const {
				return m_first;
			}

			constexpr iterator end( ) {
				return m_last;
			}

			constexpr iterator end( ) const {
				return m_last;
			}

			constexpr void remove_prefix( std::size_t count ) {
				std::advance( m_first, static_cast<difference_type>( count ) );
			}

			constexpr void remove_prefix( ) {
				remove_prefix( 1 );
			}

			constexpr void remove_suffix( std::size_t count ) {
				std::advance( m_last, -static_cast<difference_type>( count ) );
			}

			constexpr void remove_suffix( ) {
				remove_suffix( 1 );
			}
		};
		return range_t{ first, last };
	}

	template<typename... Ts>
	struct pack_index_of;

	template<typename T, typename... Ts>
	struct pack_index_of<T, Ts...> : std::integral_constant<size_t, 0> {};

	template<typename T, typename U, typename... Ts>
	struct pack_index_of<T, U, Ts...>
	  : std::integral_constant<
	      std::size_t,
	      ( std::is_same_v<T, U> ? 0 : 1 + pack_index_of<T, Ts...>::value )> {};

	template<size_t N, typename... Ts>
	using pack_type_at = traits::nth_type<N, Ts...>;

	template<typename T, typename... Ts>
	constexpr std::size_t pack_index_of_v = pack_index_of<T, Ts...>::value;

	template<typename... Args>
	struct tag_t {};

	template<typename... Args>
	constexpr tag_t<Args...> tag = tag_t<Args...>{ };

	namespace utility_details {
		template<typename T, bool>
		struct AllocDetail {};

		template<typename T>
		struct AllocDetail<T, true> {
			using allocator_type = typename T::allocator_type;
		};

		template<typename T>
		using AllocTest = typename T::allocator_type;
	} // namespace utility_details

	namespace utility_details {
		template<typename... Args>
		constexpr void is_tuple_test( std::tuple<Args...> const & ) noexcept;

		template<typename T>
		using is_tuple_detect = decltype( is_tuple_test( std::declval<T>( ) ) );

		template<typename T>
		constexpr bool is_tuple_v = daw::is_detected_v<is_tuple_detect, T>;
	} // namespace utility_details

	template<typename Destination, typename... Args>
	[[nodiscard]] constexpr decltype( auto )
	construct_from( std::tuple<Args...> &&args ) noexcept(
	  noexcept( daw::apply( construct_a<Destination>, DAW_MOVE( args ) ) ) ) {

		return daw::apply( construct_a<Destination>, DAW_MOVE( args ) );
	}

	template<typename Destination, typename... Args>
	[[nodiscard]] constexpr decltype( auto )
	construct_from( std::tuple<Args...> const &args ) noexcept(
	  noexcept( daw::apply( construct_a<Destination>, args ) ) ) {

		return daw::apply( construct_a<Destination>, args );
	}

	/*
	 The do_not_optimize(...) function can be used to prevent a value or
	 expression from being optimized away by the compiler. This function is
	 intended to add little to no overhead.
	 See: https://youtu.be/nXaxk27zwlk?t=2441
	*/

#if not defined( _MSC_VER ) or defined( __clang__ )
	template<typename T>
	inline void force_evaluation( T const &value ) {
		asm volatile( "" : : "r,m"( value ) : "memory" );
	}

	template<typename T>
	inline void force_evaluation( T &value ) {
#if defined( __clang__ )
		asm volatile( "" : "+r,m"( value ) : : "memory" );
#else
		asm volatile( "" : "+m,r"( value ) : : "memory" );
#endif
	}
#else

#pragma optimize( "", off )
	template<class T>
	void force_evalutation( T &&datum ) {
		datum = datum;
	}
#pragma optimize( "", on )

#endif
	template<typename To>
	[[nodiscard]] constexpr uint8_t value_from_chars(
	  unsigned char const *ptr,
	  std::integral_constant<size_t, sizeof( uint8_t )> ) noexcept {
		return *ptr;
	}

	template<typename To>
	[[nodiscard]] constexpr uint16_t value_from_chars(
	  unsigned char const *ptr,
	  std::integral_constant<size_t, sizeof( uint16_t )> ) noexcept {
		auto result = static_cast<uint16_t>( ( *ptr ) << 8U );
		++ptr;
		result |= *ptr;
		return result;
	}

	template<typename To>
	[[nodiscard]] constexpr uint32_t value_from_chars(
	  unsigned char const *ptr,
	  std::integral_constant<size_t, sizeof( uint32_t )> ) noexcept {
		uint32_t result = static_cast<uint32_t>( ( *ptr ) << 24U );
		++ptr;
		result |= static_cast<uint32_t>( ( *ptr ) << 16U );
		++ptr;
		result |= static_cast<uint32_t>( ( *ptr ) << 8U );
		++ptr;
		result |= static_cast<uint32_t>( *ptr );
		return result;
	}

	template<typename To>
	[[nodiscard]] constexpr uint64_t value_from_chars(
	  unsigned char const *ptr,
	  std::integral_constant<size_t, sizeof( uint64_t )> ) noexcept {
		auto result = static_cast<uint64_t>( static_cast<uint64_t>( *ptr ) << 56U );
		++ptr;
		result |= static_cast<uint64_t>( static_cast<uint64_t>( *ptr ) << 48U );
		++ptr;
		result |= static_cast<uint64_t>( static_cast<uint64_t>( *ptr ) << 40U );
		++ptr;
		result |= static_cast<uint64_t>( static_cast<uint64_t>( *ptr ) << 32U );
		++ptr;
		result |= static_cast<uint64_t>( static_cast<uint64_t>( *ptr ) << 24U );
		++ptr;
		result |= static_cast<uint64_t>( static_cast<uint64_t>( *ptr ) << 16U );
		++ptr;
		result |= static_cast<uint64_t>( static_cast<uint64_t>( *ptr ) << 8U );
		++ptr;
		result |= static_cast<uint64_t>( *ptr );
		++ptr;
		return result;
	}

	template<typename From>
	[[nodiscard]] auto as_char_array( From &&from ) noexcept {
		static_assert( std::is_trivially_copyable_v<remove_cvref_t<From>>,
		               "From type must be trivially copiable" );
		auto result = std::array<unsigned char, sizeof( From )>{ 0 };
		memcpy( result.data( ), &from, result.size( ) );
		return result;
	}

	template<typename To, typename From>
	[[nodiscard]] auto to_array_of( From &&from ) noexcept {
		constexpr std::size_t const num_values = sizeof( From ) / sizeof( To );
		static_assert( std::is_integral_v<std::decay<To>>,
		               "To must be an integer like type" );
		static_assert( sizeof( From ) == num_values * sizeof( To ),
		               "Must have integral number of To's in From" );

		auto const as_chars = as_char_array( from );
		auto result = std::array<To, num_values>{ 0 };

		for( std::size_t entry = 0; entry < num_values; ++entry ) {
			result[entry] =
			  value_from_chars( std::next( as_chars.data( ), entry * sizeof( To ) ),
			                    std::integral_constant<size_t, sizeof( To )>{ } );
		}
		return result;
	}

	template<typename T>
	constexpr std::size_t const
	  bsizeof = static_cast<size_t>( sizeof( remove_cvref_t<T> ) * 8U );

	/// Checks if value is in the range [lower, upper)
	template<typename Value, typename LowerBound, typename UpperBound>
	[[nodiscard]] constexpr bool
	in_range( Value &&value, LowerBound &&lower,
	          UpperBound &&upper ) noexcept( noexcept( lower <= value &&
	                                                   value < upper ) ) {
		return DAW_FWD2( LowerBound, lower ) <= value &&
		       value < DAW_FWD2( UpperBound, upper );
	}

	namespace utility_details {
		template<typename T>
		class value_is_utility_details;
	} // namespace utility_details

	template<typename T>
	[[nodiscard]] constexpr auto value_is( T &&value )
	  -> daw::utility_details::value_is_utility_details<
	    std::remove_reference_t<decltype( value )>> {

		return { DAW_FWD2( T, value ) };
	}

	namespace utility_details {
		template<typename T>
		class value_is_utility_details {
			T *m_value;

			constexpr value_is_utility_details( T const &v )
			  : m_value( &v ) {}
			constexpr value_is_utility_details( T &v )
			  : m_value( &v ) {}
			constexpr value_is_utility_details( T &&v )
			  : m_value( &v ) {}

			template<typename U>
			friend /*TODO Apple Clang is mean today [[nodiscard]]*/
			  constexpr auto ::daw::value_is( U &&v )
			    -> daw::utility_details::value_is_utility_details<
			      std::remove_reference_t<decltype( v )>>;

		public:
			value_is_utility_details( value_is_utility_details const & ) = delete;
			value_is_utility_details( value_is_utility_details && ) = delete;
			value_is_utility_details &
			operator=( value_is_utility_details const & ) = delete;
			value_is_utility_details &
			operator=( value_is_utility_details && ) = delete;
			~value_is_utility_details( ) = default;

			template<typename... Args>
			[[nodiscard]] constexpr bool one_of( Args &&...args ) const && {
				return ( ( *m_value == args ) or ... );
			}

			template<typename... Args>
			[[nodiscard]] constexpr bool none_of( Args &&...args ) const && {
				return ( ( *m_value != args ) and ... );
			}
		};
	} // namespace utility_details

	template<typename Integer, typename T>
	[[nodiscard]] constexpr Integer narrow_cast( T value ) noexcept {
		if constexpr( std::is_signed_v<T> ) {
			if constexpr( std::is_signed_v<Integer> ) {
				if constexpr( sizeof( T ) <= sizeof( Integer ) ) {
					return value;
				} else if( value <= static_cast<T>(
				                      ( std::numeric_limits<Integer>::max )( ) ) ) {
					return static_cast<Integer>( value );
				} else {
					daw::exception::daw_throw<std::out_of_range>( "value" );
				}
			} else if constexpr( sizeof( T ) <= sizeof( Integer ) ) {
				if( value >= 0 ) {
					return value;
				}
				daw::exception::daw_throw<std::out_of_range>( "value" );
			} else {
				if( value >= 0 and
				    value <=
				      static_cast<T>( ( std::numeric_limits<Integer>::max )( ) ) ) {
					return value;
				}
				daw::exception::daw_throw<std::out_of_range>( "value" );
			}
		} else if constexpr( std::is_signed_v<Integer> ) {
			if constexpr( sizeof( T ) < sizeof( Integer ) ) {
				return static_cast<Integer>( value );
			} else {
				daw::exception::precondition_check<std::out_of_range>(
				  value <= static_cast<T>( ( std::numeric_limits<Integer>::max )( ) ),
				  "value" );

				return static_cast<Integer>( value );
			}
		} else if constexpr( sizeof( T ) <= sizeof( Integer ) ) {
			return static_cast<Integer>( value );
		} else {
			if( value <=
			    static_cast<T>( ( std::numeric_limits<Integer>::max )( ) ) ) {
				return static_cast<Integer>( value );
			}
			daw::exception::daw_throw<std::out_of_range>( "value" );
		}
	}

	template<size_t N, std::size_t pos = 0, typename Arg, typename... Args>
	[[nodiscard]] constexpr decltype( auto ) pack_get( Arg &&arg,
	                                                   Args &&...args ) noexcept {
		if constexpr( pos == N ) {
			return DAW_FWD2( Arg, arg );
		} else {
			return pack_get<N, pos + 1>( DAW_FWD2( Args, args )... );
		}
	}

	namespace utility_details {
		template<size_t pos, typename Function, typename... Args>
		[[nodiscard]] constexpr auto utility_details( std::size_t, Function &&,
		                                              Args &&... )
		  -> std::enable_if_t<( pos >= sizeof...( Args ) )> {}

		template<size_t pos, typename Function, typename... Args>
		[[nodiscard]] constexpr auto
		utility_details( std::size_t N, Function &&func, Args &&...args )
		  -> std::enable_if_t<( pos < sizeof...( Args ) )> {
			if( N == pos ) {
				if constexpr( std::is_invocable_v<Function,
				                                  decltype( pack_get<pos>(
				                                    DAW_FWD2( Args, args )... ) )> ) {
					(void)DAW_FWD2( Function,
					                func )( pack_get<pos>( DAW_FWD2( Args, args )... ) );
				}
			} else {
				utility_details<pos + 1>( N, DAW_FWD2( Function, func ),
				                          DAW_FWD2( Args, args )... );
			}
		}
	} // namespace utility_details

	template<typename Function, typename... Args>
	constexpr void pack_apply( std::size_t N, Function &&func, Args &&...args ) {
		utility_details::utility_details<0>( N, DAW_FWD2( Function, func ),
		                                     DAW_FWD2( Args, args )... );
	}

	template<typename T, bool AllowDownSignCast = false, typename U>
	constexpr decltype( auto ) cast( U &&v ) {
		if constexpr( not AllowDownSignCast and
		              ( std::is_arithmetic_v<U> and std::is_arithmetic_v<T> ) ) {
			static_assert( std::is_signed_v<U> == std::is_signed_v<T>,
			               "Only matching signs and upcasting supported, use "
			               "static_cast if this is intentional" );
			static_assert(
			  sizeof( T ) >= sizeof( U ),
			  "Cannot downcast, use static_cast if this is intentional" );
		}
		return static_cast<T>( DAW_FWD2( U, v ) );
	}

	template<bool Bool_, typename If_, typename Then_>
	using if_t = typename std::conditional<Bool_, If_, Then_>::type;

	/// aligned_alloc is not in clang-cl 13 with vs2022
#if not( defined( _MSC_VER ) and defined( __clang__ ) )
	template<typename T, std::size_t Alignment = alignof( T )>
	inline T *get_buffer( std::size_t count ) noexcept {
		return reinterpret_cast<T *>(
		  ::aligned_alloc( Alignment, sizeof( T ) * count ) );
	}
#endif

	template<typename T>
	inline void return_buffer( T *ptr ) noexcept {
		::free( ptr );
	}

	namespace utility_details {
		template<typename T>
		using has_data_end_test = decltype( std::declval<T>( ).data_end( ) );

		template<typename T>
		inline constexpr bool has_data_end_v =
		  daw::is_detected_v<has_data_end_test, T>;
	} // namespace utility_details

	template<typename Container>
	inline constexpr decltype( auto ) data_end( Container &&c ) {
		if constexpr( utility_details::has_data_end_v<Container> ) {
			return c.data_end( );
		} else {
			return std::data( c ) + static_cast<std::ptrdiff_t>( std::size( c ) );
		}
	}
	struct Empty {};

	template<auto Value>
	using constant = std::integral_constant<DAW_TYPEOF( Value ), Value>;
} // namespace daw
