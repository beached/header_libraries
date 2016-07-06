// The MIT License (MIT)
//
// Copyright (c) 2014-2015 Darrell Wright
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

#include <boost/lexical_cast.hpp>
#include <boost/spirit/include/qi_numeric.hpp>
#include <iomanip>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#ifdef _MSC_VER
#	if _MSC_VER <1800
#		error Only Visual C++ 2013 and greater is supported
#	elif _MSC_VER == 1800
#		ifndef noexcept
#			define noexcept throw( )
#		endif
#	endif
#endif

namespace daw {
	namespace string {
		namespace details {
			using string_t = ::std::string;
		}
	}
}

template<typename CharT, typename Traits = ::std::char_traits<CharT>>
void clear( ::std::basic_stringstream<CharT, Traits> & ss ) {
	ss.str( ::std::basic_string<CharT, Traits>{ } );
	ss.clear( );
}

namespace daw {
	namespace string {
		namespace details {
			template<typename Arg>
			std::vector<details::string_t> unknowns_to_string( Arg arg ) {
				std::vector<details::string_t> result;
				result.emplace_back( boost::lexical_cast<details::string_t>(arg) );
				return result;
			}

			template<typename Arg, typename... Args>
			std::vector<details::string_t> unknowns_to_string( Arg arg, Args... args ) {
				std::vector<details::string_t> result;
				result.reserve( sizeof...(args)+1 );
				result.emplace_back( boost::lexical_cast<details::string_t>(arg) );
				auto result2 = unknowns_to_string( args... );
				result.insert( ::std::end( result ), ::std::begin( result2 ), ::std::end( result2 ) );
				return result;
			}
		}
	
		template<typename Delim, typename CharT = char, typename Traits = ::std::char_traits<CharT>, typename Allocator = ::std::allocator<CharT>>
		auto & split( ::std::basic_string<CharT, Traits, Allocator> const & s, Delim delim, ::std::vector<std::basic_string<CharT, Traits, Allocator>> & elems ) {
			std::basic_stringstream<CharT, Traits> ss( s );
			std::basic_string<CharT, Traits, Allocator> item;
			while( ::std::getline( ss, item, delim ) ) {
				elems.push_back( item );
			}
			return elems;
		}

		template<typename Delim, typename CharT = char, typename Traits = ::std::char_traits<CharT>, typename Allocator = ::std::allocator<CharT>>
		auto split( ::std::basic_string<CharT, Traits, Allocator> const & s, Delim delim ) {
			std::vector<std::basic_string<CharT, Traits, Allocator>> elems;
			split( s, delim, elems );
			return elems;
		}

		template<typename Arg>
		details::string_t to_string( Arg const & arg ) {
			return boost::lexical_cast<details::string_t>(arg);
		}

		namespace {
			template<typename Arg>
			auto string_join( Arg const & arg ) -> decltype(to_string( arg )) {
				return to_string( arg );
			}
		}

		template<typename Arg1, typename Arg2>
		auto string_join( Arg1 const & arg1, Arg2 const & arg2 ) -> decltype(to_string( arg1 ) + to_string( arg2 )) {
			return to_string( arg1 ) + to_string( arg2 );
		}

		template<typename Arg1, typename Arg2, typename... Args>
		auto string_join( Arg1 const & arg1, Arg2 const & arg2, Args const & ... args ) -> decltype(string_join( string_join( arg1, arg2 ), string_join( args... ) )) {
			return string_join( string_join( arg1, arg2 ), string_join( args... ) );
		}

		template<typename StringType>
		struct ends_with_t {
			ends_with_t( ) noexcept { }
			~ends_with_t( ) = default;
			ends_with_t( ends_with_t const & ) noexcept { }
			ends_with_t( ends_with_t&& ) noexcept { }
			ends_with_t& operator=( ends_with_t ) const noexcept { return *this; }
			bool operator==( ends_with_t const & ) const noexcept { return true; }
			bool operator<( ends_with_t const & ) const noexcept { return false; }

			bool operator()( StringType const & src, const char ending ) const noexcept {
				return 0 <src.size( ) && ending == src[src.size( ) - 1];
			}

			bool operator()( StringType const & src, StringType const & ending ) const noexcept {
				auto pos = src.find_last_of( ending );
				return details::string_t::npos != pos && pos == src.size( ) - 1;
			}
		};

		template<typename StringType, typename Ending>
		bool ends_with( StringType const & src, Ending const & ending ) {
			const static auto func = ends_with_t<StringType>( );
			return func( src, ending );
		}

