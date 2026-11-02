// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/ciso646.h"
#include "daw/daw_attributes.h"
#include "daw/daw_cpp_feature_check.h"
#include "daw/daw_static_warn_if.h"

#include <iostream>
#include <string_view>

#if defined( DAW_ATTRIB_ENABLE_IF )
#if defined( DAW_HAS_CPP26_DELETED_REASON )

#if defined( __clang__ )
_Pragma( "clang diagnostic push" );
_Pragma( "clang diagnostic ignored \"-Wc++26-extensions\"" );
#endif
template<typename StringView>
[[noreturn]] DAW_ATTRIB_NOINLINE inline void
daw_ensure_error( bool b, StringView &&msg )
  DAW_ATTRIB_ENABLE_IF( __builtin_constant_p( b ) and not b,
                        "ensure check failed" ) =
    delete( "ensure check failed" );
#if defined( __clang__ )
_Pragma( "clang diagnostic pop" );
#endif
#endif
template<typename StringView>
[[noreturn]] DAW_ATTRIB_NOINLINE inline void
daw_ensure_error( bool b, StringView &&msg )
  DAW_ATTRIB_ENABLE_IF( __builtin_constant_p( b ) and b,
                        "ensure check failed" ) {
	std::cerr << msg << std::endl << std::flush;
	std::terminate( );
}

template<typename StringView>
[[noreturn]] DAW_ATTRIB_NOINLINE inline void
daw_ensure_error( bool b, StringView &&msg )
  DAW_ATTRIB_ENABLE_IF( not __builtin_constant_p( b ), " " ) {
	std::cerr << msg << std::endl << std::flush;
	std::terminate( );
}
#else
template<typename StringView>
[[noreturn]] DAW_ATTRIB_NOINLINE inline void
daw_ensure_error( bool, StringView &&msg ) {
	std::cerr << msg << std::endl << std::flush;
	std::terminate( );
}
#endif
