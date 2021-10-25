// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/
//

#include <daw/daw_can_constant_evaluate.h>

struct NotCXExpr {
	void operator( )( ) const { };
};
inline static constexpr auto NotCXExpr_v = NotCXExpr{ };
static_assert( not daw::can_constant_evaluate_v<NotCXExpr_v> );

struct CXExpr {
	constexpr void operator( )( ) const { };
};
inline static constexpr auto CXExpr_v = CXExpr{ };

static_assert( daw::can_constant_evaluate_v<CXExpr_v> );

int main( ) {}