		//////////////////////////////////////////////////////////////////////////
		// Summary: takes a format string like "{0} {1} {2}" and in this case 3 parameters
		template<typename Arg, typename... Args>
		details::string_t fmt( details::string_t format, Arg arg, Args... args ) {
			std::regex const reg( R"^(\{(\d+(:\d)*)\})^" );
			static ::std::stringstream ss;
			clear( ss );
			auto const arguments = details::unknowns_to_string( arg, args... );
			std::smatch sm;
			while( ::std::regex_search( format, sm, reg ) ) {
				auto const & prefix = sm.prefix( ).str( );
				ss <<prefix;
				if( ends_with( prefix, "{" ) ) {
					ss <<sm[0].str( );
				} else {
					auto delims = split( sm[1].str( ), ':' );
					if( 1>= delims.size( ) ) {
						ss <<arguments[boost::lexical_cast<size_t>(sm[1].str( ))];
					} else if( 2 == delims.size( ) ) {
						// Assumes the argument at pos is a double.  If not, will crash
						size_t pos = boost::lexical_cast<size_t>(delims[0]);
						int precision = boost::lexical_cast<int>(delims[1]);
						ss <<std::fixed <<std::setprecision( precision ) <<boost::lexical_cast<double>(arguments[pos]);
					} else {
						throw ::std::out_of_range( fmt( "Unknown string format.  Too many colons(", delims.size( ), "): ", sm[1].str( ) ) );
					}
				}
				format = sm.suffix( ).str( );
			}
			ss <<format;
			return ss.str( );
		}

		namespace impl {
			constexpr auto standard_split_delimiters( char ) {
				return " \f\n\r\t\v\0";
			}

			constexpr auto standard_split_delimiters( wchar_t ) {
				return L" \f\n\r\t\v\0";
			}
		}	// namespace impl

		template<typename CharT = char, typename Traits = ::std::char_traits<CharT>, typename Allocator = ::std::allocator<CharT>>
		auto trim_right_copy( ::std::basic_string<CharT, Traits, Allocator> const & str, ::std::basic_string<CharT, Traits, Allocator> const & delimiters = impl::standard_split_delimiters( CharT{ } ) ) {
			if( str.empty( ) ) {
				return str;
			}
			return str.substr( 0, str.find_last_not_of( delimiters ) + 1 );
		}

		template<typename CharT = char, typename Traits = ::std::char_traits<CharT>, typename Allocator = ::std::allocator<CharT>>
		void trim_right( ::std::basic_string<CharT, Traits, Allocator> & str, ::std::basic_string<CharT, Traits, Allocator> const & delimiters = impl::standard_split_delimiters( CharT{ } ) ) {
			if( str.empty( ) ) {
				return;
			}
			str = str.substr( 0, str.find_last_not_of( delimiters ) + 1 );
		}


		template<typename CharT = char, typename Traits = ::std::char_traits<CharT>, typename Allocator = ::std::allocator<CharT>>
		auto trim_left_copy( ::std::basic_string<CharT, Traits, Allocator> const & str, ::std::basic_string<CharT, Traits, Allocator> const & delimiters = impl::standard_split_delimiters( CharT{ } ) ) {
			if( str.empty( ) ) {
				return str;
			}
			return str.substr( str.find_first_not_of( delimiters ) );
		}

		template<typename CharT = char, typename Traits = ::std::char_traits<CharT>, typename Allocator = ::std::allocator<CharT>>
		void trim_left( ::std::basic_string<CharT, Traits, Allocator> & str, ::std::basic_string<CharT, Traits, Allocator> const & delimiters = impl::standard_split_delimiters( CharT{ } ) ) {
			if( str.empty( ) ) {
				return;
			}
			str = str.substr( str.find_first_not_of( delimiters ) );
		}

		template<class StringElementType, class StringType>
		bool in( StringElementType const & val, StringType const & values ) {
			return StringType::npos != values.find( val );
		}

		template<typename CharT = char, typename Traits = ::std::char_traits<CharT>, typename Allocator = ::std::allocator<CharT>>
		void trim( ::std::basic_string<CharT, Traits, Allocator> & str, ::std::basic_string<CharT, Traits, Allocator> const & delimiters = impl::standard_split_delimiters( CharT{ } ) ) {
			auto const start = str.find_first_not_of( delimiters );
			if( ::std::basic_string<CharT, Traits, Allocator>::npos == start ) {
				str.clear( );
				return;
			}
			auto const end = str.find_last_not_of( delimiters );
			auto const len = end - start + 1;
			str = str.substr( start, len );
		}

		template<typename CharT = char, typename Traits = ::std::char_traits<CharT>, typename Allocator = ::std::allocator<CharT>>
		auto trim_copy( ::std::basic_string<CharT, Traits, Allocator> str, ::std::basic_string<CharT, Traits, Allocator> const & delimiters = impl::standard_split_delimiters( CharT{ } ) ) {
			trim_nocopy( str, delimiters );
			return str;
		}

		template<typename CharT = char, typename Traits = ::std::char_traits<CharT>, typename Allocator = ::std::allocator<CharT>>
		bool convertTo( ::std::basic_string<CharT, Traits, Allocator> const & from, int32_t & to ) {
			auto it = from.begin( );
			using namespace boost::spirit;
			if( !qi::parse( it, from.end( ), qi::int_, to ) ) {
				return false;
			}
			return from.end( ) == it;
		}

