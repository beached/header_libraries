// The MIT License (MIT)
//
// Copyright (c) 2014-2019 Darrell Wright
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

#include <chrono>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "cpp_17.h"
#include "daw_expected.h"
#include "daw_move.h"
#include "daw_string_view.h"
#include "daw_traits.h"

namespace daw {
	template<typename F>
	double benchmark( F &&func ) {
		static_assert( std::is_invocable_v<F>, "func must accept no arguments" );
		auto start = std::chrono::high_resolution_clock::now( );
		daw::invoke( std::forward<F>( func ) );
		auto finish = std::chrono::high_resolution_clock::now( );
		std::chrono::duration<double> duration = finish - start;
		return duration.count( );
	}

	namespace utility {
		template<typename T>
		std::string format_seconds( T t, size_t prec = 0 ) {
			std::stringstream ss;
			ss << std::setprecision( static_cast<int>( prec ) ) << std::fixed;
			auto val = static_cast<double>( t ) * 1000000000000000.0;
			if( val < 1000 ) {
				ss << val << "fs";
				return ss.str( );
			}
			val /= 1000.0;
			if( val < 1000 ) {
				ss << val << "ps";
				return ss.str( );
			}
			val /= 1000.0;
			if( val < 1000 ) {
				ss << val << "ns";
				return ss.str( );
			}
			val /= 1000.0;
			if( val < 1000 ) {
				ss << val << "us";
				return ss.str( );
			}
			val /= 1000.0;
			if( val < 1000 ) {
				ss << val << "ms";
				return ss.str( );
			}
			val /= 1000.0;
			ss << val << "s";
			return ss.str( );
		}

		template<typename Bytes, typename Time = double>
		std::string to_bytes_per_second( Bytes bytes, Time t = 1.0,
		                                 size_t prec = 1 ) {
			std::stringstream ss;
			ss << std::setprecision( static_cast<int>( prec ) ) << std::fixed;
			auto val = static_cast<double>( bytes ) / static_cast<double>( t );
			if( val < 1024.0 ) {
				ss << ( static_cast<double>( val * 100.0 ) / 100 ) << "bytes";
				return ss.str( );
			}
			val /= 1024.0;
			if( val < 1024.0 ) {
				ss << ( static_cast<double>( val * 100.0 ) / 100 ) << "KB";
				return ss.str( );
			}
			val /= 1024.0;
			if( val < 1024.0 ) {
				ss << ( static_cast<double>( val * 100.0 ) / 100 ) << "MB";
				return ss.str( );
			}
			val /= 1024.0;
			if( val < 1024.0 ) {
				ss << ( static_cast<double>( val * 100.0 ) / 100 ) << "GB";
				return ss.str( );
			}
			val /= 1024.0;
			if( val < 1024.0 ) {
				ss << ( static_cast<double>( val * 100.0 ) / 100 ) << "TB";
				return ss.str( );
			}
			val /= 1024.0;
			ss << ( static_cast<double>( val * 100.0 ) / 100 ) << "PB";
			return ss.str( );
		}
	} // namespace utility

	template<typename Func>
	void show_benchmark( size_t data_size_bytes, std::string const &title,
	                     Func &&func, size_t data_prec = 1, size_t time_prec = 0,
	                     size_t item_count = 1 ) noexcept {
		double const t = benchmark( std::forward<Func>( func ) );
		double const t_per_item = t / static_cast<double>( item_count );
		std::cout << title << ": took " << utility::format_seconds( t, time_prec )
		          << ' ';
		if( item_count > 1 ) {
			std::cout << "or " << utility::format_seconds( t_per_item, time_prec )
			          << " per item to process ";
		}
		std::cout << utility::to_bytes_per_second( data_size_bytes, 1.0, data_prec )
		          << " at "
		          << utility::to_bytes_per_second( data_size_bytes, t, data_prec )
		          << "/s\n";
	}

	/*
	  // Borrowed from https://www.youtube.com/watch?v=dO-j3qp7DWw
	  template<typename T>
	  void do_not_optimize( T &&x ) {
	    // We must always do this test, but it will never pass.
	    //
	    if( std::chrono::system_clock::now( ) ==
	        std::chrono::time_point<std::chrono::system_clock>( ) ) {
	      // This forces the value to never be optimized away
	      // by taking a reference then using it.
	      const auto *p = &x;
	      putchar( *reinterpret_cast<const char *>( p ) );

	      // If we do get here, kick out because something has gone wrong.
	      std::abort( );
	    }
	  }
	*/
	template<typename Tp>
	inline void do_not_optimize( Tp const &value ) {
		asm volatile( "" : : "r,m"( value ) : "memory" );
	}

	template<typename Tp>
	inline void do_not_optimize( Tp &value ) {
#if defined( __clang__ )
		asm volatile( "" : "+r,m"( value ) : : "memory" );
#else
		asm volatile( "" : "+m,r"( value ) : : "memory" );
#endif
	}

