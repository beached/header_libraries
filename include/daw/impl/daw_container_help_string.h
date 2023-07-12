// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once
#include "daw/daw_cpp_feature_check.h"
#include "daw_container_help_impl.h"
#include "daw_container_help_ins_ext.h"

#include <cassert>
#include <cstddef>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

namespace daw::string_extract_impl {
#if defined( __GLIBCXX__ )
	using namespace std;
	template<typename CharT, typename Traits, typename Alloc>
	struct basic_string {

		using Char_alloc_type = typename ::__gnu_cxx::__alloc_traits<
		  Alloc>::template rebind<CharT>::other;

		using Alloc_traits = __gnu_cxx::__alloc_traits<Char_alloc_type>;

		using traits_type = Traits;
		using value_type = typename Traits::char_type;
		using allocator_type = Char_alloc_type;
		using size_type = typename Alloc_traits::size_type;
		using difference_type = typename Alloc_traits::difference_type;
		using reference = typename Alloc_traits::reference;
		using const_reference = typename Alloc_traits::const_reference;
		using pointer = typename Alloc_traits::pointer;
		using const_pointer = typename Alloc_traits::const_pointer;
		using iterator = __gnu_cxx::__normal_iterator<pointer, basic_string>;
		using const_iterator =
		  __gnu_cxx::__normal_iterator<const_pointer, basic_string>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;
		using reverse_iterator = std::reverse_iterator<iterator>;

		static constexpr size_type npos = static_cast<size_type>( -1 );

		struct Alloc_hider : allocator_type {
			Alloc_hider( pointer __dat, const Alloc &__a )
			  : allocator_type( __a )
			  , m_p( __dat ) {}

			Alloc_hider( pointer __dat, Alloc &&__a = Alloc( ) )
			  : allocator_type( std::move( __a ) )
			  , m_p( __dat ) {}

			pointer m_p; // The actual data.
		};

		Alloc_hider m_dataplus;
		size_type m_string_length;

		enum { s_local_capacity = 15 / sizeof( CharT ) };

		union {
			CharT m_local_buf[s_local_capacity + 1];
			size_type m_allocated_capacity;
		};

		constexpr void m_data( pointer __p ) {
			m_dataplus.m_p = __p;
		}

		constexpr void m_length( size_type len ) {
			m_string_length = len;
		}

		constexpr pointer m_data( ) const {
			return m_dataplus.m_p;
		}

		constexpr pointer m_local_data( ) {
			return pointer( m_local_buf );
		}

		constexpr const_pointer m_local_data( ) const {
			return std::pointer_traits<const_pointer>::pointer_to( *m_local_buf );
		}

		constexpr void m_capacity( size_type cap ) {
			m_allocated_capacity = cap;
		}

		constexpr void m_set_length( size_type __n ) {
			m_length( __n );
			traits_type::assign( m_data( )[__n], CharT( ) );
		}

		constexpr bool m_is_local( ) const {
			return m_data( ) == m_local_data( );
		}

		void m_dispose( ) {
			if( not m_is_local( ) ) {
				m_destroy( m_allocated_capacity );
			}
		}

		void m_destroy( size_type sz ) throw( ) {
			Alloc_traits::deallocate( m_get_allocator( ), m_data( ), sz + 1 );
		}

		constexpr allocator_type &m_get_allocator( ) {
			return m_dataplus;
		}

		constexpr const allocator_type &m_get_allocator( ) const {
			return m_dataplus;
		}

		// Ensure that m_local_buf is the active member of the union.
		__attribute__( ( __always_inline__ ) ) constexpr pointer
		m_use_local_data( ) noexcept {
			return m_local_data( );
		}

		size_type m_check( size_type __pos, const char *__s ) const {
			if( __pos > this->size( ) ) {
				__throw_out_of_range_fmt( __N( "%s: __pos (which is %zu) > "
				                               "this->size() (which is %zu)" ),
				                          __s, __pos, this->size( ) );
			}
			return __pos;
		}

