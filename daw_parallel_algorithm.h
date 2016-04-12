#pragma once
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

#include <algorithm>
#include "daw_algorithm.h"
#include <thread>
#include <vector>
#include <future>

namespace daw {
	namespace algorithm {
		namespace parallel {
			
				template<typename Func>
				void for_each( size_t first, size_t last, Func func ) {
					auto const nthreads = std::thread::hardware_concurrency( );
					auto const chunk_sz = (last - first) / nthreads;
					std::vector<std::future<void>> workers;
					for( auto n = first; n < last; n += chunk_sz ) {
						workers.emplace_back( std::launch::async, [start = n, finish = std::min( n + chunk_sz, last ), func]( ) mutable {
							for( auto i = start; i < finish; ++i ) {
								func( i );
							}
						} );
					}
				}

				template<typename Iterator, typename Func>
				void for_each( Iterator first, Iterator last, Func func ) {
					auto const nthreads = std::thread::hardware_concurrency( );
					auto const sz = std::distance( first, last );
					auto const chunk_sz = sz / nthreads;

					std::vector<std::future<void>> workers;
					for( size_t n = 0; n < sz; n += chunk_sz ) {
						workers.emplace_back( std::launch::async, [first, start = n, finish = std::min( n + chunk_sz, last ), func]( ) mutable {
							for( auto i = start; i < finish; ++i ) {
								func( first + i );
							}
						} );
					}
				}

				template<typename InputIt1, typename InputIt2, typename OutputIt, typename Func>
				OutputIt transform_many( InputIt1 first1, InputIt1 last1, InputIt2 first2, OutputIt first_out, Func func ) {
					static_assert(!is_const_v<decltype(*first_out)>, "Output iterator cannot point to const data");
					while( first1 != last1 ) {
						*first_out++ = func( *first1++, *first2++ );
					}
					return first_out;
				}

				template<typename InputIt1, typename InputIt2, typename InputIt3, typename OutputIt, typename Func>
				OutputIt transform_many( InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt3 first3, OutputIt first_out, Func func ) {
					static_assert(!is_const_v<decltype(*first_out)>, "Output iterator cannot point to const data");
					while( first1 != last1 ) {
						*first_out++ = func( *first1++, *first2++, *first3++ );
					}
					return first_out;
				}

				template<typename InputIt1, typename InputIt2, typename InputIt3, typename InputIt4, typename OutputIt, typename Func>
				OutputIt transform_many( InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt3 first3, InputIt4 first4, OutputIt first_out, Func func ) {
					static_assert(!is_const_v<decltype(*first_out)>, "Output iterator cannot point to const data");
					while( first1 != last1 ) {
						*first_out++ = func( *first1++, *first2++, *first3++, *first4++ );
					}
					return first_out;
				}
				template<typename InputIt1, typename InputIt2, typename InputIt3, typename InputIt4, typename InputIt5, typename OutputIt, typename Func>
				OutputIt transform_many( InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt3 first3, InputIt4 first4, InputIt4 first5, OutputIt first_out, Func func ) {
					static_assert(!is_const_v<decltype(*first_out)>, "Output iterator cannot point to const data");
					while( first1 != last1 ) {
						*first_out++ = func( *first1++, *first2++, *first3++, *first4++, *first5++ );
					}
					return first_out;
				}

		}	// namespace parallel	
	}	// namespace algorithm
}	// namespace daw
