// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
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

#include <atomic>

namespace daw {
	template<typename T>
	class atomic_unique_ptr {
		::std::atomic<T *> m_ptr = nullptr;

	public:
		atomic_unique_ptr( ) noexcept = default;

		atomic_unique_ptr( atomic_unique_ptr && other ) noexcept
			: m_ptr( other.m_ptr.exchange( nullptr ) ) {} 

		atomic_unique_ptr & operator=( atomic_unique_ptr && rhs ) noexcept {
			auto tmp = rhs.m_ptr.exchange( nullptr, ::std::memory_order_acquire );
			delete m_ptr.exchange( nullptr, ::std::memory_order_acquire );
			m_ptr.save( tmp, ::std::memory_order_release );
		}

		~atomic_unique_ptr( ) noexcept {
			delete m_ptr.exchange( nullptr, ::std::memory_order_acquire );
		}

		atomic_unique_ptr( atomic_unique_ptr const & ) = delete;
		atomic_unique_ptr &operator=( atomic_unique_ptr const & ) = delete;
	};
}    // namespace daw