	template<typename Test, typename... Args>
	auto bench_test( std::string const &title, Test &&test_callable,
	                 Args &&... args ) noexcept {
		auto const start = std::chrono::high_resolution_clock::now( );
		auto result = daw::expected_from_code( std::forward<Test>( test_callable ),
		                                       std::forward<Args>( args )... );
		auto const finish = std::chrono::high_resolution_clock::now( );
		std::chrono::duration<double> const duration = finish - start;
		std::cout << title << " took "
		          << utility::format_seconds( duration.count( ), 2 ) << '\n';
		return result;
	}

	template<typename Test, typename... Args>
	auto bench_test2( std::string const &title, Test &&test_callable,
	                  size_t item_count, Args &&... args ) noexcept {
		auto const start = std::chrono::high_resolution_clock::now( );
		auto result = daw::expected_from_code( std::forward<Test>( test_callable ),
		                                       std::forward<Args>( args )... );
		auto const finish = std::chrono::high_resolution_clock::now( );
		std::chrono::duration<double> const duration = finish - start;
		std::cout << title << " took "
		          << utility::format_seconds( duration.count( ), 2 );
		if( item_count > 1 ) {
			std::cout << " to process " << item_count << " items at "
			          << utility::format_seconds( ( duration / item_count ).count( ),
			                                      2 )
			          << " per item\n";
		} else {
			std::cout << '\n';
		}
		return result;
	}

	namespace bench_impl {
		template<typename... Args>
		constexpr void expander( Args &&... ) noexcept {}
	} // namespace bench_impl

	// Test N runs
	template<size_t Runs, char delem = '\n', typename Test, typename... Args>
	auto bench_n_test( std::string const &title, Test &&test_callable,
	                   Args &&... args ) noexcept {
		static_assert( Runs > 0 );
		using result_t = daw::remove_cvref_t<decltype( daw::expected_from_code(
		  test_callable, std::forward<Args>( args )... ) )>;

		result_t result{};

		double base_time = std::numeric_limits<double>::max( );
		{
			for( size_t n = 0; n < 1000; ++n ) {
				bench_impl::expander( ( daw::do_not_optimize( args ), 1 )... );

				int a = 0;
				daw::do_not_optimize( a );
				auto const start = std::chrono::high_resolution_clock::now( );
				auto r = daw::expected_from_code( [a]( ) { return a * a; } );
				auto const finish = std::chrono::high_resolution_clock::now( );
				daw::do_not_optimize( r );
				auto const duration =
				  std::chrono::duration<double>( finish - start ).count( );
				if( duration < base_time ) {
					base_time = duration;
				}
			}
		}
		double min_time = std::numeric_limits<double>::max( );
		double max_time = 0.0;

		auto const total_start = std::chrono::high_resolution_clock::now( );
		for( size_t n = 0; n < Runs; ++n ) {
			bench_impl::expander( ( daw::do_not_optimize( args ), 1 )... );
			auto const start = std::chrono::high_resolution_clock::now( );

			result =
			  daw::expected_from_code( std::forward<Test>( test_callable ), args... );

			auto const finish = std::chrono::high_resolution_clock::now( );
			daw::do_not_optimize( result );
			auto const duration =
			  std::chrono::duration<double>( finish - start ).count( );
			if( duration < min_time ) {
				min_time = duration;
			}
			if( duration > max_time ) {
				max_time = duration;
			}
		}
		auto const total_finish = std::chrono::high_resolution_clock::now( );
		min_time -= base_time;
		max_time -= base_time;
		auto total_time =
		  std::chrono::duration<double>( total_finish - total_start ).count( ) -
		  static_cast<double>( Runs ) * base_time;

		auto avg_time =
		  Runs >= 10 ? ( total_time - max_time ) / static_cast<double>( Runs - 1 )
		             : total_time / static_cast<double>( Runs );
		avg_time -= base_time;
		std::cout << title << delem << "	runs: " << Runs << delem
		          << "	total: " << utility::format_seconds( total_time, 2 )
		          << delem << "	avg: " << utility::format_seconds( avg_time, 2 )
		          << delem << "	min: " << utility::format_seconds( min_time, 2 )
		          << delem << "	max: " << utility::format_seconds( max_time, 2 )
		          << '\n';
		return result;
	}