		void m_check_length( size_type __n1, size_type __n2,
		                     const char *__s ) const {
			if( this->max_size( ) - ( this->size( ) - __n1 ) < __n2 ) {
				__throw_length_error( __N( __s ) );
			}
		}

		// NB: m_limit doesn't check for a bad __pos value.
		size_type m_limit( size_type __pos, size_type __off ) const noexcept {
			const bool __testoff = __off < this->size( ) - __pos;
			return __testoff ? __off : this->size( ) - __pos;
		}

		// True if _Rep and source do not overlap.
		bool m_disjunct( const CharT *__s ) const noexcept {
			return ( less<const CharT *>( )( __s, m_data( ) ) ||
			         less<const CharT *>( )( m_data( ) + this->size( ), __s ) );
		}

		// When __n = 1 way faster than the general multichar
		// traits_type::copy/move/assign.
		static void _S_copy( CharT *__d, const CharT *__s, size_type __n ) {
			if( __n == 1 ) {
				traits_type::assign( *__d, *__s );
			} else {
				traits_type::copy( __d, __s, __n );
			}
		}

		static void _S_move( CharT *__d, const CharT *__s, size_type __n ) {
			if( __n == 1 ) {
				traits_type::assign( *__d, *__s );
			} else {
				traits_type::move( __d, __s, __n );
			}
		}

		static void _S_assign( CharT *__d, size_type __n, CharT __c ) {
			if( __n == 1 ) {
				traits_type::assign( *__d, __c );
			} else {
				traits_type::assign( __d, __n, __c );
			}
		}

		// _S_copy_chars is a separate template to permit specialization
		// to optimize for the common case of pointers as iterators.
		template<class _Iterator>
		static void _S_copy_chars( CharT *__p, _Iterator __k1, _Iterator __k2 ) {
			for( ; __k1 != __k2; ++__k1, (void)++__p ) {
				traits_type::assign( *__p, *__k1 ); // These types are off.
			}
		}

		static void _S_copy_chars( CharT *__p, iterator __k1,
		                           iterator __k2 ) noexcept {
			_S_copy_chars( __p, __k1.base( ), __k2.base( ) );
		}

		static void _S_copy_chars( CharT *__p, const_iterator __k1,
		                           const_iterator __k2 ) noexcept {
			_S_copy_chars( __p, __k1.base( ), __k2.base( ) );
		}

		static void _S_copy_chars( CharT *__p, CharT *__k1, CharT *__k2 ) noexcept {
			_S_copy( __p, __k1, __k2 - __k1 );
		}

		static void _S_copy_chars( CharT *__p, const CharT *__k1,
		                           const CharT *__k2 ) noexcept {
			_S_copy( __p, __k1, __k2 - __k1 );
		}

		static int _S_compare( size_type __n1, size_type __n2 ) noexcept {
			const difference_type __d = difference_type( __n1 - __n2 );

			if( __d > __gnu_cxx::__numeric_traits<int>::__max ) {
				return __gnu_cxx::__numeric_traits<int>::__max;
			} else if( __d < __gnu_cxx::__numeric_traits<int>::__min ) {
				return __gnu_cxx::__numeric_traits<int>::__min;
			} else {
				return int( __d );
			}
		}

		void m_assign( const basic_string & );

		void m_mutate( size_type __pos, size_type __len1, const CharT *__s,
		               size_type __len2 );

		void m_erase( size_type __pos, size_type __n );

		basic_string( ) noexcept( is_nothrow_default_constructible<Alloc>::value )
		  : m_dataplus( m_local_data( ) ) {
			m_use_local_data( );
			m_set_length( 0 );
		}

