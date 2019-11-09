// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once
#include <array>
#include <cstddef>
#include <iterator>
#include <memory>
#include <numeric>
#include <type_traits>
#include <utility>
#include <vector>

#include "../cpp_17.h"
#include "../daw_traits.h"

namespace daw {
	namespace inpit_impl {
		enum fn_ary_item {
			OpDel,
			OpEqual,
			OpNotEqual,
			OpPrefixInc,
			OpArrow,
			OpClone
		};

		template<typename Iterator>
		static decltype( auto ) as_orig( std::byte *p ) {
			return *reinterpret_cast<Iterator *>( p );
		}

		using op_del_t = daw::traits::fn_t<void( std::byte * )>;
		using op_cmp_t = daw::traits::fn_t<bool( std::byte *, std::byte * )>;
		using op_prefix_inc_t = daw::traits::fn_t<void( std::byte * )>;
		template<typename Value>
		using op_arrow_t = daw::traits::fn_t<Value const *( std::byte * )>;
		using op_clone_t = daw::traits::fn_t<std::byte *( std::byte * )>;

		template<typename Value>
		using vtable_t = std::tuple<op_del_t, op_cmp_t, op_cmp_t, op_prefix_inc_t,
		                            op_arrow_t<Value>, op_clone_t>;

		template<typename Iterator, typename Value>
		static inline constexpr vtable_t<Value> vtable{
		  +[]( std::byte *p ) -> void {
			  // op_del
			  delete reinterpret_cast<Iterator *>( p );
		  },
		  +[]( std::byte *l, std::byte *r ) -> bool {
			  // op_equal
			  return as_orig<Iterator>( l ) == as_orig<Iterator>( r );
		  },
		  +[]( std::byte *l, std::byte *r ) -> bool {
			  // op_not_equal
			  return as_orig<Iterator>( l ) != as_orig<Iterator>( r );
		  },
		  +[]( std::byte *p ) -> void {
			  // op_prefix_inc
			  as_orig<Iterator>( p ) = as_orig<Iterator>( p ) + 1;
		  },
		  +[]( std::byte *p ) -> Value const * {
			  // op_arrow
			  if constexpr( std::is_pointer_v<Iterator> ) {
				  return reinterpret_cast<Value const *>( as_orig<Iterator>( p ) );
			  } else {
				  return reinterpret_cast<Value const *>(
				    as_orig<Iterator>( p ).operator->( ) );
			  }
		  },
		  +[]( std::byte *p ) -> std::byte * {
			  // op_clone
			  return reinterpret_cast<std::byte *>(
			    new Iterator( std::as_const( as_orig<Iterator>( p ) ) ) );
		  }};
	} // namespace inpit_impl

	template<typename T>
	struct InputIterator {
		using value_type = T;
		using reference = T const &;
		using pointer = T const *;
		using difference_type = void;
		using iterator_category = std::input_iterator_tag;

	private:
		using op_del_t = daw::traits::fn_t<void( std::byte * )>;
		using op_cmp_t = daw::traits::fn_t<bool( std::byte *, std::byte * )>;
		using op_prefix_inc_t = daw::traits::fn_t<void( std::byte * )>;
		using op_arrow_t = daw::traits::fn_t<pointer( std::byte * )>;
		using op_clone_t = daw::traits::fn_t<std::byte *( std::byte * )>;
		using vtable_t = std::tuple<op_del_t, op_cmp_t, op_cmp_t, op_prefix_inc_t,
		                            op_arrow_t, op_clone_t>;
		std::byte *m_iterator;
		vtable_t const *m_vtable;

		op_del_t fn_op_del( ) const noexcept {
			return std::get<inpit_impl::OpDel>( *m_vtable );
		}

		op_cmp_t fn_op_equal( ) const noexcept {
			return std::get<inpit_impl::OpEqual>( *m_vtable );
		}

		op_cmp_t fn_op_not_equal( ) const noexcept {
			return std::get<inpit_impl::OpNotEqual>( *m_vtable );
		}

		op_prefix_inc_t fn_op_prefix_inc( ) const noexcept {
			return std::get<inpit_impl::OpPrefixInc>( *m_vtable );
		}

		op_arrow_t fn_op_arrow( ) const noexcept {
			return std::get<inpit_impl::OpArrow>( *m_vtable );
		}

		op_clone_t fn_op_clone( ) const noexcept {
			return std::get<inpit_impl::OpClone>( *m_vtable );
		}

		template<typename Iterator>
		static std::byte *create( Iterator const &it ) {
			auto ptr = new Iterator( it );
			return reinterpret_cast<std::byte *>( ptr );
		}

