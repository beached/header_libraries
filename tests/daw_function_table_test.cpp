// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_function_table.h"

#include <cstdint>
#include <iostream>
#include <type_traits>

using parser_func_t = std::add_pointer_t<uintmax_t( uintmax_t, char const * )>;

constexpr uintmax_t ret( uintmax_t n, char const * ) {
	return n;
}

inline constexpr daw::function_table_t<uintmax_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t,
                                       parser_func_t>
  ftable{ ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          ret,
          +[]( uintmax_t n, char const *c ) {
	          n = n * 10U + 0;
	          ++c;
	          return ftable( static_cast<size_t>( *c ), n, c );
          },
          +[]( uintmax_t n, char const *c ) {
	          n = n * 10U + 1;
	          ++c;
	          return ftable( static_cast<size_t>( *c ), n, c );
          },
          +[]( uintmax_t n, char const *c ) {
	          n = n * 10U + 2;
	          ++c;
	          return ftable( static_cast<size_t>( *c ), n, c );
          },
          +[]( uintmax_t n, char const *c ) {
	          n = n * 10U + 3;
	          ++c;
	          return ftable( static_cast<size_t>( *c ), n, c );
          },
          +[]( uintmax_t n, char const *c ) {
	          n = n * 10U + 4;
	          ++c;
	          return ftable( static_cast<size_t>( *c ), n, c );
          },
          +[]( uintmax_t n, char const *c ) {
	          n = n * 10U + 5;
	          ++c;
	          return ftable( static_cast<size_t>( *c ), n, c );
          },
          +[]( uintmax_t n, char const *c ) {
	          n = n * 10U + 6;
	          ++c;
	          return ftable( static_cast<size_t>( *c ), n, c );
          },
          +[]( uintmax_t n, char const *c ) {
	          n = n * 10U + 7;
	          ++c;
	          return ftable( static_cast<size_t>( *c ), n, c );
          },
          +[]( uintmax_t n, char const *c ) {
	          n = n * 10U + 8;
	          ++c;
	          return ftable( static_cast<size_t>( *c ), n, c );
          },
          +[]( uintmax_t n, char const *c ) {
	          n = n * 10U + 9;
	          ++c;
	          return ftable( static_cast<size_t>( *c ), n, c );
          } };

int main( int argc, char **argv ) {
	char const *ptr = "12345678";
	if( argc > 1 ) {
		ptr = argv[1];
	}
	uintmax_t result = ftable( static_cast<size_t>( *ptr ), 0U, ptr );
	std::cout << result << '\n';
}
