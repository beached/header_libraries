// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_move.h"
#include "daw/traits/daw_copy_cvref_t.h"

#include <atomic>
#include <utility>

namespace daw::sf_impl {
	enum class cvref_t { None, Const, Ref, RefRef, ConstRef, ConstRefRef };

	template<cvref_t CVRef, bool IsNoExcept, typename R, typename... Params>
	class shared_function_storage_base;

	template<typename T>
	using param_t = std::conditional_t<std::is_scalar_v<T>, T, T &&>;

#define DAW_SF_STORAGE_BASE_BODY( CALL_QUALS )                             \
	std::atomic_int m_weak_count = 1;                                        \
	std::atomic_int m_strong_count = 1;                                      \
                                                                           \
public:                                                                    \
	shared_function_storage_base( ) = default;                               \
	virtual ~shared_function_storage_base( ) = default;                      \
                                                                           \
	virtual DAW_CPP26_CX_ATOMIC R call( Params... )                          \
	  CALL_QUALS noexcept( IsNoExcept ) = 0;                                 \
                                                                           \
	[[nodiscard]] DAW_CPP26_CX_ATOMIC auto *strong_copy( this auto &self ) { \
		++self.m_strong_count;                                                 \
		++self.m_weak_count;                                                   \
		return &self;                                                          \
	}                                                                        \
                                                                           \
	[[nodiscard]] DAW_CPP26_CX_ATOMIC auto *weak_copy( this auto &self ) {   \
		++self.m_weak_count;                                                   \
		return &self;                                                          \
	}                                                                        \
                                                                           \
	DAW_CPP26_CX_ATOMIC void strong_dec( ) {                                 \
		--m_strong_count;                                                      \
	}                                                                        \
                                                                           \
	[[nodiscard]] DAW_CPP26_CX_ATOMIC bool weak_dec( ) {                     \
		return --m_weak_count == 0;                                            \
	}

// One specialization generator
#define DAW_SF_STORAGE_BASE_SPEC( CVREF_ENUM, CALL_QUALS )                   \
	template<bool IsNoExcept, typename R, typename... Params>                  \
	class shared_function_storage_base<CVREF_ENUM, IsNoExcept, R, Params...> { \
		DAW_SF_STORAGE_BASE_BODY( CALL_QUALS );                                  \
	}

	// 6 specializations
	DAW_SF_STORAGE_BASE_SPEC( cvref_t::None, );
	DAW_SF_STORAGE_BASE_SPEC( cvref_t::Const, const );
	DAW_SF_STORAGE_BASE_SPEC( cvref_t::Ref, & );
	DAW_SF_STORAGE_BASE_SPEC( cvref_t::ConstRef, const & );
	DAW_SF_STORAGE_BASE_SPEC( cvref_t::RefRef, && );
	DAW_SF_STORAGE_BASE_SPEC( cvref_t::ConstRefRef, const && );

#undef DAW_SF_STORAGE_BASE_SPEC
#undef DAW_SF_STORAGE_BASE_BODY

	template<typename Fn, cvref_t CVRef, bool IsNoExcept, typename R,
	         typename... Params>
	class shared_function_storage;

	template<typename Fn, bool IsNoExcept, typename R, typename... Params>
	class shared_function_storage<Fn, cvref_t::None, IsNoExcept, R, Params...>
	  : public shared_function_storage_base<cvref_t::None, IsNoExcept, R,
	                                        Params...> {
		DAW_NO_UNIQUE_ADDRESS Fn m_func;

	public:
		explicit DAW_CPP26_CX_ATOMIC shared_function_storage( Fn f )
		  : m_func{ DAW_FWD( f ) } {}

		DAW_CPP26_CX_ATOMIC R
		call( Params... params ) noexcept( IsNoExcept ) override {
			return m_func( std::forward<param_t<Params>>( params )... );
		}
	};

	template<typename Fn, bool IsNoExcept, typename R, typename... Params>
	class shared_function_storage<Fn, cvref_t::Const, IsNoExcept, R, Params...>
	  : public shared_function_storage_base<cvref_t::Const, IsNoExcept, R,
	                                        Params...> {
		DAW_NO_UNIQUE_ADDRESS Fn m_func;

	public:
		explicit DAW_CPP26_CX_ATOMIC shared_function_storage( Fn fn )
		  : m_func{ DAW_FWD( fn ) } {}

		DAW_CPP26_CX_ATOMIC R call( Params... params ) const
		  noexcept( IsNoExcept ) override {
			return m_func( std::forward<param_t<Params>>( params )... );
		}
	};

