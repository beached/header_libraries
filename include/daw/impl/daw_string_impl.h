// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/ciso646.h"
#include "daw/daw_traits.h"
#include "daw/deprecated/daw_string_view1_fwd.h"
#include "daw/traits/daw_traits_is_ostream_like.h"

#include <array>
#include <cstddef>
#include <type_traits>

namespace daw {
	template<typename CharT, size_t Capacity>
	struct basic_bounded_string;

	namespace details {
		template<class ForwardIt1, class ForwardIt2>
		[[nodiscard]] constexpr ForwardIt1 search( ForwardIt1 first,
		                                           ForwardIt1 last,
		                                           ForwardIt2 s_first,
		                                           ForwardIt2 s_last ) {
			for( ;; ++first ) {
				ForwardIt1 it = first;
				for( ForwardIt2 s_it = s_first;; ++it, ++s_it ) {
					if( s_it == s_last ) {
						return first;
					}
					if( it == last ) {
						return last;
					}
					if( not( *it == *s_it ) ) {
						break;
					}
				}
			}
		}

		template<class ForwardIt1, class ForwardIt2, class BinaryPredicate>
		[[nodiscard]] constexpr ForwardIt1 search( ForwardIt1 first,
		                                           ForwardIt1 last,
		                                           ForwardIt2 s_first,
		                                           ForwardIt2 s_last,
		                                           BinaryPredicate p ) {
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

		template<typename SizeT, typename CharT>
		[[nodiscard]] constexpr SizeT strlen( CharT const *const str ) noexcept {
			if( str == nullptr ) {
				return 0;
			}
			auto pos = str;
			while( *( pos ) != 0 ) {
				++pos;
			}
			return static_cast<SizeT>( pos - str );
		}

		template<typename SizeT>
		constexpr SizeT sstrlen( std::nullptr_t, SizeT, SizeT ) noexcept {
			return 0;
		}

		template<typename SizeT, typename CharT>
		[[nodiscard]] constexpr SizeT
		sstrlen( CharT const *const str, SizeT count, SizeT npos ) noexcept {
#ifndef __GNUC__
			// Bug in C++ says this isn't a constexpr
			if( nullptr == str ) {
				return 0ULL;
			}
#endif
			if( count == npos ) {
				count = strlen<SizeT>( str );
			}
			return count;
		}

		template<typename InputIt, typename ForwardIt, typename BinaryPredicate>
		[[nodiscard]] constexpr InputIt find_first_of( InputIt first,
		                                               InputIt last,
		                                               ForwardIt s_first,
		                                               ForwardIt s_last,
		                                               BinaryPredicate p ) {
			static_assert(
			  traits::is_binary_prdicate_v<
			    BinaryPredicate,
			    typename std::iterator_traits<InputIt>::value_type,
			    typename std::iterator_traits<InputIt>::value_type>,
			  "BinaryPredicate p does not fulfill the requires of a binary "
			  "predicate concept.  See "
			  "http://en.cppreference.com/w/cpp/concept/UnaryPredicate" );
			for( ; first != last; ++first ) {
				for( ForwardIt it = s_first; it != s_last; ++it ) {
					if( p( *first, *it ) ) {
						return first;
					}
				}
			}
			return last;
		}

		template<typename InputIt, typename ForwardIt, typename BinaryPredicate>
		[[nodiscard]] constexpr InputIt find_first_not_of( InputIt first,
		                                                   InputIt last,
		                                                   ForwardIt s_first,
		                                                   ForwardIt s_last,
		                                                   BinaryPredicate p ) {
			static_assert(
			  traits::is_binary_prdicate_v<
			    BinaryPredicate,
			    typename std::iterator_traits<InputIt>::value_type,
			    typename std::iterator_traits<InputIt>::value_type>,
			  "BinaryPredicate p does not fulfill the requires of a binary "
			  "predicate concept.  See "
			  "http://en.cppreference.com/w/cpp/concept/BinaryPredicate" );
			bool found = false;
			for( ; first != last; ++first ) {
				found = false;
				for( ForwardIt it = s_first; it != s_last; ++it ) {
					if( p( *first, *it ) ) {
						found = true;
						break;
					}
				}
				if( !found ) {
					return first;
				}
			}
			return last;
		}

		template<typename CharT>
		[[nodiscard]] constexpr int
		compare( CharT const *l_ptr, CharT const *r_ptr, std::size_t sz ) {
			return std::char_traits<CharT>::compare( l_ptr, r_ptr, sz );
		}

		template<typename InputIt, typename UnaryPredicate>
		[[nodiscard]] constexpr InputIt
		find_first_of_if( InputIt first, InputIt last, UnaryPredicate p ) {
			static_assert(
			  traits::is_unary_prdicate_v<
			    UnaryPredicate,
			    typename std::iterator_traits<InputIt>::value_type>,
			  "UnaryPredicate p does not fullfill the requires of a unary predicate "
			  "concept.  See "
			  "http://en.cppreference.com/w/cpp/concept/Predicate" );
			for( ; first != last; ++first ) {
				if( p( *first ) ) {
					return first;
				}
			}
			return last;
		}

		template<typename InputIt, typename UnaryPredicate>
		[[nodiscard]] constexpr InputIt
		find_first_not_of_if( InputIt first, InputIt last, UnaryPredicate p ) {
			static_assert(
			  traits::is_unary_prdicate_v<
			    UnaryPredicate,
			    typename std::iterator_traits<InputIt>::value_type>,
			  "UnaryPredicate p does not fullfill the requires of a unary predicate "
			  "concept.  See "
			  "http://en.cppreference.com/w/cpp/concept/Predicate" );
			for( ; first != last; ++first ) {
				if( !p( *first ) ) {
					return first;
				}
			}
			return last;
		}

		template<typename OStream,
		         std::enable_if_t<daw::traits::is_ostream_like_lite_v<OStream>,
		                          std::nullptr_t> = nullptr>
		inline void sv_insert_fill_chars( OStream &os, std::size_t n ) {
			using CharT = typename OStream::char_type;
			static_assert( traits::has_write_member_v<OStream, CharT>,
			               "OStream Must has write member" );

			std::array<CharT, 8> fill_chars = { 0 };
			fill_chars.fill( os.fill( ) );

			for( ; n >= fill_chars.size( ) and os.good( ); n -= fill_chars.size( ) ) {
				os.write( fill_chars.data( ),
				          static_cast<intmax_t>( fill_chars.size( ) ) );
			}
			if( n > 0 and os.good( ) ) {
				os.write( fill_chars.data( ), static_cast<intmax_t>( n ) );
			}
		}

		template<typename OStream,
		         typename CharT,
		         typename Bounds,
		         std::ptrdiff_t Ex,
		         std::enable_if_t<daw::traits::is_ostream_like_v<OStream, CharT>,
		                          std::nullptr_t> = nullptr>
		void
		sv_insert_aligned( OStream &os,
		                   daw::sv1::basic_string_view<CharT, Bounds, Ex> str ) {
			auto const size = str.size( );
			auto const alignment_size =
			  static_cast<std::size_t>( os.width( ) ) - size;
			bool const align_left =
			  ( os.flags( ) & OStream::adjustfield ) == OStream::left;
			if( !align_left ) {
				sv_insert_fill_chars( os, alignment_size );
				if( os.good( ) ) {
					os.write( str.data( ), static_cast<intmax_t>( size ) );
				}
			} else {
				os.write( str.data( ), static_cast<intmax_t>( size ) );
				if( os.good( ) ) {
					sv_insert_fill_chars( os, alignment_size );
				}
			}
		}

		template<typename CharT,
		         size_t Capacity,
		         typename OStream,
		         std::enable_if_t<daw::traits::is_ostream_like_v<OStream, CharT>,
		                          std::nullptr_t> = nullptr>
		void
		sv_insert_aligned( OStream &os,
		                   daw::basic_bounded_string<CharT, Capacity> const &str ) {

			auto const size = str.size( );
			auto const alignment_size =
			  static_cast<std::size_t>( os.width( ) ) - size;
			bool const align_left =
			  ( os.flags( ) & OStream::adjustfield ) == OStream::left;
			if( !align_left ) {
				sv_insert_fill_chars( os, alignment_size );
				if( os.good( ) ) {
					os.write( str.data( ), static_cast<intmax_t>( size ) );
				}
			} else {
				os.write( str.data( ), static_cast<intmax_t>( size ) );
				if( os.good( ) ) {
					sv_insert_fill_chars( os, alignment_size );
				}
			}
		}

		// Make argument a lower priority than T[]
		template<typename T,
		         std::enable_if_t<std::is_pointer_v<T>, std::nullptr_t> = nullptr>
		struct only_ptr {
			T *ptr;

			constexpr only_ptr( T *p ) noexcept
			  : ptr{ p } {}

			constexpr operator T *( ) const {
				return ptr;
			}
		};
	} // namespace details
} // namespace daw