		template<typename CharT = char, typename Traits = ::std::char_traits<CharT>, typename Allocator = ::std::allocator<CharT>>
		bool convertTo( ::std::basic_string<CharT, Traits, Allocator> const & from, int64_t & to ) {
			auto it = from.begin( );
			using namespace boost::spirit;
			if( !qi::parse( it, from.end( ), qi::long_long, to ) ) {
				return false;
			}
			return from.end( ) == it;
		}

		template<typename CharT = char, typename Traits = ::std::char_traits<CharT>, typename Allocator = ::std::allocator<CharT>>
		bool convertTo( ::std::basic_string<CharT, Traits, Allocator> const & from, float & to ) {
			auto it = from.begin( );
			using namespace boost::spirit;
			if( !qi::parse( it, from.end( ), qi::float_, to ) ) {
				return false;
			}
			return from.end( ) == it;
		}

		template<typename CharT = char, typename Traits = ::std::char_traits<CharT>, typename Allocator = ::std::allocator<CharT>>
		bool convertTo( ::std::basic_string<CharT, Traits, Allocator> const & from, double & to ) {
			auto it = from.begin( );
			using namespace boost::spirit;
			if( !qi::parse( it, from.end( ), qi::double_, to ) ) {
				return false;
			}
			return from.end( ) == it;
		}

		template<class T, class U>
		std::string convertToString( ::std::pair<T, U> const & from ) {
			return string_join( "{", to_string( from.first ), ", ", to_string( from.second ), " }" );
		}

		template<typename ValueType, typename CharT = char, typename Traits = ::std::char_traits<CharT>, typename Allocator = ::std::allocator<CharT>>
		void convertToString( ValueType const & from, ::std::basic_string<CharT, Traits, Allocator> & to ) {
			to = to_string( from );
		}

		template<typename CharT = char, typename Traits = ::std::char_traits<CharT>, typename Allocator = ::std::allocator<CharT>>
		bool contains( ::std::basic_string<CharT, Traits, Allocator> const & str, ::std::basic_string<CharT, Traits, Allocator> const & match ) {
			return str.find( match ) != ::std::basic_string<CharT, Traits, Allocator>::npos;
		}

		namespace impl {
			auto str_size( wchar_t const * str ) {
				return wcslen( str );
			}

			auto str_size( char const * str ) {
				return strlen( str );
			}
		}	// namespace impl

		template<typename CharT = char, typename traits = ::std::char_traits<CharT>, typename Alloc = ::std::allocator<CharT>>
		void search_replace( ::std::basic_string<CharT, traits, Alloc> & in_str, CharT const * search_for, CharT const * replace_with ) {
			size_t pos = 0u;
			auto const search_for_len = impl::str_size( search_for );
			auto const replace_with_len = impl::str_size( replace_with );

			while( (pos = in_str.find( search_for, pos )) != ::std::basic_string<CharT, traits, Alloc>::npos ) {
				in_str.replace( pos, search_for_len, replace_with );
				pos += replace_with_len;
			}
		}

		template<typename CharT = char, typename traits = ::std::char_traits<CharT>, typename Alloc = ::std::allocator<CharT>>
		auto search_replace_copy( ::std::basic_string<CharT, traits, Alloc> in_str, CharT const * search_for, CharT const * replace_with ) {
			search_replace( in_str, search_for, replace_with );	
			return in_str;
		}

		namespace impl {
			template<typename CharT = char, typename Traits = ::std::char_traits<CharT>, typename Allocator = ::std::allocator<CharT>>
			struct BasicString: public ::std::basic_string<CharT, Traits, Allocator> {

				template<typename... Args>
				BasicString( Args&& ... args ): ::std::basic_string<CharT, Traits, Allocator>( ::std::forward<Args>( args )... ) { }
				
				
				BasicString( BasicString const & ) = default;
				BasicString( BasicString && ) = default;
				~BasicString( ) = default;
				BasicString & operator=( BasicString const & ) = default;
				BasicString & operator=( BasicString && ) = default;

				BasicString & search_replace( CharT const * search_for, CharT const * replace_with ) {
					daw::string::search_replace( *this, search_for, replace_with );
					return *this;
				}
				
				BasicString & trim_left( ::std::basic_string<CharT, Traits, Allocator> const & delimiters = impl::standard_split_delimiters( CharT{ } ) ) {
					daw::string::trim_left( *this, delimiters );
					return *this;
				}

				BasicString & trim_right( ::std::basic_string<CharT, Traits, Allocator> const & delimiters = impl::standard_split_delimiters( CharT{ } ) ) {
					daw::string::trim_right( *this, delimiters );
					return *this;
				}

				BasicString & trim( ::std::basic_string<CharT, Traits, Allocator> const & delimiters = impl::standard_split_delimiters( CharT{ } ) ) {
					daw::string::trim( *this, delimiters );
					return *this;
				}

			};	// BasicString
		}
	}	// namespace string

	using String = ::daw::string::impl::BasicString<char>;
	using WString = ::daw::string::impl::BasicString<wchar_t>;	

}	// namespace daw