	template<typename Fn, bool IsNoExcept, typename R, typename... Params>
	class shared_function_storage<Fn, cvref_t::Ref, IsNoExcept, R, Params...>
	  : public shared_function_storage_base<cvref_t::Ref, IsNoExcept, R,
	                                        Params...> {
		DAW_NO_UNIQUE_ADDRESS Fn m_func;

	public:
		explicit DAW_CPP26_CX_ATOMIC shared_function_storage( Fn fn )
		  : m_func{ DAW_FWD( fn ) } {}

		DAW_CPP26_CX_ATOMIC R
		call( Params... params ) & noexcept( IsNoExcept ) override {
			return m_func( std::forward<param_t<Params>>( params )... );
		}
	};

	template<typename Fn, bool IsNoExcept, typename R, typename... Params>
	class shared_function_storage<Fn, cvref_t::ConstRef, IsNoExcept, R, Params...>
	  : public shared_function_storage_base<cvref_t::ConstRef, IsNoExcept, R,
	                                        Params...> {
		DAW_NO_UNIQUE_ADDRESS Fn m_func;

	public:
		explicit DAW_CPP26_CX_ATOMIC shared_function_storage( Fn fn )
		  : m_func{ DAW_FWD( fn ) } {}

		DAW_CPP26_CX_ATOMIC R
		call( Params... params ) const & noexcept( IsNoExcept ) override {
			return m_func( std::forward<param_t<Params>>( params )... );
		}
	};

	template<typename Fn, bool IsNoExcept, typename R, typename... Params>
	class shared_function_storage<Fn, cvref_t::RefRef, IsNoExcept, R, Params...>
	  : public shared_function_storage_base<cvref_t::RefRef, IsNoExcept, R,
	                                        Params...> {
		DAW_NO_UNIQUE_ADDRESS Fn m_func;

	public:
		explicit DAW_CPP26_CX_ATOMIC shared_function_storage( Fn fn )
		  : m_func{ DAW_FWD( fn ) } {}

		DAW_CPP26_CX_ATOMIC R
		call( Params... params ) && noexcept( IsNoExcept ) override {
			return std::move( m_func )( std::forward<param_t<Params>>( params )... );
		}
	};

	template<typename Fn, bool IsNoExcept, typename R, typename... Params>
	class shared_function_storage<Fn, cvref_t::ConstRefRef, IsNoExcept, R,
	                              Params...>
	  : public shared_function_storage_base<cvref_t::ConstRefRef, IsNoExcept, R,
	                                        Params...> {
		DAW_NO_UNIQUE_ADDRESS Fn m_func;

	public:
		explicit DAW_CPP26_CX_ATOMIC shared_function_storage( Fn fn )
		  : m_func{ DAW_FWD( fn ) } {}

		DAW_CPP26_CX_ATOMIC R
		call( Params... params ) const && noexcept( IsNoExcept ) override {
			return std::move( m_func )( std::forward<param_t<Params>>( params )... );
		}
	};

	template<cvref_t CVRef, bool IsNoExcept, typename R, typename... Params>
	DAW_CPP26_CX_ATOMIC void
	weak_release( shared_function_storage_base<CVRef, IsNoExcept, R, Params...>
	                *p ) noexcept {
		if( not p ) {
			return;
		}
		if( p->weak_dec( ) ) {
			delete p;
		}
	}

	template<cvref_t CVRef, bool IsNoExcept, typename R, typename... Params>
	DAW_CPP26_CX_ATOMIC void
	strong_release( shared_function_storage_base<CVRef, IsNoExcept, R, Params...>
	                  *p ) noexcept {
		if( not p ) {
			return;
		}
		p->strong_dec( );
		weak_release( p );
	}

	template<cvref_t CVRef, bool IsNoExcept, typename R, typename... Params,
	         typename Fn>
	requires( std::is_class_v<std::remove_cvref_t<Fn>> ) //
	  DAW_CPP26_CX_ATOMIC auto make_fn_storage( Fn &&fn ) {
		return new shared_function_storage<std::remove_cvref_t<Fn>,
		                                   CVRef,
		                                   IsNoExcept,
		                                   R,
		                                   Params...>{ DAW_FWD( fn ) };
	}

