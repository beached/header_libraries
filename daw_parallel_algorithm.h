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
			namespace non {
				using namespace daw::algorithm;
				using std::transform;
			}
		
			template<typename Func>
			void for_each( size_t first, size_t last, Func func ) {
				auto const nthreads = std::thread::hardware_concurrency( );
				auto const chunk_sz = (last - first) / nthreads;
				std::vector<std::future<void>> workers;
				for( auto n = first; n < last; n += chunk_sz ) {
					workers.push_back( std::async( std::launch::async, [start = n, finish = std::min( n + chunk_sz, last ), func]( ) {
						for( auto i = start; i != finish; ++i ) {
							func( i );
						}
					} ) );
				}
			}

			template<typename IterFirst, typename IterLast, typename Func>
			void for_each_it( IterFirst first, IterLast last, Func func ) {
				auto const nthreads = std::thread::hardware_concurrency( );
				auto const rng_sz = std::distance( first, last );
				auto const chunk_sz =  rng_sz / nthreads;
				std::vector<std::future<void>> workers;
				size_t count = 0;
				size_t next_chunk_sz = count + chunk_sz <= rng_sz ? chunk_sz : rng_sz - count;
				auto it = first;
				while( it != last ) {
					next_chunk_sz = count + chunk_sz <= rng_sz ? chunk_sz : rng_sz - count;
					auto next_pos = it;
					std::advance( next_pos, next_chunk_sz );
					workers.push_back( std::async( std::launch::async, [start = it, finish = next_pos, func]( ) {
						for( auto i = start; i != finish; ++i ) {
							func( *i );
						}
					} ) );
					it = next_pos;
				}
			}

			template<typename InputIt1, typename OutputIt, typename Func>
			OutputIt transform( InputIt1 first_in1, InputIt1 last_in1, OutputIt first_out, Func func ) {
				for_each_it( first_in1, last_in1, [&]( auto val ) {
					auto res_it = first_out;
					auto in_it1 = first_in1;
					std::advance( res_it, n );
					std::advance( in_it1, n );
					*res_it = func( *in_it1 );
				} );
				return first_out;
			}

			template<typename InputIt1, typename InputIt2, typename OutputIt, typename Func>
			OutputIt transform_many( InputIt1 first_in1, InputIt1 last_in1, InputIt2 first_in2, OutputIt first_out, Func func ) {
				auto const dist = std::distance( first_in1, last_in1 );
				for_each( 0, dist, [&]( size_t n ) {
					auto res_it = first_out;
					auto in_it1 = first_in1;
					auto in_it2 = first_in2;
					std::advance( res_it, n );
					std::advance( in_it1, n );
					std::advance( in_it2, n );
					*res_it = func( *in_it1, *in_it2 );
				} );
				return first_out;
			}


			template<typename InputIt1, typename InputIt2, typename InputIt3, typename OutputIt, typename Func>
			OutputIt transform_many( InputIt1 first_in1, InputIt1 last_in1, InputIt2 first_in2, InputIt3 first_in3, OutputIt first_out, Func func ) {
				auto const dist = std::distance( first_in1, last_in1 );
				for_each( 0, dist, [&]( size_t n ) {
					auto res_it = first_out;
					auto in_it1 = first_in1;
					auto in_it2 = first_in2;
					auto in_it3 = first_in3;
					std::advance( res_it, n );
					std::advance( in_it1, n );
					std::advance( in_it2, n );
					std::advance( in_it3, n );
					*res_it = func( *in_it1, *in_it2, *in_it3 );
				} );
				return first_out;
			}

			template<typename InputIt1, typename InputIt2, typename InputIt3, typename InputIt4, typename OutputIt, typename Func>
			OutputIt transform_many( InputIt1 first_in1, InputIt1 last_in1, InputIt2 first_in2, InputIt3 first_in3, InputIt4 first_in4, OutputIt first_out, Func func ) {
				auto const dist = std::distance( first_in1, last_in1 );
				for_each( 0, dist, [&]( size_t n ) {
					auto res_it = first_out;
					auto in_it1 = first_in1;
					auto in_it2 = first_in2;
					auto in_it3 = first_in3;
					auto in_it4 = first_in4;
					std::advance( res_it, n );
					std::advance( in_it1, n );
					std::advance( in_it2, n );
					std::advance( in_it3, n );
					std::advance( in_it4, n );
					*res_it = func( *in_it1, *in_it2, *in_it3, *in_it4 );
				} );
				return first_out;
			}

			template<typename InputIt1, typename InputIt2, typename InputIt3, typename InputIt4, typename InputIt5, typename OutputIt, typename Func>
			OutputIt transform_many( InputIt1 first_in1, InputIt1 last_in1, InputIt2 first_in2, InputIt3 first_in3, InputIt4 first_in4, InputIt4 first_in5, OutputIt first_out, Func func ) {
				auto const dist = std::distance( first_in1, last_in1 );
				for_each( 0, dist, [&]( size_t n ) {
					auto res_it = first_out;
					auto in_it1 = first_in1;
					auto in_it2 = first_in2;
					auto in_it3 = first_in3;
					auto in_it4 = first_in4;
					auto in_it5 = first_in5;
					std::advance( res_it, n );
					std::advance( in_it1, n );
					std::advance( in_it2, n );
					std::advance( in_it3, n );
					std::advance( in_it4, n );
					std::advance( in_it5, n );
					*res_it = func( *in_it1, *in_it2, *in_it3, *in_it4, *in_it5 );
				} );
				return first_out;
			}

		}	// namespace parallel	
	}	// namespace algorithm
}	// namespace daw

