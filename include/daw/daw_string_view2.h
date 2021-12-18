// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/string_view
//

#pragma once

#include "daw_string_view2_fwd.h"

#include "daw_algorithm.h"
#include "daw_cpp_feature_check.h"
#include "daw_exception.h"
#include "daw_fnv1a_hash.h"
#include "daw_generic_hash.h"
#include "daw_logic.h"
#include "daw_math.h"
#include "daw_move.h"
#include "daw_string_view_version.h"
#include "daw_swap.h"
#include "daw_traits.h"
#include "impl/daw_string_impl.h"
#include "iterator/daw_back_inserter.h"
#include "iterator/daw_iterator.h"

#include <ciso646>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iterator>
#include <limits>
#include <stdexcept>

#include <vector>

/// @brief Require Pred to be a Unary Predicate
/// @param Pred Unary predicate
/// @param Type Parameter type of predicate
#define DAW_REQ_UNARY_PRED( Pred, Type )                                       \
	std::enable_if_t<traits::is_unary_predicate_v<Pred, Type>, std::nullptr_t> = \
	  nullptr

/// @brief Require a contiguous character range
/// @param Range contiguous range
/// @param CharT character type of range elements
#define DAW_REQ_CONTIG_CHAR_RANGE( Range, CharT )                              \
	std::enable_if_t<(sv2_details::is_string_view_like<Range, CharT>::value and  \
	                  not std::is_convertible_v<Range, char const *>),           \
	                 std::nullptr_t> = nullptr

/// @brief Require a character pointer
/// @param Pointer a const_pointer
#define DAW_REQ_CHAR_PTR( Pointer )                                            \
	std::enable_if_t<sv2_details::is_char_pointer_v<Pointer, const_pointer>,     \
	                 std::nullptr_t> = nullptr

/// @brief Require Type be constructable from a CharT const * and a size_type
/// @param Type A type to construct from a pointer/size_type pair
#define DAW_REQ_CONTIG_CHAR_RANGE_CTOR( Type )                                 \
	std::enable_if_t<                                                            \
	  sv2_details::is_contigious_range_constructible<Type, CharT>::value,        \
	  std::nullptr_t> = nullptr

namespace daw {
#if DAW_STRINGVIEW_VERSION == 2
	inline
#endif
	  namespace sv2 {
		/// @brief Tag type for specifying that the searched for term/item is not
		/// to be removed from string_view
		struct nodiscard_t {};
		inline constexpr nodiscard_t nodiscard = nodiscard_t{ };

		/// @brief A predicate type used in the find based routine to return true
		/// when the element is one of the specified characters
		template<typename CharT, CharT... needles>
		struct any_of_t {
			inline constexpr bool operator( )( CharT c ) const {
				return ( ( c == needles ) | ... );
			}
		};

		template<auto needle, auto... needles>
		inline static constexpr any_of_t<decltype( needle ), needle, needles...>
		  any_of{ };

		/// @brief A predicate type used in the find based routine to return true
		/// when the element is none of the specified characters
		template<typename CharT, CharT... needles>
		struct none_of_t {
			inline constexpr bool operator( )( CharT c ) const {
				return ( ( c != needles ) & ... );
			}
		};

		template<auto needle, auto... needles>
		inline static constexpr none_of_t<decltype( needle ), needle, needles...>
		  none_of{ };

		namespace sv2_details {
			template<typename T>
			constexpr std::size_t find_predicate_result_size( T const & ) {
				return 1;
			}

			template<typename T>
			using has_datasize_test = typename std::remove_reference<
			  decltype( std::data( std::declval<T const &>( ) ) +
			            std::size( std::declval<T const &>( ) ) )>::type;

			template<typename T>
			using is_sv2_test = typename T::i_am_a_daw_string_view2;

			template<typename T>
			using is_sv2_t = daw::is_detected<is_sv2_test, T>;

			template<typename T, typename CharT>
			struct is_string_view_like
			  : std::conjunction<daw::is_detected<has_datasize_test, T>,
			                     daw::not_trait<is_sv2_t<T>>> {};