	template<cvref_t CVRef, bool IsNoExcept, typename R, typename... Params,
	         typename Fn>
	requires( not std::is_class_v<std::remove_cvref_t<Fn>> ) //
	  DAW_CPP26_CX_ATOMIC auto make_fn_storage( Fn fn ) {
		return new shared_function_storage<Fn, CVRef, IsNoExcept, R, Params...>{
		  fn };
	}

	template<cvref_t CVRef, bool IsNoExcept, typename R, typename... Params>
	class weak_shared_function_base {
	protected:
		using storage_t =
		  shared_function_storage_base<CVRef, IsNoExcept, R, Params...>;

		storage_t *m_storage = nullptr;

	public:
		explicit DAW_CPP26_CX_ATOMIC weak_shared_function_base( storage_t *storage )
		  : m_storage( storage ? storage->weak_copy( ) : nullptr ) {}

		DAW_CPP26_CX_ATOMIC
		weak_shared_function_base( weak_shared_function_base const &other )
		  : m_storage( other.m_storage ? other.m_storage->weak_copy( ) : nullptr ) {
		}

		DAW_CPP26_CX_ATOMIC
		weak_shared_function_base( weak_shared_function_base &&other ) noexcept
		  : m_storage( std::exchange( other.m_storage, nullptr ) ) {}

		DAW_CPP26_CX_ATOMIC weak_shared_function_base &
		operator=( weak_shared_function_base const &rhs ) {
			if( this != &rhs ) {
				storage_t *old = m_storage;
				m_storage = rhs.m_storage ? rhs.m_storage->weak_copy( ) : nullptr;
				weak_release( old );
			}
			return *this;
		}

		DAW_CPP26_CX_ATOMIC weak_shared_function_base &
		operator=( weak_shared_function_base &&rhs ) noexcept {
			if( this != &rhs ) {
				storage_t *old = m_storage;
				m_storage = std::exchange( rhs.m_storage, nullptr );
				weak_release( old );
			}
			return *this;
		}

		DAW_CPP26_CX_ATOMIC ~weak_shared_function_base( ) {
			weak_release( std::exchange( m_storage, nullptr ) );
		}
	};

	template<cvref_t CVRef, bool IsNoExcept, typename R, typename... Params>
	struct shared_function_base {
	protected:
		using storage_t =
		  shared_function_storage_base<CVRef, IsNoExcept, R, Params...>;
		storage_t *m_storage = nullptr;

		DAW_CPP26_CX_ATOMIC shared_function_base( storage_t *storage )
		  : m_storage( storage ? storage->strong_copy( ) : nullptr ) {}

	public:
		template<typename Fn>
		requires( not std::is_same_v<shared_function_base,
		                             std::remove_cvref_t<Fn>> ) //
		  explicit shared_function_base( Fn &&fn )
		  : m_storage(
		      make_fn_storage<CVRef, IsNoExcept, R, Params...>( DAW_FWD( fn ) ) ) {}

		shared_function_base( ) = default;

		DAW_CPP26_CX_ATOMIC ~shared_function_base( ) {
			storage_t *tmp = std::exchange( m_storage, nullptr );
			strong_release( tmp );
		}

		DAW_CPP26_CX_ATOMIC
		shared_function_base( shared_function_base const &other )
		  : m_storage( other.m_storage ? other.m_storage->strong_copy( )
		                               : nullptr ) {}

		DAW_CPP26_CX_ATOMIC
		shared_function_base( shared_function_base &&other ) noexcept
		  : m_storage( std::exchange( other.m_storage, nullptr ) ) {}

		DAW_CPP26_CX_ATOMIC shared_function_base &
		operator=( shared_function_base const &rhs ) {
			if( this != &rhs ) {
				auto *old = m_storage;
				m_storage = rhs.m_storage ? rhs.m_storage->strong_copy( ) : nullptr;
				strong_release( old );
			}
			return *this;
		}

		DAW_CPP26_CX_ATOMIC shared_function_base &
		operator=( shared_function_base &&rhs ) noexcept {
			if( this != &rhs ) {
				auto *old = m_storage;
				m_storage = std::exchange( rhs.m_storage, nullptr );
				strong_release( old );
			}
			return *this;
		}

