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
#include <cstring>
#include <exception>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#if defined( _GLIBCXX_SANITIZE_STD_ALLOCATOR )
#define DAW_VECTOR_RESIZE_GLIBCXX_SANITIZE_STD_ALLOCATOR \
	_GLIBCXX_SANITIZE_STD_ALLOCATOR
#else
#define DAW_VECTOR_RESIZE_GLIBCXX_SANITIZE_STD_ALLOCATOR 0
#endif

#if defined( _GLIBCXX_SANITIZE_VECTOR )
#define DAW_VECTOR_RESIZE_GLIBCXX_SANITIZE_VECTOR _GLIBCXX_SANITIZE_VECTOR
#else
#define DAW_VECTOR_RESIZE_GLIBCXX_SANITIZE_VECTOR 0
#endif

namespace daw {
	namespace vector_resize_and_overwrite_impl {
		template<typename T, typename Allocator>
		struct vector_layout;

#if defined( _GLIBCXX_VECTOR ) and not defined( _GLIBCXX_DEBUG )
		template<typename T, typename Allocator>
		struct vector_layout {
			using vector_type = std::vector<T, Allocator>;
			using pointer = typename vector_type::pointer;
			using rebound_allocator =
			  typename std::allocator_traits<Allocator>::template rebind_alloc<T>;

			struct control_block {
				pointer first;
				pointer finish;
				pointer storage_end;
			};

			struct layout_mirror : rebound_allocator {
				control_block control;

				explicit layout_mirror( Allocator const &alloc )
				  : rebound_allocator( alloc ) {}
			};

			[[nodiscard]] static std::size_t
			control_offset( vector_type const &vec ) {
				layout_mirror mirror( vec.get_allocator( ) );
				auto const *const object = reinterpret_cast<unsigned char const *>(
				  static_cast<void const *>( std::addressof( mirror ) ) );
				auto const *const control = reinterpret_cast<unsigned char const *>(
				  static_cast<void const *>( std::addressof( mirror.control ) ) );
				return static_cast<std::size_t>( control - object );
			}

			static void set_size( vector_type &vec, std::size_t new_size ) noexcept {
				static_assert( sizeof( layout_mirror ) == sizeof( vector_type ),
				               "Unexpected libstdc++ vector layout" );
				static_assert( alignof( layout_mirror ) == alignof( vector_type ),
				               "Unexpected libstdc++ vector alignment" );
				auto *const object = reinterpret_cast<unsigned char *>(
				  static_cast<void *>( std::addressof( vec ) ) );
				control_block control;
				auto const offset = control_offset( vec );
				std::memcpy(
				  std::addressof( control ), object + offset, sizeof( control ) );
				auto const valid =
				  control.first == vec.data( ) and
				  static_cast<std::size_t>( control.finish - control.first ) ==
				    vec.size( ) and
				  static_cast<std::size_t>( control.storage_end - control.first ) ==
				    vec.capacity( );
				assert( valid );
				if( not valid ) {
					std::terminate( );
				}
				control.finish = control.first + new_size;
				std::memcpy(
				  object + offset, std::addressof( control ), sizeof( control ) );
			}
		};
#elif defined( _LIBCPP_VECTOR )
		template<typename T, typename Allocator>
		struct vector_layout {
			using vector_type = std::vector<T, Allocator>;
			using pointer = typename vector_type::pointer;
			using size_type = typename vector_type::size_type;

#if defined( _LIBCPP_ABI_VECTOR_LAYOUT_SIZE_BASED )
			struct control_block {
				pointer first;
				size_type size;
				size_type capacity;
			};
#else
			struct control_block {
				pointer first;
				pointer finish;
				pointer storage_end;
			};
#endif

			static void set_size( vector_type &vec, std::size_t new_size ) noexcept {
				static_assert( sizeof( control_block ) <= sizeof( vector_type ),
				               "Unexpected libc++ vector layout" );
				control_block control;
				std::memcpy(
				  std::addressof( control ), std::addressof( vec ), sizeof( control ) );
#if defined( _LIBCPP_ABI_VECTOR_LAYOUT_SIZE_BASED )
				auto const valid = control.first == vec.data( ) and
				                   control.size == vec.size( ) and
				                   control.capacity == vec.capacity( );
#else
				auto const valid =
				  control.first == vec.data( ) and
				  static_cast<std::size_t>( control.finish - control.first ) ==
				    vec.size( ) and
				  static_cast<std::size_t>( control.storage_end - control.first ) ==
				    vec.capacity( );
#endif
				assert( valid );
				if( not valid ) {
					std::terminate( );
				}
#if defined( _LIBCPP_ABI_VECTOR_LAYOUT_SIZE_BASED )
				control.size = static_cast<size_type>( new_size );
#else
				control.finish = control.first + new_size;
#endif
				std::memcpy( static_cast<void *>( std::addressof( vec ) ),
				             std::addressof( control ),
				             sizeof( control ) );
			}
		};
#elif defined( _MSVC_STL_VERSION )
		template<typename T, typename Allocator>
		struct vector_layout {
			using vector_type = std::vector<T, Allocator>;
			using pointer = typename vector_type::pointer;
			using rebound_allocator =
			  typename std::allocator_traits<Allocator>::template rebind_alloc<T>;

