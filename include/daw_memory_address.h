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

#include<cstdint>

namespace daw {
	namespace impl {
		template<typename T, uintptr_t...>
		struct memory_address_impl { };

		template<typename T, uintptr_t location>
		struct memory_address_impl<T, location> {
			static_assert( location > 0, "nullptr is not a valid location" );
			using value_type = T volatile;
			private:
			static value_type & m_value;
			public:
			value_type & operator( )( ) {
				return m_value;
			};

			value_type const & operator( )( ) const {
				return m_value;
			}

			operator value_type & ( ) {
				return m_value;
			}

			operator value_type const &( ) const {
				return m_value;
			}
		};	// memory_address_impl<value_type, location>

		template<typename T, uintptr_t location>
		typename memory_address_impl<T, location>::value_type & memory_address_impl<T, location>::m_value = *((T * const)location);
	}

	template<typename T, uintptr_t base, uintptr_t offset = 0>
	using memory_address = daw::impl::memory_address_impl<T, (base + offset)>;
}