		template<typename Self>
		DAW_CPP26_CX_ATOMIC R
		operator( )( this Self &&self, Params... params ) noexcept( IsNoExcept ) {
			using type = std::conditional_t<std::is_reference_v<Self>,
			                                copy_cvref_t<Self, storage_t>,
			                                copy_cvref_t<Self, storage_t> &&>;
			daw_ensure( self.m_storage );
			return static_cast<type>( *self.m_storage )
			  .call( std::forward<param_t<Params>>( params )... );
		}

		explicit DAW_CPP26_CX_ATOMIC operator bool( ) const {
			return m_storage != nullptr;
		}
	};
} // namespace daw::sf_impl
namespace daw {
	template<typename>
	struct shared_function;

	template<typename>
	class weak_shared_function;

#define DAW_DETAIL_SHARED_FUNCTION_SPEC( Signature, CVRef, IsNoExcept )   \
	template<typename R, typename... Params>                                \
	struct shared_function<Signature>                                       \
	  : sf_impl::shared_function_base<CVRef, IsNoExcept, R, Params...> {    \
		using sf_impl::shared_function_base<CVRef, IsNoExcept, R,             \
		                                    Params...>::shared_function_base; \
                                                                          \
		template<typename>                                                    \
		friend class weak_shared_function;                                    \
                                                                          \
		DAW_CPP26_CX_ATOMIC weak_shared_function<Signature> get_weak( );      \
	}

	DAW_DETAIL_SHARED_FUNCTION_SPEC( R( Params... ), sf_impl::cvref_t::None,
	                                 false );
	DAW_DETAIL_SHARED_FUNCTION_SPEC( R( Params... ) const,
	                                 sf_impl::cvref_t::Const, false );
	DAW_DETAIL_SHARED_FUNCTION_SPEC( R( Params... ) &, sf_impl::cvref_t::Ref,
	                                 false );
	DAW_DETAIL_SHARED_FUNCTION_SPEC( R( Params... ) const &,
	                                 sf_impl::cvref_t::ConstRef, false );
	DAW_DETAIL_SHARED_FUNCTION_SPEC( R( Params... ) &&, sf_impl::cvref_t::RefRef,
	                                 false );
	DAW_DETAIL_SHARED_FUNCTION_SPEC( R( Params... ) const &&,
	                                 sf_impl::cvref_t::ConstRefRef, false );
	DAW_DETAIL_SHARED_FUNCTION_SPEC( R( Params... ) noexcept,
	                                 sf_impl::cvref_t::None, true );
	DAW_DETAIL_SHARED_FUNCTION_SPEC( R( Params... ) const noexcept,
	                                 sf_impl::cvref_t::Const, true );
	DAW_DETAIL_SHARED_FUNCTION_SPEC( R( Params... ) & noexcept,
	                                 sf_impl::cvref_t::Ref, true );
	DAW_DETAIL_SHARED_FUNCTION_SPEC( R( Params... ) const & noexcept,
	                                 sf_impl::cvref_t::ConstRef, true );
	DAW_DETAIL_SHARED_FUNCTION_SPEC( R( Params... ) && noexcept,
	                                 sf_impl::cvref_t::RefRef, true );
	DAW_DETAIL_SHARED_FUNCTION_SPEC( R( Params... ) const && noexcept,
	                                 sf_impl::cvref_t::ConstRefRef, true );

#undef DAW_DETAIL_SHARED_FUNCTION_SPEC

#define DAW_DETAIL_SHARED_FUNCTION_GET_WEAK_DEF( Signature )   \
	template<typename R, typename... Params>                     \
	DAW_CPP26_CX_ATOMIC weak_shared_function<Signature>          \
	shared_function<Signature>::get_weak( ) {                    \
		return weak_shared_function<Signature>( this->m_storage ); \
	}