			static_assert( daw::is_detected_v<has_datasize_test, std::string> );
			static_assert( daw::not_trait<is_sv2_t<std::string>>::value );
			static_assert( is_string_view_like<std::string, char>::value );

			template<typename T, typename CharT>
			struct is_contigious_range_constructible
			  : std::is_constructible<T, CharT *, std::size_t> {};

			template<typename T, typename PointerType>
			inline constexpr bool is_char_pointer_v =
			  std::is_pointer_v<T> and std::is_convertible_v<T, PointerType>;

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
			static constexpr auto bp_eq = []( CharT l, CharT r ) noexcept {
				return l == r;
			};

			template<string_view_bounds_type B>
			static constexpr bool is_last_a_pointer_v =
			  B == string_view_bounds_type::pointer;

			using last_type = std::conditional_t<is_last_a_pointer_v<BoundsType>,
			                                     const_pointer, size_type>;
			static inline constexpr last_type default_last = [] {
				if constexpr( is_last_a_pointer_v<BoundsType> ) {
					return nullptr;
				} else {
					return 0;
				}
			}( );
			using last_difference_type =
			  std::conditional_t<is_last_a_pointer_v<BoundsType>, difference_type,
			                     size_type>;

			template<string_view_bounds_type Bounds, typename LastType>
			DAW_ATTRIB_INLINE static constexpr last_type
			make_last( const_pointer f, LastType l ) noexcept {
				if constexpr( std::is_pointer_v<LastType> ) {
					if constexpr( is_last_a_pointer_v<Bounds> ) {
						(void)f;
						return l;
					} else {
						return static_cast<last_type>( l - f );
					}
				} else {
					if constexpr( is_last_a_pointer_v<Bounds> ) {
						return f + static_cast<difference_type>( l );
					} else {
						(void)f;
						return static_cast<size_type>( l );
					}
				}
			}

			template<string_view_bounds_type Bounds>
			DAW_ATTRIB_INLINE constexpr const_pointer last_pointer( ) const {
				if constexpr( is_last_a_pointer_v<Bounds> ) {
					return m_last;
				} else {
					return m_first + static_cast<difference_type>( m_last );
				}
			}

			template<string_view_bounds_type Bounds>
			DAW_ATTRIB_INLINE static constexpr size_type size( const_pointer f,
			                                                   last_type l ) {
				if constexpr( is_last_a_pointer_v<Bounds> ) {
					return static_cast<size_type>( l - f );
				} else {
					(void)f;
					return l;
				}
			}

			template<string_view_bounds_type Bounds>
			DAW_ATTRIB_INLINE constexpr void dec_front( size_type n ) {
				if constexpr( is_last_a_pointer_v<Bounds> ) {
					m_first += static_cast<difference_type>( n );
				} else {
					m_first += static_cast<difference_type>( n );
					m_last -= n;
				}
			}

			template<string_view_bounds_type Bounds>
			DAW_ATTRIB_INLINE constexpr void dec_back( size_type n ) {
				if constexpr( is_last_a_pointer_v<Bounds> ) {
					m_last -= static_cast<difference_type>( n );
				} else {
					m_last -= n;
				}
			}

			const_pointer m_first = nullptr;
			last_type m_last = default_last;

		public:
			static constexpr size_type const npos =
			  ( std::numeric_limits<size_type>::max )( );

			//******************************
			// Constructors
			//******************************

			/// @brief Construct an empty string_view
			/// @post data( ) == nullptr
			/// @post size( ) == 0
			constexpr basic_string_view( ) noexcept = default;

			/// @brief Construct an empty string_view
			/// @post data( ) == nullptr
			/// @post size( ) == 0
			constexpr basic_string_view( std::nullptr_t ) noexcept
			  : m_first( nullptr )
			  , m_last( make_last<BoundsType>( nullptr, nullptr ) ) {}

			/// @brief Prevents nullptr literals and a size_type to construct a
			/// string_view
			constexpr basic_string_view( std::nullptr_t,
			                             size_type n ) noexcept = delete;

