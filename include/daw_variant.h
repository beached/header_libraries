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

#include <cstdint>
#include <functional>
#include <ostream>
#include <string>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>

#include "daw_algorithm.h"
#include "daw_exception.h"
#include "daw_operators.h"
#include "daw_traits.h"
#include "daw_utility.h"

namespace daw {
	struct bad_variant_t_access : public std::runtime_error {
		template<typename... Args>
		bad_variant_t_access( Args &&... args )
		  : std::runtime_error{std::forward<Args>( args )...} {}

		~bad_variant_t_access( ) = default;
	};

	template<typename T>
	static std::type_index get_type_index( ) {
		using value_type = std::remove_cv_t<T>;
		return std::type_index( typeid( value_type ) );
	}

	template<typename Type, typename... Types>
	struct variant_t;
} // namespace daw

namespace daw {
	template<typename T, typename... Types>
	const T &get( daw::variant_t<Types...> const &value );

	namespace tostrings {
		template<typename CharT = char, typename Traits = ::std::char_traits<CharT>,
		         typename Allocator = ::std::allocator<CharT>>
		std::basic_string<CharT, Traits, Allocator> to_string(::std::basic_string<CharT, Traits, Allocator> s ) {
			return std::move( s );
		}

		std::string to_string(...) {
			daw::exception::daw_throw( "Attemp to call to string on unsupported type, overload to enable" );
		}

		template<typename T>
		std::string to_string( T const *ptr ) {
			if( nullptr == ptr ) {
				return "";
			}
			using daw::tostrings::to_string;
			using std::to_string;
			return to_string( *ptr );
		}
	} // namespace tostrings
	namespace has_to_string {
		using daw::tostrings::to_string;
		using std::to_string;
		template<typename T>
		using check = decltype( to_string( std::declval<T>( ) ) );
	}
	template<typename T>
	constexpr bool has_to_string_v = daw::is_detected_v<has_to_string::check, T>;

	template<typename... Types>
	class generate_to_strings_t {
		template<typename T>
		struct to_string_t {
			static_assert( has_to_string_v<T>, "to_string must be defined for type" );

			std::string operator( )( variant_t<Types...> const &value ) const {
				using daw::tostrings::to_string;
				using std::to_string;
				return to_string( get<T>( value ) );
			}
		}; // to_string_t
	public:
		template<typename T>
		auto generate( ) const {
			return to_string_t<T>{};
		}
	}; // generate_to_strings_t

	template<typename... Types>
	class generate_compare_t {
		template<typename T, std::enable_if_t<!daw::traits::is_vector_like_not_string_v<T>>>
		static bool op_eq( T const &lhs, T const &rhs ) {
			return lhs == rhs;
		}

		template<typename T, std::enable_if_t<daw::traits::is_vector_like_not_string_v<T>>>
		static auto op_eq( std::vector<T> const &lhs, std::vector<T> const &rhs ) -> decltype( lhs[0] == rhs[0], bool( ) ) {
			return std::equal( lhs.begin( ), lhs.end( ), rhs.begin( ), rhs.end( ) );
		}

		template<typename T, typename = std::enable_if_t<daw::traits::operators::has_op_eq_v<T> &&
		                                                 daw::traits::operators::has_op_lt_v<T>>>
		static constexpr auto op_compare( variant_t<Types...> const &lhs, variant_t<Types...> const &rhs )
		  -> decltype( std::declval<T>( ) == std::declval<T>( ) && std::declval<T>( ) < std::declval<T>( ), int( ) ) {
			auto const &a = lhs.template get<T>( );
			auto const &b = rhs.template get<T>( );
			if( a == b ) {
				return 0;
			} else if( a < b ) {
				return -1;
			}
			return 1;
		}

		template<typename T, typename = std::enable_if_t<daw::traits::has_to_string_v<variant_t<Types...>> &&
		                                                 !( daw::traits::operators::has_op_eq_v<T> &&
		                                                    daw::traits::operators::has_op_lt_v<T> )>>
		static int op_compare( variant_t<Types...> const &lhs, variant_t<Types...> const &rhs ) {
			return lhs.to_string( ).compare( rhs.to_string( ) );
		}

		template<typename T>
		struct compare_t final {
			int operator( )( variant_t<Types...> const &lhs, variant_t<Types...> const &rhs ) {
				if( lhs.type_index( ) == rhs.type_index( ) ) {
					return op_compare<T>( lhs, rhs );
				}
				return lhs.to_string( ).compare( rhs.to_string( ) );
			}
		}; // compare_t
	public:
		template<typename T>
		auto generate( ) const {
			return compare_t<T>{};
		}
	}; // generate_compare_t

	template<typename... Types>
	class generate_destruct_t final {
		template<typename T>
		struct destruct_t final {
			void operator( )( variant_t<Types...> &value ) const {
				auto &tmp = value.template get<T>( );
				tmp.~T( );
			}
		}; // destruct_t
	public:
		template<typename T>
		auto generate( ) const {
			return destruct_t<T>{};
		}
	}; // generate_destruct_t

