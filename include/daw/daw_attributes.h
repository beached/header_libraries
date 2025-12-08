// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "daw_cpp_feature_check.h"

#ifdef __has_cpp_attribute
#define DAW_HAS_ATTRIBUTE( Attrib ) __has_cpp_attribute( Attrib )
#else
#define DAW_HAS_ATTRIBUTE( Attrib ) false
#endif

#if DAW_HAS_ATTRIBUTE( clang::lifetimebound )
#define DAW_ATTRIB_LIFETIME_BOUND [[clang::lifetimebound]]
#elif DAW_HAS_ATTRIBUTE( msvc::lifetimebound )
#define DAW_ATTRIB_LIFETIME_BOUND [[msvc::lifetimebound]]
#elif DAW_HAS_ATTRIBUTE( lifetimebound )
#define DAW_ATTRIB_LIFETIME_BOUND __attribute( ( lifetimebound ) )
#else
#define DAW_ATTRIB_LIFETIME_BOUND
#endif

#if defined( DAW_HAS_GCC_LIKE )

#if not defined( DAW_NO_FLATTEN )

#define DAW_ATTRIB_FLATTEN [[gnu::flatten]]
#define DAW_ATTRIB_FLATINLINE [[gnu::flatten, gnu::always_inline]]

#else

#define DAW_ATTRIB_FLATTEN
#define DAW_ATTRIB_FLATINLINE

#endif

#ifndef DAW_FORCE_ALL_HIDDEN
#define DAW_ATTRIB_PUBLIC __attribute__( ( visibility( "default" ) ) )
#else
#define DAW_ATTRIB_PUBLIC __attribute__( ( visibility( "hidden" ) ) )
#endif

#define DAW_ATTRIB_HIDDEN __attribute__( ( visibility( "hidden" ) ) )

#if not defined( DAW_NO_FORCED_INLINE )
#define DAW_ATTRIB_INLINE [[gnu::always_inline]] inline
#else
#define DAW_ATTRIB_INLINE inline
#endif
#define DAW_ATTRIB_NOINLINE [[gnu::noinline]]

#elif defined( DAW_HAS_MSVC )

#if not defined( DAW_NO_FLATTEN )

#ifdef DAW_ATTRIB_USE_NEW_MSVC
#define DAW_ATTRIB_FLATTEN [[msvc::flatten]]
#define DAW_ATTRIB_FLATINLINE [[msvc::flatten, msvc::inline]]
#else
#define DAW_ATTRIB_FLATTEN
#define DAW_ATTRIB_FLATINLINE
#endif

#else

#define DAW_ATTRIB_FLATTEN
#define DAW_ATTRIB_FLATINLINE

#endif

#ifndef DAW_FORCE_ALL_HIDDEN
#define DAW_ATTRIB_PUBLIC __declspec( dllexport )
#else
#define DAW_ATTRIB_PUBLIC
#endif
#define DAW_ATTRIB_HIDDEN

#if not defined( DAW_NO_FORCED_INLINE )
#ifdef DAW_ATTRIB_USE_NEW_MSVC
#define DAW_ATTRIB_INLINE [[msvc::forceinline]] inline
#else
#define DAW_ATTRIB_INLINE __forceinline
#endif
#else
#define DAW_ATTRIB_INLINE inline
#endif
#define DAW_ATTRIB_NOINLINE __declspec( noinline )

#else

#define DAW_ATTRIB_FLATTEN
#define DAW_ATTRIB_FLATINLINE
#define DAW_ATTRIB_HIDDEN
#define DAW_ATTRIB_PUBLIC
#define DAW_ATTRIB_INLINE inline
#define DAW_ATTRIB_NOINLINE

#endif

#if defined( DAW_HAS_MSVC_LIKE )
#define DAW_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#else
#define DAW_NO_UNIQUE_ADDRESS [[no_unique_address]]
#endif

#if DAW_HAS_CLANG_VER_GTE( 13, 0 )
#define DAW_PREF_NAME( ... ) clang::preferred_name( __VA_ARGS__ )
#else
#define DAW_PREF_NAME( ... )
#endif

#if defined( DAW_HAS_GCC_LIKE )
#define DAW_ATTRIB_RETNOTNULL [[gnu::returns_nonnull]]
#else
#define DAW_ATTRIB_RETNOTNULL
#endif

#if defined( DAW_HAS_GCC_LIKE )
#define DAW_ATTRIB_NONNULL( ... ) [[gnu::nonnull __VA_ARGS__]]
#define DAW_ATTRIB_RET_NONNULL [[gnu::returns_nonnull]]
#else
#define DAW_ATTRIB_NONNULL( ... )
#define DAW_ATTRIB_RET_NONNULL
#endif

#if not defined( __has_cpp_attribute )
#define DAW_LIFETIME_BOUND
#elif __has_cpp_attribute( clang::lifetimebound )
#define DAW_LIFETIME_BOUND [[clang::lifetimebound]]
#elif __has_cpp_attribute( msvc::lifetimebound )
#define DAW_LIFETIME_BOUND [[msvc::lifetimebound]]
#elif __has_cpp_attribute( lifetimebound )
#define DAW_LIFETIME_BOUND [[lifetimebound]]
#else
#define DAW_LIFETIME_BOUND
#endif

#if defined( DAW_HAS_CLANG ) and __has_attribute( enable_if )
#define DAW_ATTRIB_ENABLE_IF(...) __attribute__((enable_if(__VA_ARGS__)))
#endif