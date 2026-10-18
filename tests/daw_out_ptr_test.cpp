// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include <daw/daw_ensure.h>
#include <daw/daw_out_ptr.h>
#include <daw/daw_unique_ptr.h>

#include <memory>
#include <type_traits>

namespace {
	// C-style API that allocates through an out parameter
	int make_int( int **out, int value ) {
		*out = new int( value );
		return 0;
	}

	int make_int_void( void **out, int value ) {
		*out = new int( value );
		return 0;
	}

	int make_nothing( int **out ) {
		*out = nullptr;
		return 1;
	}

	int deleter_calls = 0;
	struct counting_deleter {
		void operator( )( int *p ) const {
			++deleter_calls;
			delete p;
		}
	};

	void test_std_unique_ptr( ) {
		auto p = std::unique_ptr<int>( );
		daw_ensure( make_int( daw::out_ptr( p ), 42 ) == 0 );
		daw_ensure( p );
		daw_ensure( *p == 42 );
	}

	void test_std_unique_ptr_void_pp( ) {
		auto p = std::unique_ptr<int>( );
		daw_ensure( make_int_void( daw::out_ptr( p ).voidpp( ), 55 ) == 0 );
		daw_ensure( p );
		daw_ensure( *p == 55 );
	}

	void test_replaces_existing( ) {
		deleter_calls = 0;
		auto p = std::unique_ptr<int, counting_deleter>( new int( 1 ) );
		daw_ensure( make_int( daw::out_ptr( p ), 2 ) == 0 );
		daw_ensure( deleter_calls == 1 );
		daw_ensure( *p == 2 );
		p.reset( );
		daw_ensure( deleter_calls == 2 );
	}

	void test_null_result_leaves_existing( ) {
		auto p = std::unique_ptr<int>( new int( 7 ) );
		daw_ensure( make_nothing( daw::out_ptr( p ) ) == 1 );
		daw_ensure( p );
		daw_ensure( *p == 7 );
	}

	void test_reset_happens_at_end_of_full_expression( ) {
		auto p = std::unique_ptr<int>( );
		auto const was_empty_during_call = [&] {
			bool result = false;
			(void)( make_int( daw::out_ptr( p ), 3 ), result = not p );
			return result;
		}( );
		daw_ensure( was_empty_during_call );
		daw_ensure( p );
		daw_ensure( *p == 3 );
	}

	// Smart pointer whose reset takes an extra argument
	struct tagged_ptr {
		using pointer = int *;
		std::unique_ptr<int> ptr{ };
		int tag = 0;

		void reset( pointer p, int t ) {
			ptr.reset( p );
			tag = t;
		}
	};

	void test_reset_args( ) {
		auto p = tagged_ptr{ };
		daw_ensure( make_int( daw::out_ptr( p, 5 ), 9 ) == 0 );
		daw_ensure( p.ptr );
		daw_ensure( *p.ptr == 9 );
		daw_ensure( p.tag == 5 );
	}

	void test_shared_ptr_with_deleter( ) {
		deleter_calls = 0;
		{
			auto p = std::shared_ptr<int>( );
			daw_ensure( make_int( daw::out_ptr( p, counting_deleter{ } ), 9 ) == 0 );
			daw_ensure( p );
			daw_ensure( *p == 9 );
			daw_ensure( deleter_calls == 0 );
		}
		daw_ensure( deleter_calls == 1 );
	}

	void test_daw_unique_ptr( ) {
		auto p = daw::unique_ptr<int>( );
		daw_ensure( make_int( daw::out_ptr( p ), 123 ) == 0 );
		daw_ensure( p );
		daw_ensure( *p == 123 );
	}

	void test_types( ) {
		using op_t = daw::out_ptr_t<std::unique_ptr<int>, void>;
		static_assert( std::is_same_v<op_t::pointer, int *> );
		static_assert( std::is_same_v<op_t::element_type, int> );
		static_assert( std::is_convertible_v<op_t &, int **> );
#if defined( DAW_HAS_CPP20_CONCEPTS )
		static_assert( std::is_convertible_v<op_t &, void **> );
#endif
		static_assert( not std::is_copy_constructible_v<op_t> );
		static_assert( not std::is_move_constructible_v<op_t> );
		static_assert( not std::is_copy_assignable_v<op_t> );
		static_assert( not std::is_move_assignable_v<op_t> );
	}

	// Has both pointer and element_type, which disagree
	struct both_ptr {
		using pointer = long *;
		using element_type = int;
		template<typename P>
		void reset( P ) {}
	};

	// Has element_type only, like std::shared_ptr, so reset args are required
	struct element_type_only_ptr {
		using element_type = int;
		void reset( int *, counting_deleter ) {}
	};

	void test_pointer_lookup_order( ) {
		// Explicit Pointer wins
		static_assert(
		  std::is_same_v<daw::out_ptr_t<both_ptr, int *>::pointer, int *> );
		// Then SmartPtr::pointer
		static_assert(
		  std::is_same_v<daw::out_ptr_t<both_ptr, void>::pointer, long *> );
		// Then SmartPtr::element_type *
		static_assert(
		  std::is_same_v<daw::out_ptr_t<element_type_only_ptr, void,
		                                counting_deleter>::pointer,
		                 int *> );
		static_assert(
		  std::is_same_v<
		    daw::out_ptr_t<std::shared_ptr<int>, void, counting_deleter>::pointer,
		    int *> );
	}
} // namespace

int main( ) {
	test_std_unique_ptr( );
	test_std_unique_ptr_void_pp( );
	test_replaces_existing( );
	test_null_result_leaves_existing( );
	test_reset_happens_at_end_of_full_expression( );
	test_reset_args( );
	test_shared_ptr_with_deleter( );
	test_daw_unique_ptr( );
	test_types( );
	test_pointer_lookup_order( );
}
