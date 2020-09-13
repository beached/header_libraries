// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//
#pragma once

#include <cassert>
#include <cstddef>
#include <cstdio>
#include <memory>
#include <type_traits>

namespace daw {
	template<std::size_t Size = sizeof( void * ),
	         std::size_t Align = alignof( std::max_align_t )>
	struct sbo_storage {
		static constexpr std::size_t storage_size =
		  Size >= sizeof( void * ) ? Size : sizeof( void * );

		std::aligned_storage_t<storage_size, Align> data{ };
		enum class engaged_types : uint8_t { none, local, allocated };
		engaged_types engaged = engaged_types::none;

		template<typename T>
		[[nodiscard]] T *get( ) {
			if constexpr( sizeof( T ) <= storage_size ) {
				assert( engaged == engaged_types::local );
				return reinterpret_cast<T *>( &data );
			} else {
				assert( engaged == engaged_types::allocated );
				auto ptr = reinterpret_cast<T **>( &data );
				return *ptr;
			}
		}

		template<typename T, typename AllocatedType = T>
		[[nodiscard]] T const *get( ) const {
			if constexpr( sizeof( AllocatedType ) <= storage_size ) {
				assert( engaged == engaged_types::allocated );
				return reinterpret_cast<T const *>( &data );
			} else {
				assert( engaged == engaged_types::allocated );
				return *reinterpret_cast<T const **>( &data );
			}
		}

		template<typename T,
		         typename Allocator = std::allocator<std::remove_cv_t<T>>>
		void deallocate( Allocator const &alloc = Allocator{ } ) {
			static_assert( not std::is_array_v<T>, "Arrays unsupported" );
			if constexpr( sizeof( T ) <= storage_size ) {
				assert( engaged == engaged_types::local );
				get<T>( )->~T( );
			} else {
				assert( engaged == engaged_types::allocated );
				Allocator allocator = Allocator{ alloc };
				auto ptr = get<T>( );
				std::allocator_traits<Allocator>::destroy( allocator, ptr );
				std::allocator_traits<Allocator>::deallocate( allocator, ptr, 1 );
			}
			engaged = engaged_types::none;
		}

		template<typename T, typename Allocator = std::allocator<
		                       std::remove_cv_t<std::remove_cv_t<T>>>>
		std::remove_cv_t<T> *allocate( T &&value,
		                               Allocator const &alloc = Allocator{ } ) {
			using base_type = std::remove_cv_t<T>;
			static_assert( not std::is_array_v<base_type>, "Arrays unsupported" );
			assert( engaged == engaged_types::none );
			if constexpr( sizeof( base_type ) <= storage_size ) {
				if constexpr( std::is_aggregate_v<base_type> ) {
					auto *result = new( &data ) base_type{ std::forward<T>( value ) };
					engaged = engaged_types::local;
					return result;
				} else {
					auto *result = new( &data ) base_type( std::forward<T>( value ) );
					engaged = engaged_types::local;
					return result;
				}
			} else {
				// Allocate a value via allocator
				using base_type_ptr = base_type *;
				Allocator allocator = Allocator{ alloc };
				base_type_ptr ptr =
				  std::allocator_traits<Allocator>::allocate( allocator, 1 );
				// Store pointer from allocator in buffer
				new( &data ) base_type_ptr{ ptr };
				// Construct new value from passed value at location from allocator
				std::allocator_traits<Allocator>::construct( allocator, ptr,
				                                             std::forward<T>( value ) );
				engaged = engaged_types::allocated;
				return ptr;
			}
		}
	};
	static_assert( std::is_aggregate_v<sbo_storage<>> );

} // namespace daw
