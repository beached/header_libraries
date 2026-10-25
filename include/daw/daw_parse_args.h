// copyright (c) darrell wright
//
// distributed under the boost software license, version 1.0. (see accompanying
// file license or copy at http://www.boost.org/license_1_0.txt)
//
// official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/ciso646.h"
#include "daw/cpp_20.h"
#include "daw/daw_arith_traits.h"
#include "daw/daw_as.h"
#include "daw/daw_string_view.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <optional>
#include <string>

namespace daw {
	class argv_iterator {
		char const *const *m_argv = nullptr;
		std::ptrdiff_t m_position = 0;

		struct arrow_proxy {
			daw::string_view value;

			[[nodiscard]] constexpr daw::string_view const *operator->( ) const {
				return &value;
			}
		};

	public:
		using iterator_category = std::random_access_iterator_tag;
		using value_type = daw::string_view;
		using reference = value_type;
		using pointer = arrow_proxy;
		using difference_type = std::ptrdiff_t;

		constexpr argv_iterator( ) = default;

		constexpr argv_iterator( char const *const *argv,
		                         difference_type position ) noexcept
		  : m_argv( argv )
		  , m_position( position ) {}

		[[nodiscard]] constexpr reference operator*( ) const noexcept {
			return m_argv[m_position];
		}

		[[nodiscard]] constexpr pointer operator->( ) const noexcept {
			return pointer{ **this };
		}

		[[nodiscard]] constexpr reference
		operator[]( difference_type offset ) const noexcept {
			return m_argv[m_position + offset];
		}

		constexpr argv_iterator &operator++( ) noexcept {
			++m_position;
			return *this;
		}

		[[nodiscard]] constexpr argv_iterator operator++( int ) noexcept {
			auto result = *this;
			++*this;
			return result;
		}

		constexpr argv_iterator &operator--( ) noexcept {
			--m_position;
			return *this;
		}

		[[nodiscard]] constexpr argv_iterator operator--( int ) noexcept {
			auto result = *this;
			--*this;
			return result;
		}

		constexpr argv_iterator &operator+=( difference_type offset ) noexcept {
			m_position += offset;
			return *this;
		}

		constexpr argv_iterator &operator-=( difference_type offset ) noexcept {
			m_position -= offset;
			return *this;
		}

		[[nodiscard]] friend constexpr argv_iterator
		operator+( argv_iterator iter, difference_type offset ) noexcept {
			iter += offset;
			return iter;
		}

		[[nodiscard]] friend constexpr argv_iterator
		operator+( difference_type offset, argv_iterator iter ) noexcept {
			iter += offset;
			return iter;
		}

		[[nodiscard]] friend constexpr argv_iterator
		operator-( argv_iterator iter, difference_type offset ) noexcept {
			iter -= offset;
			return iter;
		}

		[[nodiscard]] friend constexpr difference_type
		operator-( argv_iterator lhs, argv_iterator rhs ) noexcept {
			return lhs.m_position - rhs.m_position;
		}

		[[nodiscard]] friend constexpr bool
		operator==( argv_iterator lhs, argv_iterator rhs ) noexcept {
			return lhs.m_argv == rhs.m_argv and lhs.m_position == rhs.m_position;
		}

		[[nodiscard]] friend constexpr bool
		operator!=( argv_iterator lhs, argv_iterator rhs ) noexcept {
			return not( lhs == rhs );
		}

		[[nodiscard]] friend constexpr bool
		operator<( argv_iterator lhs, argv_iterator rhs ) noexcept {
			return lhs.m_position < rhs.m_position;
		}

		[[nodiscard]] friend constexpr bool
		operator<=( argv_iterator lhs, argv_iterator rhs ) noexcept {
			return not( rhs < lhs );
		}

		[[nodiscard]] friend constexpr bool
		operator>( argv_iterator lhs, argv_iterator rhs ) noexcept {
			return rhs < lhs;
		}