		template<typename _InIterator>
		void m_construct( CharT const *beg, CharT const *end,
		                  std::random_access_iterator_tag ) {
			auto const dnew = static_cast<size_type>( std::distance( beg, end ) );

			if( dnew > size_type( s_local_capacity ) ) {
				m_data( m_create( dnew, size_type( 0 ) ) );
				m_capacity( dnew );
			} else {
				m_use_local_data( );
			}

			// Check for out_of_range and length_error exceptions.
			struct Guard {
				basic_string *m_guarded;
				explicit Guard( basic_string *s )
				  : m_guarded( s ) {}

				~Guard( ) {
					if( m_guarded ) {
						m_guarded->m_dispose( );
					}
				}

			} guard( this );

			this->_S_copy_chars( m_data( ), beg, end );

			guard.m_guarded = 0;

			m_set_length( dnew );
		}

		/**
		 *  @brief  Construct string with copy of value of @a __str.
		 *  @param  __str  Source string.
		 */
		basic_string( const basic_string &__str )
		  : m_dataplus( m_local_data( ), Alloc_traits::_S_select_on_copy(
		                                   __str.m_get_allocator( ) ) ) {
			m_construct( __str.m_data( ), __str.m_data( ) + __str.length( ),
			             std::forward_iterator_tag( ) );
		}

		// _GLIBCXX_RESOLVE_LIB_DEFECTS
		// 2583. no way to supply an allocator for basic_string(str, pos)
		/**
		 *  @brief  Construct string as copy of a substring.
		 *  @param  __str  Source string.
		 *  @param  __pos  Index of first character to copy from.
		 *  @param  __a  Allocator to use.
		 */
		basic_string( const basic_string &__str, size_type __pos,
		              const Alloc &__a = Alloc( ) )
		  : m_dataplus( m_local_data( ), __a ) {
			const CharT *__start =
			  __str.m_data( ) + __str.m_check( __pos, "basic_string::basic_string" );
			m_construct( __start, __start + __str.m_limit( __pos, npos ),
			             std::forward_iterator_tag( ) );
		}

		/**
		 *  @brief  Construct string as copy of a substring.
		 *  @param  __str  Source string.
		 *  @param  __pos  Index of first character to copy from.
		 *  @param  __n  Number of characters to copy.
		 */
		basic_string( const basic_string &__str, size_type __pos, size_type __n )
		  : m_dataplus( m_local_data( ) ) {
			const CharT *__start =
			  __str.m_data( ) + __str.m_check( __pos, "basic_string::basic_string" );
			m_construct( __start, __start + __str.m_limit( __pos, __n ),
			             std::forward_iterator_tag( ) );
		}

		/**
		 *  @brief  Construct string as copy of a substring.
		 *  @param  __str  Source string.
		 *  @param  __pos  Index of first character to copy from.
		 *  @param  __n  Number of characters to copy.
		 *  @param  __a  Allocator to use.
		 */
		basic_string( const basic_string &__str, size_type __pos, size_type __n,
		              const Alloc &__a )
		  : m_dataplus( m_local_data( ), __a ) {
			const CharT *__start =
			  __str.m_data( ) + __str.m_check( __pos, "string::string" );
			m_construct( __start, __start + __str.m_limit( __pos, __n ),
			             std::forward_iterator_tag( ) );
		}

		/**
		 *  @brief  Construct string initialized by a character %array.
		 *  @param  __s  Source character %array.
		 *  @param  __n  Number of characters to copy.
		 *  @param  __a  Allocator to use (default is default allocator).
		 *
		 *  NB: @a __s must have at least @a __n characters, &apos;\\0&apos;
		 *  has no special meaning.
		 */
		basic_string( const CharT *__s, size_type __n, const Alloc &__a = Alloc( ) )
		  : m_dataplus( m_local_data( ), __a ) {
			// NB: Not required, but considered best practice.
			if( __s == 0 && __n > 0 ) {
				std::__throw_logic_error(
				  __N( "basic_string: "
				       "construction from null is not valid" ) );
			}
			m_construct( __s, __s + __n, std::forward_iterator_tag( ) );
		}

