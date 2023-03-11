// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/string_view
//

#pragma once

#include "daw_string_view2_fwd.h"

#include "ciso646.h"
#include "cpp_20.h"
#include "daw_algorithm.h"
#include "daw_assume.h"
#include "daw_check_exceptions.h"
#include "daw_compiler_fixups.h"
#include "daw_consteval.h"
#include "daw_cpp_feature_check.h"
#include "daw_fnv1a_hash.h"
#include "daw_generic_hash.h"
#include "daw_likely.h"
#include "daw_logic.h"
#include "daw_math.h"
#include "daw_move.h"
#include "daw_string_view_version.h"
#include "daw_swap.h"
#include "daw_traits.h"
#include "impl/daw_view_tags.h"
#include "iterator/daw_back_inserter.h"
#include "iterator/daw_iterator.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <type_traits>

#if defined( DAW_NO_STRING_VIEW_DBG_CHECK )
#define DAW_STRING_VIEW_DBG_RNG_CHECK( Bool, ... ) \
	do {                                             \
	} while( false )
#elif not defined( NDEBUG ) or defined( DEBUG )
#define DAW_STRING_VIEW_DBG_RNG_CHECK( Bool, ... )            \
	if( DAW_UNLIKELY( not( Bool ) ) ) {                         \
		DAW_THROW_OR_TERMINATE( std::out_of_range, __VA_ARGS__ ); \
	}                                                           \
	do {                                                        \
	} while( false )
#else
#define DAW_STRING_VIEW_DBG_RNG_CHECK( Bool, ... ) DAW_ASSUME( Bool )
#endif

#if not defined( DAW_NO_STRING_VIEW_PRECOND_CHECKS )
#define DAW_STRING_VIEW_PRECOND_CHECK( Bool, ... ) \
	if( DAW_UNLIKELY( not( Bool ) ) ) {              \
		std::terminate( );                             \
	}                                                \
	do {                                             \
	} while( false )

#define DAW_STRING_VIEW_RNG_CHECK( Bool, ... )                \
	if( DAW_UNLIKELY( not( Bool ) ) ) {                         \
		DAW_THROW_OR_TERMINATE( std::out_of_range, __VA_ARGS__ ); \
	}                                                           \
	do {                                                        \
	} while( false )
#else
#define DAW_STRING_VIEW_PRECOND_CHECK( Bool, ... ) \
	do {                                             \
	} while( false )

#define DAW_STRING_VIEW_RNG_CHECK( Bool, ... ) \
	do {                                         \
	} while( false )
#endif

/// @brief Require Pred to be a Unary Predicate
/// @param Pred Unary predicate
/// @param Type Parameter type of predicate
#define DAW_REQ_UNARY_PRED( Pred, Type )                                       \
	std::enable_if_t<traits::is_unary_predicate_v<Pred, Type>, std::nullptr_t> = \
	  nullptr

/// @brief Require a contiguous character range
/// @param Range contiguous range
/// @param CharT character type of range elements
#define DAW_REQ_CONTIG_CHAR_RANGE( Range, CharT )                        \
	std::enable_if_t<(sv2_details::is_string_view_like_v<Range, CharT> and \
	                  not std::is_convertible_v<Range, char const *>),     \
	                 std::nullptr_t> = nullptr

/// @brief Require a character pointer
/// @param Pointer a const_pointer
#define DAW_REQ_CHAR_PTR( Pointer, CharT )                         \
	std::enable_if_t<                                                \
	  (std::is_same_v<CharT *, daw::remove_cvref_t<Pointer>> or      \
	   std::is_same_v<CharT const *, daw::remove_cvref_t<Pointer>>), \
	  std::nullptr_t> = nullptr

/// @brief Require Type be constructable from a CharT const * and a size_type
/// @param Type A type to construct from a pointer/size_type pair
#define DAW_REQ_CONTIG_CHAR_RANGE_CTOR( Type )                          \
	std::enable_if_t<                                                     \
	  sv2_details::is_contigious_range_constructible<Type, CharT>::value, \
	  std::nullptr_t> = nullptr

DAW_UNSAFE_BUFFER_FUNC_START

namespace daw {
#if DAW_STRINGVIEW_VERSION == 2
	inline
#endif
	  namespace sv2 {

		namespace sv2_details {
			template<typename InputIt, typename ForwardIt, typename BinaryPredicate>
			[[nodiscard]] constexpr InputIt
			find_first_of( InputIt first, InputIt last, ForwardIt s_first,
			               ForwardIt s_last, BinaryPredicate p ) {
				static_assert(
				  traits::is_binary_predicate_v<
				    BinaryPredicate,
				    typename std::iterator_traits<InputIt>::value_type>,
				  "BinaryPredicate p does not fullfill the requires of a binary "
				  "predicate concept.  See "
				  "http://en.cppreference.com/w/cpp/concept/BinaryPredicate" );

				for( ; first != last; ++first ) {
					for( ForwardIt it = s_first; it != s_last; ++it ) {
						if( p( *first, *it ) ) {
							return first;
						}
					}
				}

				return last;
			}

			template<typename InputIt, typename UnaryPredicate>
			[[nodiscard]] constexpr InputIt
			find_first_of_if( InputIt first, InputIt last, UnaryPredicate p ) {
				static_assert(
				  traits::is_unary_predicate_v<
				    UnaryPredicate, typename std::iterator_traits<InputIt>::value_type>,
				  "UnaryPredicate p does not fullfill the requires of a unary "
				  "predicate "
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
				  traits::is_unary_predicate_v<
				    UnaryPredicate, typename std::iterator_traits<InputIt>::value_type>,
				  "UnaryPredicate p does not fullfill the requires of a unary "
				  "predicate "
				  "concept.  See "
				  "http://en.cppreference.com/w/cpp/concept/Predicate" );

				for( ; first != last; ++first ) {
					if( not p( *first ) ) {
						return first;
					}
				}

				return last;
			}

			template<typename InputIt, typename ForwardIt, typename BinaryPredicate>
			[[nodiscard]] constexpr InputIt
			find_first_not_of( InputIt first, InputIt last, ForwardIt s_first,
			                   ForwardIt s_last, BinaryPredicate p ) {
				static_assert(
				  traits::is_binary_predicate_v<
				    BinaryPredicate,
				    typename std::iterator_traits<InputIt>::value_type>,
				  "BinaryPredicate p does not fullfill the requires of a binary "
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
					if( not found ) {
						return first;
					}
				}

				return last;
			}

