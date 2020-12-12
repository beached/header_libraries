// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <array>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <functional>
#include <utility>
#if __has_include( <version> )
#include <version>
#endif

#if __has_include( <bit> )  and defined(__cpp_lib_bit_cast)
#include <bit>
#define DAW_BITCAST std::bit_cast
#else
#include "cpp_17.h"
#define DAW_BITCAST daw::bit_cast
#endif

namespace daw {
	template<typename Result, typename... Args>
	struct function_view_table_storage {
		using ref_buffer_t = std::array<std::byte, sizeof( void * )>;
		using fp_t = Result ( * )( Args... );
		union data_t {
			ref_buffer_t ref_storage;
			fp_t fp_storage;

			explicit constexpr data_t( ref_buffer_t &&r )
			  : ref_storage( std::move( r ) ) {}
			explicit constexpr data_t( fp_t f )
			  : fp_storage( f ) {}
		} data;
		using reffp_t = Result ( * )( data_t const &, Args... );
		reffp_t call_ptr;

		template<typename Func>
		static constexpr function_view_table_storage make( Func &&f ) {
			using fn_t = std::remove_reference_t<Func>;
			if constexpr( std::is_pointer_v<fn_t> ) {
				return { data_t{ f }, []( data_t const &d, Args... args ) -> Result {
					        return ( d.fp_storage )( std::move( args )... );
				        } };
			} else if constexpr( std::is_empty_v<fn_t> and
			                     std::is_default_constructible_v<fn_t> ) {
				return { data_t{ nullptr },
				         []( data_t const &, Args... args ) -> Result {
					         return fn_t{ }( std::move( args )... );
				         } };
			} else {
				return { data_t{ DAW_BITCAST<ref_buffer_t>( std::addressof( f ) ) },
				         +[]( data_t const &d, Args... args ) -> Result {
					         return ( *DAW_BITCAST<fn_t const *>( d.ref_storage ) )(
					           std::move( args )... );
				         } };
			}
		}
	};

	template<typename... Args>
	struct function_view_table_storage<void, Args...> {
		using ref_buffer_t = std::array<std::byte, sizeof( void * )>;
		using fp_t = void ( * )( Args... );
		union data_t {
			ref_buffer_t ref_storage;
			fp_t fp_storage;

			explicit constexpr data_t( ref_buffer_t &&r )
			  : ref_storage( std::move( r ) ) {}
			explicit constexpr data_t( fp_t f )
			  : fp_storage( f ) {}
		} data;
		using reffp_t = void ( * )( data_t const &, Args... );
		reffp_t call_ptr;

		template<typename Func>
		static constexpr function_view_table_storage make( Func &&f ) {
			using fn_t = std::remove_reference_t<Func>;
			if constexpr( std::is_pointer_v<fn_t> ) {
				return { data_t{ f }, []( data_t const &d, Args... args ) -> void {
					        ( d.fp_storage )( std::move( args )... );
				        } };
			} else if constexpr( std::is_empty_v<fn_t> and
			                     std::is_default_constructible_v<fn_t> ) {
				return { data_t{ nullptr }, []( data_t const &, Args... args ) -> void {
					        fn_t{ }( std::move( args )... );
				        } };
			} else {
				return { data_t{ DAW_BITCAST<ref_buffer_t>( std::addressof( f ) ) },
				         +[]( data_t const &d, Args... args ) -> void {
					         ( *DAW_BITCAST<fn_t const *>( d.ref_storage ) )(
					           std::move( args )... );
				         } };
			}
		}
	};

	template<typename Signature>
	struct function_view;

	template<typename Result, typename... Args>
	struct function_view<Result( Args... )> {
		function_view_table_storage<Result, Args...> function_view_table;

		template<typename Func>
		constexpr function_view( Func &&f )
		  : function_view_table(
		      function_view_table_storage<Result, Args...>::make( f ) ) {}

		constexpr Result operator( )( Args... args ) const {
			return function_view_table.call_ptr( function_view_table.data,
			                                     std::move( args )... );
		}
	};
	template<typename... Args>
	struct function_view<void( Args... )> {
		function_view_table_storage<void, Args...> function_view_table;

		template<typename Func>
		constexpr function_view( Func &&f )
		  : function_view_table( function_view_table_storage<void, Args...>::make(
		      std::forward<Func>( f ) ) ) {}

		constexpr void operator( )( Args... args ) const {
			function_view_table.call_ptr( function_view_table.data,
			                              std::move( args )... );
		}
	};
} // namespace daw

#undef DAW_BIT_CAST