		/**
		 *  @brief  Construct string as copy of a C string.
		 *  @param  __s  Source C string.
		 *  @param  __a  Allocator to use (default is default allocator).
		 */
		basic_string( const CharT *__s, const Alloc &__a = Alloc( ) )
		  : m_dataplus( m_local_data( ), __a ) {
			// NB: Not required, but considered best practice.
			if( __s == 0 ) {
				std::__throw_logic_error(
				  __N( "basic_string: "
				       "construction from null is not valid" ) );
			}
			const CharT *__end = __s + traits_type::length( __s );
			m_construct( __s, __end, forward_iterator_tag( ) );
		}

		/**
		 *  @brief  Construct string as multiple characters.
		 *  @param  __n  Number of characters.
		 *  @param  __c  Character to use.
		 *  @param  __a  Allocator to use (default is default allocator).
		 */

		/**
		 *  @brief  Move construct string.
		 *  @param  __str  Source string.
		 *
		 *  The newly-created string contains the exact contents of @a __str.
		 *  @a __str is a valid, but unspecified string.
		 */
		basic_string( basic_string &&__str ) noexcept
		  : m_dataplus( m_local_data( ), std::move( __str.m_get_allocator( ) ) ) {
			if( __str.m_is_local( ) ) {
				traits_type::copy( m_local_buf, __str.m_local_buf,
				                   __str.length( ) + 1 );
			} else {
				m_data( __str.m_data( ) );
				m_capacity( __str.m_allocated_capacity );
			}

			// Must use m_length() here not m_set_length() because
			// basic_stringbuf relies on writing into unallocated capacity so
			// we mess up the contents if we put a '\0' in the string.
			m_length( __str.length( ) );
			__str.m_data( __str.m_local_data( ) );
			__str.m_set_length( 0 );
		}

		/**
		 *  @brief  Construct string from an initializer %list.
		 *  @param  __l  std::initializer_list of characters.
		 *  @param  __a  Allocator to use (default is default allocator).
		 */
		basic_string( initializer_list<CharT> __l, const Alloc &__a = Alloc( ) )
		  : m_dataplus( m_local_data( ), __a ) {
			m_construct( __l.begin( ), __l.end( ), std::forward_iterator_tag( ) );
		}

		basic_string( const basic_string &__str, const Alloc &__a )
		  : m_dataplus( m_local_data( ), __a ) {
			m_construct( __str.begin( ), __str.end( ), std::forward_iterator_tag( ) );
		}

		basic_string( basic_string &&__str, const Alloc &__a ) noexcept(
		  Alloc_traits::_S_always_equal( ) )
		  : m_dataplus( m_local_data( ), __a ) {
			if( __str.m_is_local( ) ) {
				traits_type::copy( m_local_buf, __str.m_local_buf,
				                   __str.length( ) + 1 );
				m_length( __str.length( ) );
				__str.m_set_length( 0 );
			} else if( Alloc_traits::_S_always_equal( ) ||
			           __str.get_allocator( ) == __a ) {
				m_data( __str.m_data( ) );
				m_length( __str.length( ) );
				m_capacity( __str.m_allocated_capacity );
				__str.m_data( __str.m_local_buf );
				__str.m_set_length( 0 );
			} else {
				m_construct( __str.begin( ), __str.end( ),
				             std::forward_iterator_tag( ) );
			}
		}

		/**
		 *  @brief  Destroy the string instance.
		 */
		~basic_string( ) {
			m_dispose( );
		}

		/**
		 *  @brief  Assign the value of @a str to this string.
		 *  @param  __str  Source string.
		 */
		basic_string &operator=( const basic_string &__str ) {
			return this->assign( __str );
		}

		/**
		 *  @brief  Copy contents of @a s into this string.
		 *  @param  __s  Source null-terminated string.
		 */
		basic_string &operator=( const CharT *__s ) {
			return this->assign( __s );
		}

