// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_exchange.h"
#include "daw/third_party/private_access.h"

#include <cassert>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

#include "daw_container_help_impl.h"
#include "daw_container_help_ins_ext.h"

namespace daw {
	namespace container_help_impl {
#if defined( _LIBCPP_VECTOR )
		// libc++

		template<typename T, typename Alloc>
		struct std_vector_layout {
			static_assert( not std::is_same_v<T, bool>, "bad instance" );
			using alloc_type = Alloc;
			using alloc_traits = std::allocator_traits<alloc_type>;
			using pointer = typename std::allocator_traits<alloc_type>::pointer;

			pointer m_first;
			pointer m_last;
			std::__compressed_pair<pointer, alloc_type> m_capacity;

			constexpr std::size_t capacity( ) const {
				return static_cast<std::size_t>( m_capacity.first( ) - m_first );
			}

			constexpr std::size_t size( ) const {
				return static_cast<std::size_t>( m_last - m_first );
			}

			constexpr alloc_type take_allocator( ) {
				static_assert(
				  alloc_traits::is_always_equal::value or
				    alloc_traits::propagate_on_container_move_assignment::value,
				  "Unsupported allocator" );
				if constexpr( alloc_traits::is_always_equal::value ) {
					return alloc_type( );
				} else {
					return std::move(
					  *static_cast<alloc_type *>( m_capacity.second( ) ) );
				}
			}

			constexpr pointer take_buffer( ) {
				auto ret = daw::exchange( m_first, pointer( ) );
				m_last = pointer( );
				m_capacity.first( ) = pointer( );
				return ret;
			}

			constexpr void capacity( std::size_t sz ) {
				m_capacity.first( ) = m_first + static_cast<std::ptrdiff_t>( sz );
			}

			constexpr void size( std::size_t sz ) {
				m_last = m_first + static_cast<std::ptrdiff_t>( sz );
			}

			constexpr void give_allocator( alloc_type alloc ) {
				assert( m_first == pointer( ) );
				static_assert(
				  alloc_traits::is_always_equal::value or
				    alloc_traits::propagate_on_container_move_assignment::value,
				  "Unsupported allocator" );
				if constexpr( alloc_traits::propagate_on_container_move_assignment::
				                value ) {
					m_capacity.second( ) = std::move( alloc );
				}
			}

			constexpr void give_buffer( pointer p ) {
				// Assumes vector is empty
				assert( m_first == pointer( ) );
				m_first = p;
			}
		};
#elif defined( _GLIBCXX_VECTOR )
		// libstdc++
	}
}
template<typename T, typename Allocator>
struct StdVecStart
  : accessor::MemberWrapper<std::vector<T, Allocator>,
                            typename std::vector<T, Allocator>::pointer> {};
template<typename T, typename Allocator>
class accessor::MakeProxy<StdVecStart<T, Allocator>,
                          &std::vector<T, Allocator>::_M_start>;

template<typename T, typename Allocator>
struct StdVecFinish
  : accessor::MemberWrapper<std::vector<T, Allocator>,
                            typename std::vector<T, Allocator>::pointer> {};
template<typename T, typename Allocator>
class accessor::MakeProxy<StdVecFinish<T, Allocator>,
                          &std::vector<T, Allocator>::_M_finish>;

template<typename T, typename Allocator>
struct StdVecEndOfStorage
  : accessor::MemberWrapper<std::vector<T, Allocator>,
                            typename std::vector<T, Allocator>::pointer> {};
template<typename T, typename Allocator>
class accessor::MakeProxy<StdVecEndOfStorage<T, Allocator>,
                          &std::vector<T, Allocator>::_M_end_of_storage>;

template<typename T, typename Allocator>
struct StdVecImpl
  : accessor::MemberWrapper<std::vector<T, Allocator>, Allocator> {};
template<typename T, typename Allocator>
class accessor::MakeProxy<StdVecImpl<T, Allocator>,
                          &std::vector<T, Allocator>::_M_impl>;

