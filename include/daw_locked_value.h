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

#include <future>
#include <memory>
#include <mutex>
#include <utility>

#include "cpp_17.h"

namespace daw {
	template<typename Value>
	class locked_value_t {
		std::mutex *m_mutex;
		Value *m_value;

	public:
		void release( ) {
			m_value = nullptr;
			if( m_mutex ) {
				m_mutex->unlock( );
				m_mutex = nullptr;
			}
		}

		~locked_value_t( ) {
			release( );
		}

		locked_value_t( locked_value_t &&other ) noexcept = default;
		locked_value_t &operator=( locked_value_t && ) noexcept = default;

		locked_value_t( std::mutex &m, Value &value )
		  : m_mutex{&m}
		  , m_value{&value} {

			m_mutex->lock( );
		}

		locked_value_t( locked_value_t const & ) = delete;
		locked_value_t &operator=( locked_value_t const & ) = delete;

		Value &get( ) noexcept {
			return *m_value;
		}

		Value const &get( ) const noexcept {
			return *m_value;
		}

		Value &operator*( ) noexcept {
			return *m_value;
		}

		Value const &operator*( ) const noexcept {
			return *m_value;
		}

		Value *operator->( ) noexcept {
			return m_value;
		}

		Value const *operator->( ) const noexcept {
			return m_value;
		}
	}; // locked_value_t

	template<typename Value>
	locked_value_t<Value> make_locked_value( std::mutex &m, Value &value ) {
		return locked_value_t<Value>{m, value};
	}

	template<typename T>
	class lockable_value_t {
		std::unique_ptr<std::mutex> m_mutex;
		std::unique_ptr<T> m_value;

	public:
		lockable_value_t( )
		  : m_mutex{std::make_unique<std::mutex>( )}
		  , m_value{std::make_unique<T>( )} {}

		template<typename U>
		explicit lockable_value_t( U value )
		  : m_mutex{std::make_unique<std::mutex>( )}
		  , m_value{std::make_unique<T>( std::move( value ) )} {}

		lockable_value_t( lockable_value_t && ) noexcept = default;
		lockable_value_t &operator=( lockable_value_t && ) noexcept = default;

		lockable_value_t( lockable_value_t const &other )
		  : m_mutex{std::make_unique<std::mutex>( )}
		  , m_value{std::make_unique<T>( *other.get( ) )} {}

		lockable_value_t &operator=( lockable_value_t const &rhs ) {
			if( this != &rhs ) {
				m_mutex = std::make_unique<std::mutex>( );
				m_value = *rhs.get( );
			}
			return *this;
		}

		locked_value_t<T> get( ) {
			return make_locked_value( *m_mutex, *m_value );
		}

		locked_value_t<std::add_const_t<T>> get( ) const {
			return make_locked_value( *m_mutex, daw::as_const( *m_value ) );
		}

		locked_value_t<T> operator*( ) {
			return get( );
		}

		locked_value_t<T> operator*( ) const {
			return get( );
		}
	}; // lockable_value_t
} // namespace daw
