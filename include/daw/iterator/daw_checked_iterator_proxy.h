// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/ciso646.h"
#include "daw/daw_check_exceptions.h"
#include "daw/daw_exception.h"
#include "daw/daw_logic.h"

#include <cstdint>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace daw {
	template<typename IteratorFirst, typename IteratorLast>
	class checked_iterator_proxy_t {
		enum flag_t : uint8_t {
			check_increment = 1,
			check_decrement = 2,
			check_dereference = 4
		};
		IteratorFirst current;
		IteratorFirst first;
		IteratorLast last;
		uint8_t flags;

		constexpr bool is_flag_set( flag_t flag ) const noexcept {
			return 0 != ( flags & flag );
		}

		constexpr auto get_flag_value( bool CheckIncrement,
		                               bool CheckDecrement,
		                               bool CheckDereference ) const noexcept {
			auto result =
			  static_cast<std::uint8_t>( CheckIncrement ? check_increment : 0 );
			result = static_cast<uint8_t>(
			  daw::nsc_or( result, CheckDecrement ) ? check_decrement : 0 );
			result = static_cast<uint8_t>(
			  daw::nsc_or( result, CheckDereference ) ? check_dereference : 0 );

			return result;
		}

	public:
		checked_iterator_proxy_t( IteratorFirst it_first,
		                          IteratorLast it_last,
		                          bool CheckIncrement = true,
		                          bool CheckDecrement = true,
		                          bool CheckDereference = true )
		  : current{ it_first }
		  , first{ std::move( it_first ) }
		  , last{ std::move( it_last ) }
		  , flags{ get_flag_value(
		      CheckIncrement, CheckDecrement, CheckDereference ) } {}

		checked_iterator_proxy_t( checked_iterator_proxy_t const & ) = default;
		checked_iterator_proxy_t( checked_iterator_proxy_t && ) = default;
		checked_iterator_proxy_t &
		operator=( checked_iterator_proxy_t const & ) = default;
		checked_iterator_proxy_t &
		operator=( checked_iterator_proxy_t && ) = default;
		checked_iterator_proxy_t( ) = default;
		~checked_iterator_proxy_t( ) = default;

		checked_iterator_proxy_t &operator++( ) {
			if( is_flag_set( check_increment ) and current == last ) {
				daw::exception::daw_throw<std::out_of_range>(
				  "Attempt to increment iterator past end of range" );
			}
			++current;
			return *this;
		}

		checked_iterator_proxy_t operator++( int ) const {
			checked_iterator_proxy_t result{ *this };
			++result;
			return result;
		}

		checked_iterator_proxy_t &operator--( ) {
			if( is_flag_set( check_decrement ) and current == first ) {
				DAW_THROW_OR_TERMINATE(
				  std::out_of_range,
				  "Attempt to decrement iterator past beginning of range" );
			}
			--current;
			return *this;
		}

		checked_iterator_proxy_t operator--( int ) const {
			checked_iterator_proxy_t result{ *this };
			--result;
			return result;
		}

		auto &operator*( ) {
			if( is_flag_set( check_dereference ) and current == last ) {
				DAW_THROW_OR_TERMINATE(
				  std::out_of_range,
				  "Attempt to dereference an iterator at end of range" );
			}
			return *current;
		}

		auto const &operator*( ) const {
			if( is_flag_set( check_dereference ) and current == last ) {
				DAW_THROW_OR_TERMINATE(
				  std::out_of_range,
				  "Attempt to dereference an iterator at end of range" );
			}
			return *current;
		}

		auto &operator->( ) {
			if( is_flag_set( check_dereference ) and current == last ) {
				DAW_THROW_OR_TERMINATE(
				  std::out_of_range,
				  "Attempt to access members an iterator at end of range" );
			}
			return current.operator->( );
		}

		auto const &operator->( ) const {
			if( is_flag_set( check_dereference ) and current == last ) {
				DAW_THROW_OR_TERMINATE(
				  std::out_of_range,
				  "Attempt to access members an iterator at end of range" );
			}
			return current.operator->( );
		}

		template<typename IteratorFirst_lhs,
		         typename IteratorFirst_rhs,
		         typename IteratorLast_lhs,
		         typename IteratorLast_rhs>
		friend bool operator==(
		  checked_iterator_proxy_t<IteratorFirst_lhs, IteratorLast_lhs> const &lhs,
		  checked_iterator_proxy_t<IteratorFirst_rhs, IteratorLast_rhs> const
		    &rhs ) {
			return lhs.current == rhs.current;
		}

		template<typename IteratorFirst_lhs,
		         typename IteratorLast_lhs,
		         typename Iterator_rhs>
		friend bool operator==(
		  checked_iterator_proxy_t<IteratorFirst_lhs, IteratorLast_lhs> const &lhs,
		  Iterator_rhs const &rhs ) {
			return lhs.current == rhs;
		}

		template<typename IteratorFirst_lhs,
		         typename IteratorFirst_rhs,
		         typename IteratorLast_lhs,
		         typename IteratorLast_rhs>
		friend bool operator!=(
		  checked_iterator_proxy_t<IteratorFirst_lhs, IteratorLast_lhs> const &lhs,
		  checked_iterator_proxy_t<IteratorFirst_rhs, IteratorLast_rhs> const
		    &rhs ) {
			return lhs.current != rhs.current;
		}

		template<typename IteratorFirst_lhs,
		         typename IteratorLast_lhs,
		         typename Iterator_rhs>
		friend bool operator!=(
		  checked_iterator_proxy_t<IteratorFirst_lhs, IteratorLast_lhs> const &lhs,
		  Iterator_rhs const &rhs ) {
			return lhs.current != rhs;
		}
	}; // checked_iterator_proxy_t

	template<typename IteratorFirst, typename IteratorLast>
	auto make_checked_iterator_proxy( IteratorFirst first,
	                                  IteratorLast last,
	                                  bool CheckIncrement = true,
	                                  bool CheckDecrement = true,
	                                  bool CheckDereference = true ) {
		return checked_iterator_proxy_t<IteratorFirst, IteratorLast>{
		  first, last, CheckIncrement, CheckDecrement, CheckDereference };
	}
} // namespace daw
