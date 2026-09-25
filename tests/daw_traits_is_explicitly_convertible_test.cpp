// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/traits/daw_traits_is_explicitly_convertible.h>

#include <memory>
#include <string>
#include <string_view>

using daw::is_explicitly_convertible_v;

namespace {
	struct Explicit {
		explicit Explicit( int ) {}
	};

	struct ExplicitOp {
		explicit operator int( ) const {
			return 0;
		}
	};

	struct RvalueOp {
		operator int( ) && {
			return 0;
		}
	};

	struct MoveOnly {
		MoveOnly( ) = default;
		MoveOnly( MoveOnly && ) = default;
		MoveOnly( MoveOnly const & ) = delete;
	};

	struct Base {
		virtual ~Base( ) = default;
	};
	struct Derived : Base {};

	struct Abstract {
		virtual void f( ) = 0;
	};

	enum class Color { red };

	struct Unrelated {};
} // namespace

// Arithmetic and enums
static_assert( is_explicitly_convertible_v<int, int> );
static_assert( is_explicitly_convertible_v<int, long> );
static_assert( is_explicitly_convertible_v<double, int> );
static_assert( is_explicitly_convertible_v<Color, int> );
static_assert( is_explicitly_convertible_v<int, Color> );

// Explicit constructors / conversion operators, which is_convertible rejects
static_assert( is_explicitly_convertible_v<int, Explicit> );
static_assert( not std::is_convertible_v<int, Explicit> );
static_assert( is_explicitly_convertible_v<ExplicitOp, int> );
static_assert( not std::is_convertible_v<ExplicitOp, int> );
static_assert( is_explicitly_convertible_v<std::string_view, std::string> );
static_assert( not std::is_convertible_v<std::string_view, std::string> );
static_assert( not is_explicitly_convertible_v<Explicit, int> );

// From is treated as an rvalue, matching std::is_convertible
static_assert( is_explicitly_convertible_v<RvalueOp, int> );
static_assert( not is_explicitly_convertible_v<RvalueOp &, int> );
static_assert( is_explicitly_convertible_v<MoveOnly, MoveOnly> );
static_assert( not is_explicitly_convertible_v<MoveOnly &, MoveOnly> );
static_assert(
  is_explicitly_convertible_v<std::unique_ptr<int>, std::unique_ptr<int>> );
static_assert( not is_explicitly_convertible_v<std::unique_ptr<int> const &,
                                               std::unique_ptr<int>> );

// References
static_assert( is_explicitly_convertible_v<int, int &&> );
static_assert( is_explicitly_convertible_v<int &, int &&> );
static_assert( is_explicitly_convertible_v<int, int const &> );
static_assert( not is_explicitly_convertible_v<int, int &> );
static_assert( is_explicitly_convertible_v<int &, int &> );
static_assert( not is_explicitly_convertible_v<int const &, int &> );

// Pointers and class hierarchies
static_assert( is_explicitly_convertible_v<void *, int *> );
static_assert( not std::is_convertible_v<void *, int *> );
static_assert( is_explicitly_convertible_v<int *, void *> );
static_assert( not is_explicitly_convertible_v<int const *, int *> );
static_assert( not is_explicitly_convertible_v<int *, long *> );
static_assert( is_explicitly_convertible_v<Derived *, Base *> );
static_assert( is_explicitly_convertible_v<Base *, Derived *> );
static_assert( is_explicitly_convertible_v<Base &, Derived &> );
static_assert( not is_explicitly_convertible_v<Base, Derived> );
static_assert( is_explicitly_convertible_v<int[3], int *> );

// void
static_assert( is_explicitly_convertible_v<int, void> );
static_assert( is_explicitly_convertible_v<void, void> );
static_assert( not is_explicitly_convertible_v<void, int> );

// Non-convertible
static_assert( not is_explicitly_convertible_v<Unrelated, int> );
static_assert( not is_explicitly_convertible_v<int, Unrelated> );
static_assert( not is_explicitly_convertible_v<std::string, int> );
static_assert( not is_explicitly_convertible_v<Derived, Abstract> );

int main( ) {}
