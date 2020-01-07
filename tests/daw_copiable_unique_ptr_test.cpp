// The MIT License (MIT)
//
// Copyright (c) 2014-2020 Darrell Wright
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

#include "daw/daw_benchmark.h"
#include "daw/daw_copiable_unique_ptr.h"
#include "daw/daw_utility.h"

#include <cstddef>
#include <iostream>
#include <mutex>
#include <type_traits>

void daw_copiable_unique_ptr_test_01( ) {
	struct lrg {
		size_t a{};
		size_t b{};
		size_t c{};
		size_t d{};
		size_t e{};
	};
	std::cout << "sizeof( int ) -> " << sizeof( int ) << '\t';
	std::cout << "sizeof( daw::copiable_unique_ptr<int> ) -> "
	          << sizeof( daw::copiable_unique_ptr<int> ) << '\n';
	std::cout << "sizeof( size_t ) -> " << sizeof( size_t ) << '\t';
	std::cout << "sizeof( daw::copiable_unique_ptr<size_t> ) -> "
	          << sizeof( daw::copiable_unique_ptr<size_t> ) << '\n';
	std::cout << "sizeof( lrg ) -> " << sizeof( lrg ) << '\t';
	std::cout << "sizeof( daw::copiable_unique_ptr<lrg> ) -> "
	          << sizeof( daw::copiable_unique_ptr<lrg> ) << '\n';

	struct A {
		constexpr A( ) noexcept {}
		A( A const & ) = delete;
		A &operator=( A const & ) = delete;
	};
	auto const a = daw::copiable_unique_ptr<int>( );
	daw::expecting( !a );
	auto b = a;

	auto e = daw::make_copiable_unique_ptr<A>( );
	daw::expecting( static_cast<bool>( e ) );

	auto f = std::move( e );

	auto g = daw::make_copiable_unique_ptr<int>( );
	auto const h = g;

	*g = 5;

	auto hash_value = std::hash<decltype( g )>{}( g );
	Unused( hash_value );

	auto i = daw::make_copiable_unique_ptr<std::mutex>( );
	i.reset( );
	daw::expecting( i.get( ), nullptr );
}

struct virt_A {
	virtual ~virt_A( ) noexcept;
	virtual int operator( )( ) const {
		return 0;
	}
};

virt_A::~virt_A( ) noexcept = default;

struct virt_B : virt_A {
	int operator( )( ) const override {
		return 1;
	}
	~virt_B( ) noexcept override;
};

virt_B::~virt_B( ) noexcept = default;

void virtual_inheritance_test( ) {
	using test_t = daw::copiable_unique_ptr<virt_A>;
	auto a = test_t( new virt_A( ) );
	auto b = test_t( new virt_B( ) );

	daw::expecting( ( *a )( ) != ( *b )( ) );
	std::cout << "a: " << ( *a )( ) << ", " << sizeof( virt_A ) << '\n';
	std::cout << "b: " << ( *b )( ) << ", " << sizeof( virt_B ) << '\n';
}

int main( ) {
	daw_copiable_unique_ptr_test_01( );
	virtual_inheritance_test( );
}