	template<typename... Types>
	struct variant_helper_funcs_t {
		using to_string_t = std::function<std::string( variant_t<Types...> const & )>;
		using compare_t = std::function<int( variant_t<Types...> const &, variant_t<Types...> const & )>;
		using destruct_t = std::function<void( variant_t<Types...> & )>;

		to_string_t to_string;
		compare_t compare;
		destruct_t destruct;

		variant_helper_funcs_t( )
		  : to_string{}
		  , compare{}
		  , destruct{} {}

		template<typename ToString, typename Compare, typename Destruct>
		variant_helper_funcs_t( ToString a, Compare b, Destruct c )
		  : to_string{a}
		  , compare{b}
		  , destruct{c} {}
	}; // variant_helper_funcs_t

	template<typename... Types>
	struct generate_variant_helper_funcs_t {
		generate_to_strings_t<Types...> generate_to_strings;
		generate_compare_t<Types...> generate_compares;
		generate_destruct_t<Types...> generate_destructs;

		template<typename T>
		auto generate( ) const {
			return variant_helper_funcs_t<Types...>{generate_to_strings.template generate<T>( ),
			                                        generate_compares.template generate<T>( ),
			                                        generate_destructs.template generate<T>( )};
		}
	}; // generate_variant_helper_funcs_t

	template<typename... Types>
	auto get_variant_helpers( ) {
		static_assert( sizeof...( Types ) > 0, "Must supply at least one type" );

		std::unordered_map<std::type_index, variant_helper_funcs_t<Types...>> results;
		generate_variant_helper_funcs_t<Types...> generate_variant_helper_funcs;

		auto list = {(
		  (void)( results[get_type_index<Types>( )] = generate_variant_helper_funcs.template generate<Types>( ) ), 0 )...};

		Unused( list );

		return results;
	}

	namespace impl {
		class stored_type_t {
			std::type_index m_value;

			static std::type_index empty_type( ) noexcept {
				struct no_type_t {};
				return get_type_index<no_type_t>( );
			}

		public:
			stored_type_t( ) noexcept
			  : m_value{empty_type( )} {}

			stored_type_t( std::type_index ti ) noexcept
			  : m_value{std::move( ti )} {}

			stored_type_t &operator=( std::type_index ti ) noexcept {
				m_value = std::move( ti );
				return *this;
			}

			stored_type_t( stored_type_t const & ) = default;
			stored_type_t( stored_type_t && ) noexcept = default;
			stored_type_t &operator=( stored_type_t const & ) = default;
			stored_type_t &operator=( stored_type_t && ) noexcept = default;
			~stored_type_t( ) noexcept = default;

			std::type_index const &operator*( ) const noexcept {
				return m_value;
			}

			operator bool( ) const noexcept {
				return m_value != empty_type( );
			}

			bool empty( ) const noexcept {
				return m_value == empty_type( );
			}

			void reset( ) noexcept {
				m_value = empty_type( );
			}
		};

		bool operator==( stored_type_t const &lhs, stored_type_t const &rhs ) noexcept {
			return *lhs == *rhs;
		}

		bool operator!=( stored_type_t const &lhs, stored_type_t const &rhs ) noexcept {
			return *lhs != *rhs;
		}

	} // namespace impl

	template<typename Type, typename... Types>
	struct variant_t {
		static constexpr size_t const BUFFER_SIZE = daw::traits::max_sizeof_v<Type, Types...>;

		template<typename T>
		static constexpr bool is_valid_type =
		  daw::traits::is_one_of_v<std::remove_cv_t<T>, std::remove_cv_t<Type>, std::remove_cv_t<Types>...>;

	private:
	public:
		bool empty( ) const noexcept {
			return m_stored_type.empty( );
		}

	private:
		void reset_type( ) noexcept {
			m_stored_type.reset( );
		}

		impl::stored_type_t m_stored_type;
		alignas( daw::traits::max_sizeof_t<Type, Types...> ) std::array<uint8_t, BUFFER_SIZE> m_buffer;

		static auto get_helper_funcs( std::type_index idx ) {
			static auto func_map = get_variant_helpers<Type, Types...>( );
			return func_map[idx];
		}

		template<typename T, typename Result = std::enable_if_t<is_valid_type<T>, std::remove_cv_t<T>>>
		Result *ptr( ) {
			using namespace daw::exception;
			using value_type = std::remove_cv_t<T>;
			daw_throw_on_true<bad_variant_t_access>( empty( ), "Attempt to access an empty value" );
			daw_throw_on_false<bad_variant_t_access>( is_same_type<value_type>( ),
			                                          "Attempt to access a value of another type" );
			static_assert( sizeof( value_type ) <= BUFFER_SIZE,
			               "This should never happen.  sizeof(T) does not fit into m_buffer" );
			return reinterpret_cast<value_type *>( static_cast<void *>( m_buffer.data( ) ) );
		}