			template<typename CharT>
			[[nodiscard]] constexpr int
			compare( CharT const *l_ptr, CharT const *r_ptr, std::size_t sz ) {
				auto const last = l_ptr + static_cast<std::ptrdiff_t>( sz );
				while( l_ptr != last ) {
					auto const diff = *l_ptr - *r_ptr;
					if( diff < 0 ) {
						return -1;
					}
					if( diff > 0 ) {
						return 1;
					}

					++l_ptr;
					++r_ptr;
				}
				return 0;
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

			template<class ForwardIt1, class ForwardIt2>
			[[nodiscard]] constexpr ForwardIt1
			search( ForwardIt1 first, ForwardIt1 last, ForwardIt2 s_first,
			        ForwardIt2 s_last ) {
				// TODO: This is a terrible detection, but not sure what to use yet as
				// it's generally available everywhere but windows
#if not defined( _WIN32 )
				if constexpr( std::is_convertible_v<ForwardIt1, char const *> and
				              std::is_convertible_v<ForwardIt2, char const *> ) {
					if( not DAW_IS_CONSTANT_EVALUATED( ) ) {
						void *result =
						  memmem( first, last - first, s_first, s_last - s_first );
						if( result == nullptr ) {
							return last;
						}
						return static_cast<ForwardIt1>( result );
					}
				}
#endif
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

			template<typename CharT, string_view_bounds_type Bounds,
			         string_view_bounds_type Bounds2>
			[[nodiscard]] constexpr CharT const *search(
			  basic_string_view<CharT, Bounds> haystack,
			  std::initializer_list<basic_string_view<CharT, Bounds2>> needles ) {
				auto const last = daw::data_end( haystack );
				for( ; not haystack.empty( ); haystack.remove_prefix( ) ) {
					auto it = haystack.data( );

					for( auto needle : needles ) {
						if( needle.size( ) > haystack.size( ) ) {}
						auto s_first = std::data( needle );
						auto const s_last = daw::data_end( needle );
						for( auto s_it = s_first;; ++it, ++s_it ) {
							if( s_it == s_last ) {
								return haystack.data( );
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
				return data_end( haystack );
			}

			template<typename T>
			constexpr std::size_t find_predicate_result_size( T const & ) {
				return 1;
			}

			template<typename T>
			using is_sv2_test = typename T::i_am_a_daw_string_view2;

			template<typename T>
			using is_sv2_t = daw::is_detected<is_sv2_test, T>;

			template<typename T, typename CharT>
			using is_string_view_like_test =
			  decltype( (void)( std::declval<std::size_t &>( ) =
			                      std::size( std::declval<T>( ) ) ),
			            (void)( std::declval<CharT const *&>( ) =
			                      std::data( std::declval<T>( ) ) ) );

			template<typename T, typename CharT>
			inline constexpr bool is_string_view_like_v =
			  daw::is_detected_v<is_string_view_like_test, T, CharT> and
			  daw::not_trait<is_sv2_t<T>>::value;

			template<typename T, typename CharT>
			struct is_contigious_range_constructible
			  : std::is_constructible<T, CharT *, std::size_t> {};

			template<typename T, typename PointerType>
			inline constexpr bool is_char_pointer_v =
			  std::is_pointer_v<T> and std::is_convertible_v<T, PointerType>;

			template<typename T>
			struct equal_t {
				explicit DAW_CONSTEVAL equal_t( int ) {}

				DAW_ATTRIB_INLINE constexpr bool operator( )( T l,
				                                              T r ) const noexcept {
					return l == r;
				}
			};

			template<typename CharT>
			inline constexpr auto bp_eq = equal_t<CharT>( 42 );
		} // namespace sv2_details

		/// @brief The class template basic_string_view describes an object that can
		/// refer to a constant contiguous sequence of char-like objects with the
		/// first element of the sequence at position zero.
		template<typename CharT, string_view_bounds_type BoundsType>
		struct basic_string_view {
			using value_type = CharT;
			using pointer = CharT *;
			using const_pointer = std::add_const_t<CharT> *;
			using reference = std::add_lvalue_reference_t<CharT>;
			using const_reference =
			  std::add_lvalue_reference_t<std::add_const_t<CharT>>;
			using const_iterator = const_pointer;
			using iterator = const_iterator;
			using reverse_iterator = std::reverse_iterator<iterator>;
			using const_reverse_iterator = std::reverse_iterator<const_iterator>;
			using size_type = std::size_t;
			using difference_type = std::ptrdiff_t;

			/// @brief A tag type for detecting string_view
			using i_am_a_daw_string_view2 = void;

		private:
			template<string_view_bounds_type B>
			using is_last_a_pointer =
			  std::bool_constant<B == string_view_bounds_type::pointer>;

			using last_type = std::conditional_t<is_last_a_pointer<BoundsType>::value,
			                                     const_pointer, size_type>;

			using default_last_t = std::integral_constant<last_type, last_type{ }>;

			using last_difference_type =
			  std::conditional_t<is_last_a_pointer<BoundsType>::value,
			                     difference_type, size_type>;

			template<string_view_bounds_type Bounds, typename LastType>
			DAW_ATTRIB_INLINE static constexpr last_type
			make_last( const_pointer f, LastType l ) noexcept {
				if constexpr( std::is_pointer_v<LastType> ) {
					if constexpr( is_last_a_pointer<Bounds>::value ) {
						(void)f;
						return l;
					} else {
						return static_cast<last_type>( l - f );
					}
				} else {
					if constexpr( is_last_a_pointer<Bounds>::value ) {

						return f + static_cast<difference_type>( l );

					} else {
						(void)f;
						return static_cast<size_type>( l );
					}
				}
			}

			template<string_view_bounds_type Bounds>
			DAW_ATTRIB_INLINE constexpr const_pointer last_pointer( ) const {
				if constexpr( is_last_a_pointer<Bounds>::value ) {
					return m_last;
				} else {
					return m_first + static_cast<difference_type>( m_last );
				}
			}

			template<string_view_bounds_type Bounds>
			DAW_ATTRIB_INLINE static constexpr size_type size( const_pointer f,
			                                                   last_type l ) {
				if constexpr( is_last_a_pointer<Bounds>::value ) {

					return static_cast<size_type>( l - f );

				} else {
					(void)f;
					return l;
				}
			}

			template<string_view_bounds_type Bounds>
			DAW_ATTRIB_INLINE constexpr void dec_front( size_type n ) {

				m_first += static_cast<difference_type>( n );

				if constexpr( not is_last_a_pointer<Bounds>::value ) {
					m_last -= n;
				}
			}

			template<string_view_bounds_type Bounds>
			DAW_ATTRIB_INLINE constexpr void dec_back( size_type n ) {

				if constexpr( is_last_a_pointer<Bounds>::value ) {
					m_last -= static_cast<difference_type>( n );
				} else {
					m_last -= n;
				}
			}

			template<string_view_bounds_type Bounds>
			DAW_ATTRIB_INLINE constexpr void inc_front( size_type n ) {

				m_first -= static_cast<difference_type>( n );
				if constexpr( not is_last_a_pointer<Bounds>::value ) {
					m_last += n;
				}
			}

			template<string_view_bounds_type Bounds>
			DAW_ATTRIB_INLINE constexpr void inc_back( size_type n ) {

				if constexpr( is_last_a_pointer<Bounds>::value ) {
					m_last += static_cast<difference_type>( n );
				} else {
					m_last += n;
				}
			}

			const_pointer m_first = nullptr;
			last_type m_last = default_last_t::value;

		public:
			static constexpr size_type npos =
			  ( std::numeric_limits<size_type>::max )( );

			//******************************
			// Constructors
			//******************************

			/// @brief Construct an empty string_view
			/// @post data( ) == nullptr
			/// @post size( ) == 0
			basic_string_view( ) = default;

			/// @brief Construct an empty string_view
			/// @post data( ) == nullptr
			/// @post size( ) == 0
			DAW_ATTRIB_INLINE constexpr basic_string_view( std::nullptr_t ) noexcept
			  : m_first( nullptr )
			  , m_last( make_last<BoundsType>( nullptr, nullptr ) ) {}

			/// @brief Prevents nullptr literals and a size_type to construct a
			/// string_view
			DAW_ATTRIB_INLINE constexpr basic_string_view( std::nullptr_t,
			                                               size_type n )
			  : m_first( nullptr )
			  , m_last( make_last<BoundsType>( nullptr, nullptr ) ) {
				DAW_STRING_VIEW_PRECOND_CHECK( n == 0,
				                               "nullptr can only form an empty range" );
			}

			/// @brief Construct a string_view
			/// @param s Pointer to start of character range
			/// @param count Size of character range
			/// @post data( ) == s
			/// @post size( ) == count
			DAW_ATTRIB_INLINE constexpr basic_string_view( const_pointer s,
			                                               size_type count ) noexcept
			  : m_first( s )
			  , m_last( make_last<BoundsType>( s, count ) ) {}

			/// @brief Construct a string_view with a range starting with s
			/// @param s Start of character range
			/// @pre s == nullptr, or s beings a valid character range that
			/// has a CharT{} terminated sentinel
			/// @post data( ) == s
			/// @post size( ) == strlen( s ) or CharT{} if s == nullptr
			template<typename CharPtr, DAW_REQ_CHAR_PTR( CharPtr, CharT )>
			constexpr basic_string_view( CharPtr s ) noexcept
			  : m_first( s )
			  , m_last(
			      make_last<BoundsType>( s, sv2_details::strlen<size_type>( s ) ) ) {}

			/// @brief Construct a string_view from a type that forms a
			/// contiguous range of characters
			/// @param sv A valid contiguous character range
			/// @post data( ) == std::data( sv )
			/// @post size( ) == std::size( sv )
			template<typename StringView,
			         DAW_REQ_CONTIG_CHAR_RANGE( StringView, CharT )>
			DAW_ATTRIB_INLINE constexpr basic_string_view( StringView &&sv ) noexcept
			  : m_first( std::data( sv ) )
			  , m_last( make_last<BoundsType>( m_first, std::size( sv ) ) ) {}

			/// @brief Construct a string_view from a type that forms a
			/// contiguous range of characters
			/// @param sv Other string_view
			/// @param count Maximum number of characters to use in range
			/// formed by sv
			/// @post data( ) == sv.data( )
			/// @post size( ) == min( count, sv.size( ) )
			template<typename StringView,
			         DAW_REQ_CONTIG_CHAR_RANGE( StringView, CharT )>
			DAW_ATTRIB_INLINE constexpr basic_string_view( StringView &&sv,
			                                               size_type count ) noexcept
			  : m_first( std::data( sv ) )
			  , m_last( make_last<BoundsType>(
			      m_first, ( std::min )( std::size( sv ), count ) ) ) {}

			/// @brief Construct a string_view from a type that forms a
			/// contiguous range of characters. Does not clip count to sv's bounds
			/// @param sv Other string_view
			/// @param count Maximum number of characters to use in range
			/// formed by sv
			/// @pre count <= sv.size( )
			/// @post data( ) == sv.data( )
			/// @post size( ) == min( count, sv.size( ) )
			template<typename StringView,
			         DAW_REQ_CONTIG_CHAR_RANGE( StringView, CharT )>
			DAW_ATTRIB_INLINE constexpr basic_string_view(
			  StringView &&sv, size_type count, dont_clip_to_bounds_t ) noexcept
			  : m_first( std::data( sv ) )
			  , m_last( make_last<BoundsType>( m_first, count ) ) {
				DAW_STRING_VIEW_DBG_RNG_CHECK(
				  std::size( sv ) >= count,
				  "Attempt to pop back more elements that are available" );
			}

			/// @brief Construct a string_view from a character array. Assumes
			/// a string literal like array with last element having a value
			/// of CharT{}
			/// @param string_literal A valid contiguous character range
			/// @post data( ) == std::data( string_literal )
			/// @post size( ) == std::size( string_literal ) - 1
			template<std::size_t N>
			DAW_ATTRIB_INLINE constexpr basic_string_view(
			  CharT const ( &string_literal )[N] ) noexcept
			  : m_first( string_literal )
			  , m_last( make_last<BoundsType>( string_literal, N - 1 ) ) {
				static_assert( N > 0 );
				if( string_literal[N - 1] != '\0' ) {
					m_last = make_last<BoundsType>( string_literal, N );
				}
			}

			/// @brief Construct a string_view from a range formed by two
			/// character pointers
			/// @param first Start of character range
			/// @param last One past last element in range
			/// @pre [first, last) form a valid character range
			/// @post data( ) == first
			/// @post size( ) == last - first
			template<typename CharPtr1, typename CharPtr2,
			         DAW_REQ_CHAR_PTR( CharPtr1, CharT ),
			         DAW_REQ_CHAR_PTR( CharPtr2, CharT )>
			constexpr basic_string_view( CharPtr1 &&first, CharPtr2 &&last ) noexcept
			  : m_first( first )
			  , m_last( make_last<BoundsType>( first, last ) ) {}

			//******************************
			// Conversions
			//******************************

			/// @brief Convert to a contiguous range type
			/// @pre T{ data( ), size( ) } is valid
			template<typename T, DAW_REQ_CONTIG_CHAR_RANGE_CTOR( T )>
			explicit constexpr operator T( ) const
			  noexcept( std::is_nothrow_constructible_v<T, CharT *, size_type> ) {
				return T{ data( ), size( ) };
			}

			//******************************
			// Iterator Support
			//******************************

			/// @brief Returns an iterator to the first character of the view.
			/// @return const_iterator to the first character
			[[nodiscard]] constexpr const_iterator begin( ) const {
				return m_first;
			}

			/// @brief Returns an iterator to the first character of the view.
			/// @return const_iterator to the first character
			[[nodiscard]] constexpr const_iterator cbegin( ) const {
				return m_first;
			}

			/// @brief Returns a reverse iterator to the first character of the
			/// reversed view. It corresponds to the last character of the
			/// non-reversed view.
			/// @return const_reverse_iterator to the first character
			[[nodiscard]] constexpr reverse_iterator rbegin( ) const {
				return const_reverse_iterator( end( ) );
			}

			/// @brief Returns a reverse iterator to the first character of the
			/// reversed view. It corresponds to the last character of the
			/// non-reversed view.
			/// @return const_reverse_iterator to the first character
			[[nodiscard]] constexpr const_reverse_iterator crbegin( ) const {
				return const_reverse_iterator( cend( ) );
			}

			/// @brief Returns an iterator to the character following the last
			/// character of the view. This character acts as a placeholder,
			/// attempting to access it results in undefined behavior
			/// @return const_iterator to the character following the last character.
			[[nodiscard]] constexpr const_iterator end( ) const {
				return last_pointer<BoundsType>( );
			}

			/// @brief Returns an iterator to the character following the last
			/// character of the view. This character acts as a placeholder,
			/// attempting to access it results in undefined behavior
			/// @return const_iterator to the character following the last character.
			[[nodiscard]] constexpr const_iterator cend( ) const {
				return last_pointer<BoundsType>( );
			}

			/// @brief Returns a reverse iterator to the character following the last
			/// character of the reversed view. It corresponds to the character
			/// preceding the first character of the non-reversed view. This character
			/// acts as a placeholder, attempting to access it results in undefined
			/// behavior.
			/// @return const_reverse_iterator to the character following the last
			/// character.
			[[nodiscard]] constexpr reverse_iterator rend( ) const {
				return const_reverse_iterator( begin( ) );
			}

			/// @brief Returns a reverse iterator to the character following the last
			/// character of the reversed view. It corresponds to the character
			/// preceding the first character of the non-reversed view. This character
			/// acts as a placeholder, attempting to access it results in undefined
			/// behavior.
			/// @return const_reverse_iterator to the character following the last
			/// character.
			[[nodiscard]] constexpr const_reverse_iterator crend( ) const {
				return const_reverse_iterator( cbegin( ) );
			}

			//******************************
			// Capacity
			//******************************

			/// @brief Returns the number of CharT elements in the view
			/// @return The number of CharT elements in the view.
			[[nodiscard]] constexpr size_type size( ) const {
				return size<BoundsType>( m_first, m_last );
			}

			/// @brief Returns the number of CharT elements in the view
			/// @return The number of CharT elements in the view.
			[[nodiscard]] constexpr size_type length( ) const {
				return size( );
			}

			/// @brief Checks if the view has no characters
			/// @return true if the view is empty, false otherwise
			[[nodiscard]] constexpr bool empty( ) const {
				return size( ) == 0;
			}

			//******************************
			// Element Access
			//******************************

			/// @brief Returns a pointer to the underlying character range
			/// corresponding to the values of the view.
			/// @return A pointer to the underlying character range
			[[nodiscard]] constexpr const_pointer data( ) const {
				return m_first;
			}

			/// @brief Returns a pointer to the character following the last
			/// character of the view. This character acts as a placeholder,
			/// attempting to access it may result in undefined behavior
			/// @return pointer to the character following the last character.
			[[nodiscard]] constexpr const_pointer data_end( ) const {
				return end( );
			}

			/// Access to the elements of range
			/// @param pos Position in range
			/// @pre size( ) > pos
			/// @pre data( ) != nullptr
			/// @return data( )[pos]
			[[nodiscard]] DAW_ATTRIB_INLINE constexpr const_reference
			operator[]( size_type pos ) const {
				DAW_STRING_VIEW_DBG_RNG_CHECK(
				  pos < size( ), "Attempt to access basic_string_view past end" );

				return m_first[pos];
			}

			/// Access to the elements of range
			/// @param pos Position in range
			/// @throws std::out_of_range when pos >= size( )
			/// @return data( )[pos]
			[[nodiscard]] constexpr const_reference at( size_type pos ) const {
				if( DAW_UNLIKELY( not( pos < size( ) ) ) ) {
					DAW_THROW_OR_TERMINATE(
					  std::out_of_range, "Attempt to access basic_string_view past end" );
				}
				return operator[]( pos );
			}

			/// @brief Access the first element in range
			/// @pre data( ) != nullptr
			/// @pre size( ) > 0
			[[nodiscard]] constexpr const_reference front( ) const {
				DAW_STRING_VIEW_DBG_RNG_CHECK(
				  not empty( ),
				  "Attempt to reference an element of an empty basic_string_view" );
				return *m_first;
			}

			/// @brief Access the last element in range
			/// @pre data( ) != nullptr
			/// @pre size( ) > 0
			[[nodiscard]] constexpr const_reference back( ) const {
				DAW_STRING_VIEW_DBG_RNG_CHECK(
				  not empty( ),
				  "Attempt to reference an element of an empty basic_string_view" );
				return *std::prev( end( ) );
			}
			//******************************
			// Modifiers
			//******************************

			/// @brief Empty the range
			/// @post size( ) == 0
			constexpr void clear( ) {
				m_last = make_last<BoundsType>( nullptr, size_type{ 0 } );
			}

			/// @brief Reset the range to a default constructed state
			/// @post data( ) == nullptr
			/// @post size( ) == 0
			constexpr void reset( ) {
				m_first = nullptr;
				m_last = make_last<BoundsType>( nullptr, size_type{ 0 } );
			}

			/// @brief Increment the data( ) pointer by n. If string_view is
			/// empty, it does nothing.
			DAW_ATTRIB_INLINE constexpr basic_string_view &
			remove_prefix( size_type n ) {
				dec_front<BoundsType>( ( std::min )( n, size( ) ) );
				return *this;
			}

			/// @brief Increment the data( ) pointer by n. If string_view is
			/// empty, it does nothing.
			/// @pre n <= size( )
			DAW_ATTRIB_INLINE constexpr basic_string_view &
			remove_prefix( size_type n, dont_clip_to_bounds_t ) {
				DAW_STRING_VIEW_DBG_RNG_CHECK(
				  size( ) >= n,
				  "Attempt to remove prefix too many elements in a basic_string_view" );
				dec_front<BoundsType>( n );
				return *this;
			}

			/// @brief Increment the data( ) pointer by 1. If string_view is
			/// empty, it does nothing.
			DAW_ATTRIB_INLINE constexpr basic_string_view &remove_prefix( ) {
				dec_front<BoundsType>( ( std::min )( size_type{ 1U }, size( ) ) );
				return *this;
			}

			/// @brief Increment the data( ) pointer by 1.
			/// @pre size( ) >= 1
			DAW_ATTRIB_INLINE constexpr basic_string_view &
			remove_prefix( dont_clip_to_bounds_t ) {
				DAW_STRING_VIEW_DBG_RNG_CHECK(
				  size( ) >= 1,
				  "Attempt to remove prefix too many elements in a basic_string_view" );
				dec_front<BoundsType>( size_type{ 1U } );
				return *this;
			}

			/// @brief Decrement the size( ) by n. If string_view is empty, it
			/// does nothing.
			DAW_ATTRIB_INLINE constexpr basic_string_view &
			remove_suffix( size_type n ) {
				dec_back<BoundsType>( ( std::min )( n, size( ) ) );
				return *this;
			}

			/// @brief Decrement the size( ) by n.
			/// @pre n <= size( )
			DAW_ATTRIB_INLINE constexpr basic_string_view &
			remove_suffix( size_type n, dont_clip_to_bounds_t ) {
				DAW_STRING_VIEW_DBG_RNG_CHECK(
				  size( ) >= n,
				  "Attempt to remove suffix too many elements in a basic_string_view" );
				dec_back<BoundsType>( n );
				return *this;
			}

			/// @brief Decrement the size( ) by 1 if size( ) > 0
			DAW_ATTRIB_INLINE constexpr basic_string_view &remove_suffix( ) {
				dec_back<BoundsType>( ( std::min )( size_type{ 1U }, size( ) ) );
				return *this;
			}

			/// @brief Decrement the size( ) by 1
			/// @pre not empty( )
			DAW_ATTRIB_INLINE constexpr basic_string_view &
			remove_suffix( dont_clip_to_bounds_t ) {
				DAW_STRING_VIEW_DBG_RNG_CHECK(
				  size( ) >= 1, "Attempt to remove suffix an empty basic_string_view" );
				dec_back<BoundsType>( size_type{ 1U } );
				return *this;
			}

			/// @brief Increment the data( ) pointer by 1.
			/// @return front( ) prior to increment
			[[nodiscard]] constexpr CharT pop_front( ) {
				auto result = front( );
				remove_prefix( 1U );
				return result;
			}

			/// @brief Increment the data( ) pointer by 1. Does not check bounds
			/// @return front( ) prior to increment
			[[nodiscard]] constexpr CharT pop_front( dont_clip_to_bounds_t ) {
				DAW_STRING_VIEW_DBG_RNG_CHECK(
				  size( ) >= 1, "Attempt to pop front an empty basic_string_view" );
				auto result = front( );
				remove_prefix( 1U, dont_clip_to_bounds );
				return result;
			}

			/// @brief Increment data( ) by count elements and return a new
			/// string_view that would be formed formed from the previous and
			/// current data( ) pointer values.
			/// @param count number of characters to increment data( ) by
			/// @return a new string_view of size count.
			[[nodiscard]] constexpr basic_string_view pop_front( size_type count ) {
				basic_string_view result = substr( 0, count );
				remove_prefix( count );
				return result;
			}

			/// @brief Increment data( ) by count elements and return a new
			/// string_view that would be formed formed from the previous and
			/// current data( ) pointer values.
			/// @param count number of characters to increment data( ) by
			/// @return a new string_view of size count.
			[[nodiscard]] constexpr basic_string_view
			pop_front( size_type count, dont_clip_to_bounds_t ) {
				DAW_STRING_VIEW_DBG_RNG_CHECK(
				  size( ) >= count,
				  "Attempt to pop front too many elements basic_string_view" );
				basic_string_view result = substr( 0, count, dont_clip_to_bounds );
				remove_prefix( count, dont_clip_to_bounds );
				return result;
			}

			/// @brief Increment data( ) until the substring where is found and return
			/// a new string_view that would be formed formed from the previous and
			/// current data( ) pointer values.
			/// @param where substring to search for
			/// @return If where is found, a new string_view from the old data( )
			/// position up to the position of the leading character in where.  If
			/// where is not found, a copy of the string_view is made
			[[nodiscard]] constexpr basic_string_view
			pop_front_until( basic_string_view where, nodiscard_t ) {
				auto pos = find( where );
				auto result = pop_front( pos );
				return result;
			}

			/// @brief Searches for where, returns substring between front and
			/// where, then pops off the substring
			/// @param where string to split on and remove from front
			/// @return substring from beginning to where string
			[[nodiscard]] constexpr basic_string_view pop_front_until( CharT where,
			                                                           nodiscard_t ) {
				auto pos = find( where );
				auto result = pop_front( pos );
				return result;
			}

			/// @brief Searches for the first non match of where, returns substring
			/// between front and where, then pops off the substring
			/// @param where The pattern to extract
			/// @return substring from beginning to first non-match of where string
			[[nodiscard]] constexpr basic_string_view pop_front_while( CharT where ) {
				auto pos = find_first_not_of( where );
				auto result = pop_front( pos );
				return result;
			}

			/// @brief Searches for where, returns substring between front and
			/// where, then pops off the substring and the where string
			/// @param where string to split on and remove from front
			/// @return substring from beginning to where string
			[[nodiscard]] constexpr basic_string_view
			pop_front_until( basic_string_view where ) {
				auto pos = find( where );
				auto result = pop_front( pos );
				remove_prefix( where.size( ) );
				return result;
			}

			/// @brief Searches for where, returns substring between front and
			/// where, then pops off the substring and the where string
			/// @param where string to split on and remove from front
			/// @return substring from beginning to where string
			[[nodiscard]] constexpr basic_string_view pop_front_until( CharT where ) {
				auto pos = find( where );
				auto result = pop_front( pos );
				remove_prefix( );
				return result;
			}

			/// @brief Increment data until the predicate is true or the string_view
			/// is empty.  Return a new string_view formed from the range of the
			/// previous value of data( ) and the position where predicate was true.
			/// @tparam UnaryPredicate A predicate taking a single CharT as parameter
			/// @param pred A predicate that returns true at the end of the newly
			/// created range.
			/// @return substring from beginning to position marked by
			/// predicate
			/// @post data( ) is set to position where the predicate was true, or one
			/// past the end of the range
			template<typename UnaryPredicate,
			         DAW_REQ_UNARY_PRED( UnaryPredicate, CharT )>
			[[nodiscard]] constexpr basic_string_view
			pop_front_until( UnaryPredicate pred, nodiscard_t ) {

				auto pos = find_first_of_if( DAW_MOVE( pred ) );
				return pop_front( pos );
			}

			/// @brief Increment data until the predicate is false or the string_view
			/// is empty.  Return a new string_view formed from the range of the
			/// previous value of data( ) and the position where predicate was false.
			/// @tparam UnaryPredicate A predicate taking a single CharT as parameter
			/// @param pred A predicate that returns false at the end of the newly
			/// created range.
			/// @return substring from beginning to position marked by
			/// predicate
			/// @post data( ) is set to position where the predicate was false, or one
			/// past the end of the range
			template<typename UnaryPredicate,
			         DAW_REQ_UNARY_PRED( UnaryPredicate, CharT )>
			[[nodiscard]] constexpr basic_string_view
			pop_front_while( UnaryPredicate pred ) {

				auto pos = find_first_not_of_if( DAW_MOVE( pred ) );
				return pop_front( pos );
			}

			/// @brief Increment data until the predicate is true or the string_view
			/// is empty.  Return a new string_view formed from the range of the
			/// previous value of data( ) and the position where predicate was true.
			/// @tparam UnaryPredicate A predicate taking a single CharT as parameter
			/// @param pred A predicate that returns true at the end of the newly
			/// created range.
			/// @return substring from beginning to position marked by
			/// predicate
			/// @post data( ) is set to one past position where the predicate was
			/// true, or one past the end of the range
			template<typename UnaryPredicate,
			         DAW_REQ_UNARY_PRED( UnaryPredicate, CharT )>
			[[nodiscard]] constexpr basic_string_view
			pop_front_until( UnaryPredicate pred ) {
				auto result = pop_front_until( pred, nodiscard );
				remove_prefix( sv2_details::find_predicate_result_size( pred ) );
				return result;
			}
			//*****************************************************************************
			//*****************************************************************************
			//*****************************************************************************

			/// @brief Return the last character and decrement the size by 1 unless
			/// empty
			[[nodiscard]] constexpr CharT pop_back( ) {
				auto result = back( );
				remove_suffix( );
				return result;
			}

			/// @brief Return the last character and decrement the size by 1
			/// @pre not empty( )
			[[nodiscard]] constexpr CharT pop_back( dont_clip_to_bounds_t ) {
				DAW_STRING_VIEW_DBG_RNG_CHECK(
				  size( ) >= 1,
				  "Attempt to pop back more elements that are available" );
				auto result = back( );
				remove_suffix( dont_clip_to_bounds );
				return result;
			}

			/// @brief create a substr of the last count characters and remove
			/// them from end
			/// @param count number of characters to remove and return
			/// @return a substr of size count ending at end of string_view
			[[nodiscard]] constexpr basic_string_view pop_back( size_type count ) {
				count = ( std::min )( count, size( ) );
				basic_string_view result = substr( size( ) - count, npos );
				remove_suffix( count );
				return result;
			}

			/// @brief create a substr of the last count characters and remove
			/// them from end
			/// @param count number of characters to remove and return
			/// @pre count <= size( )
			/// @return a substr of size count ending at end of string_view
			[[nodiscard]] constexpr basic_string_view
			pop_back( size_type count, dont_clip_to_bounds_t ) {
				DAW_STRING_VIEW_DBG_RNG_CHECK(
				  size( ) >= count,
				  "Attempt to pop back more elements that are available" );
				basic_string_view result = substr( size( ) - count, npos );
				remove_suffix( count );
				return result;
			}

			/// @brief searches for last where, returns substring between
			/// where and end, then pops off the substring
			/// @param where string to split on and remove from back
			/// @return substring from end of where string to end of string
			[[nodiscard]] constexpr basic_string_view
			pop_back_until( basic_string_view where, nodiscard_t ) {
				auto pos = rfind( where );
				if( pos == npos ) {
					auto result{ *this };
					remove_prefix( npos );
					return result;
				}
				auto result = substr( pos + where.size( ) );
				remove_suffix( size( ) - pos );
				return result;
			}

			/// @brief searches for last where, returns substring between
			/// where and end, then pops off the substring
			/// @param where string to split on and remove from back
			/// @return substring from end of where string to end of string
			[[nodiscard]] constexpr basic_string_view pop_back_until( CharT where,
			                                                          nodiscard_t ) {
				auto pos = rfind( where );
				if( pos == npos ) {
					auto result{ *this };
					remove_prefix( npos );
					return result;
				}
				auto result = substr( pos + where.size( ) );
				remove_suffix( ( size( ) - pos ) - 1 );
				return result;
			}

			/// @brief searches for last where, returns substring between
			/// where and end, then pops off the substring and the where
			/// string
			/// @param where string to split on and remove from back
			/// @return substring from end of where string to end of string
			[[nodiscard]] constexpr basic_string_view
			pop_back_until( basic_string_view where ) {
				auto pos = rfind( where );
				if( pos == npos ) {
					auto result{ *this };
					remove_prefix( npos );
					return result;
				}
				auto result = substr( pos + where.size( ) );
				remove_suffix( size( ) - pos );
				return result;
			}

			/// @brief searches for the last where, returns substring between
			/// where and end, then pops off the substring and the where
			/// string
			/// @param where CharT to split string on and remove from back
			/// @return substring from end of where string to end of string
			[[nodiscard]] constexpr basic_string_view pop_back_until( CharT where ) {
				auto pos = rfind( where );
				if( pos == npos ) {
					auto result{ *this };
					remove_prefix( npos );
					return result;
				}
				auto result = substr( pos + 1 );
				remove_suffix( size( ) - pos );
				return result;
			}

			/// @brief searches for last position UnaryPredicate would be
			/// true, returns substring between pred and end, then pops off
			/// the substring and the pred specified string
			/// @tparam UnaryPredicate a unary predicate type that accepts a
			/// char and indicates with true when to stop
			/// @param pred predicate to determine where to split
			/// @return substring from last position marked by predicate to
			/// end
			template<typename UnaryPredicate,
			         DAW_REQ_UNARY_PRED( UnaryPredicate, CharT )>
			[[nodiscard]] constexpr basic_string_view
			pop_back_until( UnaryPredicate pred ) {

				auto pos = find_last_of_if( DAW_MOVE( pred ) );
				if( pos == npos ) {
					auto result = *this;
					remove_prefix( npos );
					return result;
				}
				auto result =
				  substr( pos + sv2_details::find_predicate_result_size( pred ) );
				remove_suffix( size( ) - pos );
				return result;
			}
			/// @brief searches for last position UnaryPredicate would be
			/// true, returns substring between pred and end, then pops off
			/// the substring and the pred specified string
			/// @tparam UnaryPredicate a unary predicate type that accepts a
			/// char and indicates with true when to stop
			/// @param pred predicate to determine where to split
			/// @return substring from last position marked by predicate to
			/// end
			template<typename UnaryPredicate,
			         DAW_REQ_UNARY_PRED( UnaryPredicate, CharT )>
			[[nodiscard]] constexpr basic_string_view
			pop_back_until( UnaryPredicate pred, nodiscard_t ) {

				auto pos = find_last_of_if( DAW_MOVE( pred ) );
				if( pos == npos ) {
					auto result = *this;
					remove_prefix( npos );
					return result;
				}
				auto result = substr( pos );
				remove_suffix( size( ) - pos );
				return result;
			}

			/// @brief searches for where, returns substring between front and
			/// where, then pops off the substring and the where string. Do
			/// nothing if where is not found
			/// @param where string to split on and remove from front
			/// @return substring from beginning to where string
			[[nodiscard]] constexpr basic_string_view
			try_pop_front_until( basic_string_view where ) {
				auto pos = find( where );
				if( pos == npos ) {
					return basic_string_view<CharT, BoundsType>( );
				}
				auto result = pop_front( pos );
				remove_prefix( where.size( ) );
				return result;
			}

			/// @brief Increment data( ) until the substring where is found and return
			/// a new string_view that would be formed formed from the previous and
			/// current data( ) pointer values. Does nothing if where is not found
			/// @param where substring to search for
			/// @return If where is found, a new string_view from the old data( )
			/// position up to the position of the leading character in where.  If
			/// where is not found, a copy of the string_view is made
			[[nodiscard]] constexpr basic_string_view
			try_pop_front_until( basic_string_view where, nodiscard_t ) {
				auto pos = find( where );
				if( pos == npos ) {
					return basic_string_view<CharT, BoundsType>( );
				}
				auto result = pop_front( pos );
				return result;
			}

			/// @brief Searches for where, returns substring between front and
			/// where, then pops off the substring.  Does nothing if where is not
			/// found
			/// @param where string to split on and remove from front
			/// @return substring from beginning to where string
			[[nodiscard]] constexpr basic_string_view
			try_pop_front_until( CharT where, nodiscard_t ) {
				auto pos = find( where );
				if( pos == npos ) {
					return basic_string_view<CharT, BoundsType>( );
				}
				auto result = pop_front( pos );
				return result;
			}

			/// @brief Searches for where, returns substring between front and
			/// where, then pops off the substring and the where char. Does nothing if
			/// where is not found
			/// @param where string to split on and remove from front
			/// @return substring from beginning to where string
			[[nodiscard]] constexpr basic_string_view
			try_pop_front_until( CharT where ) {
				auto pos = find( where );
				if( pos == npos ) {
					return basic_string_view<CharT, BoundsType>( );
				}
				auto result = pop_front( pos );
				remove_prefix( );
				return result;
			}

			/// @brief Increment data until the predicate is true or the string_view
			/// is empty.  Return a new string_view formed from the range of the
			/// previous value of data( ) and the position where predicate was true.
			/// Does nothing if predicate is not found
			/// @tparam UnaryPredicate A predicate taking a single CharT as parameter
			/// @param pred A predicate that returns true at the end of the newly
			/// created range.
			/// @return substring from beginning to position marked by
			/// predicate
			/// @post data( ) is set to position where the predicate was true, or one
			/// past the end of the range
			template<typename UnaryPredicate,
			         DAW_REQ_UNARY_PRED( UnaryPredicate, CharT )>
			[[nodiscard]] constexpr basic_string_view
			try_pop_front_until( UnaryPredicate pred, nodiscard_t ) {

				auto pos = find_first_of_if( DAW_MOVE( pred ) );
				if( pos == npos ) {
					return basic_string_view<CharT, BoundsType>( );
				}
				return pop_front( pos );
			}

			/// @brief Increment data until the predicate is true or the string_view
			/// is empty.  Return a new string_view formed from the range of the
			/// previous value of data( ) and the position where predicate was true.
			/// Does nothing if predicate is not found
			/// @tparam UnaryPredicate A predicate taking a single CharT as parameter
			/// @param pred A predicate that returns true at the end of the newly
			/// created range.
			/// @return substring from beginning to position marked by
			/// predicate
			/// @post data( ) is set to one past position where the predicate was
			/// true, or one past the end of the range
			template<typename UnaryPredicate,
			         DAW_REQ_UNARY_PRED( UnaryPredicate, CharT )>
			[[nodiscard]] constexpr basic_string_view
			try_pop_front_until( UnaryPredicate pred ) {
				auto result = try_pop_front_until( pred, nodiscard );
				remove_prefix( sv2_details::find_predicate_result_size( pred ) );
				return result;
			}

			//////////////////////////////////
			/// @brief searches for last where, returns substring between
			/// where and end, then pops off the substring and the where
			/// string.  If where is not found, nothing is done
			/// @param where string to split on and remove from back
			/// @return substring from end of where string to end of string
			[[nodiscard]] constexpr basic_string_view
			try_pop_back_until( basic_string_view where ) {
				auto pos = rfind( where );
				if( pos == npos ) {
					return basic_string_view<CharT, BoundsType>( );
				}
				auto result = substr( pos + where.size( ) );
				remove_suffix( size( ) - pos );
				return result;
			}

			/// @brief searches for last where, returns substring between
			/// where and end, then pops off the substring. Does nothing if where is
			/// not found
			/// @param where string to split on and remove from back
			/// @return substring from end of where string to end of string
			[[nodiscard]] constexpr basic_string_view
			try_pop_back_until( basic_string_view where, nodiscard_t ) {
				auto pos = rfind( where );
				if( pos == npos ) {
					return basic_string_view<CharT, BoundsType>( );
				}
				auto result = substr( pos + where.size( ) );
				remove_suffix( size( ) - pos );
				return result;
			}

			/// @brief searches for last where, returns substring between
			/// where and end, then pops off the substring. Does nothing if where is
			/// not found
			/// @param where string to split on and remove from back
			/// @return substring from end of where string to end of string
			[[nodiscard]] constexpr basic_string_view
			try_pop_back_until( CharT where, nodiscard_t ) {
				auto pos = rfind( where );
				if( pos == npos ) {
					return basic_string_view<CharT, BoundsType>( );
				}
				auto result = substr( pos );
				remove_suffix( ( size( ) - pos ) - 1 );
				return result;
			}

			/// @brief searches for the last where, returns substring between
			/// where and end, then pops off the substring and the where
			/// string. Does nothing if where is not found
			/// @param where CharT to split string on and remove from back
			/// @return substring from end of where string to end of string
			[[nodiscard]] constexpr basic_string_view
			try_pop_back_until( CharT where ) {
				auto pos = rfind( where );
				if( pos == npos ) {
					return basic_string_view<CharT, BoundsType>( );
				}
				auto result = substr( pos );
				remove_suffix( size( ) - pos );
				return result;
			}

			/// @brief searches for last position UnaryPredicate would be
			/// true, returns substring between pred and end, then pops off
			/// the substring and the pred specified string. Does nothing if predicate
			/// is not found
			/// @tparam UnaryPredicate a unary predicate type that accepts a
			/// char and indicates with true when to stop
			/// @param pred predicate to determine where to split
			/// @return substring from last position marked by predicate to
			/// end
			template<typename UnaryPredicate,
			         DAW_REQ_UNARY_PRED( UnaryPredicate, CharT )>
			[[nodiscard]] constexpr basic_string_view
			try_pop_back_until( UnaryPredicate pred ) {

				auto pos = find_last_of_if( DAW_MOVE( pred ) );
				if( pos == npos ) {
					return basic_string_view<CharT, BoundsType>( );
				}
				auto result = substr( pos );
				remove_suffix( size( ) - pos );
				return result;
			}

			/// @brief searches for last position UnaryPredicate would be
			/// true, returns substring between pred and end, then pops off
			/// the substring. Does nothing if predicate is not found
			/// @tparam UnaryPredicate a unary predicate type that accepts a
			/// char and indicates with true when to stop
			/// @param pred predicate to determine where to split
			/// @return substring from last position marked by predicate to
			/// end
			template<typename UnaryPredicate,
			         DAW_REQ_UNARY_PRED( UnaryPredicate, CharT )>
			[[nodiscard]] constexpr basic_string_view
			try_pop_back_until( UnaryPredicate pred, nodiscard_t ) {

				auto pos = find_last_of_if( DAW_MOVE( pred ) );
				if( pos == npos ) {
					return basic_string_view<CharT, BoundsType>( );
				}
				auto result = substr( pos );
				remove_suffix( ( size( ) - pos - 1 ) );
				return result;
			}

			/// @brief searches for where, and disregards everything until the
			/// end of that
			/// @param where character to find and consume
			/// @return substring with everything up until the end of where
			/// removed
			DAW_ATTRIB_INLINE constexpr basic_string_view &
			remove_prefix_until( CharT where ) {
				auto pos = find( where );
				remove_prefix( pos );
				remove_prefix( );
				return *this;
			}

			/// @brief searches for where, and disregards everything prior to
			/// where
			/// @param where character to find
			/// @return substring with everything up until where removed
			DAW_ATTRIB_INLINE constexpr basic_string_view &
			remove_prefix_until( CharT where, nodiscard_t ) {
				auto pos = find( where );
				remove_prefix( pos );
				return *this;
			}

			/// @brief searches for where, and disregards everything until the
			/// end of that
			/// @param where string to find and consume
			/// @return substring with everything up until the end of where
			/// removed
			DAW_ATTRIB_INLINE constexpr basic_string_view &
			remove_prefix_until( basic_string_view where ) {
				auto pos = find( where );
				remove_prefix( pos );
				remove_prefix( where.size( ) );
				return *this;
			}

			/// @brief searches for where, and disregards everything prior to
			/// where
			/// @param where string to find and consume
			/// @return substring with everything up until the start of where
			/// removed
			DAW_ATTRIB_INLINE constexpr basic_string_view &
			remove_prefix_until( basic_string_view where, nodiscard_t ) {
				auto pos = find( where );
				remove_prefix( pos );
				return *this;
			}

			/// @brief Removes all elements until pred is true or end reached
			/// @tparam UnaryPredicate a unary predicate type that accepts a char and
			/// indicates with true when to stop
			/// @param pred Predicate object
			/// @return A reference to the current string_view object
			template<typename UnaryPredicate,
			         DAW_REQ_UNARY_PRED( UnaryPredicate, CharT )>
			DAW_ATTRIB_INLINE constexpr basic_string_view &
			remove_prefix_until( UnaryPredicate pred ) {
				auto pos = find_first_of_if( pred );
				remove_prefix( pos );
				remove_prefix( sv2_details::find_predicate_result_size( pred ) );
				return *this;
			}

			/// @brief removes all elements prior to predicate returning true
			/// @param pred predicate that takes a CharT
			/// @return substring with everything up until predicate is
			/// removed
			template<typename UnaryPredicate,
			         DAW_REQ_UNARY_PRED( UnaryPredicate, CharT )>
			DAW_ATTRIB_INLINE constexpr basic_string_view &
			remove_prefix_until( UnaryPredicate pred, nodiscard_t ) {
				auto pos = find_if( pred );
				dec_front<BoundsType>( ( std::min )( size( ), pos ) );
				return *this;
			}

			/// @brief Set the size of the range
			/// @param new_size New size of range
			/// @pre size( ) >= new_size
			/// @post size( ) == new_size
			constexpr void resize( size_type new_size ) {
				DAW_STRING_VIEW_DBG_RNG_CHECK( new_size <= size( ),
				                               "newsize is larger than size( )" );
				m_last = make_last<BoundsType>(
				  m_first,
				  std::next( m_first, static_cast<difference_type>( new_size ) ) );
			}

			/// @brief Copy the character range [data( ) + pos, data( ) + pos + count)
			/// @param dest pointer to buffer
			/// @param count maximum number of elements to copy
			/// @param pos starting position
			/// @pre pos <= size( )
			/// @return number of characters copied
			constexpr size_type copy( pointer dest, size_type count,
			                          size_type pos ) const {
				DAW_STRING_VIEW_DBG_RNG_CHECK(
				  pos <= size( ), "Attempt to access basic_string_view past end" );

				size_type const rlen = ( std::min )( count, size( ) - pos );
				if( rlen > 0 ) {
					auto const f =
					  std::next( begin( ), static_cast<difference_type>( pos ) );
					auto const l = std::next( f, static_cast<difference_type>( rlen ) );
					daw::algorithm::copy( f, l, dest );
				}
				return rlen;
			}

			constexpr size_type copy( pointer dest, size_type count ) const {
				return copy( dest, count, 0 );
			}

			/// @brief Create a new sub-range basic_string_view [data( ) + pos, data(
			/// ) + min( size( ), pos + count) )
			/// @param pos Starting position
			/// @param count Maximum number of characters to copy
			/// @pre pos <= size( )
			/// @returns a new basic_string_view of the sub-range
			[[nodiscard]] constexpr basic_string_view
			substr( size_type pos, size_type count ) const {
				DAW_STRING_VIEW_DBG_RNG_CHECK(
				  pos <= size( ), "Attempt to access basic_string_view past end" );
				auto const rcount =
				  static_cast<size_type>( ( std::min )( count, size( ) - pos ) );
				return { m_first + pos, m_first + pos + rcount };
			}

			/// @brief Create a new sub-range basic_string_view [data( ) + pos, data(
			/// ) + min( size( ), pos + count) )
			/// @param pos Starting position
			/// @param count Maximum number of characters to copy
			/// @returns a new basic_string_view of the sub-range
			/// @pre pos + count <= size( )
			[[nodiscard]] constexpr basic_string_view
			substr( size_type pos, size_type count, dont_clip_to_bounds_t ) const {
				DAW_STRING_VIEW_DBG_RNG_CHECK(
				  pos + count <= size( ),
				  "Attempt to access basic_string_view past end" );
				return { m_first + pos, m_first + pos + count };
			}

			/// @brief Return a copy of the string_view
			[[nodiscard]] constexpr basic_string_view substr( ) const {
				return substr( 0, size( ), dont_clip_to_bounds );
			}

			/// @brief Create a new sub-range basic_string_view [data( ) + pos, data()
			/// + (size( ) - pos)
			/// @param pos Starting position
			/// @returns a new basic_string_view of the sub-range
			/// @pre pos <= size( )
			[[nodiscard]] constexpr basic_string_view substr( size_type pos ) const {
				return substr( pos, npos );
			}

			/// @brief Create a new sub-range basic_string_view [data( ) + pos, data()
			/// + (size( ) - pos). Does not check bounds
			/// @param pos Starting position
			/// @returns a new basic_string_view of the sub-range
			/// @pre pos <= size( )
			[[nodiscard]] constexpr basic_string_view
			substr( size_type pos, dont_clip_to_bounds_t ) const {
				DAW_STRING_VIEW_DBG_RNG_CHECK(
				  pos <= size( ), "Attempt to access basic_string_view past end" );
				return substr( pos, size( ) - pos, dont_clip_to_bounds );
			}

		public:
			template<typename Compare = std::less<void>, string_view_bounds_type BL,
			         string_view_bounds_type BR>
			[[nodiscard]] static constexpr int
			compare( basic_string_view<CharT, BL> lhs,
			         basic_string_view<CharT, BR> rhs, Compare cmp = Compare{ } ) {
				constexpr auto const str_compare = []( CharT const *p0, CharT const *p1,
				                                       size_type len, Compare &c ) {
					auto const last = p0 + len;
					while( p0 != last ) {
						if( c( *p0, *p1 ) ) {
							return -1;
						}
						if( c( *p1, *p0 ) ) {
							return 1;
						}

						++p0;
						++p1;
					}
					return 0;
				};

				int const ret =
				  str_compare( lhs.data( ), rhs.data( ),
				               ( std::min )( lhs.size( ), rhs.size( ) ), cmp );
				if( ret == 0 ) {
					if( lhs.size( ) < rhs.size( ) ) {
						return -1;
					}
					if( rhs.size( ) < lhs.size( ) ) {
						return 1;
					}
				}
				return ret;
			}

			template<typename Compare = std::less<>, string_view_bounds_type B>
			[[nodiscard]] constexpr int compare( basic_string_view<CharT, B> rhs,
			                                     Compare cmp = Compare{ } ) const {
				return compare(
				  *this, basic_string_view( std::data( rhs ), std::size( rhs ) ), cmp );
			}

			template<typename Compare = std::less<>, typename StringView,
			         DAW_REQ_CONTIG_CHAR_RANGE( StringView, CharT )>
			[[nodiscard]] constexpr int compare( StringView &&rhs,
			                                     Compare cmp = Compare{ } ) const {
				return compare(
				  *this, basic_string_view( std::data( rhs ), std::size( rhs ) ), cmp );
			}

			template<typename Compare = std::less<>, std::size_t N>
			[[nodiscard]] constexpr int compare( CharT const ( &rhs )[N],
			                                     Compare cmp = Compare{ } ) const {
				return compare( *this, basic_string_view( rhs, N - 1 ), cmp );
			}

			template<typename Compare = std::less<>>
			constexpr int compare( size_type pos1, size_type count1,
			                       basic_string_view v,
			                       Compare cmp = Compare{ } ) const {
				return compare( substr( pos1, count1 ), v, cmp );
			}

			template<typename Compare = std::less<>, string_view_bounds_type Bounds>
			[[nodiscard]] constexpr int compare( size_type pos1, size_type count1,
			                                     basic_string_view<CharT, Bounds> v,
			                                     size_type pos2, size_type count2,
			                                     Compare cmp = Compare{ } ) const {
				return compare( substr( pos1, count1 ), v.substr( pos2, count2 ), cmp );
			}

			template<typename Compare = std::less<>>
			[[nodiscard]] constexpr int compare( size_type pos1, size_type count1,
			                                     const_pointer s,
			                                     Compare cmp = Compare{ } ) const {
				return compare( substr( pos1, count1 ),
				                basic_string_view<CharT, BoundsType>( s ), cmp );
			}

			template<typename Compare = std::less<>>
			[[nodiscard]] constexpr int compare( size_type pos1, size_type count1,
			                                     const_pointer s, size_type count2,
			                                     Compare cmp = Compare{ } ) const {
				return compare( substr( pos1, count1 ),
				                basic_string_view<CharT, BoundsType>( s, count2 ),
				                cmp );
			}

			template<string_view_bounds_type Bounds>
			[[nodiscard]] constexpr size_type
			find( basic_string_view<CharT, Bounds> v, size_type pos ) const {

				if( size( ) < v.size( ) ) {
					return npos;
				}
				if( v.empty( ) ) {
					return pos;
				}
				auto result =
				  sv2_details::search( begin( ) + pos, end( ), v.begin( ), v.end( ) );
				if( end( ) == result ) {
					return npos;
				}
				return static_cast<size_type>( result - begin( ) );
			}

			[[nodiscard]] constexpr size_type
			find_first_match( std::initializer_list<basic_string_view> needles,
			                  size_type pos ) const {

				for( auto needle : needles ) {
					if( needle.empty( ) ) {
						return pos;
					}
				}

				auto result = sv2_details::search( substr( pos ), needles );
				if( data_end( ) == result ) {
					return npos;
				}
				return static_cast<size_type>( result - data( ) );
			}

			[[nodiscard]] constexpr size_type find_first_match(
			  std::initializer_list<basic_string_view> needles ) const {
				return find_first_match( needles, 0 );
			}

			template<string_view_bounds_type Bounds>
			[[nodiscard]] constexpr size_type
			find( basic_string_view<CharT, Bounds> v ) const {
				return find( v, 0 );
			}

			[[nodiscard]] constexpr size_type find( CharT c, size_type pos ) const {
				assert( pos <= size( ) );
				auto first = data( ) + pos;
				auto const sz = static_cast<std::size_t>( data_end( ) - first );
				for( std::size_t n = pos; n < sz; ++n ) {

					char const test_c = first[n];

					if( test_c == c ) {
						return n;
					}
				}
				return npos;
			}

			[[nodiscard]] constexpr size_type find( CharT c ) const {
				return find( c, 0 );
			}

			[[nodiscard]] constexpr size_type find( const_pointer s, size_type pos,
			                                        size_type count ) const {
				assert( pos <= size( ) );
				return find( basic_string_view<CharT, BoundsType>( s, count ), pos );
			}

			[[nodiscard]] constexpr size_type find( const_pointer s,
			                                        size_type pos ) const {
				assert( pos <= size( ) );
				return find( basic_string_view<CharT, BoundsType>( s ), pos );
			}

			[[nodiscard]] constexpr size_type find( const_pointer s ) const {
				return find( basic_string_view<CharT, BoundsType>( s ), 0 );
			}

			/// @brief Test if the string_view contains the supplied pattern
			template<string_view_bounds_type Bounds>
			[[nodiscard]] constexpr bool
			contains( basic_string_view<CharT, Bounds> v ) const {
				return find( v ) != npos;
			}

			/// @brief Test if the string_view contains the supplied pattern
			[[nodiscard]] constexpr bool contains( CharT c, size_type pos ) const {
				return find( c, pos ) != npos;
			}

			/// @brief Test if the string_view contains the supplied pattern
			[[nodiscard]] constexpr bool contains( CharT c ) const {
				return find( c, 0 );
			}

			/// @brief Test if the string_view contains the supplied pattern
			[[nodiscard]] constexpr bool contains( const_pointer s, size_type pos,
			                                       size_type count ) const {
				return find( basic_string_view<CharT, BoundsType>( s, count ), pos );
			}

			/// @brief Test if the string_view contains the supplied pattern
			[[nodiscard]] constexpr bool contains( const_pointer s,
			                                       size_type pos ) const {
				return find( s, pos ) != npos;
			}

			/// @brief Test if the string_view contains the supplied pattern
			[[nodiscard]] constexpr bool contains( const_pointer s ) const {
				return find( s ) != npos;
			}

			template<string_view_bounds_type Bounds>
			[[nodiscard]] constexpr size_type
			find_first_match( basic_string_view<CharT, Bounds> v,
			                  size_type pos ) const {

				if( size( ) < v.size( ) ) {
					return npos;
				}
				if( v.empty( ) ) {
					return pos;
				}
				auto result =
				  sv2_details::search( begin( ) + pos, end( ), v.begin( ), v.end( ) );
				if( end( ) == result ) {
					return npos;
				}
				return static_cast<size_type>( result - begin( ) );
			}

			/// @brief Reverse find substring v in [data( ) + pos, data( ) + size( ) )
			/// @param v substring to search for
			/// @param pos starting position
			/// @returns starting position of substring or npos if not found
			template<string_view_bounds_type Bounds>
			[[nodiscard]] constexpr size_type
			rfind( basic_string_view<CharT, Bounds> v, size_type pos ) const {

				if( size( ) < v.size( ) ) {
					return npos;
				}
				pos = ( std::min )( pos, size( ) - v.size( ) );
				if( v.empty( ) ) {
					return pos;
				}
				const_iterator cur =
				  std::next( begin( ), static_cast<difference_type>( pos ) );
				while( true ) {
					if( sv2_details::compare( cur, v.begin( ), v.size( ) ) == 0 ) {
						return static_cast<size_type>( cur - begin( ) );
					}
					if( cur == begin( ) ) {
						return npos;
					}

					--cur;
				}
			}

			/// @brief Find the position of the last substring equal to [s, s + count)
			/// in [data( ) + pos, data( ) + size( ) )
			/// @param s start of substring to search for
			/// @param pos starting position
			/// @param count size of substring
			/// @returns starting position of substring or npos if not found
			[[nodiscard]] constexpr size_type rfind( const_pointer s, size_type pos,
			                                         size_type count ) const {
				return rfind( basic_string_view<CharT, BoundsType>( s, count ), pos );
			}

			/// @brief Find the position of the last substring in [data( ), data( ) +
			/// size( ) )
			/// @param v substring to search for
			/// @returns starting position of substring or npos if not found
			template<string_view_bounds_type Bounds>
			[[nodiscard]] constexpr size_type
			rfind( basic_string_view<CharT, Bounds> v ) const {
				return rfind( v, npos );
			}

			/// @brief find the last position of character in [data( ) + pos, data( )
			/// + size( ))
			/// @param c Character to search for
			/// @param pos starting position
			/// @returns position of found character or npos
			[[nodiscard]] constexpr size_type rfind( CharT c, size_type pos ) const {
				return rfind(
				  basic_string_view<CharT, BoundsType>( std::addressof( c ), 1 ), pos );
			}

			/// @brief find the last position of character in [data( ), data( ) +
			/// size( ))
			/// @param c Character to search for
			/// @returns position of found character or npos
			[[nodiscard]] constexpr size_type rfind( CharT c ) const {
				return rfind(
				  basic_string_view<CharT, BoundsType>( std::addressof( c ), 1 ),
				  npos );
			}

			/// @brief find the last position of character in [data( ) + pos, data( )
			/// + size( ))
			/// @param s substring to search for
			/// @param pos starting position
			/// @pre s is zero terminated
			/// @returns position of found character or npos
			[[nodiscard]] constexpr size_type rfind( const_pointer s,
			                                         size_type pos ) const {
				return rfind( basic_string_view<CharT, BoundsType>( s ), pos );
			}

			/// @brief find the last position of character in [data( ), data( ) +
			/// size( ))
			/// @param s substring to search for
			/// @pre s is zero terminated
			/// @returns position of found character or npos
			[[nodiscard]] constexpr size_type rfind( const_pointer s ) const {
				return rfind( basic_string_view<CharT, BoundsType>( s ), npos );
			}

			/// Find the first item in v that is in string from pos
			/// \param v A range of characters to look for
			/// \param pos Starting position to start searching
			/// \return position of first item in v or npos
			template<string_view_bounds_type Bounds>
			[[nodiscard]] constexpr size_type
			find_first_of( basic_string_view<CharT, Bounds> v, size_type pos ) const {
				if( pos >= size( ) or v.empty( ) ) {
					return npos;
				}
				auto const iter =
				  sv2_details::find_first_of( begin( ) + pos, end( ), v.begin( ),
				                              v.end( ), sv2_details::bp_eq<CharT> );

				if( end( ) == iter ) {
					return npos;
				}
				return static_cast<size_type>( std::distance( begin( ), iter ) );
			}

			/// Find the first item in v that is in string from beginning
			/// \param v A range of characters to look for
			/// \return position of first item in v or npos
			template<string_view_bounds_type Bounds>
			[[nodiscard]] constexpr size_type
			find_first_of( basic_string_view<CharT, Bounds> v ) const {
				return find_first_of( v, 0 );
			}

			[[nodiscard]] constexpr size_type find_first_of( const_pointer str,
			                                                 size_type pos ) const {
				return find_first_of( basic_string_view<CharT, BoundsType>( str ),
				                      pos );
			}

			[[nodiscard]] constexpr size_type
			find_first_of( const_pointer str ) const {
				return find_first_of( basic_string_view<CharT, BoundsType>( str ), 0 );
			}

			template<string_view_bounds_type Bounds>
			[[nodiscard]] constexpr size_type
			search( basic_string_view<CharT, Bounds> v, size_type pos ) const {
				if( pos + v.size( ) >= size( ) or v.empty( ) ) {
					return npos;
				}
				auto const iter = sv2_details::search( data( ) + pos, data_end( ),
				                                       v.data( ), v.data_end( ) );
				if( data_end( ) == iter ) {
					return npos;
				}
				return static_cast<size_type>( std::distance( data( ), iter ) );
			}

			template<string_view_bounds_type Bounds>
			[[nodiscard]] constexpr size_type
			search( basic_string_view<CharT, Bounds> v ) const {
				return search( v, 0 );
			}

			[[nodiscard]] constexpr size_t search( const_pointer str,
			                                       size_type pos ) const {
				return search( basic_string_view<CharT, BoundsType>( str ), pos );
			}

			[[nodiscard]] constexpr size_t search( const_pointer str ) const {
				return search( basic_string_view<CharT, BoundsType>( str ), 0 );
			}

			template<string_view_bounds_type Bounds>
			[[nodiscard]] constexpr size_type
			search_last( basic_string_view<CharT, Bounds> v, size_type pos ) const {
				if( pos + v.size( ) >= size( ) or v.empty( ) ) {
					return npos;
				}
				auto last_pos = pos;
				auto fpos = search( v, pos );
				while( fpos != npos ) {
					last_pos = fpos;
					fpos = search( v, fpos );
					if( fpos == last_pos ) {
						break;
					}
				}
				return last_pos;
			}

			template<string_view_bounds_type Bounds>
			[[nodiscard]] constexpr size_type
			search_last( basic_string_view<CharT, Bounds> v ) const {
				return search_last( v, 0 );
			}

			[[nodiscard]] constexpr size_t search_last( const_pointer str,
			                                            size_type pos ) const {
				return search_last( basic_string_view<CharT, BoundsType>( str ), pos );
			}

			[[nodiscard]] constexpr size_t search_last( const_pointer str ) const {
				return search_last( basic_string_view<CharT, BoundsType>( str ), 0 );
			}

			template<typename UnaryPredicate,
			         DAW_REQ_UNARY_PRED( UnaryPredicate, CharT )>
			[[nodiscard]] constexpr size_type
			find_first_of_if( UnaryPredicate pred, size_type pos ) const {

				(void)traits::is_unary_predicate_test<UnaryPredicate, CharT>( );

				if( pos >= size( ) ) {
					return npos;
				}
				auto const iter =
				  sv2_details::find_first_of_if( cbegin( ) + pos, cend( ), pred );
				if( cend( ) == iter ) {
					return npos;
				}
				return static_cast<size_type>( iter - cbegin( ) );
			}

			template<typename UnaryPredicate,
			         DAW_REQ_UNARY_PRED( UnaryPredicate, CharT )>
			[[nodiscard]] constexpr size_type
			find_first_of_if( UnaryPredicate pred ) const {
				return find_first_of_if( pred, 0 );
			}

			template<typename UnaryPredicate,
			         DAW_REQ_UNARY_PRED( UnaryPredicate, CharT )>
			[[nodiscard]] constexpr size_type
			find_first_not_of_if( UnaryPredicate pred, size_type pos ) const {

				traits::is_unary_predicate_test<UnaryPredicate, CharT>( );

				if( pos >= size( ) ) {
					return npos;
				}

				auto const iter =
				  sv2_details::find_first_not_of_if( begin( ) + pos, end( ), pred );
				if( end( ) == iter ) {
					return npos;
				}
				return static_cast<size_type>( std::distance( begin( ), iter ) );
			}

			template<typename UnaryPredicate,
			         DAW_REQ_UNARY_PRED( UnaryPredicate, CharT )>
			[[nodiscard]] constexpr size_type
			find_first_not_of_if( UnaryPredicate pred ) const {
				return find_first_not_of_if( pred, 0 );
			}

			[[nodiscard]] constexpr size_type find_first_of( CharT c,
			                                                 size_type pos ) const {
				return find_first_of(
				  basic_string_view<CharT, BoundsType>( std::addressof( c ), 1U ),
				  pos );
			}

			[[nodiscard]] constexpr size_type find_first_of( CharT c ) const {
				return find_first_of(
				  basic_string_view<CharT, BoundsType>( std::addressof( c ), 1U ), 0 );
			}

			[[nodiscard]] constexpr size_type
			find_first_of( const_pointer s, size_type pos, size_type count ) const {
				return find_first_of( basic_string_view<CharT, BoundsType>( s, count ),
				                      pos );
			}

		private:
			[[nodiscard]] constexpr size_type
			reverse_distance( const_reverse_iterator first,
			                  const_reverse_iterator last ) const {
				// Portability note here: std::distance is not NOEXCEPT, but
				// calling it with a string_view::reverse_iterator will not
				// throw.
				return ( size( ) - 1u ) -
				       static_cast<size_t>( std::distance( first, last ) );
			}

		public:
			template<string_view_bounds_type Bounds>
			[[nodiscard]] constexpr size_type
			find_last_of( basic_string_view<CharT, Bounds> s, size_type pos ) const {
				if( s.empty( ) ) {
					return npos;
				}
				if( pos >= size( ) ) {
					pos = 0;
				} else {
					pos = size( ) - ( pos + 1U );
				}
				auto haystack = substr( pos );
				auto iter = daw::algorithm::find_first_of(
				  haystack.rbegin( ), haystack.rend( ), s.rbegin( ), s.rend( ) );
				return iter == rend( ) ? npos : reverse_distance( rbegin( ), iter );
			}

			template<string_view_bounds_type Bounds>
			[[nodiscard]] constexpr size_type
			find_last_of( basic_string_view<CharT, Bounds> s ) const {
				return find_last_of( s, npos );
			}

			[[nodiscard]] constexpr size_type find_last_of( CharT c,
			                                                size_type pos ) const {
				return find_last_of( basic_string_view( std::addressof( c ), 1 ), pos );
			}

			[[nodiscard]] constexpr size_type find_last_of( CharT c ) const {
				return find_last_of( basic_string_view( std::addressof( c ), 1 ),
				                     npos );
			}

			template<size_type N>
			[[nodiscard]] constexpr size_type find_last_of( CharT const ( &s )[N],
			                                                size_type pos ) {
				return find_last_of( basic_string_view<CharT, BoundsType>( s, N - 1 ),
				                     pos );
			}

			template<size_type N>
			[[nodiscard]] constexpr size_type find_last_of( CharT const ( &s )[N] ) {
				return find_last_of( basic_string_view<CharT, BoundsType>( s, N - 1 ),
				                     npos );
			}

			[[nodiscard]] constexpr size_type
			find_last_of( const_pointer s, size_type pos, size_type count ) const {
				return find_last_of( basic_string_view<CharT, BoundsType>( s, count ),
				                     pos );
			}

			[[nodiscard]] constexpr size_type find_last_of( const_pointer s,
			                                                size_type pos ) const {
				return find_last_of( basic_string_view<CharT, BoundsType>( s ), pos );
			}

			[[nodiscard]] constexpr size_type find_last_of( const_pointer s ) const {
				return find_last_of( basic_string_view<CharT, BoundsType>( s ), npos );
			}

			template<typename UnaryPredicate,
			         DAW_REQ_UNARY_PRED( UnaryPredicate, CharT )>
			[[nodiscard]] constexpr size_type find_last_of_if( UnaryPredicate pred,
			                                                   size_type pos ) const {

				(void)traits::is_unary_predicate_test<UnaryPredicate, CharT>( );

				auto haystack = substr( 0, pos );
				auto iter = daw::algorithm::find_if( haystack.crbegin( ),
				                                     haystack.crend( ), pred );
				return iter == crend( ) ? npos : reverse_distance( crbegin( ), iter );
			}

			template<typename UnaryPredicate,
			         DAW_REQ_UNARY_PRED( UnaryPredicate, CharT )>
			[[nodiscard]] constexpr size_type
			find_last_of_if( UnaryPredicate pred ) const {
				return find_last_of_if( pred, npos );
			}

			template<string_view_bounds_type Bounds>
			[[nodiscard]] constexpr size_type
			find_first_not_of( basic_string_view<CharT, Bounds> v,
			                   size_type pos ) const {
				if( pos >= size( ) ) {
					return npos;
				}
				if( v.empty( ) ) {
					return pos;
				}

				auto haystack = substr( pos );
				const_iterator iter = sv2_details::find_first_not_of(
				  haystack.begin( ), haystack.end( ), v.begin( ),
				  std::next( v.begin( ), static_cast<ptrdiff_t>( v.size( ) ) ),
				  sv2_details::bp_eq<CharT> );
				if( end( ) == iter ) {
					return npos;
				}

				return static_cast<size_type>( std::distance( begin( ), iter ) );
			}

			template<string_view_bounds_type Bounds>
			[[nodiscard]] constexpr size_type
			find_first_not_of( basic_string_view<CharT, Bounds> v ) const {
				return find_first_not_of( v, 0 );
			}

			[[nodiscard]] constexpr size_type
			find_first_not_of( CharT c, size_type pos ) const {
				return find_first_not_of(
				  basic_string_view<CharT, BoundsType>( std::addressof( c ), 1U ),
				  pos );
			}

			[[nodiscard]] constexpr size_type find_first_not_of( CharT c ) const {
				return find_first_not_of(
				  basic_string_view<CharT, BoundsType>( std::addressof( c ), 1U ), 0 );
			}

			[[nodiscard]] constexpr size_type
			find_first_not_of( const_pointer s, size_type pos,
			                   size_type count ) const {
				return find_first_not_of(
				  basic_string_view<CharT, BoundsType>( s, count ), pos );
			}

			[[nodiscard]] constexpr size_type
			find_first_not_of( const_pointer s, size_type pos ) const {
				return find_first_not_of( basic_string_view<CharT, BoundsType>( s ),
				                          pos );
			}

			template<size_type N>
			[[nodiscard]] constexpr size_type
			find_first_not_of( CharT const ( &&s )[N], size_type pos ) const {
				return find_first_not_of(
				  basic_string_view<CharT, BoundsType>( s, N - 1 ), pos );
			}

			[[nodiscard]] constexpr size_type
			find_first_not_of( const_pointer s ) const {
				return find_first_not_of( basic_string_view<CharT, BoundsType>( s ),
				                          0 );
			}

			template<size_type N>
			[[nodiscard]] constexpr size_type
			find_first_not_of( CharT const ( &&s )[N] ) const {
				return find_first_not_of(
				  basic_string_view<CharT, BoundsType>( s, N - 1 ), 0 );
			}

			template<typename UnaryPredicate,
			         DAW_REQ_UNARY_PRED( UnaryPredicate, CharT )>
			[[nodiscard]] constexpr size_type
			find_last_not_of_if( UnaryPredicate pred, size_type pos ) const {

				if( empty( ) ) {
					return npos;
				}
				if( pos > size( ) ) {
					pos = size( ) - 1;
				}
				for( auto n = static_cast<difference_type>( pos ); n >= 0; --n ) {

					if( not pred( m_first[n] ) ) {
						return static_cast<size_type>( n );
					}
				}
				return npos;
			}

			template<typename UnaryPredicate,
			         DAW_REQ_UNARY_PRED( UnaryPredicate, CharT )>
			[[nodiscard]] constexpr size_type
			find_last_not_of_if( UnaryPredicate pred ) const {
				return find_last_not_of_if( pred, npos );
			}

			template<string_view_bounds_type Bounds>
			[[nodiscard]] constexpr size_type
			find_last_not_of( basic_string_view<CharT, Bounds> v,
			                  size_type pos ) const {

				if( empty( ) ) {
					return npos;
				}
				if( pos > size( ) ) {
					pos = size( ) - 1;
				}
				if( v.empty( ) ) {
					return pos;
				}
				for( auto n = static_cast<difference_type>( pos ); n >= 0; --n ) {

					if( v.find( m_first[n] ) == npos ) {
						return static_cast<size_type>( n );
					}
				}
				return npos;
			}

			template<string_view_bounds_type Bounds>
			[[nodiscard]] constexpr size_type
			find_last_not_of( basic_string_view<CharT, Bounds> v ) const {
				return find_last_not_of( v, npos );
			}

			[[nodiscard]] constexpr size_type
			find_last_not_of( CharT c, size_type pos ) const {
				return find_last_not_of(
				  basic_string_view<CharT, BoundsType>( std::addressof( c ), 1U ),
				  pos );
			}

			[[nodiscard]] constexpr size_type find_last_not_of( CharT c ) const {
				return find_last_not_of( c, npos );
			}

			[[nodiscard]] constexpr size_type
			find_last_not_of( const_pointer s, size_type pos,
			                  size_type count ) const {
				return find_last_not_of(
				  basic_string_view<CharT, BoundsType>( s, count ), pos );
			}

			[[nodiscard]] constexpr size_type
			find_last_not_of( const_pointer s, size_type pos ) const {
				return find_last_not_of( basic_string_view<CharT, BoundsType>( s ),
				                         pos );
			}

			[[nodiscard]] constexpr size_type
			find_last_not_of( const_pointer s ) const {
				return find_last_not_of( basic_string_view<CharT, BoundsType>( s ),
				                         npos );
			}

			[[nodiscard]] constexpr bool starts_with( CharT c ) const {
				if( empty( ) ) {
					return false;
				}
				return front( ) == c;
			}

			template<string_view_bounds_type Bounds>
			[[nodiscard]] constexpr bool
			starts_with( basic_string_view<CharT, Bounds> s ) const {
				if( s.size( ) > size( ) ) {
					return false;
				}
				auto lhs = begin( );
				while( not s.empty( ) ) {
					if( *lhs != s.front( ) ) {
						return false;
					}

					++lhs;

					s.remove_prefix( );
				}
				return true;
			}

			[[nodiscard]] constexpr bool starts_with( const_pointer s ) const {
				return starts_with( basic_string_view<CharT, BoundsType>( s ) );
			}

			[[nodiscard]] constexpr bool ends_with( CharT c ) const {
				if( empty( ) ) {
					return false;
				}
				return back( ) == c;
			}

			template<string_view_bounds_type Bounds>
			[[nodiscard]] constexpr bool
			ends_with( basic_string_view<CharT, Bounds> s ) const {
				if( s.size( ) > size( ) ) {
					return false;
				}
				auto lhs = rbegin( );
				while( not s.empty( ) ) {
					if( *lhs != s.back( ) ) {
						return false;
					}
					++lhs;
					s.remove_suffix( );
				}
				return true;
			}

			[[nodiscard]] constexpr bool ends_with( const_pointer s ) const {
				return ends_with( basic_string_view<CharT, BoundsType>( s ) );
			}

			[[nodiscard]] constexpr bool
			operator==( basic_string_view rhs ) noexcept {
				return compare( rhs ) == 0;
			}

#if defined( DAW_HAS_CPP20_3WAY_COMPARE )
			// clang-format off
			[[nodiscard]] constexpr std::strong_ordering
			operator<=>( basic_string_view const &rhs ) const noexcept {
				return compare( rhs ) <=> 0;
			}
			// clang-format on
#endif

			template<typename StringView,
			         DAW_REQ_CONTIG_CHAR_RANGE( StringView, CharT )>
			[[nodiscard]] friend constexpr bool
			operator==( StringView &&lhs, basic_string_view rhs ) noexcept {
				return basic_string_view( std::data( lhs ), std::size( lhs ) )
				         .compare( rhs ) == 0;
			}

			[[nodiscard]] friend constexpr bool
			operator==( const_pointer lhs, basic_string_view rhs ) noexcept {
				return basic_string_view( lhs ).compare( rhs ) == 0;
			}

			[[nodiscard]] constexpr bool
			operator!=( basic_string_view rhs ) noexcept {
				return compare( rhs ) != 0;
			}

			[[nodiscard]] friend constexpr bool
			operator!=( const_pointer lhs, basic_string_view rhs ) noexcept {
				return basic_string_view( lhs ).compare( rhs ) != 0;
			}

			template<typename StringView,
			         DAW_REQ_CONTIG_CHAR_RANGE( StringView, CharT )>
			[[nodiscard]] friend constexpr bool
			operator!=( StringView &&lhs, basic_string_view rhs ) noexcept {
				return basic_string_view( std::data( lhs ), std::size( lhs ) )
				         .compare( rhs ) != 0;
			}

			[[nodiscard]] constexpr bool operator<( basic_string_view rhs ) noexcept {
				return compare( rhs ) < 0;
			}

			[[nodiscard]] friend constexpr bool
			operator<( const_pointer lhs, basic_string_view rhs ) noexcept {
				return basic_string_view( lhs ).compare( rhs ) < 0;
			}

			template<typename StringView,
			         DAW_REQ_CONTIG_CHAR_RANGE( StringView, CharT )>
			[[nodiscard]] friend constexpr bool
			operator<( StringView &&lhs, basic_string_view rhs ) noexcept {
				return basic_string_view( std::data( lhs ), std::size( lhs ) )
				         .compare( rhs ) < 0;
			}

			[[nodiscard]] constexpr bool
			operator<=( basic_string_view rhs ) noexcept {
				return compare( rhs ) <= 0;
			}

			[[nodiscard]] friend constexpr bool
			operator<=( const_pointer lhs, basic_string_view rhs ) noexcept {
				return basic_string_view( lhs ).compare( rhs ) <= 0;
			}

			template<typename StringView,
			         DAW_REQ_CONTIG_CHAR_RANGE( StringView, CharT )>
			[[nodiscard]] friend constexpr bool
			operator<=( StringView &&lhs, basic_string_view rhs ) noexcept {
				return basic_string_view( std::data( lhs ), std::size( lhs ) )
				         .compare( rhs ) <= 0;
			}

			[[nodiscard]] constexpr bool operator>( basic_string_view rhs ) noexcept {
				return compare( rhs ) > 0;
			}

			[[nodiscard]] friend constexpr bool
			operator>( const_pointer lhs, basic_string_view rhs ) noexcept {
				return basic_string_view( lhs ).compare( rhs ) > 0;
			}

			template<typename StringView,
			         DAW_REQ_CONTIG_CHAR_RANGE( StringView, CharT )>
			[[nodiscard]] friend constexpr bool
			operator>( StringView &&lhs, basic_string_view rhs ) noexcept {
				return basic_string_view( std::data( lhs ), std::size( lhs ) )
				         .compare( rhs ) > 0;
			}

			[[nodiscard]] constexpr bool
			operator>=( basic_string_view rhs ) noexcept {
				return compare( rhs ) >= 0;
			}

			[[nodiscard]] friend constexpr bool
			operator>=( const_pointer lhs, basic_string_view rhs ) noexcept {
				return basic_string_view( lhs ).compare( rhs ) >= 0;
			}

			template<typename StringView,
			         DAW_REQ_CONTIG_CHAR_RANGE( StringView, CharT )>
			[[nodiscard]] friend constexpr bool
			operator>=( StringView &&lhs, basic_string_view rhs ) noexcept {
				return basic_string_view( std::data( lhs ), std::size( lhs ) )
				         .compare( rhs ) >= 0;
			}

		private:
			struct is_space {
				inline constexpr bool operator( )( CharT c ) const noexcept {
					return daw::nsc_or( c == CharT( ' ' ), c == CharT( '\t' ),
					                    c == CharT( '\n' ), c == CharT( '\v' ),
					                    c == CharT( '\f' ), c == CharT( '\r' ) );
				}
			};

		public:
			template<typename UnaryPred, DAW_REQ_UNARY_PRED( UnaryPred, CharT )>
			DAW_ATTRIB_INLINE constexpr basic_string_view &
			remove_prefix_while( UnaryPred pred ) noexcept {
				auto const last_pos = find_first_not_of_if( pred );
				remove_prefix( last_pos );
				return *this;
			}

			DAW_ATTRIB_INLINE constexpr basic_string_view &
			remove_prefix_while( CharT c ) noexcept {
				auto pos = find_first_not_of( c );
				remove_prefix( pos );
				return *this;
			}

			constexpr basic_string_view &trim_prefix( ) noexcept {
				remove_prefix_while( is_space{ } );
				return *this;
			}

			constexpr basic_string_view trim_prefix_copy( ) const noexcept {
				auto result = *this;
				result.remove_prefix_while( is_space{ } );
				return result;
			}

			template<typename UnaryPred, DAW_REQ_UNARY_PRED( UnaryPred, CharT )>
			DAW_ATTRIB_INLINE constexpr basic_string_view &
			remove_suffix_while( UnaryPred pred ) noexcept {
				auto pos = find_last_not_of_if( pred );
				resize( pos + 1U );
				return *this;
			}

			DAW_ATTRIB_INLINE constexpr basic_string_view &
			remove_suffix_while( CharT c ) noexcept {
				auto pos = find_last_not_of( c );
				resize( pos + 1U );
				return *this;
			}

			constexpr basic_string_view &trim_suffix( ) noexcept {
				remove_suffix_while( is_space{ } );
				return *this;
			}

			constexpr basic_string_view trim_suffix_copy( ) const noexcept {
				auto result = *this;
				result = remove_suffix_while( is_space{ } );
				return result;
			}

			constexpr basic_string_view &trim( ) noexcept {
				(void)trim_prefix( );
				return trim_suffix( );
			}

			constexpr basic_string_view trim_copy( ) const noexcept {
				auto result = trim_prefix_copy( );
				result.trim_suffix( );
				return result;
			}

			/// @brief Decrement the data( ) pointer by 1.
			/// @pre data( ) != null and data( ) - 1 is a valid object
			constexpr void expand_prefix( ) noexcept {
				DAW_STRING_VIEW_DBG_RNG_CHECK(
				  data( ), "Attempt to expand a null basic_string_view" );
				inc_front<BoundsType>( 1 );
			}

			/// @brief Decrement the data( ) pointer by n.
			/// @pre data( ) != null and data( ) - n is a valid object
			constexpr void expand_prefix( size_type n ) {
				DAW_STRING_VIEW_DBG_RNG_CHECK(
				  data( ), "Attempt to expand a null basic_string_view" );
				inc_front<BoundsType>( n );
			}

			/// @brief Increment the size( ) by 1
			/// @pre data( ) != null and data( ) + size( ) + 1 is a valid object
			constexpr void expand_suffix( ) noexcept {
				DAW_STRING_VIEW_DBG_RNG_CHECK(
				  data( ), "Attempt to expand a null basic_string_view" );
				inc_back<BoundsType>( 1 );
			}

			/// @brief Increment the size( ) by n
			/// @pre data( ) != null and data( ) + size( ) + n is a valid object
			constexpr void expand_suffix( size_type n ) {
				DAW_STRING_VIEW_DBG_RNG_CHECK(
				  data( ), "Attempt to expand a null basic_string_view" );
				inc_back<BoundsType>( n );
			}
		}; // basic_string_view

		// CTAD
		template<typename StringView, typename CharT = std::decay_t<DAW_TYPEOF(
		                                std::data( std::declval<StringView>( ) ) )>>
		basic_string_view( StringView && ) -> basic_string_view<CharT>;

		template<typename CharT>
		basic_string_view( CharT const * ) -> basic_string_view<CharT>;

		template<typename CharT>
		basic_string_view( CharT const *, std::size_t count )
		  -> basic_string_view<CharT>;

		template<typename CharT>
		basic_string_view( CharT const *, CharT const * )
		  -> basic_string_view<CharT>;

		namespace string_view_literals {
			[[nodiscard]] constexpr string_view
			operator""_sv( char const *str, std::size_t len ) noexcept {
				return string_view{ str, len };
			}

#if defined( __cpp_char8_t )
			[[nodiscard]] constexpr u8string_view
			operator""_sv( char8_t const *str, std::size_t len ) noexcept {
				return u8string_view{ str, len };
			}
#endif

			[[nodiscard]] constexpr u16string_view
			operator""_sv( char16_t const *str, std::size_t len ) noexcept {
				return u16string_view{ str, len };
			}

			[[nodiscard]] constexpr u32string_view
			operator""_sv( char32_t const *str, std::size_t len ) noexcept {
				return u32string_view{ str, len };
			}

			[[nodiscard]] constexpr wstring_view
			operator""_sv( wchar_t const *str, std::size_t len ) noexcept {
				return wstring_view{ str, len };
			}
		} // namespace string_view_literals

		namespace sv2_details {
			template<typename OStream,
			         std::enable_if_t<daw::traits::is_ostream_like_lite_v<OStream>,
			                          std::nullptr_t> = nullptr>
			inline void sv_insert_fill_chars( OStream &os, std::size_t n ) {
				using CharT = typename OStream::char_type;
				static_assert(
				  traits::traits_details::ostream_detectors::has_write_member_v<OStream,
				                                                                CharT>,
				  "OStream Must has write member" );

				std::array<CharT, 8> fill_chars = { 0 };
				fill_chars.fill( os.fill( ) );

				for( ; n >= fill_chars.size( ) and os.good( );
				     n -= fill_chars.size( ) ) {
					os.write( fill_chars.data( ),
					          static_cast<std::make_signed_t<std::size_t>>(
					            fill_chars.size( ) ) );
				}
				if( n > 0 and os.good( ) ) {
					os.write( fill_chars.data( ),
					          static_cast<std::make_signed_t<std::size_t>>( n ) );
				}
			}

			template<typename OStream, typename CharT,
			         daw::string_view_bounds_type Bounds,
			         std::enable_if_t<daw::traits::is_ostream_like_v<OStream, CharT>,
			                          std::nullptr_t> = nullptr>
			void sv_insert_aligned( OStream &os,
			                        daw::sv2::basic_string_view<CharT, Bounds> str ) {
				auto const size = str.size( );
				auto const alignment_size =
				  static_cast<std::size_t>( os.width( ) ) - size;
				bool const align_left =
				  ( os.flags( ) & OStream::adjustfield ) == OStream::left;
				if( not align_left ) {
					sv_insert_fill_chars( os, alignment_size );
					if( os.good( ) ) {
						os.write( str.data( ),
						          static_cast<std::make_signed_t<std::size_t>>( size ) );
					}
				} else {
					os.write( str.data( ),
					          static_cast<std::make_signed_t<std::size_t>>( size ) );
					if( os.good( ) ) {
						sv_insert_fill_chars( os, alignment_size );
					}
				}
			}
		} // namespace sv2_details

		template<typename CharT, string_view_bounds_type Bounds, typename OStream,
		         std::enable_if_t<daw::traits::is_ostream_like_v<OStream, CharT>,
		                          std::nullptr_t> = nullptr>
		OStream &operator<<( OStream &os, basic_string_view<CharT, Bounds> v ) {
			if( os.good( ) ) {
				auto const size = v.size( );
				auto const w = static_cast<std::size_t>( os.width( ) );
				if( w <= size ) {
					os.write( v.data( ),
					          static_cast<std::make_signed_t<std::size_t>>( size ) );
				} else {
					daw::sv2_details::sv_insert_aligned( os, v );
				}
				os.width( 0 );
			}
			return os;
		}
	} // namespace sv2
} // namespace daw

namespace std {
	template<typename CharT, daw::sv2::string_view_bounds_type Bounds>
	struct hash<daw::sv2::basic_string_view<CharT, Bounds>> {
		[[nodiscard]] constexpr size_t
		operator( )( daw::sv2::basic_string_view<CharT, Bounds> s ) {
			return daw::fnv1a_hash( s.data( ), s.size( ) );
		}
	};
} // namespace std

#undef DAW_STRING_VIEW_DBG_RNG_CHECK
#undef DAW_STRING_VIEW_RNG_CHECK
#undef DAW_STRING_VIEW_PRECOND_CHECK
#undef DAW_REQ_UNARY_PRED
#undef DAW_REQ_CONTIG_CHAR_RANGE
#undef DAW_REQ_CONTIG_CHAR_RANGE
#undef DAW_REQ_CONTIG_CHAR_RANGE_CTOR
DAW_UNSAFE_BUFFER_FUNC_STOP
