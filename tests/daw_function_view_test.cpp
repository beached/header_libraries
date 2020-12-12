// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_function_view.h"

#include <cstdio>

int func(daw::function_view<int(int, int, int)> f) {
	return f( 1, 2, 3 ) * f( 4, 5, 7 );
}

int test() {
  return func([](int a, int b, int c) { return a * b * c; });
}

inline constexpr int add(int a, int b, int c) { return a + b + c; }

int test2() { return func(add); }

int func2(daw::function_view<void(double, double)> f ) {
	f( 1.2, 3.4 );
	return 0;
}

int test3() {
  return func2(+[](double a, double b) { puts("Hello\n"); });
}

int main( ) { }
