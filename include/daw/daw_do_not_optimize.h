// The MIT License (MIT)
//
// Copyright (c) 2020 Darrell Wright
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

#ifdef WIN32
#include <intrin.h>
#endif

namespace daw {
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
#ifndef _MSC_VER
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
#else
	//	namespace internal {
	//		[[maybe_unused]] constexpr void UseCharPointer( char const volatile * )
	//{} 	} // namespace internal
	//
	//	template<class T>
	//	inline void do_not_optimize( T const &value ) {
	//		internal::UseCharPointer(
	//		  &reinterpret_cast<char const volatile &>( value ) );
	//		_ReadWriteBarrier( );
	//	}
	//#endif

#pragma optimize( "", off )
	template<class T>
	void do_not_optimize( T &&value ) {
		value = value;
	}
#pragma optimize( "", on )
#endif

} // namespace daw