namespace daw {
	namespace container_help_impl {
		template<typename T, typename Alloc>
		struct std_vector_layout_impl {
			static_assert( not std::is_same_v<T, bool>, "bad instance" );

			template<typename A>
			using alloc_traits_t = typename __gnu_cxx::__alloc_traits<A>;

			using allocator_type = Alloc;
			using allocator_traits = alloc_traits_t<allocator_type>;
			using rebound_allocator_type =
			  typename allocator_traits::template rebind<T>::other;
			using rebound_allocator_traits = alloc_traits_t<rebound_allocator_type>;
			using pointer = typename rebound_allocator_traits::pointer;

			static_assert( not std::is_same_v<T, bool>, "bad instance" );

			struct impl_type : rebound_allocator_type {
				pointer m_first;
				pointer m_last;
				pointer m_capacity;
			};
		};

		template<typename T, typename Allocator>
		using rebound_allocator_type =
		  typename std::allocator_traits<Allocator>::template rebind_alloc<T>;

		template<typename T, typename Allocator>
		constexpr decltype( auto ) first_ptr( std::vector<T, Allocator> &vec ) {
			return ::accessor::accessMember<StdVecStart<T, Allocator>>( vec );
		}

		template<typename T, typename Allocator>
		constexpr decltype( auto ) last_ptr( std::vector<T, Allocator> &vec ) {
			return ::accessor::accessMember<StdVecFinish<T, Allocator>>( vec );
		}

		template<typename T, typename Allocator>
		constexpr decltype( auto ) capacity_ptr( std::vector<T, Allocator> &vec ) {
			return ::accessor::accessMember<StdVecEndOfStorage<T, Allocator>>( vec );
		}

		template<typename T, typename Allocator>
		constexpr decltype( auto ) get_alloc( std::vector<T, Allocator> &vec ) {
			return ::accessor::accessMember<StdVecImpl<T, Allocator>>( vec );
		}

		template<typename T, typename Allocator>
		constexpr auto take_allocator( std::vector<T, Allocator> &vec ) {
			using alloc_traits = std::allocator_traits<Allocator>;
			using alloc = rebound_allocator_type<T, Allocator>;
			static_assert(
			  alloc_traits::is_always_equal::value or
			    alloc_traits::propagate_on_container_move_assignment::value,
			  "Unsupported allocator" );
			if constexpr( alloc_traits::is_always_equal::value ) {
				return alloc{ };
			} else {
				return std::move( get_alloc( vec ).get( ) );
			}
		}

		template<typename T, typename Allocator>
		constexpr auto take_buffer( std::vector<T, Allocator> &vec ) {
			using pointer = typename std::vector<T, Allocator>::pointer;
			auto ret = daw::exchange( first_ptr( vec ).get( ), pointer( ) );
			last_ptr( vec ).get( ) = pointer( );
			capacity_ptr( vec ).get( ) = pointer( );
			return ret;
		}

		template<typename T, typename Allocator>
		constexpr void give_allocator( std::vector<T, Allocator> &vec,
		                               Allocator &&alloc ) {
			using pointer = typename std::vector<T, Allocator>::pointer;
			using alloc_traits = std::allocator_traits<Allocator>;
			assert( first_ptr( vec ) == pointer( ) );
			static_assert(
			  alloc_traits::is_always_equal::value or
			    alloc_traits::propagate_on_container_move_assignment::value,
			  "Unsupported allocator" );
			if constexpr( alloc_traits::propagate_on_container_move_assignment::
			                value ) {
				get_alloc( vec ).get( ) = std::move( alloc );
			}
		}

		template<typename T, typename Allocator>
		constexpr void
		give_buffer( std::vector<T, Allocator> &vec,
		             typename std::vector<T, Allocator>::pointer p ) {
			// Assumes vector is empty
			assert( ( first_ptr( vec ) ==
			          typename std::vector<T, Allocator>::pointer( ) ) );
			first_ptr( vec ).get( ) = p;
		}

