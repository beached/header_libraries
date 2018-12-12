// The MIT License (MIT)
//
// Copyright (c) 2013-2018 Darrell Wright
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

#include <algorithm>
#include <functional>
namespace daw {
	namespace algorithm_impl {
	// Sorting networks are from http://pages.ripco.net/~jgamble/nw.html

		template<intmax_t Pos0, intmax_t Pos1, typename Iterator,
		         typename Compare = std::less<>>
		constexpr void swap_if( Iterator first,
		                        Compare comp = Compare{} ) noexcept {
			auto const f = std::next( first, Pos0 );
			auto const l = std::next( first, Pos1 );
			if( !comp( *f, *l ) ) {
				std::iter_swap( f, l );
			}
		}
	} // namespace algorithm_impl

	template<typename Iterator, typename Compare = std::less<>>
	constexpr void sort_32( Iterator first, Compare comp = Compare{} ) noexcept {
		algorithm_impl::swap_if<0, 1>( first );
		algorithm_impl::swap_if<2, 3>( first );
		algorithm_impl::swap_if<0, 2>( first );
		algorithm_impl::swap_if<1, 3>( first );
		algorithm_impl::swap_if<1, 2>( first );
		algorithm_impl::swap_if<4, 5>( first );
		algorithm_impl::swap_if<6, 7>( first );
		algorithm_impl::swap_if<4, 6>( first );
		algorithm_impl::swap_if<5, 7>( first );
		algorithm_impl::swap_if<5, 6>( first );
		algorithm_impl::swap_if<0, 4>( first );
		algorithm_impl::swap_if<1, 5>( first );
		algorithm_impl::swap_if<1, 4>( first );
		algorithm_impl::swap_if<2, 6>( first );
		algorithm_impl::swap_if<3, 7>( first );
		algorithm_impl::swap_if<3, 6>( first );
		algorithm_impl::swap_if<2, 4>( first );
		algorithm_impl::swap_if<3, 5>( first );
		algorithm_impl::swap_if<3, 4>( first );
		algorithm_impl::swap_if<8, 9>( first );
		algorithm_impl::swap_if<10, 11>( first );
		algorithm_impl::swap_if<8, 10>( first );
		algorithm_impl::swap_if<9, 11>( first );
		algorithm_impl::swap_if<9, 10>( first );
		algorithm_impl::swap_if<12, 13>( first );
		algorithm_impl::swap_if<14, 15>( first );
		algorithm_impl::swap_if<12, 14>( first );
		algorithm_impl::swap_if<13, 15>( first );
		algorithm_impl::swap_if<13, 14>( first );
		algorithm_impl::swap_if<8, 12>( first );
		algorithm_impl::swap_if<9, 13>( first );
		algorithm_impl::swap_if<9, 12>( first );
		algorithm_impl::swap_if<10, 14>( first );
		algorithm_impl::swap_if<11, 15>( first );
		algorithm_impl::swap_if<11, 14>( first );
		algorithm_impl::swap_if<10, 12>( first );
		algorithm_impl::swap_if<11, 13>( first );
		algorithm_impl::swap_if<11, 12>( first );
		algorithm_impl::swap_if<0, 8>( first );
		algorithm_impl::swap_if<1, 9>( first );
		algorithm_impl::swap_if<1, 8>( first );
		algorithm_impl::swap_if<2, 10>( first );
		algorithm_impl::swap_if<3, 11>( first );
		algorithm_impl::swap_if<3, 10>( first );
		algorithm_impl::swap_if<2, 8>( first );
		algorithm_impl::swap_if<3, 9>( first );
		algorithm_impl::swap_if<3, 8>( first );
		algorithm_impl::swap_if<4, 12>( first );
		algorithm_impl::swap_if<5, 13>( first );
		algorithm_impl::swap_if<5, 12>( first );
		algorithm_impl::swap_if<6, 14>( first );
		algorithm_impl::swap_if<7, 15>( first );
		algorithm_impl::swap_if<7, 14>( first );
		algorithm_impl::swap_if<6, 12>( first );
		algorithm_impl::swap_if<7, 13>( first );
		algorithm_impl::swap_if<7, 12>( first );
		algorithm_impl::swap_if<4, 8>( first );
		algorithm_impl::swap_if<5, 9>( first );
		algorithm_impl::swap_if<5, 8>( first );
		algorithm_impl::swap_if<6, 10>( first );
		algorithm_impl::swap_if<7, 11>( first );
		algorithm_impl::swap_if<7, 10>( first );
		algorithm_impl::swap_if<6, 8>( first );
		algorithm_impl::swap_if<7, 9>( first );
		algorithm_impl::swap_if<7, 8>( first );
		algorithm_impl::swap_if<16, 17>( first );
		algorithm_impl::swap_if<18, 19>( first );
		algorithm_impl::swap_if<16, 18>( first );
		algorithm_impl::swap_if<17, 19>( first );
		algorithm_impl::swap_if<17, 18>( first );
		algorithm_impl::swap_if<20, 21>( first );
		algorithm_impl::swap_if<22, 23>( first );
		algorithm_impl::swap_if<20, 22>( first );
		algorithm_impl::swap_if<21, 23>( first );
		algorithm_impl::swap_if<21, 22>( first );
		algorithm_impl::swap_if<16, 20>( first );
		algorithm_impl::swap_if<17, 21>( first );
		algorithm_impl::swap_if<17, 20>( first );
		algorithm_impl::swap_if<18, 22>( first );
		algorithm_impl::swap_if<19, 23>( first );
		algorithm_impl::swap_if<19, 22>( first );
		algorithm_impl::swap_if<18, 20>( first );
		algorithm_impl::swap_if<19, 21>( first );
		algorithm_impl::swap_if<19, 20>( first );
		algorithm_impl::swap_if<24, 25>( first );
		algorithm_impl::swap_if<26, 27>( first );
		algorithm_impl::swap_if<24, 26>( first );
		algorithm_impl::swap_if<25, 27>( first );
		algorithm_impl::swap_if<25, 26>( first );
		algorithm_impl::swap_if<28, 29>( first );
		algorithm_impl::swap_if<30, 31>( first );
		algorithm_impl::swap_if<28, 30>( first );
		algorithm_impl::swap_if<29, 31>( first );
		algorithm_impl::swap_if<29, 30>( first );
		algorithm_impl::swap_if<24, 28>( first );
		algorithm_impl::swap_if<25, 29>( first );
		algorithm_impl::swap_if<25, 28>( first );
		algorithm_impl::swap_if<26, 30>( first );
		algorithm_impl::swap_if<27, 31>( first );
		algorithm_impl::swap_if<27, 30>( first );
		algorithm_impl::swap_if<26, 28>( first );
		algorithm_impl::swap_if<27, 29>( first );
		algorithm_impl::swap_if<27, 28>( first );
		algorithm_impl::swap_if<16, 24>( first );
		algorithm_impl::swap_if<17, 25>( first );
		algorithm_impl::swap_if<17, 24>( first );
		algorithm_impl::swap_if<18, 26>( first );
		algorithm_impl::swap_if<19, 27>( first );
		algorithm_impl::swap_if<19, 26>( first );
		algorithm_impl::swap_if<18, 24>( first );
		algorithm_impl::swap_if<19, 25>( first );
		algorithm_impl::swap_if<19, 24>( first );
		algorithm_impl::swap_if<20, 28>( first );
		algorithm_impl::swap_if<21, 29>( first );
		algorithm_impl::swap_if<21, 28>( first );
		algorithm_impl::swap_if<22, 30>( first );
		algorithm_impl::swap_if<23, 31>( first );
		algorithm_impl::swap_if<23, 30>( first );
		algorithm_impl::swap_if<22, 28>( first );
		algorithm_impl::swap_if<23, 29>( first );
		algorithm_impl::swap_if<23, 28>( first );
		algorithm_impl::swap_if<20, 24>( first );
		algorithm_impl::swap_if<21, 25>( first );
		algorithm_impl::swap_if<21, 24>( first );
		algorithm_impl::swap_if<22, 26>( first );
		algorithm_impl::swap_if<23, 27>( first );
		algorithm_impl::swap_if<23, 26>( first );
		algorithm_impl::swap_if<22, 24>( first );
		algorithm_impl::swap_if<23, 25>( first );
		algorithm_impl::swap_if<23, 24>( first );
		algorithm_impl::swap_if<0, 16>( first );
		algorithm_impl::swap_if<1, 17>( first );
		algorithm_impl::swap_if<1, 16>( first );
		algorithm_impl::swap_if<2, 18>( first );
		algorithm_impl::swap_if<3, 19>( first );
		algorithm_impl::swap_if<3, 18>( first );
		algorithm_impl::swap_if<2, 16>( first );
		algorithm_impl::swap_if<3, 17>( first );
		algorithm_impl::swap_if<3, 16>( first );
		algorithm_impl::swap_if<4, 20>( first );
		algorithm_impl::swap_if<5, 21>( first );
		algorithm_impl::swap_if<5, 20>( first );
		algorithm_impl::swap_if<6, 22>( first );
		algorithm_impl::swap_if<7, 23>( first );
		algorithm_impl::swap_if<7, 22>( first );
		algorithm_impl::swap_if<6, 20>( first );
		algorithm_impl::swap_if<7, 21>( first );
		algorithm_impl::swap_if<7, 20>( first );
		algorithm_impl::swap_if<4, 16>( first );
		algorithm_impl::swap_if<5, 17>( first );
		algorithm_impl::swap_if<5, 16>( first );
		algorithm_impl::swap_if<6, 18>( first );
		algorithm_impl::swap_if<7, 19>( first );
		algorithm_impl::swap_if<7, 18>( first );
		algorithm_impl::swap_if<6, 16>( first );
		algorithm_impl::swap_if<7, 17>( first );
		algorithm_impl::swap_if<7, 16>( first );
		algorithm_impl::swap_if<8, 24>( first );
		algorithm_impl::swap_if<9, 25>( first );
		algorithm_impl::swap_if<9, 24>( first );
		algorithm_impl::swap_if<10, 26>( first );
		algorithm_impl::swap_if<11, 27>( first );
		algorithm_impl::swap_if<11, 26>( first );
		algorithm_impl::swap_if<10, 24>( first );
		algorithm_impl::swap_if<11, 25>( first );
		algorithm_impl::swap_if<11, 24>( first );
		algorithm_impl::swap_if<12, 28>( first );
		algorithm_impl::swap_if<13, 29>( first );
		algorithm_impl::swap_if<13, 28>( first );
		algorithm_impl::swap_if<14, 30>( first );
		algorithm_impl::swap_if<15, 31>( first );
		algorithm_impl::swap_if<15, 30>( first );
		algorithm_impl::swap_if<14, 28>( first );
		algorithm_impl::swap_if<15, 29>( first );
		algorithm_impl::swap_if<15, 28>( first );
		algorithm_impl::swap_if<12, 24>( first );
		algorithm_impl::swap_if<13, 25>( first );
		algorithm_impl::swap_if<13, 24>( first );
		algorithm_impl::swap_if<14, 26>( first );
		algorithm_impl::swap_if<15, 27>( first );
		algorithm_impl::swap_if<15, 26>( first );
		algorithm_impl::swap_if<14, 24>( first );
		algorithm_impl::swap_if<15, 25>( first );
		algorithm_impl::swap_if<15, 24>( first );
		algorithm_impl::swap_if<8, 16>( first );
		algorithm_impl::swap_if<9, 17>( first );
		algorithm_impl::swap_if<9, 16>( first );
		algorithm_impl::swap_if<10, 18>( first );
		algorithm_impl::swap_if<11, 19>( first );
		algorithm_impl::swap_if<11, 18>( first );
		algorithm_impl::swap_if<10, 16>( first );
		algorithm_impl::swap_if<11, 17>( first );
		algorithm_impl::swap_if<11, 16>( first );
		algorithm_impl::swap_if<12, 20>( first );
		algorithm_impl::swap_if<13, 21>( first );
		algorithm_impl::swap_if<13, 20>( first );
		algorithm_impl::swap_if<14, 22>( first );
		algorithm_impl::swap_if<15, 23>( first );
		algorithm_impl::swap_if<15, 22>( first );
		algorithm_impl::swap_if<14, 20>( first );
		algorithm_impl::swap_if<15, 21>( first );
		algorithm_impl::swap_if<15, 20>( first );
		algorithm_impl::swap_if<12, 16>( first );
		algorithm_impl::swap_if<13, 17>( first );
		algorithm_impl::swap_if<13, 16>( first );
		algorithm_impl::swap_if<14, 18>( first );
		algorithm_impl::swap_if<15, 19>( first );
		algorithm_impl::swap_if<15, 18>( first );
		algorithm_impl::swap_if<14, 16>( first );
		algorithm_impl::swap_if<15, 17>( first );
		algorithm_impl::swap_if<15, 16>( first );
	}