		[[nodiscard]] friend constexpr bool
		operator>=( argv_iterator lhs, argv_iterator rhs ) noexcept {
			return not( lhs < rhs );
		}
	};

	class argv_view {
		char const *const *m_argv = nullptr;
		std::size_t m_size = 0;

	public:
		using value_type = daw::string_view;
		using reference = value_type;
		using const_reference = value_type;
		using iterator = argv_iterator;
		using const_iterator = iterator;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		constexpr argv_view( ) = default;

		constexpr argv_view( int argc, char const *const *argv ) noexcept
		  : m_argv( argv )
		  , m_size( argc > 0 and argv != nullptr ? static_cast<size_type>( argc )
		                                         : size_type{ 0 } ) {}

		[[nodiscard]] constexpr iterator begin( ) const noexcept {
			return iterator{ m_argv, 0 };
		}

		[[nodiscard]] constexpr iterator end( ) const noexcept {
			return iterator{ m_argv, static_cast<difference_type>( m_size ) };
		}

		[[nodiscard]] constexpr const_iterator cbegin( ) const noexcept {
			return begin( );
		}

		[[nodiscard]] constexpr const_iterator cend( ) const noexcept {
			return end( );
		}

		[[nodiscard]] constexpr size_type size( ) const noexcept {
			return m_size;
		}

		[[nodiscard]] constexpr bool empty( ) const noexcept {
			return m_size == 0;
		}

		[[nodiscard]] constexpr reference
		operator[]( size_type index ) const noexcept {
			return m_argv[index];
		}
	};

	class Arguments;
	class args_iterator;

	struct Argument {
		daw::string_view name{ };
		daw::string_view value{ };

	private:
		friend class ::daw::Arguments;
		friend class ::daw::args_iterator;

		explicit constexpr Argument( ) = default;

		explicit constexpr Argument( daw::string_view args ) {
			if( args.starts_with( "--" ) ) {
				// We are a named argument, starting with '--'
				args.remove_prefix( 2 );
				name = args.pop_front_until( '=' );
				if( args.empty( ) ) {
					return;
				}
			}
			// We have an assigned value
			if( args.starts_with( '"' ) and args.ends_with( '"' ) ) {
				args.remove_prefix( 1 );
				args.remove_suffix( 1 );
			}
			value = args;
		}

	public:
		[[nodiscard]] DAW_CPP20_CX_ALLOC std::optional<std::string>
		as_string( ) const {
			if( not *this ) {
				return std::nullopt;
			}
			return std::string( value );
		}

	private:
		[[nodiscard]] static std::optional<std::uint64_t>
		parse_unsigned( daw::string_view sv ) noexcept {
			if( sv.empty( ) ) {
				return std::nullopt;
			}
			std::uint64_t result = 0;
			while( not sv.empty( ) ) {
				auto v = as<unsigned>( as<unsigned char>( sv.pop_front( ) ) ) -
				         as<unsigned>( as<unsigned char>( '0' ) );
				if( v >= 10U ) {
					return std::nullopt;
				}
				if( result > ( max_value<std::uint64_t> - v ) / 10U ) {
					return std::nullopt;
				}
				result *= 10U;
				result += v;
			}
			return result;
		}

	public:
		[[nodiscard]] std::optional<std::uint64_t> as_unsigned( ) const noexcept {
			if( not *this ) {
				return std::nullopt;
			}
			return parse_unsigned( value );
		}

		[[nodiscard]] std::optional<std::int64_t> as_signed( ) const noexcept {
			if( not *this ) {
				return std::nullopt;
			}
			auto sv = value;
			if( sv.empty( ) ) {
				return std::nullopt;
			}
			std::int64_t const sign = sv.front( ) == '-' ? -1 : 1;
			if( sign < 0 ) {
				sv.remove_prefix( );
			}
			auto v = parse_unsigned( sv );
			if( not v ) {
				return std::nullopt;
			}
			if( *v <= as<std::uint64_t>( max_value<std::int64_t> ) ) {
				return sign * as<std::int64_t>( *v );
			}
			if( *v == ( as<std::uint64_t>( max_value<std::int64_t> ) + 1U ) and
			    sign < 0 ) {
				return min_value<std::int64_t>;
			}
			return std::nullopt;
		}

