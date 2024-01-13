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
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace daw::string_extract_impl {
#if defined( __GLIBCXX__ )
	using namespace std;
	template<typename CharT, typename Traits, typename Alloc>
	struct basic_string {

		using Char_alloc_type =
		  typename ::std::allocator_traits<Alloc>::template rebind<CharT>::other;

		using Alloc_traits = std::allocator_traits<Char_alloc_type>;

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
			Alloc_hider( pointer _dat, const Alloc &_a )
			  : allocator_type( _a )
			  , m_p( _dat ) {}

			Alloc_hider( pointer _dat, Alloc &&_a = Alloc( ) )
			  : allocator_type( std::move( _a ) )
			  , m_p( _dat ) {}

			pointer m_p; // The actual data.
		};

		Alloc_hider m_dataplus;
		size_type m_string_length;

		enum { s_local_capacity = 15 / sizeof( CharT ) };

		union {
			CharT m_local_buf[s_local_capacity + 1];
			size_type m_allocated_capacity;
		};

		constexpr void m_data( pointer _p ) {
			m_dataplus.m_p = _p;
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

		constexpr void m_set_length( size_type _n ) {
			m_length( _n );
			traits_type::assign( m_data( )[_n], CharT( ) );
		}

		constexpr bool m_is_local( ) const {
			return m_data( ) == m_local_data( );
		}

		void m_dispose( ) {
			if( not m_is_local( ) ) {
				m_destroy( m_allocated_capacity );
			}
		}

		void m_destroy( size_type sz ) noexcept {
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

		size_type m_check( size_type _pos, const char *_s ) const {
			if( _pos > this->size( ) ) {
				__throw_out_of_range_fmt( __N( "%s: _pos (which is %zu) > "
				                               "this->size() (which is %zu)" ),
				                          _s, _pos, this->size( ) );
			}
			return _pos;
		}

		void m_check_length( size_type _n1, size_type _n2, const char *_s ) const {
			if( this->max_size( ) - ( this->size( ) - _n1 ) < _n2 ) {
				__throw_length_error( __N( _s ) );
			}
		}

		// NB: m_limit doesn't check for a bad _pos value.
		size_type m_limit( size_type _pos, size_type _off ) const noexcept {
			const bool _testoff = _off < this->size( ) - _pos;
			return _testoff ? _off : this->size( ) - _pos;
		}

		// True if _Rep and source do not overlap.
		bool m_disjunct( const CharT *_s ) const noexcept {
			return ( less<const CharT *>( )( _s, m_data( ) ) ||
			         less<const CharT *>( )( m_data( ) + this->size( ), _s ) );
		}

		// When _n = 1 way faster than the general multichar
		// traits_type::copy/move/assign.
		static void S_copy( CharT *_d, const CharT *_s, size_type _n ) {
			if( _n == 1 ) {
				traits_type::assign( *_d, *_s );
			} else {
				traits_type::copy( _d, _s, _n );
			}
		}

		static void S_move( CharT *_d, const CharT *_s, size_type _n ) {
			if( _n == 1 ) {
				traits_type::assign( *_d, *_s );
			} else {
				traits_type::move( _d, _s, _n );
			}
		}

		static void S_assign( CharT *_d, size_type _n, CharT _c ) {
			if( _n == 1 ) {
				traits_type::assign( *_d, _c );
			} else {
				traits_type::assign( _d, _n, _c );
			}
		}

		// S_copy_chars is a separate template to permit specialization
		// to optimize for the common case of pointers as iterators.
		template<class Iterator>
		static void S_copy_chars( CharT *_p, Iterator _k1, Iterator _k2 ) {
			for( ; _k1 != _k2; ++_k1, (void)++_p ) {
				traits_type::assign( *_p, *_k1 ); // These types are off.
			}
		}

		static void S_copy_chars( CharT *_p, iterator _k1, iterator _k2 ) noexcept {
			S_copy_chars( _p, _k1.base( ), _k2.base( ) );
		}

		static void S_copy_chars( CharT *_p, const_iterator _k1,
		                          const_iterator _k2 ) noexcept {
			S_copy_chars( _p, _k1.base( ), _k2.base( ) );
		}

		static void S_copy_chars( CharT *_p, CharT *_k1, CharT *_k2 ) noexcept {
			S_copy( _p, _k1, _k2 - _k1 );
		}

		static void S_copy_chars( CharT *_p, const CharT *_k1,
		                          const CharT *_k2 ) noexcept {
			S_copy( _p, _k1, _k2 - _k1 );
		}

		static int S_compare( size_type _n1, size_type _n2 ) noexcept {
			const difference_type _d = difference_type( _n1 - _n2 );

			if( _d > __gnu_cxx::__numeric_traits<int>::__max ) {
				return __gnu_cxx::__numeric_traits<int>::__max;
			} else if( _d < __gnu_cxx::__numeric_traits<int>::__min ) {
				return __gnu_cxx::__numeric_traits<int>::__min;
			} else {
				return int( _d );
			}
		}

		void m_assign( const basic_string & );

		void m_mutate( size_type _pos, size_type _len1, const CharT *_s,
		               size_type _len2 );

		void m_erase( size_type _pos, size_type _n );

		basic_string( ) noexcept( is_nothrow_default_constructible<Alloc>::value )
		  : m_dataplus( m_local_data( ) ) {
			m_use_local_data( );
			m_set_length( 0 );
		}

		template<typename InIterator>
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

			this->S_copy_chars( m_data( ), beg, end );

			guard.m_guarded = 0;

			m_set_length( dnew );
		}

		/**
		 *  @brief  Construct string with copy of value of @a _str.
		 *  @param  _str  Source string.
		 */
		basic_string( const basic_string &_str )
		  : m_dataplus( m_local_data( ), Alloc_traits::_S_select_on_copy(
		                                   _str.m_get_allocator( ) ) ) {
			m_construct( _str.m_data( ), _str.m_data( ) + _str.length( ),
			             std::forward_iterator_tag( ) );
		}

		// _GLIBCXX_RESOLVE_LIB_DEFECTS
		// 2583. no way to supply an allocator for basic_string(str, pos)
		/**
		 *  @brief  Construct string as copy of a substring.
		 *  @param  _str  Source string.
		 *  @param  _pos  Index of first character to copy from.
		 *  @param  _a  Allocator to use.
		 */
		basic_string( const basic_string &_str, size_type _pos,
		              const Alloc &_a = Alloc( ) )
		  : m_dataplus( m_local_data( ), _a ) {
			const CharT *_start =
			  _str.m_data( ) + _str.m_check( _pos, "basic_string::basic_string" );
			m_construct( _start, _start + _str.m_limit( _pos, npos ),
			             std::forward_iterator_tag( ) );
		}

		/**
		 *  @brief  Construct string as copy of a substring.
		 *  @param  _str  Source string.
		 *  @param  _pos  Index of first character to copy from.
		 *  @param  _n  Number of characters to copy.
		 */
		basic_string( const basic_string &_str, size_type _pos, size_type _n )
		  : m_dataplus( m_local_data( ) ) {
			const CharT *_start =
			  _str.m_data( ) + _str.m_check( _pos, "basic_string::basic_string" );
			m_construct( _start, _start + _str.m_limit( _pos, _n ),
			             std::forward_iterator_tag( ) );
		}

		/**
		 *  @brief  Construct string as copy of a substring.
		 *  @param  _str  Source string.
		 *  @param  _pos  Index of first character to copy from.
		 *  @param  _n  Number of characters to copy.
		 *  @param  _a  Allocator to use.
		 */
		basic_string( const basic_string &_str, size_type _pos, size_type _n,
		              const Alloc &_a )
		  : m_dataplus( m_local_data( ), _a ) {
			const CharT *_start =
			  _str.m_data( ) + _str.m_check( _pos, "string::string" );
			m_construct( _start, _start + _str.m_limit( _pos, _n ),
			             std::forward_iterator_tag( ) );
		}

		/**
		 *  @brief  Construct string initialized by a character %array.
		 *  @param  _s  Source character %array.
		 *  @param  _n  Number of characters to copy.
		 *  @param  _a  Allocator to use (default is default allocator).
		 *
		 *  NB: @a _s must have at least @a _n characters, &apos;\\0&apos;
		 *  has no special meaning.
		 */
		basic_string( const CharT *_s, size_type _n, const Alloc &_a = Alloc( ) )
		  : m_dataplus( m_local_data( ), _a ) {
			// NB: Not required, but considered best practice.
			if( _s == 0 && _n > 0 ) {
				throw std::logic_error(
				  "basic_string: "
				  "construction from null is not valid" );
			}
			m_construct( _s, _s + _n, std::forward_iterator_tag( ) );
		}

		/**
		 *  @brief  Construct string as copy of a C string.
		 *  @param  _s  Source C string.
		 *  @param  _a  Allocator to use (default is default allocator).
		 */
		basic_string( const CharT *_s, const Alloc &_a = Alloc( ) )
		  : m_dataplus( m_local_data( ), _a ) {
			// NB: Not required, but considered best practice.
			if( _s == 0 ) {
				throw std::logic_error(
				  "basic_string: "
				  "construction from null is not valid" );
			}
			const CharT *_end = _s + traits_type::length( _s );
			m_construct( _s, _end, forward_iterator_tag( ) );
		}

		/**
		 *  @brief  Construct string as multiple characters.
		 *  @param  _n  Number of characters.
		 *  @param  _c  Character to use.
		 *  @param  _a  Allocator to use (default is default allocator).
		 */

		/**
		 *  @brief  Move construct string.
		 *  @param  _str  Source string.
		 *
		 *  The newly-created string contains the exact contents of @a _str.
		 *  @a _str is a valid, but unspecified string.
		 */
		basic_string( basic_string &&_str ) noexcept
		  : m_dataplus( m_local_data( ), std::move( _str.m_get_allocator( ) ) ) {
			if( _str.m_is_local( ) ) {
				traits_type::copy( m_local_buf, _str.m_local_buf, _str.length( ) + 1 );
			} else {
				m_data( _str.m_data( ) );
				m_capacity( _str.m_allocated_capacity );
			}

			// Must use m_length() here not m_set_length() because
			// basic_stringbuf relies on writing into unallocated capacity so
			// we mess up the contents if we put a '\0' in the string.
			m_length( _str.length( ) );
			_str.m_data( _str.m_local_data( ) );
			_str.m_set_length( 0 );
		}

		/**
		 *  @brief  Construct string from an initializer %list.
		 *  @param  _l  std::initializer_list of characters.
		 *  @param  _a  Allocator to use (default is default allocator).
		 */
		basic_string( initializer_list<CharT> _l, const Alloc &_a = Alloc( ) )
		  : m_dataplus( m_local_data( ), _a ) {
			m_construct( _l.begin( ), _l.end( ), std::forward_iterator_tag( ) );
		}

		basic_string( const basic_string &_str, const Alloc &_a )
		  : m_dataplus( m_local_data( ), _a ) {
			m_construct( _str.begin( ), _str.end( ), std::forward_iterator_tag( ) );
		}

		basic_string( basic_string &&_str,
		              const Alloc &_a ) noexcept( Alloc_traits::_S_always_equal( ) )
		  : m_dataplus( m_local_data( ), _a ) {
			if( _str.m_is_local( ) ) {
				traits_type::copy( m_local_buf, _str.m_local_buf, _str.length( ) + 1 );
				m_length( _str.length( ) );
				_str.m_set_length( 0 );
			} else if( Alloc_traits::_S_always_equal( ) ||
			           _str.get_allocator( ) == _a ) {
				m_data( _str.m_data( ) );
				m_length( _str.length( ) );
				m_capacity( _str.m_allocated_capacity );
				_str.m_data( _str.m_local_buf );
				_str.m_set_length( 0 );
			} else {
				m_construct( _str.begin( ), _str.end( ), std::forward_iterator_tag( ) );
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
		 *  @param  _str  Source string.
		 */
		basic_string &operator=( const basic_string &_str ) {
			return this->assign( _str );
		}

		/**
		 *  @brief  Copy contents of @a s into this string.
		 *  @param  _s  Source null-terminated string.
		 */
		basic_string &operator=( const CharT *_s ) {
			return this->assign( _s );
		}

		/**
		 *  @brief  Set value to string of length 1.
		 *  @param  _c  Source character.
		 *
		 *  Assigning to a character makes this string length 1 and
		 *  (*this)[0] == @a c.
		 */
		basic_string &operator=( CharT _c ) {
			this->assign( 1, _c );
			return *this;
		}

		/**
		 *  @brief  Move assign the value of @a str to this string.
		 *  @param  _str  Source string.
		 *
		 *  The contents of @a str are moved into this string (without copying).
		 *  @a str is a valid, but unspecified string.
		 */
		// _GLIBCXX_RESOLVE_LIB_DEFECTS
		// 2063. Contradictory requirements for string move assignment
		basic_string &operator=( basic_string &&_str ) noexcept(
		  Alloc_traits::_S_nothrow_move( ) ) {
			if( !m_is_local( ) && Alloc_traits::_S_propagate_on_move_assign( ) &&
			    !Alloc_traits::_S_always_equal( ) &&
			    m_get_allocator( ) != _str.m_get_allocator( ) ) {
				// Destroy existing storage before replacing allocator.
				m_destroy( m_allocated_capacity );
				m_data( m_local_data( ) );
				m_set_length( 0 );
			}
			// Replace allocator if POCMA is true.
			std::__alloc_on_move( m_get_allocator( ), _str.m_get_allocator( ) );

			if( _str.m_is_local( ) ) {
				// We've always got room for a short string, just copy it
				// (unless this is a self-move, because that would violate the
				// char_traits::copy precondition that the ranges don't overlap).
				if( _builtin_expect( std::addressof( _str ) != this, true ) ) {
					if( _str.size( ) ) {
						this->S_copy( m_data( ), _str.m_data( ), _str.size( ) );
					}
					m_set_length( _str.size( ) );
				}
			} else if( Alloc_traits::_S_propagate_on_move_assign( ) ||
			           Alloc_traits::_S_always_equal( ) ||
			           m_get_allocator( ) == _str.m_get_allocator( ) ) {
				// Just move the allocated pointer, our allocator can free it.
				pointer _data = nullptr;
				size_type _capacity;
				if( !m_is_local( ) ) {
					if( Alloc_traits::_S_always_equal( ) ) {
						// _str can reuse our existing storage.
						_data = m_data( );
						_capacity = m_allocated_capacity;
					} else { // _str can't use it, so free it.
						m_destroy( m_allocated_capacity );
					}
				}

				m_data( _str.m_data( ) );
				m_length( _str.length( ) );
				m_capacity( _str.m_allocated_capacity );
				if( _data ) {
					_str.m_data( _data );
					_str.m_capacity( _capacity );
				} else {
					_str.m_data( _str.m_local_buf );
				}
			} else { // Need to do a deep copy
				assign( _str );
			}
			_str.clear( );
			return *this;
		}

		/**
		 *  @brief  Set value to string constructed from initializer %list.
		 *  @param  _l  std::initializer_list.
		 */
		basic_string &operator=( initializer_list<CharT> _l ) {
			this->assign( _l.begin( ), _l.size( ) );
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
		 *  @param  _n  Number of characters the %string should contain.
		 *  @param  _c  Character to fill any new elements.
		 *
		 *  This function will %resize the %string to the specified
		 *  number of characters.  If the number is smaller than the
		 *  %string's current size the %string is truncated, otherwise
		 *  the %string is extended and new elements are %set to @a _c.
		 */
		void resize( size_type _n, CharT _c );

		/**
		 *  @brief  Resizes the %string to the specified number of characters.
		 *  @param  _n  Number of characters the %string should contain.
		 *
		 *  This function will resize the %string to the specified length.  If
		 *  the new size is smaller than the %string's current size the %string
		 *  is truncated, otherwise the %string is extended and new characters
		 *  are default-constructed.  For basic types such as char, this means
		 *  setting them to 0.
		 */
		void resize( size_type _n ) {
			this->resize( _n, CharT( ) );
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
#define _cpp_lib_string_resize_and_overwrite 202110L
		template<typename _Operation>
		constexpr void resize_and_overwrite( size_type _n, _Operation _op );
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
		 *  @param  _res_arg  Number of characters required.
		 *  @throw  std::length_error  If @a _res_arg exceeds @c max_size().
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
		void reserve( size_type _res_arg );

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
		 *  @param  _pos  The index of the character to access.
		 *  @return  Read-only (constant) reference to the character.
		 *
		 *  This operator allows for easy, array-style, data access.
		 *  Note that data access with this operator is unchecked and
		 *  out_of_range lookups are not defined. (For checked lookups
		 *  see at().)
		 */
		const_reference operator[]( size_type _pos ) const noexcept {
			_glibcxx_assert( _pos <= size( ) );
			return m_data( )[_pos];
		}

		/**
		 *  @brief  Subscript access to the data contained in the %string.
		 *  @param  _pos  The index of the character to access.
		 *  @return  Read/write reference to the character.
		 *
		 *  This operator allows for easy, array-style, data access.
		 *  Note that data access with this operator is unchecked and
		 *  out_of_range lookups are not defined. (For checked lookups
		 *  see at().)
		 */
		reference operator[]( size_type _pos ) {
			// Allow pos == size() both in C++98 mode, as v3 extension,
			// and in C++11 mode.
			_glibcxx_assert( _pos <= size( ) );
			// In pedantic mode be strict in C++98 mode.
			_GLIBCXX_DEBUG_PEDASSERT( __cplusplus >= 201103L || _pos < size( ) );
			return m_data( )[_pos];
		}

		/**
		 *  @brief  Provides access to the data contained in the %string.
		 *  @param _n The index of the character to access.
		 *  @return  Read-only (const) reference to the character.
		 *  @throw  std::out_of_range  If @a n is an invalid index.
		 *
		 *  This function provides for safer data access.  The parameter is
		 *  first checked that it is in the range of the string.  The function
		 *  throws out_of_range if the check fails.
		 */
		const_reference at( size_type _n ) const {
			if( _n >= this->size( ) ) {
				__throw_out_of_range_fmt( __N( "basic_string::at: _n "
				                               "(which is %zu) >= this->size() "
				                               "(which is %zu)" ),
				                          _n, this->size( ) );
			}
			return m_data( )[_n];
		}

		/**
		 *  @brief  Provides access to the data contained in the %string.
		 *  @param _n The index of the character to access.
		 *  @return  Read/write reference to the character.
		 *  @throw  std::out_of_range  If @a n is an invalid index.
		 *
		 *  This function provides for safer data access.  The parameter is
		 *  first checked that it is in the range of the string.  The function
		 *  throws out_of_range if the check fails.
		 */
		reference at( size_type _n ) {
			if( _n >= size( ) ) {
				__throw_out_of_range_fmt( __N( "basic_string::at: _n "
				                               "(which is %zu) >= this->size() "
				                               "(which is %zu)" ),
				                          _n, this->size( ) );
			}
			return m_data( )[_n];
		}

		/**
		 *  Returns a read/write reference to the data at the first
		 *  element of the %string.
		 */
		reference front( ) noexcept {
			_glibcxx_assert( !empty( ) );
			return operator[]( 0 );
		}

		/**
		 *  Returns a read-only (constant) reference to the data at the first
		 *  element of the %string.
		 */
		const_reference front( ) const noexcept {
			_glibcxx_assert( !empty( ) );
			return operator[]( 0 );
		}

		/**
		 *  Returns a read/write reference to the data at the last
		 *  element of the %string.
		 */
		reference back( ) noexcept {
			_glibcxx_assert( !empty( ) );
			return operator[]( this->size( ) - 1 );
		}

		/**
		 *  Returns a read-only (constant) reference to the data at the
		 *  last element of the %string.
		 */
		const_reference back( ) const noexcept {
			_glibcxx_assert( !empty( ) );
			return operator[]( this->size( ) - 1 );
		}

		/**
		 *  @brief  Append a single character.
		 *  @param _c  Character to append.
		 */
		void push_back( CharT _c ) {
			const size_type _size = this->size( );
			if( _size + 1 > this->capacity( ) ) {
				this->m_mutate( _size, size_type( 0 ), 0, size_type( 1 ) );
			}
			traits_type::assign( this->m_data( )[_size], _c );
			this->m_set_length( _size + 1 );
		}

	private:
		template<class Integer>
		basic_string &m_replace_dispatch( const_iterator _i1, const_iterator _i2,
		                                  Integer _n, Integer _val, __true_type ) {
			return m_replace_aux( _i1 - begin( ), _i2 - _i1, _n, _val );
		}

		template<class InputIterator>
		basic_string &m_replace_dispatch( const_iterator _i1, const_iterator _i2,
		                                  InputIterator _k1, InputIterator _k2,
		                                  __false_type );

		basic_string &m_replace_aux( size_type _pos1, size_type _n1, size_type _n2,
		                             CharT _c );

		basic_string &m_replace( size_type _pos, size_type _len1, const CharT *_s,
		                         const size_type _len2 );

		basic_string &m_append( const CharT *_s, size_type _n );

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
