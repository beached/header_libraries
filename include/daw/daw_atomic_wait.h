// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_attributes.h"
#include "daw_concepts.h"

#include <atomic>
#include <chrono>
#include <thread>
#include <utility>

namespace daw {
	enum class wait_status { found, timeout };
}

namespace daw::atomic_impl {
	inline constexpr struct {
		DAW_ATTRIB_INLINE DAW_CPP23_STATIC_CALL_OP bool operator( )(
		  std::chrono::nanoseconds elapsed ) DAW_CPP23_STATIC_CALL_OP_CONST {
			if( elapsed > std::chrono::milliseconds( 128 ) ) {
				std::this_thread::sleep_for( std::chrono::milliseconds( 8 ) );
			} else if( elapsed > std::chrono::microseconds( 64 ) ) {
				std::this_thread::sleep_for( elapsed / 2 );
			} else if( elapsed > std::chrono::microseconds( 4 ) ) {
				std::this_thread::yield( );
			} else {
				// poll
			}
			return false;
		}
	} timed_backoff_policy{ };

	[[nodiscard]] bool
	poll_with_backoff( Fn<bool( std::chrono::nanoseconds )> auto &&backoff_policy,
	                   Fn<bool( )> auto &&func,
	                   std::chrono::nanoseconds max_elapsed ) {

		auto const start_time = std::chrono::high_resolution_clock::now( );
		int count = 0;
		while( true ) {
			if( func( ) ) {
				return true;
			}
			if( count < 64 ) {
				++count;
				continue;
			}
			auto const elapsed =
			  std::chrono::high_resolution_clock::now( ) - start_time;
			if( elapsed >= max_elapsed ) {
				return false;
			}
			if( backoff_policy( elapsed ) ) {
				return false;
			}
		}
	}
} // namespace daw::atomic_impl

namespace daw {

	template<typename T, typename Rep, typename Period>
	/**
	 * Waits for the atomic object to change from the old value or until the
	 * timeout expires.
	 *
	 * This function blocks the calling thread, periodically polling the atomic
	 * object, and returns as soon as the atomic value differs from the specified
	 * old value or the relative timeout period elapses.
	 *
	 * @tparam T The type of the atomic object.
	 * @tparam Rep The representation type of the relative time duration.
	 * @tparam Period The tick period type of the relative time duration.
	 *
	 * @param object Pointer to the atomic object to wait on.
	 * @param old The value that the atomic object is expected to differ from.
	 * @param rel_time The maximum duration to wait before timing out.
	 * @param order The memory order to use for atomic load operations. Defaults
	 * to `std::memory_order_acquire`.
	 *
	 * @return A wait_status indicating whether the atomic value changed or the
	 * timeout expired.
	 */
	[[nodiscard]] wait_status
	atomic_wait_for( std::atomic<T> const *object, T const &old,
	                 std::chrono::duration<Rep, Period> const &rel_time,
	                 std::memory_order order = std::memory_order_acquire ) {
		auto const final_time =
		  std::chrono::high_resolution_clock::now( ) + rel_time;
		auto current =
		  std::atomic_load_explicit( object, std::memory_order_acquire );

		for( int tries = 0; current == old and tries < 16; ++tries ) {
			std::this_thread::yield( );
			current = std::atomic_load_explicit( object, order );
		}
		auto const poll_fn = [&] {
			current = std::atomic_load_explicit( object, order );
			if( current != old ) {
				return true;
			}
			auto const current_time = std::chrono::high_resolution_clock::now( );
			auto const is_timed_out = current_time >= final_time;
			return is_timed_out;
		};
		(void)atomic_impl::poll_with_backoff( atomic_impl::timed_backoff_policy,
		                                      poll_fn, rel_time );
		if( current == old ) {
			return wait_status::timeout;
		}
		return wait_status::found;
	}

	/**
	 * Waits until the atomic object changes from the specified value or the
	 * timeout time is reached.
	 *
	 * This function blocks the calling thread until the atomic value
	 * pointed to by `object` is not equal to `old` or until the specified
	 * `timeout_time` is reached.
	 *
	 * @tparam T The type of the atomic object.
	 * @tparam Clock The clock type used to measure the timeout.
	 * @tparam Duration The duration type used to represent the timeout.
	 *
	 * @param object Pointer to the atomic object to wait on.
	 * @param old The value that the atomic object is expected to change from
	 * @param timeout_time The time point at which the wait is aborted if the
	 * value has not been reached.
	 * @param order The memory order to use. Defaults to
	 * `std::memory_order_acquire`.
	 *
	 * @return A `wait_status` indicating whether the wait was successful or if it
	 * timed out.
	 */
	template<typename T, typename Clock, typename Duration>
	[[nodiscard]] wait_status atomic_wait_until(
	  std::atomic<T> const *object, T const &old,
	  std::chrono::time_point<Clock, Duration> const &timeout_time,
	  std::memory_order order = std::memory_order_acquire ) {

		return daw::atomic_wait_for( object, std::move( old ),
		                             timeout_time - Clock::now( ), order );
	}