		explicit constexpr operator bool( ) const {
			return value.data( ) != nullptr;
		}
	};

	class args_iterator {
		argv_iterator m_iterator{ };

		struct arrow_proxy {
			Argument value;

			[[nodiscard]] constexpr Argument const *operator->( ) const {
				return &value;
			}
		};

	public:
		using iterator_category = std::random_access_iterator_tag;
		using value_type = Argument;
		using reference = value_type;
		using pointer = arrow_proxy;
		using difference_type = std::ptrdiff_t;

		constexpr args_iterator( ) = default;

		explicit constexpr args_iterator( argv_iterator iterator ) noexcept
		  : m_iterator( iterator ) {}

		[[nodiscard]] constexpr reference operator*( ) const {
			return Argument{ *m_iterator };
		}

		[[nodiscard]] constexpr pointer operator->( ) const {
			return pointer{ **this };
		}

		[[nodiscard]] constexpr reference
		operator[]( difference_type offset ) const {
			return Argument{ m_iterator[offset] };
		}

		constexpr args_iterator &operator++( ) noexcept {
			++m_iterator;
			return *this;
		}

		[[nodiscard]] constexpr args_iterator operator++( int ) noexcept {
			auto result = *this;
			++*this;
			return result;
		}

		constexpr args_iterator &operator--( ) noexcept {
			--m_iterator;
			return *this;
		}

		[[nodiscard]] constexpr args_iterator operator--( int ) noexcept {
			auto result = *this;
			--*this;
			return result;
		}

		constexpr args_iterator &operator+=( difference_type offset ) noexcept {
			m_iterator += offset;
			return *this;
		}

		constexpr args_iterator &operator-=( difference_type offset ) noexcept {
			m_iterator -= offset;
			return *this;
		}

		[[nodiscard]] friend constexpr args_iterator
		operator+( args_iterator iter, difference_type offset ) noexcept {
			iter += offset;
			return iter;
		}

		[[nodiscard]] friend constexpr args_iterator
		operator+( difference_type offset, args_iterator iter ) noexcept {
			iter += offset;
			return iter;
		}

		[[nodiscard]] friend constexpr args_iterator
		operator-( args_iterator iter, difference_type offset ) noexcept {
			iter -= offset;
			return iter;
		}

		[[nodiscard]] friend constexpr difference_type
		operator-( args_iterator lhs, args_iterator rhs ) noexcept {
			return lhs.m_iterator - rhs.m_iterator;
		}

		[[nodiscard]] friend constexpr bool
		operator==( args_iterator lhs, args_iterator rhs ) noexcept {
			return lhs.m_iterator == rhs.m_iterator;
		}

		[[nodiscard]] friend constexpr bool
		operator!=( args_iterator lhs, args_iterator rhs ) noexcept {
			return not( lhs == rhs );
		}

		[[nodiscard]] friend constexpr bool
		operator<( args_iterator lhs, args_iterator rhs ) noexcept {
			return lhs.m_iterator < rhs.m_iterator;
		}

		[[nodiscard]] friend constexpr bool
		operator<=( args_iterator lhs, args_iterator rhs ) noexcept {
			return not( rhs < lhs );
		}

		[[nodiscard]] friend constexpr bool
		operator>( args_iterator lhs, args_iterator rhs ) noexcept {
			return rhs < lhs;
		}

		[[nodiscard]] friend constexpr bool
		operator>=( args_iterator lhs, args_iterator rhs ) noexcept {
			return not( lhs < rhs );
		}
	};

	class args_view {
		argv_view m_arguments{ };

	public:
		using value_type = Argument;
		using reference = value_type;
		using const_reference = value_type;
		using iterator = args_iterator;
		using const_iterator = iterator;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		constexpr args_view( ) = default;