		/**
		 *  @brief  Set value to string of length 1.
		 *  @param  __c  Source character.
		 *
		 *  Assigning to a character makes this string length 1 and
		 *  (*this)[0] == @a c.
		 */
		basic_string &operator=( CharT __c ) {
			this->assign( 1, __c );
			return *this;
		}

		/**
		 *  @brief  Move assign the value of @a str to this string.
		 *  @param  __str  Source string.
		 *
		 *  The contents of @a str are moved into this string (without copying).
		 *  @a str is a valid, but unspecified string.
		 */
		// _GLIBCXX_RESOLVE_LIB_DEFECTS
		// 2063. Contradictory requirements for string move assignment
		basic_string &operator=( basic_string &&__str ) noexcept(
		  Alloc_traits::_S_nothrow_move( ) ) {
			if( !m_is_local( ) && Alloc_traits::_S_propagate_on_move_assign( ) &&
			    !Alloc_traits::_S_always_equal( ) &&
			    m_get_allocator( ) != __str.m_get_allocator( ) ) {
				// Destroy existing storage before replacing allocator.
				m_destroy( m_allocated_capacity );
				m_data( m_local_data( ) );
				m_set_length( 0 );
			}
			// Replace allocator if POCMA is true.
			std::__alloc_on_move( m_get_allocator( ), __str.m_get_allocator( ) );

			if( __str.m_is_local( ) ) {
				// We've always got room for a short string, just copy it
				// (unless this is a self-move, because that would violate the
				// char_traits::copy precondition that the ranges don't overlap).
				if( __builtin_expect( std::__addressof( __str ) != this, true ) ) {
					if( __str.size( ) ) {
						this->_S_copy( m_data( ), __str.m_data( ), __str.size( ) );
					}
					m_set_length( __str.size( ) );
				}
			} else if( Alloc_traits::_S_propagate_on_move_assign( ) ||
			           Alloc_traits::_S_always_equal( ) ||
			           m_get_allocator( ) == __str.m_get_allocator( ) ) {
				// Just move the allocated pointer, our allocator can free it.
				pointer __data = nullptr;
				size_type __capacity;
				if( !m_is_local( ) ) {
					if( Alloc_traits::_S_always_equal( ) ) {
						// __str can reuse our existing storage.
						__data = m_data( );
						__capacity = m_allocated_capacity;
					} else { // __str can't use it, so free it.
						m_destroy( m_allocated_capacity );
					}
				}

				m_data( __str.m_data( ) );
				m_length( __str.length( ) );
				m_capacity( __str.m_allocated_capacity );
				if( __data ) {
					__str.m_data( __data );
					__str.m_capacity( __capacity );
				} else {
					__str.m_data( __str.m_local_buf );
				}
			} else { // Need to do a deep copy
				assign( __str );
			}
			__str.clear( );
			return *this;
		}

		/**
		 *  @brief  Set value to string constructed from initializer %list.
		 *  @param  __l  std::initializer_list.
		 */
		basic_string &operator=( initializer_list<CharT> __l ) {
			this->assign( __l.begin( ), __l.size( ) );
			return *this;
		}

		// Iterators:
		/**
		 *  Returns a read/write iterator that points to the first character in
		 *  the %string.
		 */
		iterator begin( ) noexcept {
			return iterator( m_data( ) );
		}

		/**
		 *  Returns a read-only (constant) iterator that points to the first
		 *  character in the %string.
		 */
		const_iterator begin( ) const noexcept {
			return const_iterator( m_data( ) );
		}

		/**
		 *  Returns a read/write iterator that points one past the last
		 *  character in the %string.
		 */
		iterator end( ) noexcept {
			return iterator( m_data( ) + this->size( ) );
		}

		/**
		 *  Returns a read-only (constant) iterator that points one past the
		 *  last character in the %string.
		 */
		const_iterator end( ) const noexcept {
			return const_iterator( m_data( ) + this->size( ) );
		}