	public:
		template<typename Iterator,
		         std::enable_if_t<not std::is_same_v<InputIterator, Iterator>,
		                          std::nullptr_t> = nullptr>
		explicit InputIterator( Iterator const &it )
		  : m_iterator( create( it ) )
		  , m_vtable( &inpit_impl::vtable<Iterator, value_type> ) {
			static_assert(
			  std::is_same_v<std::remove_cv_t<value_type>,
			                 typename std::iterator_traits<Iterator>::value_type> );
		}

		InputIterator( InputIterator const &other )
		  : m_iterator( other.fn_op_clone( )( other.m_iterator ) )
		  , m_vtable( other.m_vtable ) {}

		InputIterator &operator=( InputIterator const &rhs ) {
			if( this != &rhs ) {
				auto tmp = m_iterator;
#ifdef CAN_THROW
				try {
#endif
					m_iterator = rhs.fn_op_clone( )( rhs.m_iterator );
					m_vtable = rhs.m_vtable;
					fn_op_del( tmp );
#ifdef CAN_THROW
				} catch( ... ) {
					m_iterator = tmp;
					throw;
				}
#endif
			}
			return *this;
		}

		InputIterator( InputIterator &&other ) noexcept
		  : m_iterator( std::exchange( other.m_iterator, nullptr ) )
		  , m_vtable( other.m_vtable ) {}

		InputIterator &operator=( InputIterator &&rhs ) noexcept {
			if( this != &rhs ) {
				m_iterator = std::exchange( rhs.m_iterator, nullptr );
				m_vtable = rhs.m_vtable;
			}
			return *this;
		}

		~InputIterator( ) {
			fn_op_del( )( m_iterator );
		}

		reference operator*( ) const {
			return *operator->( );
		}
		pointer operator->( ) const {
			return reinterpret_cast<pointer>( fn_op_arrow( )( m_iterator ) );
		}

		InputIterator &operator++( ) {
			fn_op_prefix_inc( )( m_iterator );
			return *this;
		}

		InputIterator operator++( int ) {
			auto result = *this;
			fn_op_prefix_inc( )( m_iterator );
			return result;
		}

	private:
		constexpr static bool same_op_cmp( InputIterator const &lhs,
		                                   InputIterator const &rhs ) noexcept {
			return lhs.m_vtable == rhs.m_vtable;
		}

	public:
		friend bool operator==( InputIterator const &lhs,
		                        InputIterator const &rhs ) noexcept {
			return same_op_cmp( lhs, rhs ) and
			       lhs.fn_op_equal( )( lhs.m_iterator, rhs.m_iterator );
		}

		friend bool operator!=( InputIterator const &lhs,
		                        InputIterator const &rhs ) noexcept {
			// Should being of diff type be unequal?  I think so
			return not same_op_cmp( lhs, rhs ) or
			       lhs.fn_op_not_equal( )( lhs.m_iterator, rhs.m_iterator );
		}

		bool are_same_base_iterator_type( InputIterator const &other ) const
		  noexcept {
			return same_op_cmp( *this, other );
		}
	};

	template<typename Iterator>
	InputIterator( Iterator )
	  ->InputIterator<typename std::iterator_traits<Iterator>::value_type>;

	// *****************************************************
	template<typename T>
	struct InputRange {
		using value_type = T;
		using iterator = InputIterator<value_type>;
		using const_iterator = InputIterator<std::add_const_t<value_type>>;

		InputIterator<value_type> first;
		InputIterator<value_type> last;

		template<typename Container,
		         std::enable_if_t<
		           (not std::is_rvalue_reference_v<Container> and
		            not std::is_same_v<InputRange, daw::remove_cvref_t<Container>>),
		           std::nullptr_t> = nullptr>
		InputRange( Container &&c )
		  : first( std::begin( c ) )
		  , last( std::end( c ) ) {}

		template<typename Iterator>
		InputRange( Iterator f, Iterator l )
		  : first( f )
		  , last( l ) {}

		decltype( auto ) begin( ) {
			return first;
		}
		decltype( auto ) begin( ) const {
			return first;
		}
		decltype( auto ) cbegin( ) const {
			return first;
		}
		decltype( auto ) end( ) {
			return last;
		}
		decltype( auto ) end( ) const {
			return last;
		}
		decltype( auto ) cend( ) const {
			return last;
		}
	};

	template<typename Iterator>
	InputRange( Iterator, Iterator )
	  ->InputRange<typename std::iterator_traits<Iterator>::value_type>;

	template<typename T>
	InputRange( T *, T * )->InputRange<T>;

	template<typename Container>
	InputRange( Container )
	  ->InputRange<typename std::iterator_traits<
	    decltype( std::begin( std::declval<Container>( ) ) )>::value_type>;

} // namespace daw
