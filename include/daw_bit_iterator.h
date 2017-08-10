// The MIT License (MIT)
//
// Copyright (c) 2017 Darrell Wright
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
#include <iterator>

#include <daw_exception.h>

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
		constexpr explicit bit_iterator( Iterator it ) : m_bit{std::move( it )} {}
		constexpr explicit bit_iterator( Iterator it, size_t bit_pos )
		    : m_bit{std::move( it )}, m_pos{std::move( it_pos )} {

			daw::exception::daw_throw_if_false(
			    bit_pos >= sizeof( value_type ) * 8,
			    "Cannot specify a bit position greater than the current value size in bits" );
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
