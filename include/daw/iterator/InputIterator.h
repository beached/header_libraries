// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "../cpp_17.h"
#include "../daw_exchange.h"
#include "../daw_traits.h"

#include <array>
#include <cassert>
#include <ciso646>
#include <cstddef>
#include <iterator>
#include <memory>
#include <numeric>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace daw {
	template<typename T>
	using fn_t = std::add_pointer_t<T>;

	template<typename T>
	using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

	namespace input_impl {

		template<typename It>
		static decltype( auto ) as_orig( std::byte *p ) {
			return *reinterpret_cast<It *>( p );
		}

		using op_del_t = fn_t<void( std::byte * )>;
		using op_cmp_t = fn_t<bool( std::byte *, std::byte * )>;
		using op_inc_t = fn_t<void( std::byte * )>;
		template<typename Value>
		using op_arrow_t = fn_t<Value const *( std::byte * )>;
		using op_clone_t = fn_t<std::byte *( std::byte * )>;

		enum fn_ary_item { OpDel, OpEqual, OpNotEqual, OpInc, OpArrow, OpClone };

		template<typename Value>
		using vtable_t = std::tuple<op_del_t, op_cmp_t, op_cmp_t, op_inc_t,
		                            op_arrow_t<Value>, op_clone_t>;

		template<fn_ary_item Idx, typename Value>
		decltype( auto ) get( vtable_t<Value> const *tbl ) {
			return std::get<Idx>( *tbl );
		}

		template<typename Iterator, typename Value>
		static vtable_t<Value> const *get_vtable( ) noexcept {
			static constexpr vtable_t<Value> vtable{
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
				  // op_inc
				  ++as_orig<Iterator>( p );
			  },
			  +[]( std::byte *p ) -> Value const * {
				  // op_arrow
				  if constexpr( std::is_pointer_v<Iterator> ) {
					  return as_orig<Iterator>( p );
				  } else {
					  return as_orig<Iterator>( p ).operator->( );
				  }
			  },
			  +[]( std::byte *p ) -> std::byte * {
				  // op_clone
				  return reinterpret_cast<std::byte *>(
				    new Iterator( std::as_const( as_orig<Iterator>( p ) ) ) );
			  } };
			return &vtable;
		}
	} // namespace input_impl

	template<typename T>
	struct InputIterator {
		using value_type = T;
		using reference = T const &;
		using pointer = T const *;
		using difference_type = void;
		using iterator_category = std::input_iterator_tag;

	private:
		using op_clone_t = fn_t<std::byte *( std::byte * )>;

		std::byte *m_iterator;
		input_impl::vtable_t<value_type> const *m_vtable;
		[[nodiscard]] op_clone_t fn_op_clone( ) const noexcept {
			return input_impl::get<input_impl::OpClone, value_type>( m_vtable );
		}

		template<typename Iterator>
		[[nodiscard]] static std::byte *create( Iterator const &it ) {
			auto ptr = new Iterator( it );
			return reinterpret_cast<std::byte *>( ptr );
		}

	public:
		template<typename Iterator,
		         std::enable_if_t<not std::is_same_v<InputIterator, Iterator>,
		                          std::nullptr_t> = nullptr>
		explicit InputIterator( Iterator const &it )
		  : m_iterator( create( it ) )
		  , m_vtable( input_impl::get_vtable<Iterator, value_type>( ) ) {
			static_assert(
			  std::is_same_v<std::remove_cv_t<value_type>,
			                 typename std::iterator_traits<Iterator>::value_type> );
		}

		InputIterator( InputIterator const &other )
		  : m_iterator( other.fn_op_clone( )( other.m_iterator ) )
		  , m_vtable( other.m_vtable ) {}

		InputIterator &operator=( InputIterator const &rhs ) {
			if( this != &rhs ) {
				// copy first so that if it throws we are cool
				auto tmp = rhs.fn_op_clone( )( rhs.m_iterator );
				std::swap( m_iterator, tmp );
				m_vtable = rhs.m_vtable;
				input_impl::get<input_impl::OpDel, value_type>( m_vtable )( tmp );
			}
			return *this;
		}

		InputIterator( InputIterator &&other ) noexcept
		  : m_iterator( daw::exchange( other.m_iterator, nullptr ) )
		  , m_vtable( other.m_vtable ) {}

		InputIterator &operator=( InputIterator &&rhs ) noexcept {
			if( this != &rhs ) {
				m_iterator = daw::exchange( rhs.m_iterator, nullptr );
				m_vtable = rhs.m_vtable;
			}
			return *this;
		}

		~InputIterator( ) {
			input_impl::get<input_impl::OpDel, value_type>( m_vtable )( m_iterator );
		}

		[[nodiscard]] reference operator*( ) const {
			return *operator->( );
		}

		[[nodiscard]] pointer operator->( ) const {
			return reinterpret_cast<pointer>(
			  input_impl::get<input_impl::OpArrow, value_type>( m_vtable )(
			    m_iterator ) );
		}

		InputIterator &operator++( ) {
			input_impl::get<input_impl::OpInc, value_type>( m_vtable )( m_iterator );
			return *this;
		}

		InputIterator operator++( int ) {
			auto result = *this;
			input_impl::get<input_impl::OpInc, value_type>( m_vtable )( m_iterator );
			return result;
		}

	private:
		[[nodiscard]] constexpr static bool
		same_op_cmp( InputIterator const &lhs, InputIterator const &rhs ) noexcept {
			return lhs.m_vtable == rhs.m_vtable;
		}

	public:
		bool operator==( InputIterator const &rhs ) const noexcept {
			// Cannot think of another way right now to safely determine at runtime if
			// two iterators of erased type are comparable.  So I will disallow it
			assert( same_op_cmp( *this, rhs ) );
			return input_impl::get<input_impl::OpEqual, value_type>( m_vtable )(
			  m_iterator, rhs.m_iterator );
		}

		bool operator!=( InputIterator const &rhs ) const noexcept {
			// Cannot think of another way right now to safely determine at runtime if
			// two iterators of erased type are comparable.  So I will disallow it
			assert( same_op_cmp( *this, rhs ) );
			return input_impl::get<input_impl::OpNotEqual, value_type>( m_vtable )(
			  m_iterator, rhs.m_iterator );
		}

		[[nodiscard]] bool
		are_same_base_iterator_type( InputIterator const &other ) const noexcept {
			return same_op_cmp( *this, other );
		}
	};

	template<typename Iterator>
	InputIterator( Iterator )
	  -> InputIterator<typename std::iterator_traits<Iterator>::value_type>;

	// *****************************************************
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

		[[nodiscard]] decltype( auto ) begin( ) {
			return first;
		}
		[[nodiscard]] decltype( auto ) begin( ) const {
			return first;
		}
		[[nodiscard]] decltype( auto ) cbegin( ) const {
			return first;
		}
		[[nodiscard]] decltype( auto ) end( ) {
			return last;
		}
		[[nodiscard]] decltype( auto ) end( ) const {
			return last;
		}
		[[nodiscard]] decltype( auto ) cend( ) const {
			return last;
		}
	};

	template<typename Iterator>
	InputRange( Iterator, Iterator )
	  -> InputRange<typename std::iterator_traits<Iterator>::value_type>;
	template<typename Container>
	InputRange( Container ) -> InputRange<std::remove_reference_t<
	  decltype( *std::begin( std::declval<Container>( ) ) )>>;

} // namespace daw