	DAW_DETAIL_SHARED_FUNCTION_GET_WEAK_DEF( R( Params... ) );
	DAW_DETAIL_SHARED_FUNCTION_GET_WEAK_DEF( R( Params... ) const );
	DAW_DETAIL_SHARED_FUNCTION_GET_WEAK_DEF( R( Params... ) & );
	DAW_DETAIL_SHARED_FUNCTION_GET_WEAK_DEF( R( Params... ) const & );
	DAW_DETAIL_SHARED_FUNCTION_GET_WEAK_DEF( R( Params... ) && );
	DAW_DETAIL_SHARED_FUNCTION_GET_WEAK_DEF( R( Params... ) const && );
	DAW_DETAIL_SHARED_FUNCTION_GET_WEAK_DEF( R( Params... ) noexcept );
	DAW_DETAIL_SHARED_FUNCTION_GET_WEAK_DEF( R( Params... ) const noexcept );
	DAW_DETAIL_SHARED_FUNCTION_GET_WEAK_DEF( R( Params... ) & noexcept );
	DAW_DETAIL_SHARED_FUNCTION_GET_WEAK_DEF( R( Params... ) const & noexcept );
	DAW_DETAIL_SHARED_FUNCTION_GET_WEAK_DEF( R( Params... ) && noexcept );
	DAW_DETAIL_SHARED_FUNCTION_GET_WEAK_DEF( R( Params... ) const && noexcept );

#undef DAW_DETAIL_SHARED_FUNCTION_GET_WEAK_DEF

#define DAW_DETAIL_WEAK_SHARED_FUNCTION_SPEC( Signature, CVRef, IsNoExcept ) \
	template<typename R, typename... Params>                                   \
	class weak_shared_function<Signature>                                      \
	  : sf_impl::weak_shared_function_base<CVRef, IsNoExcept, R, Params...> {  \
		template<typename>                                                       \
		friend struct shared_function;                                           \
                                                                             \
		using base_t =                                                           \
		  sf_impl::weak_shared_function_base<CVRef, IsNoExcept, R, Params...>;   \
                                                                             \
		explicit DAW_CPP26_CX_ATOMIC                                             \
		weak_shared_function( shared_function<Signature> const &s )              \
		  : base_t( s.m_storage ) {}                                             \
                                                                             \
	public:                                                                    \
		[[nodiscard]] DAW_CPP26_CX_ATOMIC shared_function<Signature> lock( ) {   \
			return shared_function<Signature>( this->m_storage );                  \
		}                                                                        \
	}

	DAW_DETAIL_WEAK_SHARED_FUNCTION_SPEC( R( Params... ), sf_impl::cvref_t::None,
	                                      false );
	DAW_DETAIL_WEAK_SHARED_FUNCTION_SPEC( R( Params... ) const,
	                                      sf_impl::cvref_t::Const, false );
	DAW_DETAIL_WEAK_SHARED_FUNCTION_SPEC( R( Params... ) &, sf_impl::cvref_t::Ref,
	                                      false );
	DAW_DETAIL_WEAK_SHARED_FUNCTION_SPEC( R( Params... ) const &,
	                                      sf_impl::cvref_t::ConstRef, false );
	DAW_DETAIL_WEAK_SHARED_FUNCTION_SPEC( R( Params... ) &&,
	                                      sf_impl::cvref_t::RefRef, false );
	DAW_DETAIL_WEAK_SHARED_FUNCTION_SPEC( R( Params... ) const &&,
	                                      sf_impl::cvref_t::ConstRefRef, false );
	DAW_DETAIL_WEAK_SHARED_FUNCTION_SPEC( R( Params... ) noexcept,
	                                      sf_impl::cvref_t::None, true );
	DAW_DETAIL_WEAK_SHARED_FUNCTION_SPEC( R( Params... ) const noexcept,
	                                      sf_impl::cvref_t::Const, true );
	DAW_DETAIL_WEAK_SHARED_FUNCTION_SPEC( R( Params... ) & noexcept,
	                                      sf_impl::cvref_t::Ref, true );
	DAW_DETAIL_WEAK_SHARED_FUNCTION_SPEC( R( Params... ) const & noexcept,
	                                      sf_impl::cvref_t::ConstRef, true );
	DAW_DETAIL_WEAK_SHARED_FUNCTION_SPEC( R( Params... ) && noexcept,
	                                      sf_impl::cvref_t::RefRef, true );
	DAW_DETAIL_WEAK_SHARED_FUNCTION_SPEC( R( Params... ) const && noexcept,
	                                      sf_impl::cvref_t::ConstRefRef, true );

#undef DAW_DETAIL_WEAK_SHARED_FUNCTION_SPEC
} // namespace daw
