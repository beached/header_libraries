// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/ciso646.h"
#include "daw/daw_arith_traits.h"
#include "daw/daw_cxmath.h"
#include "daw/daw_do_not_optimize.h"
#include "daw/daw_expected.h"
#include "daw/daw_move.h"
#include "daw/daw_string_view.h"

#include <array>
#include <chrono>
#include <exception>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

namespace daw {
	namespace benchmark_impl {
		using second_duration = std::chrono::duration<double>;
	} // namespace benchmark_impl

	template<typename F>
	DAW_ATTRIB_NOINLINE double benchmark( F &&func ) {
		static_assert( std::is_invocable_v<F>, "func must accept no arguments" );
		auto start = std::chrono::steady_clock::now( );
		(void)DAW_FWD( func )( );
		auto finish = std::chrono::steady_clock::now( );
		benchmark_impl::second_duration duration = finish - start;
		return duration.count( );
	}

	namespace utility {
		template<typename T>
		[[nodiscard]] std::string format_seconds( T t, size_t prec = 0 ) {
			std::stringstream ss;
			ss << std::setprecision( static_cast<int>( prec ) ) << std::fixed;
			auto val = static_cast<double>( t ) * 1'000'000'000'000'000.0;
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
		[[nodiscard]] std::string to_bytes_per_second( Bytes bytes, Time t = 1.0,
		                                               size_t prec = 1 ) {
			std::stringstream ss;
			ss << std::setprecision( static_cast<int>( prec ) ) << std::fixed;
			auto val = static_cast<double>( bytes ) / static_cast<double>( t );
			if( val < 1000.0 ) {
				ss << ( static_cast<double>( val * 100.0 ) / 100 ) << "bytes";
				return ss.str( );
			}
			val /= 1000.0;
			if( val < 1000.0 ) {
				ss << ( static_cast<double>( val * 100.0 ) / 100 ) << "KB";
				return ss.str( );
			}
			val /= 1000.0;
			if( val < 1000.0 ) {
				ss << ( static_cast<double>( val * 100.0 ) / 100 ) << "MB";
				return ss.str( );
			}
			val /= 1000.0;
			if( val < 1000.0 ) {
				ss << ( static_cast<double>( val * 100.0 ) / 100 ) << "GB";
				return ss.str( );
			}
			val /= 1000.0;
			if( val < 1000.0 ) {
				ss << ( static_cast<double>( val * 100.0 ) / 100 ) << "TB";
				return ss.str( );
			}
			val /= 1000.0;
			ss << ( static_cast<double>( val * 100.0 ) / 100 ) << "PB";
			return ss.str( );
		}
	} // namespace utility