		/**
		 *  Returns a read/write reverse iterator that points to the last
		 *  character in the %string.  Iteration is done in reverse element
		 *  order.
		 */
		reverse_iterator rbegin( ) noexcept {
			return reverse_iterator( this->end( ) );
		}

		/**
		 *  Returns a read-only (constant) reverse iterator that points
		 *  to the last character in the %string.  Iteration is done in
		 *  reverse element order.
		 */
		const_reverse_iterator rbegin( ) const noexcept {
			return const_reverse_iterator( this->end( ) );
		}

		/**
		 *  Returns a read/write reverse iterator that points to one before the
		 *  first character in the %string.  Iteration is done in reverse
		 *  element order.
		 */
		reverse_iterator rend( ) noexcept {
			return reverse_iterator( this->begin( ) );
		}

		/**
		 *  Returns a read-only (constant) reverse iterator that points
		 *  to one before the first character in the %string.  Iteration
		 *  is done in reverse element order.
		 */
		const_reverse_iterator rend( ) const noexcept {
			return const_reverse_iterator( this->begin( ) );
		}

#if __cplusplus >= 201103L
		/**
		 *  Returns a read-only (constant) iterator that points to the first
		 *  character in the %string.
		 */
		const_iterator cbegin( ) const noexcept {
			return const_iterator( this->m_data( ) );
		}

		/**
		 *  Returns a read-only (constant) iterator that points one past the
		 *  last character in the %string.
		 */
		const_iterator cend( ) const noexcept {
			return const_iterator( this->m_data( ) + this->size( ) );
		}

		/**
		 *  Returns a read-only (constant) reverse iterator that points
		 *  to the last character in the %string.  Iteration is done in
		 *  reverse element order.
		 */
		const_reverse_iterator crbegin( ) const noexcept {
			return const_reverse_iterator( this->end( ) );
		}

		/**
		 *  Returns a read-only (constant) reverse iterator that points
		 *  to one before the first character in the %string.  Iteration
		 *  is done in reverse element order.
		 */
		const_reverse_iterator crend( ) const noexcept {
			return const_reverse_iterator( this->begin( ) );
		}
#endif

	public:
		// Capacity:
		///  Returns the number of characters in the string, not including any
		///  null-termination.
		size_type size( ) const noexcept {
			return m_string_length;
		}

		///  Returns the number of characters in the string, not including any
		///  null-termination.
		size_type length( ) const noexcept {
			return m_string_length;
		}

		///  Returns the size() of the largest possible %string.
		size_type max_size( ) const noexcept {
			return ( Alloc_traits::max_size( m_get_allocator( ) ) - 1 ) / 2;
		}

		/**
		 *  @brief  Resizes the %string to the specified number of characters.
		 *  @param  __n  Number of characters the %string should contain.
		 *  @param  __c  Character to fill any new elements.
		 *
		 *  This function will %resize the %string to the specified
		 *  number of characters.  If the number is smaller than the
		 *  %string's current size the %string is truncated, otherwise
		 *  the %string is extended and new elements are %set to @a __c.
		 */
		void resize( size_type __n, CharT __c );

		/**
		 *  @brief  Resizes the %string to the specified number of characters.
		 *  @param  __n  Number of characters the %string should contain.
		 *
		 *  This function will resize the %string to the specified length.  If
		 *  the new size is smaller than the %string's current size the %string
		 *  is truncated, otherwise the %string is extended and new characters
		 *  are default-constructed.  For basic types such as char, this means
		 *  setting them to 0.
		 */
		void resize( size_type __n ) {
			this->resize( __n, CharT( ) );
		}

#if __cplusplus >= 201103L
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
		///  A non-binding request to reduce capacity() to size().
		void shrink_to_fit( ) noexcept {
			reserve( );
		}
#pragma GCC diagnostic pop
#endif

#if __cplusplus > 202002L
#define __cpp_lib_string_resize_and_overwrite 202110L
		template<typename _Operation>
		constexpr void resize_and_overwrite( size_type __n, _Operation __op );
#endif

