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
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <memory>
#include <random>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

#include "daw_algorithm.h"
#include "daw_exception.h"
#include "daw_random.h"
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

			constexpr EqualToImpl( T value )
			  : m_value( std::move( value ) ) {}

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

		constexpr equal_to_last( ) noexcept
		  : m_value( nullptr ) {}

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
			NotImpl( Function func )
			  : m_function( func ) {}
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
	struct function_traits : public function_traits<decltype( &T{}( ) )> {};

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
		using root_type = std::function<daw::traits::root_type_t<ReturnType>( daw::traits::root_type_t<Args>... )>;
		using arg_types = std::tuple<Args...>;
		using result_type = ReturnType;
	};
	template<typename F>
	using function_traits_t = typename function_traits<F>::type;

	template<typename L>
	constexpr function_traits_t<L> make_function( L l ) noexcept {
		return static_cast<function_traits_t<L>>( l );
	}

	template<typename L>
	constexpr auto make_root_function( L l ) noexcept {
		return static_cast<typename function_traits<L>::root_type>( l );
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
	  daw::traits::root_type_t<ReturnType>( t( std::declval<daw::traits::root_type_t<Args>>( )... ) ) )( Args... )> {
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

	constexpr bool is_space( char chr ) noexcept {
		return 32 == chr;
	}

	template<typename Iterator1, typename Iterator2, typename Pred>
	std::vector<Iterator1> find_all_where( Iterator1 first, Iterator2 const last, Pred predicate ) {
		std::vector<Iterator1> results{};
		std::copy_if( first, last, std::back_inserter( results ), predicate );
		return results;
	}

	template<typename T, typename Pred>
	decltype( auto ) find_all_where( T const &values, Pred predicate ) {
		return find_all_where( std::cbegin( values ), std::cend( values ), predicate );
	}

	constexpr char AsciiUpper( char chr ) noexcept {
		if( 'a' <= chr && chr <= 'z' ) {
			return chr & ~static_cast<char>( 32 );
		}
		return chr;
	}

	constexpr char AsciiLower( char chr ) noexcept {
		if( 'A' <= chr && chr <= 'Z' ) {
			return chr | static_cast<char>( 32 );
		}
		return chr;
	}

	template<typename CharType, typename Traits, typename Allocator>
	constexpr auto AsciiUpper( std::basic_string<CharType, Traits, Allocator> str ) noexcept {
		daw::algorithm::map( str.cbegin( ), str.cend( ),
		                     str.begin( ), []( CharType c ) noexcept { return AsciiUpper( c ); } );
		return std::move( str );
	}

	template<typename CharType, typename Traits, typename Allocator>
	constexpr auto AsciiLower( std::basic_string<CharType, Traits, Allocator> str ) noexcept {
		daw::algorithm::map( str.cbegin( ), str.cend( ),
		                     str.begin( ), []( CharType c ) noexcept { return AsciiLower( c ); } );
		return std::move( str );
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
			RunIfValid( std::weak_ptr<T> w_ptr )
			  : m_link( w_ptr ) {}

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

		constexpr not_null( T *ptr )
		  : m_ptr{ptr} {
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
		return container.insert( container.end( ), {std::forward<Args>( args )...} );
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

	namespace impl {
		constexpr char get_nibble( uint8_t c ) noexcept {
			c &= 0x0F;
			if( c < 10 ) {
				return static_cast<char>( '0' + c );
			}
			return static_cast<char>( 'A' + ( c - 10 ) );
		}

		constexpr char get_lc_nibble( uint8_t c ) noexcept {
			c &= 0x0F;
			if( c < 10 ) {
				return static_cast<char>( '0' + c );
			}
			return static_cast<char>( 'a' + ( c - 10 ) );
		}

	} // namespace impl

	template<typename OutputIterator>
	constexpr OutputIterator hex( char c, OutputIterator it_out ) noexcept {
		uint8_t n = static_cast<uint8_t>( c );
		*it_out++ = impl::get_nibble( n >> 4 );
		*it_out++ = impl::get_nibble( n & 0x0F );
		return it_out;
	}

	template<typename OutputIterator>
	constexpr OutputIterator hex_lc( char c, OutputIterator it_out ) noexcept {
		uint8_t n = static_cast<uint8_t>( c );
		*it_out++ = impl::get_lc_nibble( n >> 4 );
		*it_out++ = impl::get_lc_nibble( n & 0x0F );
		return it_out;
	}

	template<typename T, typename OutputIterator, std::enable_if_t<daw::is_integral_v<T>, std::nullptr_t> = nullptr>
	constexpr OutputIterator hex( T const &val, OutputIterator it_out ) noexcept {
		for( size_t n = sizeof( T ); n > 0; --n ) {
			it_out = hex( static_cast<char>( val >> ( 8 * ( n - 1 ) ) ), it_out );
		}
		return it_out;
	}

	template<typename T, typename OutputIterator, std::enable_if_t<!daw::is_integral_v<T>, std::nullptr_t> = nullptr>
	OutputIterator hex( T const &val, OutputIterator it_out ) noexcept {
		auto chr_ptr = reinterpret_cast<char const *>( &val );
		for( size_t n = 0; n < sizeof( T ); ++n ) {
			it_out = hex( *chr_ptr++, it_out );
		}
		return it_out;
	}

	template<typename ForwardIterator1, typename ForwardIterator2, typename OutputIterator,
	         std::enable_if_t<daw::is_integral_v<typename std::iterator_traits<ForwardIterator1>::value_type>,
	                          std::nullptr_t> = nullptr>
	constexpr OutputIterator hex( ForwardIterator1 first_in, ForwardIterator2 const last_in,
	                              OutputIterator first_out ) noexcept {
		for( ; first_in != last_in; ++first_in ) {
			auto const val = *first_in;
			first_out = hex( val, first_out );
		}
		return first_out;
	}

	template<typename OutputIterator>
	constexpr OutputIterator hex( char const *str, size_t len, OutputIterator first_out ) noexcept {
		for( size_t n = 0; n < len; ++n ) {
			first_out = hex( str[n], first_out );
		}
		return first_out;
	}

	template<size_t N, typename OutputIterator>
	constexpr OutputIterator hex( char const ( &str )[N], OutputIterator first_out ) noexcept {
		return hex( str, N - 1, first_out );
	}

	template<typename ForwardIterator1, typename ForwardIterator2, typename OutputIterator,
	         std::enable_if_t<!daw::is_integral_v<typename std::iterator_traits<ForwardIterator1>::value_type>,
	                          std::nullptr_t> = nullptr>
	OutputIterator hex( ForwardIterator1 first_in, ForwardIterator2 const last_in, OutputIterator first_out ) noexcept {
		for( ; first_in != last_in; ++first_in ) {
			first_out = hex( *first_out, first_out );
		}
		return first_out;
	}

	template<typename Iterator>
	constexpr auto make_range( Iterator first, Iterator last ) noexcept {
		struct range_t {
			using iterator = Iterator;
			using difference_type = typename std::iterator_traits<iterator>::difference_type;

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

			constexpr void remove_prefix( size_t count ) {
				m_first += static_cast<difference_type>( count );
			}

			constexpr void remove_prefix( ) {
				++m_first;
			}

			constexpr void remove_suffix( ) {
				--m_last;
			}

			constexpr void remove_suffix( size_t count ) {
				m_last -= static_cast<difference_type>( count );
			}
		};
		return range_t{first, last};
	}

	template<typename... Ts>
	struct pack_index_of;

	template<typename T, typename... Ts>
	struct pack_index_of<T, Ts...> : std::integral_constant<size_t, 0> {};

	template<typename T, typename U, typename... Ts>
	struct pack_index_of<T, U, Ts...>
	  : std::integral_constant<size_t, ( daw::is_same_v<T, U> ? 0 : 1 + pack_index_of<T, Ts...>::value )> {};

	template<typename T, typename... Ts>
	constexpr size_t pack_index_of_v = pack_index_of<T, Ts...>::value;

	template<typename CharT = char>
	std::basic_string<CharT> read_file( daw::string_view path ) {
		std::basic_ifstream<CharT> in_file{path.to_string( )};
		daw::exception::Assert( in_file, "Could not open file" );
		return std::string{std::istreambuf_iterator<CharT>{in_file}, {}};
	}

} // namespace daw

template<typename... Ts>
constexpr void Unused( Ts &&... ) noexcept {}
