// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_exception.h"
#include "daw_string.h"

#include <ciso646>
#include <exception>
#include <memory>

template<typename T, typename... Args>
[[nodiscard]] T *new_throw( Args &&...args ) {
	std::unique_ptr<T> result;
	try {
		result = std::make_unique<T>( std::forward<Args>( args )... );
	} catch( ... ) { std::rethrow_exception( std::current_exception( ) ); }
	daw::exception::daw_throw_on_false( result, "Error allocating" );
	return result.release( );
}

template<typename T>
[[nodiscard]] T *new_array_throw( const size_t size ) {
	T *result = nullptr;
	try {
		result = new T[size];
	} catch( ... ) { std::rethrow_exception( std::current_exception( ) ); }
	daw::exception::daw_throw_on_null(
	  result, daw::string::string_join( "Error allocating ", size, " items" ) );
	return result;
}