		/**
		 *  Returns the total number of characters that the %string can hold
		 *  before needing to allocate more memory.
		 */
		size_type capacity( ) const noexcept {
			return m_is_local( ) ? size_type( s_local_capacity )
			                     : m_allocated_capacity;
		}

		/**
		 *  @brief  Attempt to preallocate enough memory for specified number of
		 *          characters.
		 *  @param  __res_arg  Number of characters required.
		 *  @throw  std::length_error  If @a __res_arg exceeds @c max_size().
		 *
		 *  This function attempts to reserve enough memory for the
		 *  %string to hold the specified number of characters.  If the
		 *  number requested is more than max_size(), length_error is
		 *  thrown.
		 *
		 *  The advantage of this function is that if optimal code is a
		 *  necessity and the user can determine the string length that will be
		 *  required, the user can reserve the memory in %advance, and thus
		 *  prevent a possible reallocation of memory and copying of %string
		 *  data.
		 */
		void reserve( size_type __res_arg );

		/**
		 *  Equivalent to shrink_to_fit().
		 */
#if __cplusplus > 201703L
		[[deprecated( "use shrink_to_fit() instead" )]]
#endif
		void
		reserve( );

		/**
		 *  Erases the string, making it empty.
		 */
		void clear( ) noexcept {
			m_set_length( 0 );
		}

		/**
		 *  Returns true if the %string is empty.  Equivalent to
		 *  <code>*this == ""</code>.
		 */
		[[nodiscard]] bool empty( ) const noexcept {
			return this->size( ) == 0;
		}

		// Element access:
		/**
		 *  @brief  Subscript access to the data contained in the %string.
		 *  @param  __pos  The index of the character to access.
		 *  @return  Read-only (constant) reference to the character.
		 *
		 *  This operator allows for easy, array-style, data access.
		 *  Note that data access with this operator is unchecked and
		 *  out_of_range lookups are not defined. (For checked lookups
		 *  see at().)
		 */
		const_reference operator[]( size_type __pos ) const noexcept {
			__glibcxx_assert( __pos <= size( ) );
			return m_data( )[__pos];
		}

		/**
		 *  @brief  Subscript access to the data contained in the %string.
		 *  @param  __pos  The index of the character to access.
		 *  @return  Read/write reference to the character.
		 *
		 *  This operator allows for easy, array-style, data access.
		 *  Note that data access with this operator is unchecked and
		 *  out_of_range lookups are not defined. (For checked lookups
		 *  see at().)
		 */
		reference operator[]( size_type __pos ) {
			// Allow pos == size() both in C++98 mode, as v3 extension,
			// and in C++11 mode.
			__glibcxx_assert( __pos <= size( ) );
			// In pedantic mode be strict in C++98 mode.
			_GLIBCXX_DEBUG_PEDASSERT( __cplusplus >= 201103L || __pos < size( ) );
			return m_data( )[__pos];
		}

		/**
		 *  @brief  Provides access to the data contained in the %string.
		 *  @param __n The index of the character to access.
		 *  @return  Read-only (const) reference to the character.
		 *  @throw  std::out_of_range  If @a n is an invalid index.
		 *
		 *  This function provides for safer data access.  The parameter is
		 *  first checked that it is in the range of the string.  The function
		 *  throws out_of_range if the check fails.
		 */
		const_reference at( size_type __n ) const {
			if( __n >= this->size( ) ) {
				__throw_out_of_range_fmt( __N( "basic_string::at: __n "
				                               "(which is %zu) >= this->size() "
				                               "(which is %zu)" ),
				                          __n, this->size( ) );
			}
			return m_data( )[__n];
		}