	/**
	 * Waits until the atomic object satisfies the given predicate.
	 *
	 * This function blocks the calling thread until the value of the atomic
	 * object pointed to by `object` satisfies the provided `predicate`.
	 *
	 * @tparam T The type of the atomic object.
	 * @tparam Fn A templated function type that takes a value of type T and
	 * returns bool.
	 *
	 * @param object Pointer to the atomic object to wait on.
	 * @param predicate A callable object (such as a lambda or function object)
	 * that takes a value of type T and returns a boolean indicating whether the
	 * condition is met.
	 * @param order The memory order to use. Defaults to
	 * `std::memory_order_acquire`.
	 */
	template<typename T>
	void atomic_wait_if( std::atomic<T> const *object,
	                     Fn<bool( T )> auto &&predicate,
	                     std::memory_order order = std::memory_order_acquire ) {
		auto current =
		  std::atomic_load_explicit( object, std::memory_order_acquire );
		auto const &const_current = current;
		while( not predicate( const_current ) ) {
			std::atomic_wait_explicit( object, current, order );
			current = std::atomic_load_explicit( object, std::memory_order_relaxed );
		}
	}

	/**
	 * Waits for an atomic object to satisfy a predicate within a specified time
	 * duration.
	 *
	 * This function blocks the calling thread until the predicate applied
	 * to the atomic object evaluates to true or the specified timeout period
	 * elapses.
	 *
	 * @tparam T The type of the atomic object.
	 * @tparam Fn A callable type that returns a boolean value when applied to an
	 * object of type T.
	 * @tparam Rep A type representing the number of ticks of the time duration.
	 * @tparam Period A std::ratio type representing the tick period of the time
	 * duration.
	 *
	 * @param object Pointer to the atomic object to wait on.
	 * @param predicate Predicate function to apply to the atomic object's value.
	 * @param rel_time The relative time duration to wait for the predicate to
	 * become true.
	 * @param order The memory order to use for atomic operations. Defaults to
	 * `std::memory_order_acquire`.
	 *
	 * @return Returns wait_status::found if the predicate evaluates to true
	 * before the timeout, and wait_status::timeout otherwise.
	 */
	template<typename T, typename Rep, typename Period>
	[[nodiscard]] wait_status
	atomic_wait_if_for( std::atomic<T> const *object,
	                    Fn<bool( T )> auto &&predicate,
	                    std::chrono::duration<Rep, Period> const &rel_time,
	                    std::memory_order order = std::memory_order_acquire ) {
		auto const final_time =
		  std::chrono::high_resolution_clock::now( ) + rel_time;
		auto current_value =
		  std::atomic_load_explicit( object, std::memory_order_acquire );

		for( int tries = 0; not predicate( current_value ) and tries < 16;
		     ++tries ) {
			std::this_thread::yield( );
			current_value = std::atomic_load_explicit( object, order );
		}
		auto const poll_fn = [&] {
			current_value = std::atomic_load_explicit( object, order );
			auto const pred_result = predicate( current_value );
			if( pred_result ) {
				return true;
			}
			auto const current_time = std::chrono::high_resolution_clock::now( );
			auto const is_timed_out = current_time >= final_time;
			return is_timed_out;
		};
		(void)atomic_impl::poll_with_backoff( atomic_impl::timed_backoff_policy,
		                                      poll_fn, rel_time );
		if( predicate( current_value ) ) {
			return wait_status::found;
		}
		return wait_status::timeout;
	}

