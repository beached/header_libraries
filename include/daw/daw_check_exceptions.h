// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#pragma once

#include "daw_attributes.h"
#include "daw_move.h"

#include <ciso646>
#include <exception>

#if not( defined( __cpp_exceptions ) or defined( __EXCEPTIONS ) or             \
         defined( _CPPUNWIND ) ) or                                            \
  defined( DAW_DONT_USE_EXCEPTIONS )

#if not defined( DAW_DONT_USE_EXCEPTIONS )
#define DAW_DONT_USE_EXCEPTIONS
#endif

#if defined( DAW_USE_EXCEPTIONS )
#error Mismatch in parameters. Cannot have both states exceptions and no exceptions be true.  undefine DAW_USE_EXCEPTIONS, or enabled exceptions
#endif

#else

#ifndef DAW_USE_EXCEPTIONS
#define DAW_USE_EXCEPTIONS
#endif

#endif

namespace daw::check_except_detail {
	namespace {
		template<typename Exception>
		[[noreturn, maybe_unused]] DAW_ATTRIB_NOINLINE void
		throw_error( Exception &&except ) {
#if defined( DAW_USE_EXCEPTIONS )
			throw DAW_FWD( except );
#endif
		}

		template<typename>
		[[noreturn, maybe_unused]] DAW_ATTRIB_NOINLINE void terminate_error( ) {
			std::terminate( );
		}
	} // namespace
} // namespace daw::check_except_detail

#if defined( DAW_USE_EXCEPTIONS )
#define DAW_THROW_OR_TERMINATE( ExceptionType, ... )                           \
	::daw::check_except_detail::throw_error( ExceptionType{ __VA_ARGS__ } )
#define DAW_THROW_OR_TERMINATE_NA( ExceptionType )                             \
	::daw::check_except_detail::throw_error( ExceptionType{ } )
#else
#define DAW_THROW_OR_TERMINATE( ExceptionType, ... )                           \
	::daw::check_except_detail::terminate_error<ExceptionType>( )
#define DAW_THROW_OR_TERMINATE_NA( ExceptionType )                             \
	::daw::check_except_detail::terminate_error<ExceptionType>( )
#endif
