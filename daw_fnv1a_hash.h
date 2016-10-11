// The MIT License (MIT)
//
// Copyright (c) 2016 Darrell Wright
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
#include <type_traits>
#include <cstddef>

namespace daw {
	namespace impl {
		using is_64bit_t = std::integral_constant<bool, sizeof(size_t) == sizeof(uint64_t)>;

		template<typename T>
		struct fnv1a_hash {
			constexpr size_t operator( )( T const * const ptr ) { 
				auto hash = impl::is_64bit_t::value ? 14695981039346656037ULL : 2166136261UL;
				auto bptr = static_cast<uint8_t const * const>(static_cast<void const * const>(ptr));
				for( size_t n=0; n<sizeof(T); ++n ) { 
					hash = hash ^ static_cast<size_t>(bptr[n]);
					hash *= impl::is_64bit_t::value ? 1099511628211ULL : 16777619UL;
				}   
				return hash;
			}   
		};  

		template<>
		struct fnv1a_hash<char const *> {
			constexpr size_t operator()( char const * ptr ) { 
				auto hash = impl::is_64bit_t::value ? 14695981039346656037ULL : 2166136261UL;
				while( *ptr != 0 ) {    
					hash = hash ^ static_cast<size_t>(*ptr);
					hash *= impl::is_64bit_t::value ? 1099511628211ULL : 16777619UL;
					++ptr;
				}
				return hash;
			}
		};
	}	// namespace impl 

	template<typename T>
	constexpr size_t fnv1a_hash( T const & value ) {
		return impl::fnv1a_hash<T>{ }( value );
	}  

	template<typename T>
	constexpr size_t fnv1a_hash( T const * value ) {
		return impl::fnv1a_hash<T const *>{ }( value );
	}
}    // namespace daw
