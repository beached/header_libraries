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

#include <stdexcept>
#include <type_traits>

#include "daw_exception.h"
#include "daw_fnv1a_hash.h"
#include "daw_stack_array.h"
#include "daw_traits.h"
#include "daw_utility.h"

namespace daw {
	namespace impl {
		namespace sentinals {
			enum sentinals : size_t { empty = 0, removed, sentinals_size };
		} // namespace sentinals
	}   // namespace impl

	template<typename Value, size_t N>
	struct fixed_unordered_map {
		static_assert( N > 0, "Must supply a positive initial_size larger than 0" );
		using value_type = daw::traits::root_type_t<Value>;
		using reference = value_type &;
		using const_reference = value_type const &;

	private:
		daw::array_t<size_t, N> m_hashes;
		daw::array_t<value_type, N> m_values;

		static constexpr size_t capacity( ) noexcept {
			return static_cast<size_t>( std::numeric_limits<ptrdiff_t>::max( ) - 1 );
		}

		template<typename KeyType>
		static constexpr size_t hash_fn( KeyType &&key ) noexcept {
			return ( daw::fnv1a_hash( std::forward<KeyType>( key ) ) %
			         ( std::numeric_limits<size_t>::max( ) - impl::sentinals::sentinals_size ) ) +
			       impl::sentinals::sentinals_size;
		}

		static constexpr size_t scale_hash( size_t hash, size_t table_size ) {
			// Scale value to capacity using MAD(Multiply-Add-Divide) compression
			// Use the two largest Prime's that fit in a 64bit unsigned integral
			daw::exception::daw_throw_on_false( table_size > 0 );
			daw::exception::daw_throw_on_false(
			  table_size < capacity( ) ); // Table size must be less than max of ptrdiff_t as we use the value 0
			                              // as a sentinel.  This should be rare
			daw::exception::daw_throw_on_false( hash >= impl::sentinals::sentinals_size );

			size_t const prime_a = 18446744073709551557u;
			size_t const prime_b = 18446744073709551533u;
			return ( hash * prime_a + prime_b ) % table_size;
		}

		constexpr auto lookup( size_t const hash ) const noexcept {
			struct lookup_result_t {
				size_t position;
				bool found;

				constexpr lookup_result_t( ) noexcept : position{0}, found{false} {}

				constexpr lookup_result_t( size_t pos, bool is_found ) noexcept : position{pos}, found{is_found} {}

				constexpr operator bool( ) const noexcept {
					return found;
				}
			};

			lookup_result_t result{};

			auto const s_hash = scale_hash( hash, m_hashes.size( ) );
			auto hash_pos = s_hash;
			size_t removed_found = std::numeric_limits<size_t>::max( ); // Need a check and this will be rare
			for( ; hash_pos != m_hashes.size( ); ++hash_pos ) {
				if( m_hashes[hash_pos] == hash ) {
					return lookup_result_t{hash_pos, true};
				} else if( m_hashes[hash_pos] == impl::sentinals::empty ) {
					return lookup_result_t{hash_pos, false};
				} else if( m_hashes[result.position] == impl::sentinals::removed && result.position < removed_found ) {
					removed_found = hash_pos;
				}
			}
			for( hash_pos = 0; hash_pos != s_hash; ++hash_pos ) {
				if( m_hashes[hash_pos] == hash ) {
					return lookup_result_t{hash_pos, true};
				} else if( m_hashes[result.position] == impl::sentinals::empty ) {
					return lookup_result_t{hash_pos, false};
				} else if( m_hashes[result.position] == impl::sentinals::removed && result.position < removed_found ) {
					removed_found = result.position;
				}
			}
			if( removed_found != std::numeric_limits<size_t>::max( ) ) {
				return lookup_result_t{removed_found, false};
			}
			// TODO: throw here so the link fails
			return lookup_result_t{m_hashes.size( ), false};
		}

	public:
		constexpr fixed_unordered_map( ) : m_hashes{impl::sentinals::empty}, m_values{value_type{}} {}

		constexpr fixed_unordered_map( size_t initial_size ) : m_hashes{impl::sentinals::empty}, m_values{value_type{}} {}

		constexpr fixed_unordered_map( fixed_unordered_map const &other ) noexcept(
		  noexcept( m_values.front( ) = other.m_values.front( ) ) )
		  : m_hashes{impl::sentinals::empty}, m_values{value_type{}} {

			daw::cxpr_copy( other.m_hashes.cbegin( ), other.m_hashes.cend( ), m_hashes.begin( ) );
			daw::cxpr_copy( other.m_values.cbegin( ), other.m_values.cend( ), m_values.begin( ) );
		}

		constexpr fixed_unordered_map( fixed_unordered_map &&other ) noexcept(
		  noexcept( m_values.front( ) = other.m_values.front( ) ) )
		  : m_hashes{impl::sentinals::empty}, m_values{value_type{}} {

			daw::cxpr_move( other.m_hashes.begin( ), other.m_hashes.end( ), m_hashes.begin( ) );
			daw::cxpr_move( other.m_values.begin( ), other.m_values.end( ), m_values.begin( ) );
		}

		constexpr fixed_unordered_map &
		operator=( fixed_unordered_map const &rhs ) noexcept( noexcept( m_values.front( ) = rhs.m_values.front( ) ) ) {

			if( this != &rhs ) {
				daw::cxpr_copy( rhs.m_hashes.cbegin( ), rhs.m_hashes.cend( ), m_hashes.begin( ) );
				daw::cxpr_copy( rhs.m_values.cbegin( ), rhs.m_values.cend( ), m_values.begin( ) );
			}
			return *this;
		}

		constexpr fixed_unordered_map &
		operator=( fixed_unordered_map &&rhs ) noexcept( noexcept( m_values.front( ) = rhs.m_values.front( ) ) ) {

			if( this != &rhs ) {
				daw::cxpr_move( rhs.m_hashes.begin( ), rhs.m_hashes.end( ), m_hashes.begin( ) );
				daw::cxpr_move( rhs.m_values.begin( ), rhs.m_values.end( ), m_values.begin( ) );
			}
			return *this;
		}
		~fixed_unordered_map( ) = default;

		template<typename Key>
		constexpr const_reference operator[]( Key &&key ) const {
			auto const hash = hash_fn<Key>( std::forward<Key>( key ) );
			auto const is_found = lookup( hash );
			daw::exception::daw_throw_on_false( is_found, "Attempt to access an undefined key" );
			return m_values[is_found.position];
		}

		template<typename Key>
		constexpr reference operator[]( Key &&key ) {
			auto const hash = hash_fn<Key>( std::forward<Key>( key ) );
			auto const is_found = lookup( hash );
			daw::exception::daw_throw_on_true( !is_found && is_found.position == m_hashes.size( ),
			                                   "Fixed hash table does not have enough space to allocate all entries" );
			m_hashes[is_found.position] = hash;
			return m_values[is_found.position];
		}
	};
} // namespace daw
