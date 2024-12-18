// copyright (c) darrell wright
//
// distributed under the boost software license, version 1.0. (see accompanying
// file license or copy at http://www.boost.org/license_1_0.txt)
//
// official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "cpp_20.h"
#include "daw_string_view.h"

#include <optional>
#include <vector>

namespace daw {
	namespace parse_arg_details {
		struct is_ws_t {
			constexpr bool operator( )( char c ) const {
				return c <= 0x20 and c > 0x0;
			}
		};

		inline constexpr auto is_ws = is_ws_t{ };
	} // namespace parse_arg_details

	class Arguments;

	struct Argument {
		daw::string_view name{ };
		daw::string_view value{ };

	private:
		friend class ::daw::Arguments;

		constexpr Argument( daw::string_view args ) {
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
	};

	class Arguments {
		daw::string_view m_prog_name = "";
		std::vector<Argument> m_arguments{ };

	public:
		using value_type = Argument;
		using reference = Argument &;
		using const_reference = Argument const &;
		using values_type = std::vector<value_type>;
		using iterator = typename values_type::iterator;
		using const_iterator = typename values_type::const_iterator;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		DAW_CPP20_CX_ALLOC Arguments( int argc, char **argv ) {
			if( argc > 0 and argv[0] != nullptr ) {
				m_prog_name = argv[0];
			}
			for( int n = 1; n < argc; ++n ) {
				m_arguments.push_back( Argument{ argv[n] } );
			}
		}

		DAW_CPP20_CX_ALLOC daw::string_view program_name( ) const {
			return m_prog_name;
		}

		DAW_CPP20_CX_ALLOC const_iterator begin( ) const {
			return m_arguments.begin( );
		}

		DAW_CPP20_CX_ALLOC const_iterator cbegin( ) const {
			return m_arguments.cbegin( );
		}

		DAW_CPP20_CX_ALLOC const_iterator end( ) const {
			return m_arguments.end( );
		}

		DAW_CPP20_CX_ALLOC const_iterator cend( ) const {
			return m_arguments.cend( );
		}

		DAW_CPP20_CX_ALLOC size_type size( ) const {
			return m_arguments.size( );
		}

		DAW_CPP20_CX_ALLOC bool empty( ) const {
			return m_arguments.empty( );
		}

		DAW_CPP20_CX_ALLOC const_reference operator[]( std::size_t idx ) const {
			return m_arguments[idx];
		}

		DAW_CPP20_CX_ALLOC std::optional<size_type>
		find_argument_position( daw::string_view name ) const {
			auto pos = std::find_if( begin( ), end( ), [name]( Argument const &arg ) {
				return arg.name == name;
			} );
			if( pos == end( ) ) {
				return std::nullopt;
			}
			return static_cast<size_type>( std::distance( begin( ), pos ) );
		}

		DAW_CPP20_CX_ALLOC std::optional<size_type> first_named_argument( ) const {
			auto pos = std::find_if( begin( ), end( ), []( Argument const &arg ) {
				return not arg.name.empty( );
			} );
			if( pos == end( ) ) {
				return std::nullopt;
			}
			return static_cast<size_type>( std::distance( begin( ), pos ) );
		}

		DAW_CPP20_CX_ALLOC bool has_named_argument( ) const {
			return static_cast<bool>( first_named_argument( ) );
		}
	};
} // namespace daw
