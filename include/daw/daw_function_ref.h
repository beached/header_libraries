// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <cassert>
#include <memory>
#include <type_traits>

#ifdef DAW_CPP26_CONSTEXPR
#undef DAW_CPP26_CONSTEXPR
#endif
#ifdef DAW_CPP26_CONSTEVAL
#undef DAW_CPP26_CONSTEVAL
#endif
#if __cpp_constexpr >= 202306L
#define DAW_CPP26_CONSTEXPR constexpr
#define DAW_CPP26_CONSTEVAL consteval
#else
#define DAW_CPP26_CONSTEXPR
#define DAW_CPP26_CONSTEVAL
#endif

namespace daw {
	namespace func_ref_details {
		template<typename Ret, typename T, typename... Ps>
		inline constexpr bool is_callable_v =
		  std::is_class_v<T> and std::is_invocable_r_v<Ret, T, Ps...>;
	} // namespace func_ref_details

	template<typename>
	struct function_ref;

	template<typename Result, typename... Params>
	struct function_ref<Result( Params... )> {
		union data_t {
			void const *obj_ptr;
			Result ( *func_ptr )( Params... );

			constexpr data_t( void const *ptr ) noexcept
			  : obj_ptr( ptr ) {}

			constexpr data_t( Result ( *ptr )( Params... ) ) noexcept
			  : func_ptr( ptr ) {}
		};

		data_t m_data;
		Result ( *m_thunk )( data_t const &, Params... );

		template<typename Func>
		static DAW_CPP26_CONSTEXPR Result obj_thunk( data_t const &d,
		                                             Params... params ) {
			auto const &obj = *static_cast<Func const *>( d.obj_ptr );
			return obj( params... );
		}

		static constexpr Result fp_thunk( data_t const &d, Params... params ) {
			return d.func_ptr( params... );
		}

	public:
		template<
		  typename Func,
		  std::enable_if_t<func_ref_details::is_callable_v<Result, Func, Params...>,
		                   std::nullptr_t> = nullptr>
		constexpr function_ref( Func const &func ) noexcept
		  : m_data( static_cast<void const *>( std::addressof( func ) ) )
		  , m_thunk( obj_thunk<Func> ) {}

		template<
		  typename Func,
		  std::enable_if_t<func_ref_details::is_callable_v<void, Func, Params...>,
		                   std::nullptr_t> = nullptr>
		constexpr function_ref &operator=( Func const &func ) noexcept {
			m_data = static_cast<void const *>( std::addressof( func ) );
			m_thunk = obj_thunk<Func>;
			return *this;
		}

		constexpr function_ref( Result ( *ptr )( Params... ) ) noexcept
		  : m_data( ptr )
		  , m_thunk( nullptr ) {
			assert( ptr );
		}

		constexpr function_ref &operator=( Result ( *ptr )( Params... ) ) noexcept {
			m_data = ptr;
			m_thunk = nullptr;
			assert( ptr );
			return *this;
		}

		constexpr Result operator( )( Params... params ) const {
			if( not m_thunk ) {
				return m_data.func_ptr( std::forward<Params>( params )... );
			}
			return m_thunk( m_data, std::forward<Params>( params )... );
		}
	};

	template<typename... Params>
	struct function_ref<void( Params... )> {
		union data_t {
			void const *obj_ptr;
			void ( *func_ptr )( Params... );

			constexpr data_t( void const *ptr ) noexcept
			  : obj_ptr( ptr ) {}

			constexpr data_t( void ( *ptr )( Params... ) ) noexcept
			  : func_ptr( ptr ) {}
		};

		data_t m_data;
		void ( *m_thunk )( data_t const &, Params... );

		template<typename Func>
		static DAW_CPP26_CONSTEXPR void obj_thunk( data_t const &d,
		                                           Params... params ) {
			auto const &obj = *static_cast<Func const *>( d.obj_ptr );
			(void)obj( params... );
		}

		static constexpr void fp_thunk( data_t const &d, Params... params ) {
			d.func_ptr( params... );
		}

		template<
		  typename Func,
		  std::enable_if_t<func_ref_details::is_callable_v<void, Func, Params...>,
		                   std::nullptr_t> = nullptr>
		constexpr function_ref( Func const &func ) noexcept
		  : m_data( static_cast<void const *>( std::addressof( func ) ) )
		  , m_thunk( obj_thunk<Func> ) {}

		template<
		  typename Func,
		  std::enable_if_t<func_ref_details::is_callable_v<void, Func, Params...>,
		                   std::nullptr_t> = nullptr>
		constexpr function_ref &operator=( Func const &func ) noexcept {
			m_data = static_cast<void const *>( std::addressof( func ) );
			m_thunk = obj_thunk<Func>;
			return *this;
		}

		constexpr function_ref( void ( *ptr )( Params... ) ) noexcept
		  : m_data( ptr )
		  , m_thunk( fp_thunk ) {
			assert( ptr );
		}

		constexpr function_ref &operator=( void ( *ptr )( Params... ) ) noexcept {
			m_data = ptr;
			m_thunk = fp_thunk;
			assert( ptr );
			return *this;
		}

		constexpr void operator( )( Params... params ) const {
			m_thunk( m_data, params... );
		}
	};
} // namespace daw
