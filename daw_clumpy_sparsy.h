// The MIT License (MIT)
//
// Copyright (c) 2013-2015 Darrell Wright
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

#include <cstdint>
#include <vector>

namespace daw {
	// Provide a vector like structure that assumes that the sparseness has clumps
	template<typename T>
	class ClumpySparsy {
		class Chunk {
			size_t m_start;
			size_t m_end;
			std::vector<T> m_items;
		public:
			size_t & start( ) {
				assert( m_end >= m_start );
				return m_start;
			}
			
			size_t const & start( ) const {
				assert( m_end >= m_start );
				return m_start;
			}

			size_t & end( ) {
				assert( m_end >= m_start );
				return m_end;
			}
			
			size_t const & end( ) const {
				assert( m_end >= m_start );
				return m_end;
			}

			size_t size( ) const {
				assert( m_end >= m_start );
				auto result = m_items.size( );
				assert( (m_end - m_start) == result );
				return result;
			}

		};	// class Chunk
	public:
		using values_type = ::std::vector<Chunk>;
	private:
		values_type m_items;	
	public:
		using value_type = values_type::value_type;
		using reference = values_type::reference;
		using const_reference = values_type::reference;

		size_t size( ) qconst {
			return std::accumulate( m_items.begin( ), m_items.end( ), static_cast<size_t>( 0 ), []( auto const & prev, auto const & current ) {
				return prev + current.size( );
			}
		}
		
		reference operator[]( size_t pos ) {
			size_t count = 0;
			
		}
		

	};
}	// namespace daw

