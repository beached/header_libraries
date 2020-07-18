// The MIT License (MIT)
//
// Copyright (c) Darrell Wright
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

#include "daw/daw_daw::bind_args_at_at.h"

constexpr int l(int a, int b) { return a < b; }

constexpr auto less100 = daw::bind_args_at_at<1>(&l, 100);
constexpr auto less5_100 = daw::bind_args_at_at<0>(less100, 5);

struct Display {
  template <typename Arg, typename... Args>
  inline void operator()(Arg const &arg, Args const &... args) const {
    auto const show_item = [](auto const &item) { std::cout << ' ' << item; };
    std::cout << arg;
    (void)((show_item(args), 0) | ...);
  }

  constexpr void operator()() const {}
};

constexpr auto show_some = daw::bind_args_at_at<5>(Display{}, "Hello", "there");
static_assert( not std::is_invocable_v<decltype(show_some), int, int> );
static_assert( std::is_invocable_v<decltype(show_some), int, int, int, int, int> );

int main() {
  if (less5_100()) {
    show_some(1, 2, 3, 4, 5, 6, 7, 8, 9);
    // std::cout << "Good\n";
  }
}