	template<typename Func>
	DAW_ATTRIB_NOINLINE void
	show_benchmark( size_t data_size_bytes, std::string const &title, Func &&func,
	                size_t data_prec = 1, size_t time_prec = 0,
	                size_t item_count = 1 ) {
		double const t = benchmark( DAW_FWD( func ) );
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

	template<typename Test, typename... Args>
	DAW_ATTRIB_NOINLINE auto bench_test( std::string const &title,
	                                     Test &&test_callable, Args &&...args ) {
		static_assert( std::is_invocable_v<Test, Args...>,
		               "Unable to call Test with provided Args" );
		using result_t = std::invoke_result_t<Test, Args...>;
		auto result = daw::expected_t<result_t>{ };
#if defined( DAW_USE_EXCEPTIONS )
		try {
#endif
			auto const start = std::chrono::steady_clock::now( );
			if constexpr( std::is_same_v<result_t, void> ) {
				DAW_FWD( test_callable )( DAW_FWD( args )... );
				result.set_value( );
			} else {
				result.set_value( DAW_FWD( test_callable )( DAW_FWD( args )... ) );
			}
			auto const finish = std::chrono::steady_clock::now( );
			benchmark_impl::second_duration const duration = finish - start;
			std::cout << title << " took "
			          << utility::format_seconds( duration.count( ), 2 ) << '\n';
#if defined( DAW_USE_EXCEPTIONS )
		} catch( ... ) { result.set_exception( ); }
#endif
		return result;
	}

	template<typename Test, typename... Args>
	DAW_ATTRIB_NOINLINE auto bench_test2( std::string const &title,
	                                      Test &&test_callable, size_t item_count,
	                                      Args &&...args ) {
		static_assert( std::is_invocable_v<Test, Args...>,
		               "Unable to call Test with provided Args" );
		using result_t = std::invoke_result_t<Test, Args...>;
		auto result = daw::expected_t<result_t>{ };
#if defined( DAW_USE_EXCEPTIONS )
		try {
#endif
			auto const start = std::chrono::steady_clock::now( );
			if constexpr( std::is_same_v<result_t, void> ) {
				DAW_FWD( test_callable )( DAW_FWD( args )... );
				result.set_value( );
			} else {
				result.set_value( DAW_FWD( test_callable )( DAW_FWD( args )... ) );
			}
			auto const finish = std::chrono::steady_clock::now( );
			benchmark_impl::second_duration const duration = finish - start;
			std::cout << title << " took "
			          << utility::format_seconds( duration.count( ), 2 );
			if( item_count > 1 ) {
				std::cout << " to process " << item_count << " items at "
				          << utility::format_seconds(
				               ( duration / item_count ).count( ), 2 )
				          << " per item\n";
			} else {
				std::cout << '\n';
			}
#if defined( DAW_USE_EXCEPTIONS )
		} catch( ... ) { result.set_exception( ); }
#endif
		return result;
	}

	/// @tparam Runs Number of runs to do
	/// @tparam delem delemiter in output
	/// @tparam Test Callable type to benchmark
	/// @tparam Args types passed to callable
	/// @param title Title of benchmark
	/// @param test_callable callable to benchmark
	/// @param args arg values to pass to callable
	/// @return last value from callable
	template<size_t Runs, char delem = '\n', typename Test, typename... Args>
	DAW_ATTRIB_NOINLINE auto bench_n_test( std::string const &title,
	                                       Test &&test_callable,
	                                       Args const &...args ) noexcept {
		static_assert( Runs > 0 );
		static_assert( std::is_invocable_v<Test, Args...>,
		               "Unable to call Test with provided Args" );

		double base_time = max_value<double>;
		{
			for( size_t n = 0; n < 1000; ++n ) {
				daw::do_not_optimize( args... );

				int a = 0;
				daw::do_not_optimize( a );
				auto const start = std::chrono::steady_clock::now( );
				auto r = daw::expected_from_code( [a]( ) mutable {
					daw::do_not_optimize( a );
					return a * a;
				} );
				auto const finish = std::chrono::steady_clock::now( );
				daw::do_not_optimize( r );
				auto const duration =
				  benchmark_impl::second_duration( finish - start ).count( );
				if( duration < base_time ) {
					base_time = duration;
				}
			}
		}
		double min_time = max_value<double>;
		double max_time = 0.0;

		auto const total_start = std::chrono::steady_clock::now( );
		for( size_t n = 0; n < Runs; ++n ) {
			daw::do_not_optimize( args... );
			auto const start = std::chrono::steady_clock::now( );
#if defined( DAW_USE_EXCEPTIONS )
			try {
#endif
				(void)test_callable( args... );
#if defined( DAW_USE_EXCEPTIONS )
			} catch( ... ) {}
#endif
			auto const finish = std::chrono::steady_clock::now( );
			auto const duration =
			  benchmark_impl::second_duration( finish - start ).count( );
			if( duration < min_time ) {
				min_time = duration;
			}
			if( duration > max_time ) {
				max_time = duration;
			}
		}
		auto const total_finish = std::chrono::steady_clock::now( );

		using result_t = std::invoke_result_t<Test, Args...>;
		auto result = daw::expected_t<result_t>{ };
#if defined( DAW_USE_EXCEPTIONS )
		try {
#endif
			if constexpr( std::is_same_v<result_t, void> ) {
				test_callable( args... );
				result.set_value( );
			} else {
				result.set_value( test_callable( args... ) );
			}
#if defined( DAW_USE_EXCEPTIONS )
		} catch( ... ) { result.set_exception( ); }
#endif

		min_time -= base_time;
		max_time -= base_time;
		auto total_time =
		  benchmark_impl::second_duration( total_finish - total_start ).count( ) -
		  static_cast<double>( Runs ) * base_time;

		auto avg_time = [&] {
			if constexpr( Runs >= 10 ) {
				return ( total_time - max_time ) / static_cast<double>( Runs - 1 );
			} else if constexpr( Runs > 1 ) {
				return total_time / static_cast<double>( Runs );
			} else /* Runs == 1 */ {
				return ( total_time - max_time ) / 1.0;
			}
		}( );

		avg_time -= base_time;
		std::cout << title << delem << "	runs: " << Runs << delem
		          << "	total: " << utility::format_seconds( total_time, 2 )
		          << delem << "	avg:   " << utility::format_seconds( avg_time, 2 )
		          << delem << "	min:   " << utility::format_seconds( min_time, 2 )
		          << delem << "	max:   " << utility::format_seconds( max_time, 2 )
		          << '\n';
		return result;
	}

	/// @tparam Runs Number of runs
	/// @tparam delem Delemiter in output
	/// @tparam Validator Callable to validate results
	/// @tparam Function Callable type to be timed
	/// @tparam Args types to pass to callable
	/// @param title Title for output
	/// @param bytes Size of data in bytes
	/// @param validator validatio object that takes func's result as arg
	/// @param func Callable value to bench
	/// @param args args values to pass to func
	/// @return last result timing counts of runs
	template<size_t Runs, char delem = '\n', typename Validator,
	         typename Function, typename... Args>
	[[nodiscard]] DAW_ATTRIB_NOINLINE std::array<double, Runs>
	bench_n_test_mbs2( std::string const &, size_t, Validator &&validator,
	                   Function &&func, Args const &...args ) noexcept {
		static_assert( Runs > 0 );
		static_assert( std::is_invocable_v<Function, Args...>,
		               "Unable to call Function with provided Args" );
		static_assert(
		  std::is_invocable_v<Validator, std::invoke_result_t<Function, Args...>>,
		  "Validator must be callable with the results of Function" );

		auto results = std::array<double, Runs>{ };
		double base_time = max_value<double>;
		{
			for( size_t n = 0; n < 1000; ++n ) {
				int a = 0;
				daw::do_not_optimize( a );
				auto const start = std::chrono::steady_clock::now( );
				auto r = daw::expected_from_code( [a]( ) mutable {
					daw::do_not_optimize( a );
					return a * a;
				} );
				auto const finish = std::chrono::steady_clock::now( );
				daw::do_not_optimize( r );
				auto const duration =
				  benchmark_impl::second_duration( finish - start ).count( );
				if( duration < base_time ) {
					base_time = duration;
				}
			}
		}
		// auto const total_start = std::chrono::steady_clock::now( );
		benchmark_impl::second_duration valid_time = std::chrono::seconds( 0 );
		for( size_t n = 0; n < Runs; ++n ) {
			daw::do_not_optimize( args... );
			auto const start = std::chrono::steady_clock::now( );
			using result_t = std::invoke_result_t<Function, Args...>;
			result_t result = func( args... );
			auto const finish = std::chrono::steady_clock::now( );
			daw::do_not_optimize( result );
			auto const valid_start = std::chrono::steady_clock::now( );
			if( DAW_UNLIKELY( not validator( result ) ) ) {
				std::cerr << "Error validating result\n" << std::flush;
				std::terminate( );
			}
			valid_time += benchmark_impl::second_duration(
			  std::chrono::steady_clock::now( ) - valid_start );

			auto const duration =
			  benchmark_impl::second_duration( finish - start ).count( );
			results[n] = duration;
		}
		return results;
	}

	template<size_t Runs, char delem = '\n', typename Test, typename... Args>
	[[nodiscard]] DAW_ATTRIB_NOINLINE auto
	bench_n_test_mbs( std::string const &title, size_t bytes,
	                  Test &&test_callable, Args const &...args ) noexcept {
		static_assert( Runs > 0 );
		static_assert( std::is_invocable_v<Test, Args...>,
		               "Unable to call Test with provided Args" );
		double base_time = max_value<double>;
		{
			for( size_t n = 0; n < 1000; ++n ) {
				intmax_t a = 0;
				daw::do_not_optimize( a );
				auto const start = std::chrono::steady_clock::now( );
				auto r = daw::expected_from_code( [a]( ) mutable {
					daw::do_not_optimize( a );
					return a * a;
				} );
				auto const finish = std::chrono::steady_clock::now( );
				daw::do_not_optimize( r );
				auto const duration =
				  benchmark_impl::second_duration( finish - start ).count( );
				if( duration < base_time ) {
					base_time = duration;
				}
			}
		}
		double min_time = max_value<double>;
		double max_time = 0.0;

		auto const total_start = std::chrono::steady_clock::now( );
		for( size_t n = 0; n < Runs; ++n ) {
			daw::do_not_optimize( args... );
			auto const start = std::chrono::steady_clock::now( );
#if defined( DAW_USE_EXCEPTIONS )
			try {
#endif
				(void)test_callable( args... );
#if defined( DAW_USE_EXCEPTIONS )
			} catch( ... ) {}
#endif
			auto const finish = std::chrono::steady_clock::now( );

			auto const duration =
			  benchmark_impl::second_duration( finish - start ).count( );
			if( duration < min_time ) {
				min_time = duration;
			}
			if( duration > max_time ) {
				max_time = duration;
			}
		}
		auto const total_finish = std::chrono::steady_clock::now( );

		using result_t = std::invoke_result_t<Test, Args...>;
		auto result = daw::expected_t<result_t>{ };
#if defined( DAW_USE_EXCEPTIONS )
		try {
#endif
			if constexpr( std::is_same_v<result_t, void> ) {
				test_callable( args... );
				result.set_value( );
			} else {
				result.set_value( test_callable( args... ) );
			}
#if defined( DAW_USE_EXCEPTIONS )
		} catch( ... ) {
			result.set_exception( );
			return result;
		}
#endif
		min_time -= base_time;
		max_time -= base_time;
		auto total_time =
		  benchmark_impl::second_duration( total_finish - total_start ).count( ) -
		  static_cast<double>( Runs ) * base_time;

		auto avg_time = [&] {
			if constexpr( Runs >= 10 ) {
				return ( total_time - max_time ) / static_cast<double>( Runs - 1 );
			} else if constexpr( Runs > 1 ) {
				return total_time / static_cast<double>( Runs );
			} else /* Runs == 1 */ {
				return ( total_time - max_time ) / 1.0;
			}
		}( );
		avg_time -= base_time;
		std::cout << title << delem;

		std::cout << "	runs:        " << Runs << delem
		          << "	total:       " << utility::format_seconds( total_time, 2 )
		          << delem
		          << "	min:         " << utility::format_seconds( min_time, 2 )
		          << " -> " << utility::to_bytes_per_second( bytes, min_time, 2 )
		          << "/s" << delem
		          << "	avg:         " << utility::format_seconds( avg_time, 2 )
		          << " -> " << utility::to_bytes_per_second( bytes, avg_time, 2 )
		          << "/s" << delem
		          << "	max:         " << utility::format_seconds( max_time, 2 )
		          << " -> " << utility::to_bytes_per_second( bytes, max_time, 2 )
		          << "/s" << delem << "	runs/second: " << ( 1.0 / min_time )
		          << '\n';
		return result;
	} // namespace daw

	/// @tparam Runs Number of runs
	/// @tparam Function Callable type to be timed
	/// @tparam Args types to pass to callable
	/// @param func Callable value to bench
	/// @param args args values to pass to func
	/// @return last result timing counts of runs
	template<size_t Runs, typename Function, typename... Args>
	[[nodiscard]] DAW_ATTRIB_NOINLINE std::vector<std::chrono::nanoseconds>
	bench_n_test_json( Function &&func, Args &&...args ) noexcept {
		static_assert( Runs > 0 );
		static_assert( std::is_invocable_v<Function, Args...>,
		               "Unable to call Test with provided Args" );

		auto base_time = std::chrono::nanoseconds( max_value<long long> );
		{
			for( size_t n = 0; n < 1000; ++n ) {
				daw::do_not_optimize( args... );

				int a = 0;
				daw::do_not_optimize( a );
				auto const start = std::chrono::steady_clock::now( );
				auto r = daw::expected_from_code( [a]( ) mutable {
					auto const b = a;
					daw::do_not_optimize( a );
					return a + b;
				} );
				auto const finish = std::chrono::steady_clock::now( );
				daw::do_not_optimize( r );
				auto const duration = std::chrono::nanoseconds( finish - start );
				if( duration < base_time ) {
					base_time = duration;
				}
			}
		}
		auto results = std::vector<std::chrono::nanoseconds>( Runs );

		for( size_t n = 0; n < Runs; ++n ) {
			daw::do_not_optimize( func );
			daw::do_not_optimize( args... );
			auto const start = std::chrono::steady_clock::now( );
			(void)func( args... );
			auto const finish = std::chrono::steady_clock::now( );

			auto const duration =
			  std::chrono::nanoseconds( finish - start ) - base_time;
			results[n] = duration;
		}
		return results;
	}

	/// @tparam Runs Number of runs
	/// @tparam Validator Callable to validate results
	/// @tparam Function Callable type to be timed
	/// @tparam Args types to pass to callable
	/// @param validator validatio object that takes func's result as arg
	/// @param func Callable value to bench
	/// @param args args values to pass to func
	/// @return last result timing counts of runs
	template<size_t Runs, typename Validator, typename Function, typename... Args>
	DAW_ATTRIB_NOINLINE std::vector<std::chrono::nanoseconds>
	bench_n_test_json_val( Validator &&validator, Function &&func,
	                       Args &&...args ) noexcept {
		static_assert( Runs > 0 );
		static_assert( std::is_invocable_v<Function, Args...>,
		               "Unable to call Test with provided Args" );
		static_assert(
		  std::is_invocable_v<Validator, std::invoke_result_t<Function, Args...>>,
		  "Validator must be callable with the results of Function" );

		auto base_time = std::chrono::nanoseconds( max_value<long long> );
		{
			for( size_t n = 0; n < 1000; ++n ) {
				daw::do_not_optimize( args... );

				int a = 0;
				daw::do_not_optimize( a );
				auto const start = std::chrono::steady_clock::now( );
				auto r = daw::expected_from_code( [a]( ) mutable {
					auto const b = a;
					daw::do_not_optimize( a );
					return a + b;
				} );
				auto const finish = std::chrono::steady_clock::now( );
				daw::do_not_optimize( r );
				auto const duration = std::chrono::nanoseconds( finish - start );
				if( duration < base_time ) {
					base_time = duration;
				}
			}
		}

		auto results = std::vector<std::chrono::nanoseconds>( Runs );

		for( size_t n = 0; n < Runs; ++n ) {
			auto const start = std::chrono::steady_clock::now( );
			auto result = func( args... );
			auto const finish = std::chrono::steady_clock::now( );
			daw::do_not_optimize( result );
			if( DAW_UNLIKELY( not validator( std::move( result ) ) ) ) {
				std::cerr << "Error validating result\n" << std::flush;
				std::terminate( );
			}

			auto const duration =
			  std::chrono::nanoseconds( finish - start ) - base_time;
			results[n] = duration;
		}
		return results;
	}

	namespace benchmark_impl {
		template<typename T, typename = void>
		inline constexpr bool is_streamable_v = false;

		template<typename T>
		inline constexpr bool is_streamable_v<
		  T, std::void_t<decltype( std::declval<std::ios &>( )
		                           << std::declval<T const &>( ) )>> = true;

		template<typename T, typename U,
		         std::enable_if_t<is_streamable_v<T> and is_streamable_v<U>,
		                          std::nullptr_t> = nullptr>
		DAW_ATTRIB_NOINLINE void output_expected_error( T &&expected_result,
		                                                U &&result ) {
			std::cerr << "Invalid result. Expecting '" << expected_result
			          << "' but got '" << result << "'\n"
			          << std::flush;
		}

		template<
		  typename T, typename U,
		  std::enable_if_t<(not is_streamable_v<T> and not is_streamable_v<U>),
		                   std::nullptr_t> = nullptr>
		DAW_ATTRIB_NOINLINE constexpr void output_expected_error( T &&, U && ) {
			std::cerr << "Invalid or unexpected result\n" << std::flush;
		}
	} // namespace benchmark_impl

	template<typename T, typename U>
	DAW_ATTRIB_NOINLINE constexpr void expecting( T &&expected_result,
	                                              U &&result ) {
		if( not( daw::cmp_equal( expected_result, result ) ) ) {
#ifdef __VERSION__
			if constexpr( not daw::string_view( __VERSION__ )
			                    .starts_with( daw::string_view(
			                      "4.2.1 Compatible Apple LLVM" ) ) ) {
				benchmark_impl::output_expected_error( expected_result, result );
			}
#endif
			std::terminate( );
		}
	}

	template<typename Bool>
	DAW_ATTRIB_NOINLINE constexpr void expecting( Bool const &expected_result ) {
		if( not static_cast<bool>( expected_result ) ) {
			std::cerr << "Invalid result. Expecting true\n" << std::flush;
			std::terminate( );
		}
	}

	template<typename Bool, typename String>
	DAW_ATTRIB_NOINLINE constexpr void expecting_message( Bool &&expected_result,
	                                                      String &&message ) {
		do_not_optimize( expected_result );
		if( DAW_UNLIKELY( not( expected_result ) ) ) {
			std::cerr << message << '\n' << std::flush;
			std::terminate( );
		}
		(void)message;
	}

	namespace benchmark_impl {
		struct always_true {
			template<typename... Args>
			[[nodiscard]] DAW_ATTRIB_INLINE constexpr bool
			operator( )( Args &&... ) noexcept {
				return true;
			}
		};
	} // namespace benchmark_impl

	template<typename Exception = std::exception, typename Expression,
	         typename Predicate = benchmark_impl::always_true,
	         std::enable_if_t<std::is_invocable_v<Predicate, Exception>,
	                          std::nullptr_t> = nullptr>
	DAW_ATTRIB_NOINLINE void
	expecting_exception( Expression &&expression,
	                     Predicate &&pred = Predicate{ } ) {
#if defined( DAW_USE_EXCEPTIONS )
		try {
#endif
			(void)DAW_FWD( expression )( );
			(void)pred;
#if defined( DAW_USE_EXCEPTIONS )
		} catch( Exception const &ex ) {
			if( DAW_FWD( pred )( ex ) ) {
				return;
			}
			std::cerr << "Failed predicate\n" << std::flush;
		} catch( ... ) {
			std::cerr << "Unexpected exception\n" << std::flush;
			throw;
		}
		std::terminate( );
#endif
	}
} // namespace daw