		template<typename T, typename Alloc>
		struct std_vector_layout : std_vector_layout_impl<T, Alloc>::impl_type {
			using pointer = typename std_vector_layout_impl<T, Alloc>::pointer;
			using base = typename std_vector_layout_impl<T, Alloc>::impl_type;
			using alloc_traits = std::allocator_traits<Alloc>;

			using alloc_type =
			  typename std_vector_layout_impl<T, Alloc>::rebound_allocator_type;

			constexpr std::size_t capacity( ) const {
				return static_cast<std::size_t>( base::m_capacity - base::m_first );
			}

			constexpr std::size_t size( ) const {
				return static_cast<std::size_t>( base::m_last - base::m_first );
			}

			constexpr alloc_type take_allocator( ) {
				return ::daw::container_help_impl::take_allocator( *this );
			}

			constexpr pointer take_buffer( ) {
				return ::daw::container_help_impl::take_buffer( *this );
			}

			constexpr void capacity( std::size_t sz ) {
				base::m_capacity = base::m_first + static_cast<std::ptrdiff_t>( sz );
			}

			constexpr void size( std::size_t sz ) {
				base::m_last = base::m_first + static_cast<std::ptrdiff_t>( sz );
			}

			constexpr void give_allocator( alloc_type alloc ) {
				assert( base::m_first == pointer( ) );
				static_assert(
				  alloc_traits::is_always_equal::value or
				    alloc_traits::propagate_on_container_move_assignment::value,
				  "Unsupported allocator" );
				if constexpr( alloc_traits::propagate_on_container_move_assignment::
				                value ) {
					*static_cast<alloc_type *>( this ) = std::move( alloc );
				}
			}

			constexpr void give_buffer( pointer p ) {
				// Assumes vector is empty
				assert( base::m_first == pointer( ) );
				base::m_first = p;
			}
		};
#endif
	} // namespace container_help_impl

	template<typename T, typename Allocator>
	struct extract_from_container_t<std::vector<T, Allocator>> {
		explicit extract_from_container_t( ) = default;

		constexpr container_data<T, Allocator>
		operator( )( std::vector<T, Allocator> &vec ) const {
			using vec_t = std::vector<T>;
			using layout_t = container_help_impl::std_vector_layout<T, Allocator>;
			static_assert( sizeof( layout_t ) == sizeof( vec_t ), "mismatch" );
			static_assert( alignof( layout_t ) == alignof( vec_t ), "mismatch" );

			auto const sz = vec.size( );
			auto const cap = vec.capacity( );
			auto alloc = daw::container_help_impl::take_allocator( vec );
			auto buff = daw::container_help_impl::take_buffer( vec );
			return container_data<T, Allocator>( buff, std::move( alloc ), sz, cap );
		}
	};

	template<typename T, typename Allocator>
	struct insert_into_container_t<std::vector<T, Allocator>> {
		using alloc_type = Allocator;
		using alloc_traits = std::allocator_traits<alloc_type>;
		using pointer = typename alloc_traits::pointer;

		explicit insert_into_container_t( ) = default;

		constexpr void operator( )( std::vector<T, Allocator> &vec,
		                            pointer buff,
		                            alloc_type alloc,
		                            std::size_t capacity,
		                            std::size_t size ) const {
			using vec_t = std::vector<T>;
			using layout_t = container_help_impl::std_vector_layout<T, Allocator>;
			static_assert( sizeof( layout_t ) == sizeof( vec_t ), "mismatch" );
			static_assert( alignof( layout_t ) == alignof( vec_t ), "mismatch" );
			if( not vec.empty( ) ) {
				vec.clear( );
			}
			vec.shrink_to_fit( );
			if( vec.capacity( ) > 0 ) {
				(void)extract_from_container( vec );
			}
			auto *vec_ptr = reinterpret_cast<layout_t *>( std::addressof( vec ) );
			vec_ptr->give_allocator( std::move( alloc ) );
			vec_ptr->give_buffer( buff );
			vec_ptr->capacity( capacity );
			vec_ptr->size( size );
		}
	};
} // namespace daw
