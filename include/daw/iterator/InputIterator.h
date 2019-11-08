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

namespace daw {

	template<typename T>
	using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

	template<typename T>
	using fn_t = std::add_pointer_t<T>;

	namespace input_iterator_impl {
		enum fn_ary_item {
			OpDel,
			OpEqual,
			OpNotEqual,
			OpPrefixInc,
			OpArrow,
			OpClone
		};

		template<typename Iterator, typename Value>
		struct InputIteratorImpl {
			static decltype( auto ) as_orig( std::byte *p ) {
				return *reinterpret_cast<Iterator *>( p );
			}

			inline static void const *fn_ary[] = {
			  reinterpret_cast<void *>( +[]( std::byte *p ) -> void {
				  // op_del
				  delete reinterpret_cast<Iterator *>( p );
			  } ),
			  reinterpret_cast<void *>( +[]( std::byte *l, std::byte *r ) -> bool {
				  // op_equal
				  return as_orig( l ) == as_orig( r );
			  } ),
			  reinterpret_cast<void *>( +[]( std::byte *l, std::byte *r ) -> bool {
				  // op_not_equal
				  return as_orig( l ) != as_orig( r );
			  } ),
			  reinterpret_cast<void *>( +[]( std::byte *p ) -> void {
				  // op_prefix_inc
				  as_orig( p ) = as_orig( p ) + 1;
			  } ),
			  reinterpret_cast<void *>( +[]( std::byte *p ) -> Value const * {
				  // op_arrow
				  if constexpr( std::is_pointer_v<Iterator> ) {
					  return as_orig( p );
				  } else {
					  return as_orig( p ).operator->( );
				  }
			  } ),
			  reinterpret_cast<void *>( +[]( std::byte *p ) -> std::byte * {
				  // op_clone
				  return reinterpret_cast<std::byte *>(
				    new Iterator( std::as_const( as_orig( p ) ) ) );
			  } )};
		};
	} // namespace input_iterator_impl

	template<typename T>
	struct InputIterator {
		using value_type = T;
		using reference = T const &;
		using pointer = T const *;
		using difference_type = void;
		using iterator_category = std::input_iterator_tag;

	private:
		std::byte *m_iterator;
		void const **m_ops;

		using op_del_t = fn_t<void( std::byte * )>;
		using op_cmp_t = fn_t<bool( std::byte *, std::byte * )>;
		using op_prefix_inc_t = fn_t<void( std::byte * )>;
		using op_arrow_t = fn_t<pointer( std::byte * )>;
		using op_clone_t = fn_t<std::byte *( std::byte * )>;

		op_del_t fn_op_del( ) const noexcept {
			return reinterpret_cast<op_del_t>( m_ops[input_iterator_impl::OpDel] );
		}

		op_cmp_t fn_op_equal( ) const noexcept {
			return reinterpret_cast<op_cmp_t>( m_ops[input_iterator_impl::OpEqual] );
		}

		op_cmp_t fn_op_not_equal( ) const noexcept {
			return reinterpret_cast<op_cmp_t>(
			  m_ops[input_iterator_impl::OpNotEqual] );
		}

		op_prefix_inc_t fn_op_prefix_inc( ) const noexcept {
			return reinterpret_cast<op_prefix_inc_t>(
			  m_ops[input_iterator_impl::OpPrefixInc] );
		}

		op_arrow_t fn_op_arrow( ) const noexcept {
			return reinterpret_cast<op_arrow_t>(
			  m_ops[input_iterator_impl::OpArrow] );
		}

		op_clone_t fn_op_clone( ) const noexcept {
			return reinterpret_cast<op_clone_t>(
			  m_ops[input_iterator_impl::OpClone] );
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
		  , m_ops( input_iterator_impl::InputIteratorImpl<Iterator,
		                                                  value_type>::fn_ary ) {}

		InputIterator( InputIterator const &other )
		  : m_iterator( other.fn_op_clone( )( other.m_iterator ) )
		  , m_ops( other.m_ops ) {}

		InputIterator &operator=( InputIterator const &rhs ) {
			if( this != &rhs ) {
				auto tmp = m_iterator;
				try {
					m_iterator = rhs.fn_op_clone( )( rhs.m_iterator );
					m_ops = rhs.m_ops;
					fn_op_del( tmp );
				} catch( ... ) {
					m_iterator = tmp;
					throw;
				}
			}
			return *this;
		}

		InputIterator( InputIterator &&other ) noexcept
		  : m_iterator( std::exchange( other.m_iterator, nullptr ) )
		  , m_ops( other.m_ops ) {}

		InputIterator &operator=( InputIterator &&rhs ) noexcept {
			if( this != &rhs ) {
				m_iterator = std::exchange( rhs.m_iterator, nullptr );
				m_ops = rhs.m_ops;
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
			return lhs.m_ops == rhs.m_ops;
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

	template<typename T>
	struct InputRange {
		using value_type = T;
		InputIterator<value_type> first;
		InputIterator<value_type> last;

		template<typename Container,
		         std::enable_if_t<
		           (not std::is_rvalue_reference_v<Container> and
		            not std::is_same_v<InputRange, remove_cvref_t<Container>>),
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
	template<typename Container>
	InputRange( Container )
	  ->InputRange<std::remove_reference_t<
	    decltype( *std::begin( std::declval<Container>( ) ) )>>;

	static_assert(
	  std::is_same_v<
	    InputIterator<int>::value_type,
	    typename std::iterator_traits<InputIterator<int>>::value_type> );

} // namespace daw
