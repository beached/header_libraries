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
					workers.push_back( std::async( std::launch::async, [start = n, finish = std::min( n + chunk_sz, last ), func]( ) mutable {
						for( auto i = start; i < finish; ++i ) {
							func( i );
						}
					} ) );
				}
			}

//			template<typename InputIt1, typename OutputIt, typename Func>
//			OutputIt transform( InputIt1 first_in1, InputIt1 last_in1, OutputIt first_out, Func func ) {
//				static_assert(!is_const_v<decltype(*first_out)>, "Output iterator cannot point to const data");
//				auto const dist = std::distance( first_in1, last_in1 );
//				for_each( 0, dist, [&]( size_t n ) {
//					*(first_out + n) = func( *(first_in1 + n) );
//				} );
//				return first_out;
//			}
//
//
			template< class InputIt, class OutputIt, class UnaryOperation >
			using transform = std::transform( InputIt first1, InputIt last1, OutputIt d_first, UnaryOperation unary_op );
			template<typename InputIt1, typename InputIt2, typename OutputIt, typename Func>
			OutputIt transform_many( InputIt1 first_in1, InputIt1 last_in1, InputIt2 first_in2, OutputIt first_out, Func func ) {
				static_assert(!is_const_v<decltype(*first_out)>, "Output iterator cannot point to const data");
				auto const dist = std::distance( first_in1, last_in1 );
				for_each( 0, dist, [&]( size_t n ) {
					*(first_out + n) = func( *(first_in1 + n), *(first_in2 + n) );
				} );
				return first_out;
			}


			template<typename InputIt1, typename InputIt2, typename InputIt3, typename OutputIt, typename Func>
			OutputIt transform_many( InputIt1 first_in1, InputIt1 last_in1, InputIt2 first_in2, InputIt3 first_in3, OutputIt first_out, Func func ) {
				static_assert(!is_const_v<decltype(*first_out)>, "Output iterator cannot point to const data");
				auto const dist = std::distance( first_in1, last_in1 );
				for_each( 0, dist, [&]( size_t n ) {
					*(first_out + n) = func( *(first_in1 + n), *(first_in2 + n), *(first_in3 + n) );
				} );
				return first_out;
			}

			template<typename InputIt1, typename InputIt2, typename InputIt3, typename InputIt4, typename OutputIt, typename Func>
			OutputIt transform_many( InputIt1 first_in1, InputIt1 last_in1, InputIt2 first_in2, InputIt3 first_in3, InputIt4 first_in4, OutputIt first_out, Func func ) {
				static_assert(!is_const_v<decltype(*first_out)>, "Output iterator cannot point to const data");
				auto const dist = std::distance( first_in1, last_in1 );
				for_each( 0, dist, [&]( size_t n ) {
					*(first_out + n) = func( *(first_in1 + n), *(first_in2 + n), *(first_in3 + n), *(first_in4 + n) );
				} );
				return first_out;
			}

			template<typename InputIt1, typename InputIt2, typename InputIt3, typename InputIt4, typename InputIt5, typename OutputIt, typename Func>
			OutputIt transform_many( InputIt1 first_in1, InputIt1 last_in1, InputIt2 first_in2, InputIt3 first_in3, InputIt4 first_in4, InputIt4 first_in5, OutputIt first_out, Func func ) {
				static_assert(!is_const_v<decltype(*first_out)>, "Output iterator cannot point to const data");
				auto const dist = std::distance( first_in1, last_in1 );
				for_each( 0, dist, [&]( size_t n ) {
					*(first_out + n) = func( *(first_in1 + n), *(first_in2 + n), *(first_in3 + n), *(first_in4 + n), *(first_in5 + n) );
				} );
				return first_out;
			}

		}	// namespace parallel	
	}	// namespace algorithm
}	// namespace daw
