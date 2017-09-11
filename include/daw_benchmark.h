// The MIT License (MIT)
//
// Copyright (c) 2014-2017 Darrell Wright
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

#pragma once

#include <chrono>
#include <iomanip>
#include <sstream>

namespace daw {
	template<typename F>
	double benchmark( F func ) {
		auto start = std::chrono::high_resolution_clock::now( );
		func( );
		auto finish = std::chrono::high_resolution_clock::now( );
		std::chrono::duration<double> duration = finish - start;
		return duration.count( );
	}
	namespace utility {
		template<typename T>
		std::string format_seconds( T t ) {
			auto val = t * 1000000000000000.0;
			if( val < 1000 ) {
				return std::to_string( static_cast<uint64_t>( val ) ) + "fs";
			}
			val /= 1000.0;
			if( val < 1000 ) {
				return std::to_string( static_cast<uint64_t>( val ) ) + "ps";
			}
			val /= 1000.0;
			if( val < 1000 ) {
				return std::to_string( static_cast<uint64_t>( val ) ) + "ns";
			}
			val /= 1000.0;
			if( val < 1000 ) {
				return std::to_string( static_cast<uint64_t>( val ) ) + "µs";
			}
			val /= 1000.0;
			if( val < 1000 ) {
				return std::to_string( static_cast<uint64_t>( val ) ) + "ms";
			}
			val /= 1000.0;
			return std::to_string( static_cast<uint64_t>( val ) ) + "s";
		}

		template<typename Bytes, typename Time>
		std::string to_bytes_per_second( Bytes bytes, Time t = 1.0, size_t prec = 1 ) {
			std::stringstream ss;
			ss << std::setprecision( prec ) << std::fixed;
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
	}	// utility 
} // namespace daw