		template<typename T, typename Result = std::enable_if_t<is_valid_type<T>, std::remove_cv_t<T>>>
		Result const *ptr( ) const {
			using namespace daw::exception;
			using value_type = std::remove_cv_t<T>;
			daw_throw_on_true<bad_variant_t_access>( empty( ), "Attempt to access an empty value" );
			daw_throw_on_false<bad_variant_t_access>( is_same_type<value_type>( ),
			                                          "Attempt to access a value of another type" );
			static_assert( sizeof( value_type ) <= BUFFER_SIZE,
			               "This should never happen.  sizeof(T) does not fit into m_buffer" );
			return reinterpret_cast<value_type const *>( static_cast<void const *>( m_buffer.data( ) ) );
		}

		void *raw_ptr( ) {
			return static_cast<void *>( m_buffer.data( ) );
		}

		/*template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
		void set_type( ) {
		  m_stored_type = get_type_index<T>( );
		  new( raw_ptr( ) ) T{};
		}*/

		template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
		void set_type( T &&value ) {
			if( !is_same_type<T>( ) ) {
				reset( );
			}
			m_stored_type = get_type_index<T>( );
			void *p = raw_ptr( );
			new( p ) T{std::forward<T>( value )};
		}

	public:
		variant_t( )
		  : m_stored_type{}
		  , m_buffer{} {}

		template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
		variant_t( T value )
		  : m_stored_type{}
		  , m_buffer{} {

			store( std::move( value ) );
		}

		~variant_t( ) {
			if( !empty( ) ) {
				get_helper_funcs( *m_stored_type ).destruct( *this );
			}
		}

		variant_t( variant_t const & ) = default;
		variant_t &operator=( variant_t const & ) = default;
		variant_t( variant_t && ) noexcept = default;
		variant_t &operator=( variant_t && ) noexcept = default;

		template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
		variant_t &operator=( T value ) {
			store( std::move( value ) );
			return *this;
		}

		friend void swap( variant_t &lhs, variant_t &rhs ) noexcept {
			using std::swap;
			lhs.m_buffer.swap( rhs.m_buffer );
			swap( lhs.m_stored_type, rhs.m_stored_type );
		}

		std::type_index type_index( ) const {
			return *m_stored_type;
		}

		explicit operator bool( ) const {
			return !empty( );
		}

		void reset( ) {
			if( !empty( ) ) {
				get_helper_funcs( *m_stored_type ).destruct( *this );
			}
			reset_type( );
		}

		template<typename T>
		bool is_same_type( ) const {
			return m_stored_type == get_type_index<T>( );
		}

		template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
		variant_t &store( T &&value ) {
			set_type<T>( std::forward<T>( value ) );
			//			get<T>( ) = value;
			return *this;
		}

		template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
		T &get( ) {
			return *ptr<T>( );
		}

		template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
		T const &get( ) const {
			return *ptr<T>( );
		}

		template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
		explicit operator T const &( ) const {
			return get<T>( );
		}

		template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
		explicit operator T &( ) {
			return get<T>( );
		}

		std::string to_string( ) const {
			if( empty( ) ) {
				return "";
			}
			return get_helper_funcs( *m_stored_type ).to_string( *this );
		}

		std::string operator*( ) const {
			return to_string( );
		}

		auto compare( variant_t const &rhs ) const {
			return get_helper_funcs( *m_stored_type ).compare( *this, rhs );
		}

		template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
		auto compare( T const &rhs ) const {
			return compare( variant_t( rhs ) );
		}

		template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
		friend bool operator==( variant_t const &lhs, T const &rhs ) {
			return lhs.compare( rhs ) == 0;
		}

		template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
		friend bool operator!=( variant_t const &lhs, T const &rhs ) {
			return lhs.compare( rhs ) != 0;
		}

		template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
		friend bool operator<( variant_t const &lhs, T const &rhs ) {
			return lhs.compare( rhs ) < 0;
		}

		template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
		friend bool operator>( variant_t const &lhs, T const &rhs ) {
			return lhs.compare( rhs ) > 0;
		}

		template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
		friend bool operator<=( variant_t const &lhs, T const &rhs ) {
			return lhs.compare( rhs ) <= 0;
		}

		template<typename T, typename = std::enable_if_t<is_valid_type<T>>>
		friend bool operator>=( variant_t const &lhs, T const &rhs ) {
			return lhs.compare( rhs ) >= 0;
		}

		create_friend_comparison_operators( variant_t );
	}; // variant_t

	template<typename... Args>
	std::string to_string( variant_t<Args...> const &value ) {
		return value.to_string( );
	}

	template<typename... Args>
	std::ostream &operator>>( std::ostream &os, variant_t<Args...> const &value ) {
		using std::to_string;
		os << to_string( value );
		return os;
	}

	template<typename Type, typename... Types>
	auto as_variant_t( Type const &value ) {
		return variant_t<Type, Types...>{}.store( value );
	};

	template<typename T, typename... Types>
	T const &get( daw::variant_t<Types...> const &value ) {
		return value.template get<T>( );
	}

	template<typename T, typename... Types>
	T &get( daw::variant_t<Types...> &value ) {
		return value.template get<T>( );
	}
} // namespace daw
