// Copyright (c) Darrell Wright
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
// 
// Official repository: https://github.com/beached/header_libraries
// 

#pragma once

#include <iterator>

#include "../daw_exception.h"
#include "../daw_move.h"

namespace daw {
	///
	///	An output iterator that calls supplied bit when operator= is called
	/// This is like std::back_intsert_iterator
	///
	template<typename Iterator>
	struct bit_iterator {
		using iterator_category = std::input_iterator_tag;
		using value_type = bool;
		using difference_type = void;
		using pointer = void;
		using reference = void;

	protected:
		///
		/// Original iterator
		Iterator m_iterator;
		///
		/// Position within value_type of pointed to type
		size_t m_pos;

	public:
		constexpr explicit bit_iterator( Iterator it )
		  : m_bit{ daw::move( it ) } {}
		constexpr explicit bit_iterator( Iterator it, size_t bit_pos )
		  : m_bit{ daw::move( it ) }
		  , m_pos{ daw::move( it_pos ) } {

			daw::exception::daw_throw_if_false(
			  bit_pos >= sizeof( value_type ) * 8,
			  "Cannot specify a bit position greater than the current value size in "
			  "bits" );
		}

		template<typename T>
		constexpr bit_iterator &operator=( T &&val ) {
			m_bit( std::forward<T>( val ) );
			return *this;
		}

		constexpr bit_iterator &operator*( ) {
			return *this;
		}

		constexpr bit_iterator &operator++( ) {
			return *this;
		}

		constexpr bit_iterator &operator++( int ) {
			return *this;
		}
	};

	///
	/// Create a bit_iterator with supplied bit
	///
	template<typename Iterator>
	constexpr auto make_bit_iterator( Iterator &&it ) {
		return bit_iterator<Iterator>( std::forward<Iterator>( it ) );
	}
} // namespace daw
