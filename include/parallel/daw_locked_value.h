// The MIT License (MIT)
//
// Copyright (c) 2017-2018 Darrell Wright
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

#include <future>
#include <mutex>
#include <utility>

#include "../cpp_17.h"
#include "daw_value_ptr.h"

namespace daw {
	template<typename T>
	class locked_value_t {
		std::unique_lock<std::mutex> m_lock;
		std::reference_wrapper<T> m_value;

	public:
		using value_type = T;
		using reference = T &;
		using const_reference = T const &;
		using pointer = T *;
		using const_pointer = T const *;

		locked_value_t( locked_value_t const &other ) = delete;
		locked_value_t &operator=( locked_value_t const &other ) = delete;

		locked_value_t( locked_value_t &&other ) noexcept = default;
		locked_value_t &operator=( locked_value_t && ) noexcept = default;

		locked_value_t( std::mutex &m, T &value )
		  : m_lock( m )
		  , m_value( value ) {}

		void release( ) noexcept {
			m_lock.unlock( );
		}

		reference get( ) noexcept {
			return m_value.get( );
		}

		const_reference get( ) const noexcept {
			return *m_value;
		}

		reference operator*( ) noexcept {
			return get( );
		}

		const_reference operator*( ) const noexcept {
			return get( );
		}

		pointer operator->( ) noexcept {
			return &m_value.get( );
		}

		const_pointer operator->( ) const noexcept {
			return &m_value.get( );
		}
	}; // locked_value_t

	template<typename T>
	locked_value_t<T> make_locked_value( std::mutex &m, T &value ) {
		return locked_value_t<T>( m, value );
	}

	template<typename T>
	class lockable_value_t {
		mutable daw::value_ptr<std::mutex> m_mutex;
		daw::value_ptr<T> m_value;

	public:
		lockable_value_t( ) noexcept( is_nothrow_default_constructible_v<T> )
		  : m_mutex( )
		  , m_value( ) {}

		template<typename U, std::enable_if_t<
		                       !daw::traits::is_first_type_v<lockable_value_t, U>,
		                       std::nullptr_t> = nullptr>
		explicit lockable_value_t( U &&value ) noexcept(
		  noexcept( daw::value_ptr<T>( std::forward<U>( value ) ) ) )
		  : m_mutex( )
		  , m_value( std::forward<T>( value ) ) {}

		locked_value_t<T> get( ) {
			return make_locked_value( *m_mutex, *m_value );
		}

		locked_value_t<std::add_const_t<T>> get( ) const {
			return make_locked_value( *m_mutex, daw::as_const( *m_value ) );
		}

		locked_value_t<T> operator*( ) {
			return get( );
		}

		locked_value_t<T const> operator*( ) const {
			return get( );
		}
	}; // lockable_value_t
} // namespace daw