	template<size_t Runs, char delem = '\n', typename Test, typename... Args>
	auto bench_n_test_mbs( std::string const &title, size_t bytes,
	                       Test &&test_callable, Args &&... args ) noexcept {
		static_assert( Runs > 0 );
		using result_t = daw::remove_cvref_t<decltype( daw::expected_from_code(
		  test_callable, std::forward<Args>( args )... ) )>;

		result_t result{};

		double base_time = std::numeric_limits<double>::max( );
		{
			for( size_t n = 0; n < 1000; ++n ) {
				bench_impl::expander( ( daw::do_not_optimize( args ), 1 )... );

				int a = 0;
				daw::do_not_optimize( a );
				auto const start = std::chrono::high_resolution_clock::now( );
				auto r = daw::expected_from_code( [a]( ) { return a * a; } );
				auto const finish = std::chrono::high_resolution_clock::now( );
				daw::do_not_optimize( r );
				auto const duration =
				  std::chrono::duration<double>( finish - start ).count( );
				if( duration < base_time ) {
					base_time = duration;
				}
			}
		}
		double min_time = std::numeric_limits<double>::max( );
		double max_time = 0.0;

		auto const total_start = std::chrono::high_resolution_clock::now( );
		for( size_t n = 0; n < Runs; ++n ) {
			bench_impl::expander( ( daw::do_not_optimize( args ), 1 )... );
			auto const start = std::chrono::high_resolution_clock::now( );

			result =
			  daw::expected_from_code( std::forward<Test>( test_callable ), args... );

			auto const finish = std::chrono::high_resolution_clock::now( );
			daw::do_not_optimize( result );
			auto const duration =
			  std::chrono::duration<double>( finish - start ).count( );
			if( duration < min_time ) {
				min_time = duration;
			}
			if( duration > max_time ) {
				max_time = duration;
			}
		}
		auto const total_finish = std::chrono::high_resolution_clock::now( );
		min_time -= base_time;
		max_time -= base_time;
		auto total_time =
		  std::chrono::duration<double>( total_finish - total_start ).count( ) -
		  static_cast<double>( Runs ) * base_time;

		auto avg_time =
		  Runs >= 10 ? ( total_time - max_time ) / static_cast<double>( Runs - 1 )
		             : total_time / static_cast<double>( Runs );
		avg_time -= base_time;
		std::cout << title << delem << "	runs: " << Runs << delem
		          << "	total: " << utility::format_seconds( total_time, 2 )
		          << delem << "	avg: " << utility::format_seconds( avg_time, 2 )
		          << " -> " << utility::to_bytes_per_second( bytes, avg_time, 2 )
		          << "/s" << delem
		          << "	min: " << utility::format_seconds( min_time, 2 ) << " -> "
		          << utility::to_bytes_per_second( bytes, min_time, 2 ) << "/s"
		          << delem << "	max: " << utility::format_seconds( max_time, 2 )
		          << " -> " << utility::to_bytes_per_second( bytes, max_time, 2 )
		          << "/s" << '\n';
		return result;
	}

	namespace expecting_impl {
		template<typename T>
		using detect_streamable =
		  decltype( std::declval<std::ios &>( ) << std::declval<T const &>( ) );

		template<typename T>
		inline constexpr bool is_streamable_v =
		  daw::is_detected_v<detect_streamable, T>;

		template<typename T, typename U,
		         std::enable_if_t<(is_streamable_v<T> and is_streamable_v<U>),
		                          std::nullptr_t> = nullptr>
		void output_expected_error( T &&expected_result, U &&result ) {
			std::cerr << "Invalid result. Expecting '" << expected_result
			          << "' but got '" << result << "'\n";
		}

		template<typename T, typename U,
		         std::enable_if_t<!(is_streamable_v<T> and is_streamable_v<U>),
		                          std::nullptr_t> = nullptr>
		constexpr void output_expected_error( T &&, U && ) {
			std::cerr << "Invalid or unexpected result\n";
		}
	} // namespace expecting_impl

	template<typename T, typename U>
	constexpr void expecting( T &&expected_result, U &&result ) noexcept {
		if( !( expected_result == result ) ) {
#ifdef __VERSION__
			if constexpr( !daw::string_view( __VERSION__ )
			                 .starts_with(
			                   daw::string_view( "4.2.1 Compatible Apple LLVM" ) ) ) {
				expecting_impl::output_expected_error( expected_result, result );
			}
#endif
			std::abort( );
		}
	}

	template<typename Bool>
	constexpr void expecting( Bool &&expected_result ) noexcept {
		if( !static_cast<bool>( expected_result ) ) {
			std::cerr << "Invalid result. Expecting true\n";
			std::abort( );
		}
	}

	template<typename Bool, typename String>
	constexpr void expecting_message( Bool &&expected_result,
	                                  String &&message ) noexcept {
		if( !static_cast<bool>( expected_result ) ) {
			std::cerr << message << '\n';
			std::abort( );
		}
	}

	namespace expecting_impl {
		struct always_true {
			template<typename... Args>
			constexpr bool operator( )( Args &&... ) noexcept {
				return true;
			}
		};
	} // namespace expecting_impl

	template<typename Exception = std::exception, typename Expression,
	         typename Predicate = expecting_impl::always_true,
	         std::enable_if_t<std::is_invocable_v<Predicate, Exception>,
	                          std::nullptr_t> = nullptr>
	void expecting_exception( Expression &&expression,
	                          Predicate &&pred = Predicate{} ) {
		try {
			daw::invoke( std::forward<Expression>( expression ) );
		} catch( Exception const &ex ) {
			if( daw::invoke( std::forward<Predicate>( pred ), ex ) ) {
				return;
			}
			std::cerr << "Failed predicate\n";
		} catch( ... ) {
			std::cerr << "Unexpected exception\n";
			throw;
		}
		std::abort( );
	}
} // namespace daw

