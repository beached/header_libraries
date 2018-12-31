// The MIT License (MIT)
//
// Copyright (c) 2018 Darrell Wright
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

#include <utility>

#include "cpp_17.h"
#include "daw_fnv1a_hash.h"
#include "daw_hash_adaptor.h"
#include "daw_traits.h"

namespace daw {
	template<typename Key, typename Value, size_t N,
	         typename Hash = daw::fnv1a_hash_t>
	class static_hash_map {
		daw::static_hash_adaptor_t<Key, N, Hash> m_hashes{};
		std::array<Value, N> m_values{};

	public:
		constexpr static_hash_map( ) noexcept = default;

		template<size_t ItemCount>
		constexpr static_hash_map(
		  std::pair<Key, Value> const ( &init_values )[ItemCount] ) noexcept {

			static_assert( ItemCount <= N );
			for( auto const &kv : init_values ) {
				insert( kv.first, kv.second );
			}
		}

		template<typename K, typename V,
		         std::enable_if_t<
		           daw::all_true_v<std::is_same_v<Key, daw::remove_cvref_t<K>>,
		                           is_same_v<Value, daw::remove_cvref_t<V>>>,
		           std::nullptr_t> = nullptr>
		constexpr void insert( K &&key, V &&value ) noexcept {

			auto const index = m_hashes.insert( std::forward<K>( key ) );
			m_values[index] = std::forward<V>( value );
		}

		constexpr bool exists( Key const &key ) const noexcept {
			return m_hashes.exists( key );
		}

		constexpr Value &operator[]( Key const &key ) noexcept {
			return m_values[m_hashes.insert( key )];
		}

		constexpr Value const &operator[]( Key const &key ) const noexcept {
			auto const index = m_hashes.find( key );
			if( !index ) {
				std::terminate( );
			}
			return m_values[*index];
		}

		constexpr std::optional<Value> try_get( Key const &key ) const noexcept {
			auto const index = m_hashes.find( key );
			if( !index ) {
				return {};
			}
			return m_values[*index];
		}

		constexpr bool count( Key const &key ) const noexcept {
			return m_hashes.count( key );
		}

		static constexpr size_t capacity( ) noexcept {
			return N;
		}

		constexpr size_t size( ) const noexcept {
			return m_hashes.size( );
		}
	};

	template<typename Key, typename Value, typename Hash = daw::fnv1a_hash_t,
	         size_t N>
	constexpr auto
	make_static_hash_map( std::pair<Key, Value> const ( &items )[N] ) noexcept {
		return static_hash_map<Key, Value, N>( items );
	}
} // namespace daw