			struct control_block {
				pointer first;
				pointer finish;
				pointer storage_end;
			};

			struct vector_value_mirror {
#if _ITERATOR_DEBUG_LEVEL != 0
				void *proxy;
#endif
				control_block control;
			};

			struct ebo_layout_mirror : rebound_allocator {
				vector_value_mirror value;

				explicit ebo_layout_mirror( Allocator const &alloc )
				  : rebound_allocator( alloc ) {}
			};

			struct member_layout_mirror {
				rebound_allocator alloc;
				vector_value_mirror value;

				explicit member_layout_mirror( Allocator const &a )
				  : alloc( a ) {}
			};

			static constexpr bool use_ebo = std::is_empty_v<rebound_allocator> and
			                                not std::is_final_v<rebound_allocator>;
			using layout_mirror =
			  std::conditional_t<use_ebo, ebo_layout_mirror, member_layout_mirror>;

			[[nodiscard]] static std::size_t
			control_offset( vector_type const &vec ) {
				layout_mirror mirror( vec.get_allocator( ) );
				auto const *const object = reinterpret_cast<unsigned char const *>(
				  static_cast<void const *>( std::addressof( mirror ) ) );
				auto const *const control = reinterpret_cast<unsigned char const *>(
				  static_cast<void const *>( std::addressof( mirror.value.control ) ) );
				return static_cast<std::size_t>( control - object );
			}

			static void set_size( vector_type &vec, std::size_t new_size ) noexcept {
				static_assert( sizeof( layout_mirror ) == sizeof( vector_type ),
				               "Unexpected Microsoft STL vector layout" );
				static_assert( alignof( layout_mirror ) == alignof( vector_type ),
				               "Unexpected Microsoft STL vector alignment" );
				auto *const object = reinterpret_cast<unsigned char *>(
				  static_cast<void *>( std::addressof( vec ) ) );
				control_block control;
				auto const offset = control_offset( vec );
				std::memcpy(
				  std::addressof( control ), object + offset, sizeof( control ) );
				auto const valid =
				  control.first == vec.data( ) and
				  static_cast<std::size_t>( control.finish - control.first ) ==
				    vec.size( ) and
				  static_cast<std::size_t>( control.storage_end - control.first ) ==
				    vec.capacity( );
				assert( valid );
				if( not valid ) {
					std::terminate( );
				}
				control.finish = control.first + new_size;
				std::memcpy(
				  object + offset, std::addressof( control ), sizeof( control ) );
			}
		};
#endif

		template<typename T, typename Allocator>
		inline void
		annotate_contiguous_container( std::vector<T, Allocator> const &vec,
		                               std::size_t old_size,
		                               std::size_t new_size ) noexcept {
			if( old_size == new_size or vec.data( ) == nullptr ) {
				return;
			}
#if defined( _LIBCPP_VECTOR )
			std::__annotate_contiguous_container<Allocator>( vec.data( ),
			                                                 vec.data( ) +
			                                                   vec.capacity( ),
			                                                 vec.data( ) + old_size,
			                                                 vec.data( ) + new_size );
#elif defined( _GLIBCXX_VECTOR ) and                   \
  DAW_VECTOR_RESIZE_GLIBCXX_SANITIZE_STD_ALLOCATOR and \
  DAW_VECTOR_RESIZE_GLIBCXX_SANITIZE_VECTOR
			if constexpr( std::is_same_v<Allocator, std::allocator<T>> ) {
				::__sanitizer_annotate_contiguous_container( vec.data( ),
				                                             vec.data( ) +
				                                               vec.capacity( ),
				                                             vec.data( ) + old_size,
				                                             vec.data( ) + new_size );
			}
#elif defined( _MSVC_STL_VERSION ) and defined( _INSERT_VECTOR_ANNOTATION )
			if constexpr( not std::_Disable_ASan_container_annotations_for_allocator<
			                Allocator> ) {
				if( not std::_Asan_vector_should_annotate ) {
					return;
				}
				using vector_type = std::vector<T, Allocator>;
				auto const *const first = static_cast<void const *>( vec.data( ) );
				auto const *const storage_end =
				  static_cast<void const *>( vec.data( ) + vec.capacity( ) );
				auto const *const old_end =
				  static_cast<void const *>( vec.data( ) + old_size );
				auto const *const new_end =
				  static_cast<void const *>( vec.data( ) + new_size );
				if constexpr( std::_Container_allocation_minimum_asan_alignment<
				                vector_type> >= std::_Asan_granularity ) {
					::__sanitizer_annotate_contiguous_container(
					  first,
					  std::_Get_asan_aligned_after( storage_end ),
					  old_end,
					  new_end );
				} else {
					auto const aligned =
					  std::_Get_asan_aligned_first_end( first, storage_end );
					if( aligned._First == aligned._End ) {
						return;
					}
					::__sanitizer_annotate_contiguous_container(
					  aligned._First,
					  aligned._End,
					  aligned._Clamp_to_end( old_end ),
					  aligned._Clamp_to_end( new_end ) );
				}
			}
#else
			(void)vec;
			(void)old_size;
			(void)new_size;
#endif
		}

