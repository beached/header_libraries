// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
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

#include <optional>
#include <variant>

#include "daw/daw_benchmark.h"
#include "daw/daw_visit.h"

constexpr bool visit_nt_001( ) {
	std::variant<int, double> a = 5.5;
	auto result = ::daw::visit_nt(
	  a, []( int v ) { return v == 5.5; }, []( double d ) { return d == 5.5; } );
	daw::expecting( result );
	return true;
}
static_assert( visit_nt_001( ) );

constexpr bool visit_nt_002( ) {
	std::variant<int, double> a = 5.5;
	std::optional<bool> result;
	::daw::visit_nt( a, [&result]( auto v ) { result = ( v == 5.5 ); } );
	daw::expecting( result );
	daw::expecting( *result );
	return true;
}
static_assert( visit_nt_002( ) );

constexpr bool visit_nt_003( ) {
	std::variant<int, double> a = 5.5;
	auto result = ::daw::visit_nt<bool>(
	  a, []( int v ) { return v == 5.5; }, []( double d ) { return d == 5.5; } );
	daw::expecting( result );
	return true;
}
static_assert( visit_nt_003( ) );

struct A {
	constexpr bool operator( )( int ) const {
		return true;
	}
};
static_assert( ::daw::is_visitable_v<int, A> );

struct B {};
static_assert( !::daw::is_visitable_v<int, B> );

int main( ) {}