	template<typename Iterator, typename Compare = std::less<>>
	constexpr void sort_16( Iterator first, Compare comp = Compare{} ) noexcept {
		algorithm_impl::swap_if<0, 1>( first );
		algorithm_impl::swap_if<2, 3>( first );
		algorithm_impl::swap_if<4, 5>( first );
		algorithm_impl::swap_if<6, 7>( first );
		algorithm_impl::swap_if<8, 9>( first );
		algorithm_impl::swap_if<10, 11>( first );
		algorithm_impl::swap_if<12, 13>( first );
		algorithm_impl::swap_if<14, 15>( first );
		algorithm_impl::swap_if<0, 2>( first );
		algorithm_impl::swap_if<4, 6>( first );
		algorithm_impl::swap_if<8, 10>( first );
		algorithm_impl::swap_if<12, 14>( first );
		algorithm_impl::swap_if<1, 3>( first );
		algorithm_impl::swap_if<5, 7>( first );
		algorithm_impl::swap_if<9, 11>( first );
		algorithm_impl::swap_if<13, 15>( first );
		algorithm_impl::swap_if<0, 4>( first );
		algorithm_impl::swap_if<8, 12>( first );
		algorithm_impl::swap_if<1, 5>( first );
		algorithm_impl::swap_if<9, 13>( first );
		algorithm_impl::swap_if<2, 6>( first );
		algorithm_impl::swap_if<10, 14>( first );
		algorithm_impl::swap_if<3, 7>( first );
		algorithm_impl::swap_if<11, 15>( first );
		algorithm_impl::swap_if<0, 8>( first );
		algorithm_impl::swap_if<1, 9>( first );
		algorithm_impl::swap_if<2, 10>( first );
		algorithm_impl::swap_if<3, 11>( first );
		algorithm_impl::swap_if<4, 12>( first );
		algorithm_impl::swap_if<5, 13>( first );
		algorithm_impl::swap_if<6, 14>( first );
		algorithm_impl::swap_if<7, 15>( first );
		algorithm_impl::swap_if<5, 10>( first );
		algorithm_impl::swap_if<6, 9>( first );
		algorithm_impl::swap_if<3, 12>( first );
		algorithm_impl::swap_if<13, 14>( first );
		algorithm_impl::swap_if<7, 11>( first );
		algorithm_impl::swap_if<1, 2>( first );
		algorithm_impl::swap_if<4, 8>( first );
		algorithm_impl::swap_if<1, 4>( first );
		algorithm_impl::swap_if<7, 13>( first );
		algorithm_impl::swap_if<2, 8>( first );
		algorithm_impl::swap_if<11, 14>( first );
		algorithm_impl::swap_if<2, 4>( first );
		algorithm_impl::swap_if<5, 6>( first );
		algorithm_impl::swap_if<9, 10>( first );
		algorithm_impl::swap_if<11, 13>( first );
		algorithm_impl::swap_if<3, 8>( first );
		algorithm_impl::swap_if<7, 12>( first );
		algorithm_impl::swap_if<6, 8>( first );
		algorithm_impl::swap_if<10, 12>( first );
		algorithm_impl::swap_if<3, 5>( first );
		algorithm_impl::swap_if<7, 9>( first );
		algorithm_impl::swap_if<3, 4>( first );
		algorithm_impl::swap_if<5, 6>( first );
		algorithm_impl::swap_if<7, 8>( first );
		algorithm_impl::swap_if<9, 10>( first );
		algorithm_impl::swap_if<11, 12>( first );
		algorithm_impl::swap_if<6, 7>( first );
		algorithm_impl::swap_if<8, 9>( first );
	}
} // namespace daw