			/// @brief Construct a string_view
			/// @param s Pointer to start of character range
			/// @param count Size of character range
			/// @post data( ) == s
			/// @post size( ) == count
			constexpr basic_string_view( const_pointer s, size_type count ) noexcept
			  : m_first( s )
			  , m_last( make_last<BoundsType>( s, count ) ) {}

			/// @brief Construct a string_view with a range starting with s
			/// @param s Start of character range
			/// @pre s == nullptr, or s beings a valid character range that
			/// has a CharT{} terminated sentinel
			/// @post data( ) == s
			/// @post size( ) == strlen( s ) or CharT{} if s == nullptr
			template<typename CharPtr, DAW_REQ_CHAR_PTR( CharPtr )>
			constexpr basic_string_view( CharPtr s ) noexcept
			  : m_first( s )
			  , m_last(
			      make_last<BoundsType>( s, details::strlen<size_type>( s ) ) ) {}

			/// @brief Converting substr constructor from any string_view with
			/// matching CharT types.
			/// @param sv Other string_view
			/// @param count Maximum number of characters to use in range
			/// formed by sv
			/// @post data( ) == sv.data( )
			/// @post size( ) == min( count, sv.size( ) )
			template<string_view_bounds_type B>
			constexpr basic_string_view( basic_string_view<CharT, B> sv,
			                             size_type count ) noexcept
			  : m_first( sv.data( ) )
			  , m_last( make_last<BoundsType>( sv.data( ),
			                                   ( std::min )( sv.size( ), count ) ) ) {
			}

			/// @brief Construct a string_view from a type that forms a
			/// contiguous range of characters
			/// @param sv A valid contiguous character range
			/// @post data( ) == std::data( sv )
			/// @post size( ) == std::size( sv )
			template<typename StringView,
			         DAW_REQ_CONTIG_CHAR_RANGE( StringView, CharT )>
			constexpr basic_string_view( StringView &&sv ) noexcept
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
			constexpr basic_string_view( StringView &&sv, size_type count ) noexcept
			  : m_first( std::data( sv ) )
			  , m_last( make_last<BoundsType>(
			      m_first, ( std::min )( std::size( sv ), count ) ) ) {}

			/// @brief Construct a string_view from a character array. Assumes
			/// a string literal like array with last element having a value
			/// of CharT{}
			/// @param string_literal A valid contiguous character range
			/// @post data( ) == std::data( string_literal )
			/// @post size( ) == std::size( string_literal ) - 1
			template<std::size_t N>
			constexpr basic_string_view( CharT const ( &string_literal )[N] ) noexcept
			  : m_first( string_literal )
			  , m_last( make_last<BoundsType>( string_literal, N - 1 ) ) {}

			/// @brief Construct a string_view from a range formed by two
			/// character pointers
			/// @param first Start of character range
			/// @param last One past last element in range
			/// @pre [first, last) form a valid character range
			/// @post data( ) == first
			/// @post size( ) == last - first
			constexpr basic_string_view(
			  basic_string_view::const_pointer first,
			  basic_string_view::const_pointer last ) noexcept
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
			[[nodiscard]] constexpr const_reference
			operator[]( size_type pos ) const {
				return m_first[pos];
			}

			/// Access to the elements of range
			/// @param pos Position in range
			/// @throws std::out_of_range when pos >= size( )
			/// @return data( )[pos]
			[[nodiscard]] constexpr const_reference at( size_type pos ) const {
				daw::exception::precondition_check<std::out_of_range>(
				  pos < size( ), "Attempt to access basic_string_view past end" );
				return operator[]( pos );
			}

			/// @brief Access the first element in range
			/// @pre data( ) != nullptr
			/// @pre size( ) > 0
			[[nodiscard]] constexpr const_reference front( ) const {
				return *m_first;
			}