	/**
	 * Waits until the atomic object satisfies the given predicate or the absolute
	 * timeout time is reached.
	 *
	 * This function blocks the calling thread until the atomic value pointed to
	 * by `object` satisfies the `predicate` function or the specified timeout
	 * time `timeout_time` is reached.
	 *
	 * @tparam T The type of the atomic object.
	 * @tparam Clock The clock used for the timeout.
	 * @tparam Duration The duration type used by the clock.
	 * @tparam Fn A callable type that takes a value of type T and returns a bool.
	 *
	 * @param object Pointer to the atomic object to wait on.
	 * @param predicate A callable that takes the atomic object's value and
	 * returns whether it satisfies the condition.
	 * @param timeout_time The absolute time point by which the wait operation
	 * should be aborted if the predicate is not satisfied.
	 * @param order The memory order to use. Defaults to
	 * std::memory_order_acquire.
	 *
	 * @return A `wait_status` enum indicating whether the predicate was satisfied
	 * or a timeout occurred.
	 */
	template<typename T, typename Clock, typename Duration>
	[[nodiscard]] wait_status atomic_wait_if_until(
	  std::atomic<T> const *object, Fn<bool( T )> auto &&predicate,
	  std::chrono::time_point<Clock, Duration> const &timeout_time,
	  std::memory_order order = std::memory_order_acquire ) {
		return daw::atomic_wait_if_for( object, std::move( predicate ),
		                                timeout_time - Clock::now( ), order );
	}

	template<typename T>
	/**
	 * Waits until the atomic object reaches the desired value.
	 *
	 * This function blocks the calling thread until the atomic value
	 * pointed to by `object` equals `desired_value`.
	 *
	 * @tparam T The type of the atomic object.
	 *
	 * @param object Pointer to the atomic object to wait on.
	 * @param desired_value The value that the atomic object is expected to reach.
	 * @param order The memory order to use. Defaults to
	 * `std::memory_order_acquire`.
	 */
	void
	atomic_wait_value( std::atomic<T> const *object, T const &desired_value,
	                   std::memory_order order = std::memory_order_acquire ) {
		atomic_wait_if(
		  object,
		  [&desired_value]( T const &current_value ) {
			  return current_value == desired_value;
		  },
		  order );
	}

	/**
	 * Waits until the atomic object reaches the desired value or the relative
	 * time duration has elapsed.
	 *
	 * This function blocks the calling thread until the atomic value pointed to
	 * by `object` equals `desired_value` or the specified relative time duration
	 * `rel_time` has elapsed.
	 *
	 * @tparam T The type of the atomic object.
	 * @tparam Rep The type representing the number of ticks.
	 * @tparam Period The type representing the tick period.
	 *
	 * @param object Pointer to the atomic object to wait on.
	 * @param desired_value The value that the atomic object is expected to reach.
	 * @param rel_time The maximum duration to wait for the atomic object to reach
	 * the desired value.
	 * @param order The memory order to use. Defaults to
	 * `std::memory_order_acquire`.
	 *
	 * @return A `wait_status` enum indicating whether the desired value was found
	 * or a timeout occurred.
	 */
	template<typename T, typename Rep, typename Period>
	[[nodiscard]] wait_status
	atomic_wait_value_for( std::atomic<T> const *object, T const &desired_value,
	                       std::chrono::duration<Rep, Period> const &rel_time,
	                       std::memory_order order = std::memory_order_acquire ) {
		return atomic_wait_if_for(
		  object,
		  [&desired_value]( T const &current_value ) {
			  return current_value == desired_value;
		  },
		  rel_time, order );
	}

	/**
	 * Waits until the specified object reaches the desired value or the timeout
	 * is reached.
	 *
	 * This function blocks the calling thread until the atomic value pointed to
	 * by `object` equals `desired_value` or the specified timeout time
	 * `timeout_time` is reached.
	 *
	 * @tparam T The type of the atomic object.
	 * @tparam Clock The clock used for the timeout.
	 * @tparam Duration The duration type used by the clock.
	 *
	 * @param object Pointer to the atomic object to wait on.
	 * @param desired_value The value that the atomic object is expected to reach.
	 * @param timeout_time The absolute time point by which the wait operation
	 * should be aborted if the desired value is not reached.
	 * @param order The memory order to use. Defaults to
	 * `std::memory_order_acquire`.
	 *
	 * @return A `wait_status` enum indicating whether the desired value was found
	 * or a timeout occurred.
	 */
	template<typename T, typename Clock, typename Duration>
	[[nodiscard]] wait_status atomic_wait_value_until(
	  std::atomic<T> const *object, T const &desired_value,
	  std::chrono::time_point<Clock, Duration> const &timeout_time,
	  std::memory_order order = std::memory_order_acquire ) {
		return daw::atomic_wait_if_for(
		  object,
		  [&desired_value]( T const &current_value ) {
			  return current_value == desired_value;
		  },
		  timeout_time - Clock::now( ), order );
	}

} // namespace daw
