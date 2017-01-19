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

#include <type_traits>
#include <utility>

namespace daw {
	template<typename T>
	struct read_only {
		using value_type = std::decay_t<T>;
		using reference = value_type &;
		using const_reference = value_type const &;
	private:
		value_type m_value;
	public:
		read_only( value_type value = value_type { } ):
			m_value { std::move( value ) } { }

		read_only( read_only const & ) = default;
		read_only( read_only && ) = default;
		read_only & operator=( read_only const & ) = delete;
		read_only & operator=( read_only && ) = delete;
		~read_only( ) = default;

		operator const_reference( ) const {
			return m_value;
		}

		const_reference get( ) const {
			return m_value;
		}

		reference read_write( ) {
			return m_value;
		}

		value_type move_out( ) {
			return std::move( m_value );
		}
	};	// read_only

}	// namespace daw