			/// @brief Access the last element in range
			/// @pre data( ) != nullptr
			/// @pre size( ) > 0
			[[nodiscard]] constexpr const_reference back( ) const {
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
			/// @pre data( ) != nullptr
			constexpr void remove_prefix( size_type n ) {
				dec_front<BoundsType>( ( std::min )( n, size( ) ) );
			}

			/// @brief Increment the data( ) pointer by 1. If string_view is
			/// empty, it does nothing.
			/// @pre data( ) != nullptr
			constexpr void remove_prefix( ) {
				dec_front<BoundsType>( ( std::min )( size_type{ 1U }, size( ) ) );
			}

			/// @brief Decrement the size( ) by n. If string_view is empty, it
			/// does nothing.
			constexpr void remove_suffix( size_type n ) {
				dec_back<BoundsType>( ( std::min )( n, size( ) ) );
			}

			/// @brief Decrement the size( ) by 1 if size( ) > 0
			constexpr void remove_suffix( ) {
				dec_back<BoundsType>( ( std::min )( size_type{ 1U }, size( ) ) );
			}

			/// @brief Increment the data( ) pointer by 1.
			/// @return front( ) prior to increment
			[[nodiscard]] constexpr CharT pop_front( ) {
				auto result = front( );
				remove_prefix( 1U );
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

			[[nodiscard]] constexpr CharT pop_back( ) {
				auto result = back( );
				remove_suffix( );
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
				return substr( pos + where.size( ) );
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
				return substr( pos + where.size( ) );
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

			/// @brief searches for where, and disregards everything until the
			/// end of that
			/// @param where character to find and consume
			/// @return substring with everything up until the end of where
			/// removed
			constexpr basic_string_view &remove_prefix_until( CharT where ) {
				auto pos = find( where );
				remove_prefix( pos );
				remove_prefix( 1 );
				return *this;
			}

			/// @brief searches for where, and disregards everything prior to
			/// where
			/// @param where character to find
			/// @return substring with everything up until where removed
			constexpr basic_string_view &remove_prefix_until( CharT where,
			                                                  nodiscard_t ) {
				auto pos = find( where );
				dec_front<BoundsType>( ( std::min )( size( ), pos ) );
				return *this;
			}

			/// @brief searches for where, and disregards everything until the
			/// end of that
			/// @param where string to find and consume
			/// @return substring with everything up until the end of where
			/// removed
			constexpr basic_string_view &
			remove_prefix_until( basic_string_view where ) {
				auto pos = find( where );
				dec_front<BoundsType>( ( std::min )( size( ), pos + where.size( ) ) );
				return *this;
			}

			/// @brief searches for where, and disregards everything prior to
			/// where
			/// @param where string to find and consume
			/// @return substring with everything up until the start of where
			/// removed
			constexpr basic_string_view &remove_prefix_until( basic_string_view where,
			                                                  nodiscard_t ) {
				auto pos = find( where );
				dec_front<BoundsType>( ( std::min )( size( ), pos ) );
				return *this;
			}

			/// @brief Removes all elements until pred is true or end reached
			/// @tparam UnaryPredicate a unary predicate type that accepts a char and
			/// indicates with true when to stop
			/// @param pred Predicate object
			/// @return A reference to the current string_view object
			template<typename UnaryPredicate,
			         DAW_REQ_UNARY_PRED( UnaryPredicate, CharT )>
			constexpr basic_string_view &remove_prefix_until( UnaryPredicate pred ) {
				auto pos = find_first_of_if( pred );
				dec_front<BoundsType>( ( std::min )(
				  size( ), pos + sv2_details::find_predicate_result_size( pred ) ) );
				return *this;
			}

			/// @brief removes all elements prior to predicate returning true
			/// @param pred predicate that takes a CharT
			/// @return substring with everything up until predicate is
			/// removed
			template<typename UnaryPredicate,
			         DAW_REQ_UNARY_PRED( UnaryPredicate, CharT )>
			constexpr basic_string_view &remove_prefix_until( UnaryPredicate pred,
			                                                  nodiscard_t ) {
				auto pos = find_if( pred );
				dec_front<BoundsType>( ( std::min )( size( ), pos ) );
				return *this;
			}

			/// @brief Set the size of the range
			/// @param new_size New size of range
			/// @pre size( ) >= new_size
			/// @post size( ) == new_size
			constexpr void resize( size_type new_size ) {
				daw::exception::precondition_check<std::out_of_range>( new_size <=
				                                                       size( ) );
				m_last = make_last<BoundsType>(
				  m_first,
				  std::next( m_first, static_cast<difference_type>( new_size ) ) );
			}

			constexpr size_type copy( pointer dest, size_type count,
			                          size_type pos ) const {
				daw::exception::precondition_check<std::out_of_range>(
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

			[[nodiscard]] constexpr basic_string_view
			substr( size_type pos, size_type count ) const {
				daw::exception::precondition_check<std::out_of_range>(
				  pos <= size( ), "Attempt to access basic_string_view past end" );
				auto const rcount =
				  static_cast<size_type>( ( std::min )( count, size( ) - pos ) );
				return { m_first + pos, m_first + pos + rcount };
			}

			[[nodiscard]] constexpr basic_string_view substr( ) const {
				return substr( 0, npos );
			}

			[[nodiscard]] constexpr basic_string_view substr( size_type pos ) const {
				return substr( pos, npos );
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
				  details::search( begin( ) + pos, end( ), v.begin( ), v.end( ) );
				if( end( ) == result ) {
					return npos;
				}
				return static_cast<size_type>( result - begin( ) );
			}

			template<string_view_bounds_type Bounds>
			[[nodiscard]] constexpr size_type
			find( basic_string_view<CharT, Bounds> v ) const {
				return find( v, 0 );
			}

			[[nodiscard]] constexpr size_type find( CharT c, size_type pos ) const {
				return find(
				  basic_string_view<CharT, BoundsType>( std::addressof( c ), 1 ), pos );
			}

			[[nodiscard]] constexpr size_type find( CharT c ) const {
				return find( c, 0 );
			}

			[[nodiscard]] constexpr size_type find( const_pointer s, size_type pos,
			                                        size_type count ) const {
				return find( basic_string_view<CharT, BoundsType>( s, count ), pos );
			}

			[[nodiscard]] constexpr size_type find( const_pointer s,
			                                        size_type pos ) const {
				return find( basic_string_view<CharT, BoundsType>( s ), pos );
			}

			[[nodiscard]] constexpr size_type find( const_pointer s ) const {
				return find( basic_string_view<CharT, BoundsType>( s ), 0 );
			}

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
					if( details::compare( cur, v.begin( ), v.size( ) ) == 0 ) {
						return static_cast<size_type>( cur - begin( ) );
					}
					if( cur == begin( ) ) {
						return npos;
					}
					--cur;
				}
			}

			[[nodiscard]] constexpr size_type rfind( const_pointer s, size_type pos,
			                                         size_type count ) const {
				return rfind( basic_string_view<CharT, BoundsType>( s, count ), pos );
			}

			template<string_view_bounds_type Bounds>
			[[nodiscard]] constexpr size_type
			rfind( basic_string_view<CharT, Bounds> v ) const {
				return rfind( v, npos );
			}

			[[nodiscard]] constexpr size_type rfind( CharT c, size_type pos ) const {
				return rfind(
				  basic_string_view<CharT, BoundsType>( std::addressof( c ), 1 ), pos );
			}

			[[nodiscard]] constexpr size_type rfind( CharT c ) const {
				return rfind(
				  basic_string_view<CharT, BoundsType>( std::addressof( c ), 1 ),
				  npos );
			}

			[[nodiscard]] constexpr size_type rfind( const_pointer s,
			                                         size_type pos ) const {
				return rfind( basic_string_view<CharT, BoundsType>( s ), pos );
			}

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
				auto const iter = details::find_first_of( begin( ) + pos, end( ),
				                                          v.begin( ), v.end( ), bp_eq );

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
				auto const iter =
				  details::search( begin( ) + pos, end( ), v.begin( ), v.end( ) );
				if( cend( ) == iter ) {
					return npos;
				}
				return static_cast<size_type>( std::distance( begin( ), iter ) );
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
				  details::find_first_of_if( cbegin( ) + pos, cend( ), pred );
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
				  details::find_first_not_of_if( begin( ) + pos, end( ), pred );
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
				const_iterator iter = details::find_first_not_of(
				  haystack.begin( ), haystack.end( ), v.begin( ),
				  std::next( v.begin( ), static_cast<ptrdiff_t>( v.size( ) ) ), bp_eq );
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
			find_first_not_of( CharT const( &&s )[N], size_type pos ) const {
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
			find_first_not_of( CharT const( &&s )[N] ) const {
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
					if( *lhs++ != s.front( ) ) {
						return false;
					}
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
					if( *lhs++ != s.back( ) ) {
						return false;
					}
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
			constexpr void remove_prefix_while( UnaryPred is_whitespace ) noexcept {
				auto const last_pos = find_first_not_of_if( is_whitespace );
				remove_prefix( last_pos );
			}

			constexpr void trim_prefix( ) noexcept {
				remove_prefix_while( is_space{ } );
			}

			constexpr basic_string_view trim_prefix_copy( ) const noexcept {
				auto result = *this;
				result.remove_prefix_while( is_space{ } );
				return result;
			}

			template<typename UnaryPred, DAW_REQ_UNARY_PRED( UnaryPred, CharT )>
			constexpr void remove_suffix_while( UnaryPred is_whitespace ) noexcept {
				auto pos = find_last_not_of_if( is_whitespace );
				resize( pos + 1U );
			}

			constexpr void trim_suffix( ) noexcept {
				remove_suffix_while( is_space{ } );
			}

			constexpr basic_string_view trim_suffix_copy( ) const noexcept {
				auto result = *this;
				result = remove_suffix_while( is_space{ } );
				return result;
			}

			constexpr void trim( ) noexcept {
				trim_prefix( is_space{ } );
				trim_suffix( is_space{ } );
			}

			constexpr basic_string_view trim_copy( ) const noexcept {
				auto result = trim_prefix_copy( );
				result.trim_suffix( );
				return result;
			}
		}; // basic_string_view

		// CTAD
		template<typename CharT>
		basic_string_view( CharT const *s, std::size_t count )
		  -> basic_string_view<CharT>;

		template<typename CharT, std::size_t N>
		basic_string_view( CharT const ( &/*string_literal*/ )[N] )
		  -> basic_string_view<CharT, default_string_view_bounds_type>;
		//
		//
		namespace string_view_literals {
			[[nodiscard]] constexpr string_view
			operator"" _sv( char const *str, std::size_t len ) noexcept {
				return string_view{ str, len };
			}

#if defined( __cpp_char8_t )
			[[nodiscard]] constexpr u8string_view
			operator"" _sv( char8_t const *str, std::size_t len ) noexcept {
				return u8string_view{ str, len };
			}
#endif

			[[nodiscard]] constexpr u16string_view
			operator"" _sv( char16_t const *str, std::size_t len ) noexcept {
				return u16string_view{ str, len };
			}

			[[nodiscard]] constexpr u32string_view
			operator"" _sv( char32_t const *str, std::size_t len ) noexcept {
				return u32string_view{ str, len };
			}

			[[nodiscard]] constexpr wstring_view
			operator"" _sv( wchar_t const *str, std::size_t len ) noexcept {
				return wstring_view{ str, len };
			}
		} // namespace string_view_literals

		template<typename CharT, string_view_bounds_type Bounds, typename OStream,
		         std::enable_if_t<daw::traits::is_ostream_like_v<OStream, CharT>,
		                          std::nullptr_t> = nullptr>
		OStream &operator<<( OStream &os, basic_string_view<CharT, Bounds> v ) {
			if( os.good( ) ) {
				auto const size = v.size( );
				auto const w = static_cast<std::size_t>( os.width( ) );
				if( w <= size ) {
					os.write( v.data( ), static_cast<intmax_t>( size ) );
				} else {
					daw::details::sv_insert_aligned( os, v );
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

#undef DAW_REQ_UNARY_PRED
#undef DAW_REQ_CONTIG_CHAR_RANGE
#undef DAW_REQ_CONTIG_CHAR_RANGE
#undef DAW_REQ_CONTIG_CHAR_RANGE_CTOR

