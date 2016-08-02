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
#include <list>
#include <future>
#include "daw_zipcontainer.h"

namespace daw {
	namespace algorithm {
		namespace parallel {
			namespace non {
				using namespace daw::algorithm;
				using std::transform;
			}

			using std_task_t = std::function<void( )>;
			template<typename task_t> class task_manager;
//
//			template<typename task_t=std_task_t>
//			std::shared_ptr<task_manager<task_t>> get_task_manager( ) {
//				static const size_t number_of_workers = std::thread::hardware_concurrency( ) > 0 ? std::thread::hardware_concurrency( ) + 1 : 2;
//				static auto manager = std::shared_ptr<task_manager<task_t>>( new task_manager<task_t>( number_of_workers ) );
//
//				return manager; 
//			}
//
//			auto std_task_manager( ) {
//				return get_task_manager<std_task_t>( );
//			}
//
//			template<typename task_t>
//			class task_manager {
//				size_t m_threads;
//				std::list<std::future<void>> m_workers;
//				std::vector<task_t> m_tasks;
//				std::atomic<bool> m_continue;
//
//				void job_loop( ) {
//					while( m_continue ) {
//						get_task( )( );
//					}
//				}
//
//				auto get_task( ) {
//					auto result = m_tasks.back( );
//					m_tasks.pop_back( );
//					return result;
//				}
//
//				task_manager( size_t number_of_workers ):m_threads( number_of_workers ), m_workers( number_of_workers, std::async( std::launch::async, [&] { job_loop( ); } ) ), m_tasks( ), m_continue( true ) { }
//			public:
//				template<typename T>
//				friend std::shared_ptr<task_manager<T>> get_task_manager( );
//
//				void stop( ) {
//					m_continue = false;
//				}
//
//				~task_manager( ) {
//					stop( );
//				}
//				
//				task_manager( task_manager const & ) = delete;
//				task_manager & operator=( task_manager const & ) = delete;
//
//				task_manager( task_manager && ) = default;
//				task_manager & operator=( task_manager && ) = default;
//
//				explicit operator bool( ) const {
//					return m_continue;
//				}
//				
//				void add_task( task_t task ) {
//					m_tasks.push_back( task );
//				}
//
//				auto max_concurrent( ) const {
//					return m_threads;
//				}
//
//			};	// struct task_manager
//		
			template<typename Func>
			void for_each( size_t first, size_t last, Func func ) {
				static size_t const nthreads = std::thread::hardware_concurrency( ) > 0 ? std::thread::hardware_concurrency( ) : 1; 
				auto const chunk_sz = (last - first) / nthreads;
				std::vector<std::future<void>> workers;
				for( size_t n = 0; n<nthreads; ++n ) {
					workers.push_back( std::async( std::launch::async, [=]( ) {
						auto const start = first + n*chunk_sz;
						auto const finish = start + chunk_sz <= last ? start + chunk_sz : last; 
						for( auto i = start; i < finish; ++i ) {
							func( i );
						}
					} ) );
				}
			}

//			template<typename ForwardIteratorFirst, typename ForwardIteratorLast, typename Func>
//			void for_each_it( ForwardIteratorFirst first, ForwardIteratorLast last, Func func ) {
//				size_t const sz = std::distance( first, last );
//				assert( sz >= 0 );
//				auto const max_chunk_sz = sz/std_task_manager( )->max_concurrent( );
//				size_t pos = 0;
//				auto it_begin = first; 
//				auto next_last = clamp( pos + max_chunk_sz, sz );
//				auto it_end = it_begin;
//				std::advance( it_end, next_last );
//				std::vector<std::future<void( )>> tasks;
//				while( pos < sz ) {
//					tasks.push_back( std::async( std::launch::async, [start = it_begin, finish = it_end, func]( ) {
//						for( auto it = start; it != finish; ++it ) {
//							func( it );
//						}
//					} ) );
//					it_begin = it_end;
//					pos += next_last;
//					next_last = clamp( pos + max_chunk_sz, sz );
//					std::advance( it_end, next_last );
//				}
//			}
//
//			//////////////////////////////////////////////////////////////////////////
//			/// Summary: Iterate over provided iterators.  Assumes that iterator
//			/// 0 is start and 1 is finish.  
//			template<typename... ForwardIterator, typename Func>
//			void for_each_it( ZipIter<ForwardIterator...> z_fwdit, Func func ) {
//				static_assert(ZipIter<ForwardIterator...>::size( ) >= 3, "Must supply parameters like {out, begin1, end1, ..., beginn, endn}");
//				auto it_begin = std::get<0>( z_fwdit.as_tuple( ) );
//				size_t const sz = std::distance( it_begin, std::get<1>( z_fwdit.as_tuple( ) ) );
//				assert( sz >= 0 );
//				auto const max_chunk_sz = sz / std_task_manager( )->max_concurrent( );
//				size_t pos = 0;
//				auto next_last = clamp( pos + max_chunk_sz, sz );
//				auto it_end = it_begin;
//				std::advance( it_end, next_last );
//				std::vector<std::future<void( )>> tasks;
//				while( pos < sz ) {
//					tasks.push_back( std::async( std::launch::async, [start = it_begin, finish = it_end, func]( ) {
//						for( auto it = start; it != finish; ++it ) {
//							func( it );
//						}
//					} ) );
//					it_begin = it_end;
//					pos += next_last;
//					next_last = clamp( pos + max_chunk_sz, sz );
//					std::advance( it_end, next_last );
//				}
//			}
//
//			template<typename InputIt1, typename OutputIt, typename Func>
//			OutputIt transform( InputIt1 first_in1, InputIt1 last_in1, OutputIt first_out, Func func ) {
//				auto zit = make_zipiter( first_out, first_in1, last_in1 );
//				for_each_it( zit, []( auto it ) {
//					*std::get<2>( it ) == *std::get<0>( it );
//				} );
//				return first_out;
//			}
//
//			template<typename InputIt1, typename InputIt2, typename OutputIt, typename Func>
//			OutputIt transform_many( InputIt1 first_in1, InputIt1 last_in1, InputIt2 first_in2, OutputIt first_out, Func func ) {
//				auto const dist = std::distance( first_in1, last_in1 );
//				for_each( 0, dist, [&]( size_t n ) {
//					auto res_it = first_out;
//					auto in_it1 = first_in1;
//					auto in_it2 = first_in2;
//					std::advance( res_it, n );
//					std::advance( in_it1, n );
//					std::advance( in_it2, n );
//					*res_it = func( *in_it1, *in_it2 );
//				} );
//				return first_out;
//			}
//
//
//			template<typename InputIt1, typename InputIt2, typename InputIt3, typename OutputIt, typename Func>
//			OutputIt transform_many( InputIt1 first_in1, InputIt1 last_in1, InputIt2 first_in2, InputIt3 first_in3, OutputIt first_out, Func func ) {
//				auto const dist = std::distance( first_in1, last_in1 );
//				for_each( 0, dist, [&]( size_t n ) {
//					auto res_it = first_out;
//					auto in_it1 = first_in1;
//					auto in_it2 = first_in2;
//					auto in_it3 = first_in3;
//					std::advance( res_it, n );
//					std::advance( in_it1, n );
//					std::advance( in_it2, n );
//					std::advance( in_it3, n );
//					*res_it = func( *in_it1, *in_it2, *in_it3 );
//				} );
//				return first_out;
//			}
//
//			template<typename InputIt1, typename InputIt2, typename InputIt3, typename InputIt4, typename OutputIt, typename Func>
//			OutputIt transform_many( InputIt1 first_in1, InputIt1 last_in1, InputIt2 first_in2, InputIt3 first_in3, InputIt4 first_in4, OutputIt first_out, Func func ) {
//				auto const dist = std::distance( first_in1, last_in1 );
//				for_each( 0, dist, [&]( size_t n ) {
//					auto res_it = first_out;
//					auto in_it1 = first_in1;
//					auto in_it2 = first_in2;
//					auto in_it3 = first_in3;
//					auto in_it4 = first_in4;
//					std::advance( res_it, n );
//					std::advance( in_it1, n );
//					std::advance( in_it2, n );
//					std::advance( in_it3, n );
//					std::advance( in_it4, n );
//					*res_it = func( *in_it1, *in_it2, *in_it3, *in_it4 );
//				} );
//				return first_out;
//			}
//
//			template<typename InputIt1, typename InputIt2, typename InputIt3, typename InputIt4, typename InputIt5, typename OutputIt, typename Func>
//			OutputIt transform_many( InputIt1 first_in1, InputIt1 last_in1, InputIt2 first_in2, InputIt3 first_in3, InputIt4 first_in4, InputIt4 first_in5, OutputIt first_out, Func func ) {
//				auto const dist = std::distance( first_in1, last_in1 );
//				for_each( 0, dist, [&]( size_t n ) {
//					auto res_it = first_out;
//					auto in_it1 = first_in1;
//					auto in_it2 = first_in2;
//					auto in_it3 = first_in3;
//					auto in_it4 = first_in4;
//					auto in_it5 = first_in5;
//					std::advance( res_it, n );
//					std::advance( in_it1, n );
//					std::advance( in_it2, n );
//					std::advance( in_it3, n );
//					std::advance( in_it4, n );
//					std::advance( in_it5, n );
//					*res_it = func( *in_it1, *in_it2, *in_it3, *in_it4, *in_it5 );
//				} );
//				return first_out;
//			}
//
		}	// namespace parallel	
	}	// namespace algorithm
}	// namespace daw