		template<typename T, typename Allocator>
		class overwrite_annotation_guard {
			std::vector<T, Allocator> &m_vec;
			std::size_t m_old_size;
			std::size_t m_expanded_size;
			bool m_active = true;

		public:
			overwrite_annotation_guard( std::vector<T, Allocator> &vec,
			                            std::size_t old_size,
			                            std::size_t requested_size ) noexcept
			  : m_vec( vec )
			  , m_old_size( old_size )
			  , m_expanded_size( old_size < requested_size ? requested_size
			                                               : old_size ) {
				annotate_contiguous_container( m_vec, m_old_size, m_expanded_size );
			}

			overwrite_annotation_guard( overwrite_annotation_guard const & ) = delete;
			overwrite_annotation_guard &
			operator=( overwrite_annotation_guard const & ) = delete;

			~overwrite_annotation_guard( ) {
				if( m_active ) {
					annotate_contiguous_container( m_vec, m_expanded_size, m_old_size );
				}
			}

			void commit( std::size_t result_size ) noexcept {
				annotate_contiguous_container( m_vec, m_expanded_size, result_size );
				m_active = false;
			}

			void rollback( ) noexcept {
				annotate_contiguous_container( m_vec, m_expanded_size, m_old_size );
				m_active = false;
			}
		};

		template<typename Func, typename T, typename Allocator>
		[[nodiscard]] inline decltype( auto )
		invoke( Func &&func, T *ptr, std::size_t size, Allocator &alloc ) {
			if constexpr( std::
			                is_invocable_v<Func &&, T *, std::size_t, Allocator &> ) {
				return std::invoke( std::forward<Func>( func ), ptr, size, alloc );
			} else {
				static_assert( std::is_invocable_v<Func &&, T *, std::size_t>,
				               "Func must be invocable as (T*, size_t) or "
				               "(T*, size_t, Allocator&)" );
				return std::invoke( std::forward<Func>( func ), ptr, size );
			}
		}
	} // namespace vector_resize_and_overwrite_impl

	/// Calls func with storage for new_size elements and adopts the returned
	/// size.
	///
	/// Existing elements in [0, min(old_size, new_size)) remain alive.  Func must
	/// construct every new element in [old_size, result), must return a value no
	/// greater than new_size, and must not throw after constructing an element.
	/// The optional Allocator& argument refers to a copy of vec.get_allocator().
	///
	/// This relies on implementation-specific vector layouts.  It supports the
	/// three-pointer layouts used by libstdc++, libc++, and the Microsoft STL,
	/// along with libc++'s opt-in pointer/size/capacity layout.  It deliberately
	/// does not support std::vector<bool> or fancy allocator pointer types.
	template<typename T, typename Allocator, typename Func>
	[[nodiscard]] inline std::size_t
	resize_and_overwrite( std::vector<T, Allocator> &vec, std::size_t new_size,
	                      Func &&func ) {
		static_assert( not std::is_same_v<T, bool>,
		               "daw::resize_and_overwrite does not support vector<bool>" );
		static_assert(
		  std::is_same_v<typename std::vector<T, Allocator>::pointer, T *>,
		  "daw::resize_and_overwrite requires an allocator with T* as its "
		  "pointer type" );

		vec.reserve( new_size );
		auto const old_size = vec.size( );
		auto annotation_guard =
		  vector_resize_and_overwrite_impl::overwrite_annotation_guard<T,
		                                                               Allocator>(
		    vec, old_size, new_size );
		auto alloc = vec.get_allocator( );
		auto const result_value = vector_resize_and_overwrite_impl::invoke(
		  std::forward<Func>( func ), vec.data( ), new_size, alloc );
		static_assert( std::is_convertible_v<decltype( result_value ), std::size_t>,
		               "Func must return a value convertible to size_t" );
		auto const result = static_cast<std::size_t>( result_value );

		assert( result <= new_size );
		if( result > new_size ) {
			annotation_guard.rollback( );
			std::terminate( );
		}
		if( result < old_size ) {
			using alloc_traits = std::allocator_traits<Allocator>;
			for( auto n = old_size; n > result; --n ) {
				alloc_traits::destroy( alloc, vec.data( ) + ( n - 1 ) );
			}
		}
		if( result != old_size ) {
			vector_resize_and_overwrite_impl::vector_layout<T, Allocator>::set_size(
			  vec, result );
		}
		annotation_guard.commit( result );
		return result;
	}
} // namespace daw

#undef DAW_VECTOR_RESIZE_GLIBCXX_SANITIZE_VECTOR
#undef DAW_VECTOR_RESIZE_GLIBCXX_SANITIZE_STD_ALLOCATOR