		constexpr args_view( int argc, char const *const *argv ) noexcept
		  : m_arguments( argc, argv ) {}

		[[nodiscard]] constexpr iterator begin( ) const noexcept {
			return iterator{ m_arguments.begin( ) };
		}

		[[nodiscard]] constexpr iterator end( ) const noexcept {
			return iterator{ m_arguments.end( ) };
		}

		[[nodiscard]] constexpr const_iterator cbegin( ) const noexcept {
			return begin( );
		}

		[[nodiscard]] constexpr const_iterator cend( ) const noexcept {
			return end( );
		}

		[[nodiscard]] constexpr size_type size( ) const noexcept {
			return m_arguments.size( );
		}

		[[nodiscard]] constexpr bool empty( ) const noexcept {
			return m_arguments.empty( );
		}

		[[nodiscard]] constexpr reference operator[]( size_type index ) const {
			return begin( )[static_cast<difference_type>( index )];
		}
	};

	class Arguments {
		daw::string_view m_prog_name = "";
		args_view m_arguments{ };

	public:
		using value_type = Argument;
		using reference = value_type;
		using const_reference = value_type;
		using values_type = args_view;
		using iterator = typename values_type::iterator;
		using const_iterator = typename values_type::const_iterator;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		constexpr Arguments( int argc, char **argv )
		  : m_prog_name( argc > 0 and argv != nullptr and argv[0] != nullptr
		                   ? daw::string_view( argv[0] )
		                   : daw::string_view( ) )
		  , m_arguments( argc > 0 ? argc - 1 : 0,
		                 argc > 0 and argv != nullptr ? argv + 1 : nullptr ) {}

		[[nodiscard]] constexpr daw::string_view program_name( ) const {
			return m_prog_name;
		}

		[[nodiscard]] constexpr const_iterator begin( ) const {
			return m_arguments.begin( );
		}

		[[nodiscard]] constexpr const_iterator cbegin( ) const {
			return m_arguments.cbegin( );
		}

		[[nodiscard]] constexpr const_iterator end( ) const {
			return m_arguments.end( );
		}

		[[nodiscard]] constexpr const_iterator cend( ) const {
			return m_arguments.cend( );
		}

		[[nodiscard]] constexpr size_type size( ) const {
			return m_arguments.size( );
		}

		[[nodiscard]] constexpr bool empty( ) const {
			return m_arguments.empty( );
		}

		[[nodiscard]] constexpr const_reference
		operator[]( std::size_t idx ) const {
			return m_arguments[idx];
		}

		[[nodiscard]] constexpr Argument
		get_argument( daw::string_view name ) const {
			auto const pos = find_argument_position( name );
			if( pos ) {
				return m_arguments[*pos];
			}
			return Argument( );
		}

		[[nodiscard]] constexpr std::optional<daw::string_view>
		operator[]( daw::string_view name ) const {
			auto a = get_argument( name );
			if( a ) {
				return a.value;
			}
			return std::nullopt;
		}

		[[nodiscard]] constexpr std::optional<size_type>
		find_argument_position( daw::string_view name ) const {
			auto pos = std::find_if( begin( ), end( ), [name]( Argument const &arg ) {
				return arg.name == name;
			} );
			if( pos == end( ) ) {
				return std::nullopt;
			}
			return static_cast<size_type>( std::distance( begin( ), pos ) );
		}

		[[nodiscard]] constexpr std::optional<size_type>
		first_named_argument( ) const {
			auto pos = std::find_if( begin( ), end( ), []( Argument const &arg ) {
				return not arg.name.empty( );
			} );
			if( pos == end( ) ) {
				return std::nullopt;
			}
			return static_cast<size_type>( std::distance( begin( ), pos ) );
		}

		[[nodiscard]] constexpr bool has_named_argument( ) const {
			return static_cast<bool>( first_named_argument( ) );
		}
	};
} // namespace daw