		/**
		 *  @brief  Provides access to the data contained in the %string.
		 *  @param __n The index of the character to access.
		 *  @return  Read/write reference to the character.
		 *  @throw  std::out_of_range  If @a n is an invalid index.
		 *
		 *  This function provides for safer data access.  The parameter is
		 *  first checked that it is in the range of the string.  The function
		 *  throws out_of_range if the check fails.
		 */
		reference at( size_type __n ) {
			if( __n >= size( ) ) {
				__throw_out_of_range_fmt( __N( "basic_string::at: __n "
				                               "(which is %zu) >= this->size() "
				                               "(which is %zu)" ),
				                          __n, this->size( ) );
			}
			return m_data( )[__n];
		}

		/**
		 *  Returns a read/write reference to the data at the first
		 *  element of the %string.
		 */
		reference front( ) noexcept {
			__glibcxx_assert( !empty( ) );
			return operator[]( 0 );
		}

		/**
		 *  Returns a read-only (constant) reference to the data at the first
		 *  element of the %string.
		 */
		const_reference front( ) const noexcept {
			__glibcxx_assert( !empty( ) );
			return operator[]( 0 );
		}

		/**
		 *  Returns a read/write reference to the data at the last
		 *  element of the %string.
		 */
		reference back( ) noexcept {
			__glibcxx_assert( !empty( ) );
			return operator[]( this->size( ) - 1 );
		}

		/**
		 *  Returns a read-only (constant) reference to the data at the
		 *  last element of the %string.
		 */
		const_reference back( ) const noexcept {
			__glibcxx_assert( !empty( ) );
			return operator[]( this->size( ) - 1 );
		}

		/**
		 *  @brief  Append a single character.
		 *  @param __c  Character to append.
		 */
		void push_back( CharT __c ) {
			const size_type __size = this->size( );
			if( __size + 1 > this->capacity( ) ) {
				this->m_mutate( __size, size_type( 0 ), 0, size_type( 1 ) );
			}
			traits_type::assign( this->m_data( )[__size], __c );
			this->m_set_length( __size + 1 );
		}

	private:
		template<class _Integer>
		basic_string &m_replace_dispatch( const_iterator __i1, const_iterator __i2,
		                                  _Integer __n, _Integer __val,
		                                  __true_type ) {
			return m_replace_aux( __i1 - begin( ), __i2 - __i1, __n, __val );
		}

		template<class _InputIterator>
		basic_string &m_replace_dispatch( const_iterator __i1, const_iterator __i2,
		                                  _InputIterator __k1, _InputIterator __k2,
		                                  __false_type );

		basic_string &m_replace_aux( size_type __pos1, size_type __n1,
		                             size_type __n2, CharT __c );

		basic_string &m_replace( size_type __pos, size_type __len1,
		                         const CharT *__s, const size_type __len2 );

		basic_string &m_append( const CharT *__s, size_type __n );

		// String operations:
		/**
		 *  @brief  Return const pointer to null-terminated contents.
		 *
		 *  This is a handle to internal data.  Do not modify or dire things may
		 *  happen.
		 */
		const CharT *c_str( ) const noexcept {
			return m_data( );
		}

		/**
		 *  @brief  Return const pointer to contents.
		 *
		 *  This is a pointer to internal data.  It is undefined to modify
		 *  the contents through the returned pointer. To get a pointer that
		 *  allows modifying the contents use @c &str[0] instead,
		 *  (or in C++17 the non-const @c str.data() overload).
		 */
		const CharT *data( ) const noexcept {
			return m_data( );
		}

		/**
		 *  @brief  Return non-const pointer to contents.
		 *
		 *  This is a pointer to the character sequence held by the string.
		 *  Modifying the characters in the sequence is allowed.
		 */
		CharT *data( ) noexcept {
			return m_data( );
		}

		/**
		 *  @brief  Return copy of allocator used to construct this string.
		 */
		allocator_type get_allocator( ) const noexcept {
			return m_get_allocator( );
		}
	};
#endif
} // namespace daw::string_extract_impl
