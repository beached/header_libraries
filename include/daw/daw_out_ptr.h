// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/cpp_20.h"
#include "daw/daw_concepts.h"
#include "daw/daw_move.h"
#include "daw/daw_not_null.h"

#include <memory>
#include <tuple>
#include <type_traits>

namespace daw {
	namespace op_impl {
		// Lookup order matches std::out_ptr: Pointer, SmartPtr::pointer, then
		// SmartPtr::element_type *
#if defined( DAW_HAS_CPP20_CONCEPTS )
		template<typename SmartPtr, typename P, typename... Args>
		concept resetable_smart_ptr_v =
		  requires( SmartPtr &sptr, P &p, Args &&...args ) {
			  sptr.reset( p, DAW_FWD( args )... );
		  };

		template<typename SmartPtr>
		concept has_pointer_type_v = requires { typename SmartPtr::pointer; };

		template<typename SmartPtr>
		struct smart_ptr_pointer {
			using type = typename SmartPtr::element_type *;
		};

		template<has_pointer_type_v SmartPtr>
		struct smart_ptr_pointer<SmartPtr> {
			using type = typename SmartPtr::pointer;
		};
#else
		template<typename AlwaysVoid, typename SmartPtr, typename P,
		         typename... Args>
		inline constexpr bool resetable_smart_ptr_impl_v = false;

		template<typename SmartPtr, typename P, typename... Args>
		inline constexpr bool resetable_smart_ptr_impl_v<
		  std::void_t<decltype( std::declval<SmartPtr &>( ).reset(
		    std::declval<P &>( ), DAW_FWD( std::declval<Args &&>( ) )... ) )>,
		  SmartPtr, P, Args...> = true;

		template<typename SmartPtr, typename P, typename... Args>
		inline constexpr bool resetable_smart_ptr_v =
		  resetable_smart_ptr_impl_v<void, SmartPtr, P, Args...>;

		template<typename SmartPtr, typename = void>
		inline constexpr bool has_pointer_type_v = false;

		template<typename SmartPtr>
		inline constexpr bool
		  has_pointer_type_v<SmartPtr, std::void_t<typename SmartPtr::pointer>> =
		    true;

		template<typename SmartPtr, typename = void>
		struct smart_ptr_pointer {
			using type = typename SmartPtr::element_type *;
		};

		template<typename SmartPtr>
		struct smart_ptr_pointer<SmartPtr,
		                         std::void_t<typename SmartPtr::pointer>> {
			using type = typename SmartPtr::pointer;
		};
#endif

		template<typename SmartPtr, typename Pointer>
		struct out_ptr_pointer {
			using type = Pointer;
		};

		template<typename SmartPtr>
		struct out_ptr_pointer<SmartPtr, void> : smart_ptr_pointer<SmartPtr> {};

#if defined( DAW_HAS_CPP20_CONCEPTS )
		template<typename P>
		concept VoidPointer =
		  Pointers<P> and
		  std::same_as<void, std::remove_cv_t<std::remove_pointer_t<P>>>;
#endif
	} // namespace op_impl

	template<typename SmartPtr, typename Pointer, typename... Args>
	struct out_ptr_t {
		static_assert( not std::is_reference_v<SmartPtr> );
		static_assert( not std::is_const_v<SmartPtr> );
		static_assert( op_impl::has_pointer_type_v<SmartPtr> or
		                 sizeof...( Args ) > 0,
		               "SmartPtr types without a pointer member type require "
		               "reset arguments (e.g. a deleter) to be passed to out_ptr" );
		using pointer = typename op_impl::out_ptr_pointer<SmartPtr, Pointer>::type;
		static_assert( op_impl::resetable_smart_ptr_v<SmartPtr, pointer, Args...> );
		static_assert( not std::is_same_v<pointer, void> );
		using element_type = typename std::pointer_traits<pointer>::element_type;
		using difference_type =
		  typename std::pointer_traits<pointer>::difference_type;

	private:
		SmartPtr &m_sptr;
		pointer m_new_ptr = nullptr;
		DAW_NO_UNIQUE_ADDRESS std::tuple<Args...> m_reset_args{ };

	public:
		template<typename... A>
		explicit constexpr out_ptr_t( SmartPtr &sptr, A &&...args )
		  : m_sptr( sptr )
		  , m_reset_args( DAW_FWD( args )... ) {}

		DAW_CPP20_CX_DTOR ~out_ptr_t( ) {
			if( m_new_ptr ) {
				std::apply(
				  [&]( auto &&...args ) {
					  m_sptr.reset( m_new_ptr, DAW_FWD( args )... );
				  },
				  std::move( m_reset_args ) );
			}
		}

		out_ptr_t( out_ptr_t && ) = delete;
		out_ptr_t( out_ptr_t const & ) = delete;
		out_ptr_t &operator=( out_ptr_t && ) = delete;
		out_ptr_t &operator=( out_ptr_t const & ) = delete;

		[[nodiscard]] constexpr operator pointer *( ) noexcept {
			return std::addressof( m_new_ptr );
		}

#if defined( DAW_HAS_CPP20_CONCEPTS )
		[[nodiscard]] operator void **( ) noexcept
		requires( not op_impl::VoidPointer<pointer> )
		{
			return reinterpret_cast<void **>( std::addressof( m_new_ptr ) );
		}
#endif

		[[nodiscard]] void **voidpp( ) noexcept {
			static_assert( std::is_pointer_v<pointer> );
			return reinterpret_cast<void **>( std::addressof( m_new_ptr ) );
		}
	};

	template<typename SmartPtr, typename Pointer = void, typename... Args>
	[[nodiscard]] constexpr out_ptr_t<SmartPtr, Pointer, Args...>
	out_ptr( SmartPtr &sptr, Args &&...args ) {
		return out_ptr_t<SmartPtr, Pointer, Args...>( sptr, DAW_FWD( args )... );
	}
} // namespace daw
