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

	struct shared_function_storage_counts {
		shared_function_storage_counts( ) = default;
		virtual ~shared_function_storage_counts( ) = default;

		shared_function_storage_counts( shared_function_storage_counts const & ) =
		  delete;
		shared_function_storage_counts( shared_function_storage_counts && ) =
		  delete;
		shared_function_storage_counts &
		operator=( shared_function_storage_counts const & ) = delete;
		shared_function_storage_counts &
		operator=( shared_function_storage_counts && ) = delete;

		std::atomic_size_t m_weak_count = 1;
		std::atomic_size_t m_strong_count = 1;

		[[nodiscard]] auto *strong_copy( this auto &self ) {
			++self.m_strong_count;
			++self.m_weak_count;
			return &self;
		}

		[[nodiscard]] auto *weak_copy( this auto &self ) {
			++self.m_weak_count;
			return &self;
		}

		void strong_dec( ) {
			--m_strong_count;
		}

		[[nodiscard]] bool weak_dec( ) {
			return --m_weak_count == 0;
		}
	};

	template<cvref_t CVRef, bool IsNoExcept, typename R, typename... Params>
	struct shared_function_storage_base;

	template<typename T>
	using param_t = std::conditional_t<std::is_scalar_v<T>, T, T &&>;

	template<bool IsNoExcept, typename R, typename... Params>
	struct shared_function_storage_base<cvref_t::None, IsNoExcept, R, Params...>
	  : shared_function_storage_counts {
		shared_function_storage_base( ) = default;

		virtual R call( Params... ) noexcept( IsNoExcept ) = 0;
		~shared_function_storage_base( ) override = default;
	};

	template<bool IsNoExcept, typename R, typename... Params>
	struct shared_function_storage_base<cvref_t::Const, IsNoExcept, R, Params...>
	  : shared_function_storage_counts {
		shared_function_storage_base( ) = default;

		virtual R call( Params... ) const noexcept( IsNoExcept ) = 0;
		~shared_function_storage_base( ) override = default;
	};

	template<bool IsNoExcept, typename R, typename... Params>
	struct shared_function_storage_base<cvref_t::Ref, IsNoExcept, R, Params...>
	  : shared_function_storage_counts {
		shared_function_storage_base( ) = default;

		virtual R call( Params... ) & noexcept( IsNoExcept ) = 0;
		~shared_function_storage_base( ) override = default;
	};

	template<bool IsNoExcept, typename R, typename... Params>
	struct shared_function_storage_base<cvref_t::ConstRef, IsNoExcept, R,
	                                    Params...>
	  : shared_function_storage_counts {
		shared_function_storage_base( ) = default;

		virtual R call( Params... ) const & noexcept( IsNoExcept ) = 0;
		~shared_function_storage_base( ) override = default;
	};

	template<bool IsNoExcept, typename R, typename... Params>
	struct shared_function_storage_base<cvref_t::RefRef, IsNoExcept, R, Params...>
	  : shared_function_storage_counts {
		shared_function_storage_base( ) = default;

		virtual R call( Params... ) && noexcept( IsNoExcept ) = 0;
		~shared_function_storage_base( ) override = default;
	};

	template<bool IsNoExcept, typename R, typename... Params>
	struct shared_function_storage_base<cvref_t::ConstRefRef, IsNoExcept, R,
	                                    Params...>
	  : shared_function_storage_counts {
		shared_function_storage_base( ) = default;

		virtual R call( Params... ) const && noexcept( IsNoExcept ) = 0;
		~shared_function_storage_base( ) override = default;
	};

	template<typename Fn, cvref_t CVRef, bool IsNoExcept, typename R,
	         typename... Params>
	struct shared_function_storage;

	template<typename Fn, bool IsNoExcept, typename R, typename... Params>
	struct shared_function_storage<Fn, cvref_t::None, IsNoExcept, R, Params...>
	  : private Fn,
	    shared_function_storage_base<cvref_t::None, IsNoExcept, R, Params...> {

		shared_function_storage( Fn f )
		  : Fn{ DAW_FWD( f ) } {}

		R call( Params... params ) noexcept( IsNoExcept ) override {
			auto &fn_self = *static_cast<Fn *>( this );
			return fn_self( std::forward<param_t<Params>>( params )... );
		}
	};

	template<typename Fn, bool IsNoExcept, typename R, typename... Params>
	struct shared_function_storage<Fn, cvref_t::Const, IsNoExcept, R, Params...>
	  : private Fn,
	    shared_function_storage_base<cvref_t::Const, IsNoExcept, R, Params...> {
		shared_function_storage( Fn fn )
		  : Fn{ DAW_FWD( fn ) } {}

		R call( Params... params ) const noexcept( IsNoExcept ) override {
			auto &fn_self = *static_cast<Fn const *>( this );
			return fn_self( std::forward<param_t<Params>>( params )... );
		}
	};

	template<typename Fn, bool IsNoExcept, typename R, typename... Params>
	struct shared_function_storage<Fn, cvref_t::Ref, IsNoExcept, R, Params...>
	  : private Fn,
	    shared_function_storage_base<cvref_t::Ref, IsNoExcept, R, Params...> {
		shared_function_storage( Fn fn )
		  : Fn{ DAW_FWD( fn ) } {}

		R call( Params... params ) & noexcept( IsNoExcept ) override {
			auto &fn_self = *static_cast<Fn *>( this );
			return fn_self( std::forward<param_t<Params>>( params )... );
		}
	};

	template<typename Fn, bool IsNoExcept, typename R, typename... Params>
	struct shared_function_storage<Fn, cvref_t::ConstRef, IsNoExcept, R,
	                               Params...>
	  : private Fn,
	    shared_function_storage_base<cvref_t::ConstRef, IsNoExcept, R,
	                                 Params...> {
		shared_function_storage( Fn fn )
		  : Fn{ DAW_FWD( fn ) } {}

		R call( Params... params ) const & noexcept( IsNoExcept ) override {
			auto &fn_self = *static_cast<Fn const *>( this );
			return fn_self( std::forward<param_t<Params>>( params )... );
		}
	};

	template<typename Fn, bool IsNoExcept, typename R, typename... Params>
	struct shared_function_storage<Fn, cvref_t::RefRef, IsNoExcept, R, Params...>
	  : private Fn,
	    shared_function_storage_base<cvref_t::RefRef, IsNoExcept, R, Params...> {
		shared_function_storage( Fn fn )
		  : Fn{ DAW_FWD( fn ) } {}

		R call( Params... params ) && noexcept( IsNoExcept ) override {
			auto &fn_self = *static_cast<Fn *>( this );
			return std::move( fn_self )( std::forward<param_t<Params>>( params )... );
		}
	};

	template<typename Fn, bool IsNoExcept, typename R, typename... Params>
	struct shared_function_storage<Fn, cvref_t::ConstRefRef, IsNoExcept, R,
	                               Params...>
	  : private Fn,
	    shared_function_storage_base<cvref_t::ConstRefRef, IsNoExcept, R,
	                                 Params...> {
		shared_function_storage( Fn fn )
		  : Fn{ DAW_FWD( fn ) } {}

		R call( Params... params ) const && noexcept( IsNoExcept ) override {
			auto &fn_self = *static_cast<Fn const *>( this );
			return std::move( fn_self )( std::forward<param_t<Params>>( params )... );
		}
	};

	template<cvref_t CVRef, bool IsNoExcept, typename R, typename... Params>
	void weak_release(
	  shared_function_storage_base<CVRef, IsNoExcept, R, Params...> *p ) {
		if( not p ) {
			return;
		}
		if( p->weak_dec( ) ) {
			delete p;
		}
	}

	template<cvref_t CVRef, bool IsNoExcept, typename R, typename... Params>
	void strong_release(
	  shared_function_storage_base<CVRef, IsNoExcept, R, Params...> *p ) {
		if( not p ) {
			return;
		}
		p->strong_dec( );
		weak_release( p );
	}

	template<typename Fn>
	struct func_wrapper_t {
		Fn fn;

		template<typename... Params>
		constexpr decltype( auto ) operator( )( Params &&...params ) const
		  noexcept( std::is_nothrow_invocable_v<Fn, Params...> ) {
			return fn( DAW_FWD( params )... );
		}
	};

	template<cvref_t CVRef, bool IsNoExcept, typename R, typename... Params,
	         typename Fn>
	requires( std::is_class_v<std::remove_cvref_t<Fn>> ) //
	  constexpr auto make_fn_storage( Fn &&fn ) {
		return new shared_function_storage<std::remove_cvref_t<Fn>,
		                                   CVRef,
		                                   IsNoExcept,
		                                   R,
		                                   Params...>{ DAW_FWD( fn ) };
	}

	template<cvref_t CVRef, bool IsNoExcept, typename R, typename... Params,
	         typename Fn>
	requires( not std::is_class_v<std::remove_cvref_t<Fn>> ) //
	  constexpr auto make_fn_storage( Fn fn ) {
		using func_t = func_wrapper_t<Fn>;
		return new shared_function_storage<func_t, CVRef, IsNoExcept, R, Params...>{
		  func_t( fn ) };
	}

	template<cvref_t CVRef, bool IsNoExcept, typename R, typename... Params>
	class weak_shared_function_base {
	protected:
		using storage_t =
		  shared_function_storage_base<CVRef, IsNoExcept, R, Params...>;

		storage_t *m_storage = nullptr;

	public:
		weak_shared_function_base( storage_t *storage )
		  : m_storage( storage ? storage->weak_copy( ) : nullptr ) {}

		weak_shared_function_base( weak_shared_function_base const &other )
		  : m_storage( other.m_storage ? other.m_storage->weak_copy( ) : nullptr ) {
		}

		weak_shared_function_base( weak_shared_function_base &&other ) noexcept
		  : m_storage( std::exchange( other.m_storage, nullptr ) ) {}

		weak_shared_function_base &
		operator=( weak_shared_function_base const &rhs ) {
			if( this != &rhs ) {
				storage_t *old = m_storage;
				m_storage = rhs.m_storage ? rhs.m_storage->weak_copy( ) : nullptr;
				weak_release( old );
			}
			return *this;
		}

		weak_shared_function_base &
		operator=( weak_shared_function_base &&rhs ) noexcept {
			if( this != &rhs ) {
				storage_t *old = m_storage;
				m_storage = std::exchange( rhs.m_storage, nullptr );
				weak_release( old );
			}
			return *this;
		}

		~weak_shared_function_base( ) {
			weak_release( std::exchange( m_storage, nullptr ) );
		}
	};

	template<cvref_t CVRef, bool IsNoExcept, typename R, typename... Params>
	struct shared_function_base {
	protected:
		using storage_t =
		  shared_function_storage_base<CVRef, IsNoExcept, R, Params...>;
		storage_t *m_storage = nullptr;

		shared_function_base( storage_t *storage )
		  : m_storage( storage ? storage->strong_copy( ) : nullptr ) {}

	public:
		template<typename Fn>
		requires( not std::is_same_v<shared_function_base,
		                             std::remove_cvref_t<Fn>> ) //
		  explicit shared_function_base( Fn &&fn )
		  : m_storage(
		      make_fn_storage<CVRef, IsNoExcept, R, Params...>( DAW_FWD( fn ) ) ) {}

		shared_function_base( ) = default;

		~shared_function_base( ) {
			storage_t *tmp = std::exchange( m_storage, nullptr );
			strong_release( tmp );
		}

		shared_function_base( shared_function_base const &other )
		  : m_storage( other.m_storage ? other.m_storage->strong_copy( )
		                               : nullptr ) {}

		shared_function_base( shared_function_base &&other ) noexcept
		  : m_storage( std::exchange( other.m_storage, nullptr ) ) {}

		shared_function_base &operator=( shared_function_base const &rhs ) {
			if( this != &rhs ) {
				auto *old = m_storage;
				m_storage = rhs.m_storage ? rhs.m_storage->strong_copy( ) : nullptr;
				strong_release( old );
			}
			return *this;
		}

		shared_function_base &operator=( shared_function_base &&rhs ) noexcept {
			if( this != &rhs ) {
				auto *old = m_storage;
				m_storage = std::exchange( rhs.m_storage, nullptr );
				strong_release( old );
			}
			return *this;
		}

		template<typename Self>
		R operator( )( this Self &&self, Params... params ) noexcept( IsNoExcept ) {
			using type = std::conditional_t<std::is_reference_v<Self>,
			                                copy_cvref_t<Self, storage_t>,
			                                copy_cvref_t<Self, storage_t> &&>;
			return static_cast<type>( *self.m_storage )
			  .call( std::forward<param_t<Params>>( params )... );
		}

		explicit operator bool( ) const {
			return m_storage != nullptr;
		}
	};
} // namespace daw::sf_impl
namespace daw {
	template<typename>
	struct shared_function;

	template<typename>
	class weak_shared_function;

	template<typename R, typename... Params>
	struct shared_function<R( Params... )>
	  : sf_impl::shared_function_base<sf_impl::cvref_t::None, false, R,
	                                  Params...> {
		using sf_impl::shared_function_base<sf_impl::cvref_t::None, false, R,
		                                    Params...>::shared_function_base;

		template<typename>
		friend class weak_shared_function;

		weak_shared_function<R( Params... )> get_weak( );
	};

	template<typename R, typename... Params>
	struct shared_function<R( Params... ) const>
	  : sf_impl::shared_function_base<sf_impl::cvref_t::Const, false, R,
	                                  Params...> {
		using sf_impl::shared_function_base<sf_impl::cvref_t::Const, false, R,
		                                    Params...>::shared_function_base;
		template<typename>
		friend class weak_shared_function;

		weak_shared_function<R( Params... ) const> get_weak( );
	};

	template<typename R, typename... Params>
	struct shared_function<R( Params... ) &>
	  : sf_impl::shared_function_base<sf_impl::cvref_t::Ref, false, R,
	                                  Params...> {

		using sf_impl::shared_function_base<sf_impl::cvref_t::Ref, false, R,
		                                    Params...>::shared_function_base;
		template<typename>
		friend class weak_shared_function;

		weak_shared_function<R( Params... ) &> get_weak( );
	};

	template<typename R, typename... Params>
	struct shared_function<R( Params... ) const &>
	  : sf_impl::shared_function_base<sf_impl::cvref_t::ConstRef, false, R,
	                                  Params...> {

		using sf_impl::shared_function_base<sf_impl::cvref_t::ConstRef, false, R,
		                                    Params...>::shared_function_base;
		template<typename>
		friend class weak_shared_function;

		weak_shared_function<R( Params... ) const &> get_weak( );
	};

	template<typename R, typename... Params>
	struct shared_function<R( Params... ) &&>
	  : sf_impl::shared_function_base<sf_impl::cvref_t::RefRef, false, R,
	                                  Params...> {
		using sf_impl::shared_function_base<sf_impl::cvref_t::RefRef, false, R,
		                                    Params...>::shared_function_base;
		template<typename>
		friend class weak_shared_function;

		weak_shared_function<R( Params... ) &&> get_weak( );
	};

	template<typename R, typename... Params>
	struct shared_function<R( Params... ) const &&>
	  : sf_impl::shared_function_base<sf_impl::cvref_t::ConstRefRef, false, R,
	                                  Params...> {
		using sf_impl::shared_function_base<sf_impl::cvref_t::ConstRefRef, false, R,
		                                    Params...>::shared_function_base;
		template<typename>
		friend class weak_shared_function;

		weak_shared_function<R( Params... ) const &&> get_weak( );
	};

	template<typename R, typename... Params>
	struct shared_function<R( Params... ) noexcept>
	  : sf_impl::shared_function_base<sf_impl::cvref_t::None, true, R,
	                                  Params...> {
		using sf_impl::shared_function_base<sf_impl::cvref_t::None, true, R,
		                                    Params...>::shared_function_base;
		template<typename>
		friend class weak_shared_function;

		weak_shared_function<R( Params... ) noexcept> get_weak( );
	};

	template<typename R, typename... Params>
	struct shared_function<R( Params... ) const noexcept>
	  : sf_impl::shared_function_base<sf_impl::cvref_t::Const, true, R,
	                                  Params...> {
		using sf_impl::shared_function_base<sf_impl::cvref_t::Const, true, R,
		                                    Params...>::shared_function_base;
		template<typename>
		friend class weak_shared_function;

		weak_shared_function<R( Params... ) const noexcept> get_weak( );
	};

	template<typename R, typename... Params>
	struct shared_function<R( Params... ) & noexcept>
	  : sf_impl::shared_function_base<sf_impl::cvref_t::Ref, true, R, Params...> {

		using sf_impl::shared_function_base<sf_impl::cvref_t::Ref, true, R,
		                                    Params...>::shared_function_base;
		template<typename>
		friend class weak_shared_function;

		weak_shared_function<R( Params... ) & noexcept> get_weak( );
	};

	template<typename R, typename... Params>
	struct shared_function<R( Params... ) const & noexcept>
	  : sf_impl::shared_function_base<sf_impl::cvref_t::ConstRef, true, R,
	                                  Params...> {
		using sf_impl::shared_function_base<sf_impl::cvref_t::ConstRef, true, R,
		                                    Params...>::shared_function_base;
		template<typename>
		friend class weak_shared_function;

		weak_shared_function<R( Params... ) const & noexcept> get_weak( );
	};

	template<typename R, typename... Params>
	struct shared_function<R( Params... ) && noexcept>
	  : sf_impl::shared_function_base<sf_impl::cvref_t::RefRef, true, R,
	                                  Params...> {
		using sf_impl::shared_function_base<sf_impl::cvref_t::RefRef, true, R,
		                                    Params...>::shared_function_base;
		template<typename>
		friend class weak_shared_function;

		weak_shared_function<R( Params... ) && noexcept> get_weak( );
	};

	template<typename R, typename... Params>
	struct shared_function<R( Params... ) const && noexcept>
	  : sf_impl::shared_function_base<sf_impl::cvref_t::ConstRefRef, true, R,
	                                  Params...> {
		using sf_impl::shared_function_base<sf_impl::cvref_t::ConstRefRef, true, R,
		                                    Params...>::shared_function_base;
		template<typename>
		friend class weak_shared_function;

		weak_shared_function<R( Params... ) const && noexcept> get_weak( );
	};

	template<typename R, typename... Params>
	weak_shared_function<R( Params... )>
	shared_function<R( Params... )>::get_weak( ) {
		return weak_shared_function<R( Params... )>( this->m_storage );
	}

	template<typename R, typename... Params>
	weak_shared_function<R( Params... ) const>
	shared_function<R( Params... ) const>::get_weak( ) {
		return weak_shared_function<R( Params... ) const>( this->m_storage );
	}

	template<typename R, typename... Params>
	weak_shared_function<R( Params... ) &>
	shared_function<R( Params... ) &>::get_weak( ) {
		return weak_shared_function<R( Params... ) &>( this->m_storage );
	}

	template<typename R, typename... Params>
	weak_shared_function<R( Params... ) const &>
	shared_function<R( Params... ) const &>::get_weak( ) {
		return weak_shared_function<R( Params... ) const &>( this->m_storage );
	}

	template<typename R, typename... Params>
	weak_shared_function<R( Params... ) &&>
	shared_function<R( Params... ) &&>::get_weak( ) {
		return weak_shared_function<R( Params... ) &&>( this->m_storage );
	}

	template<typename R, typename... Params>
	weak_shared_function<R( Params... ) const &&>
	shared_function<R( Params... ) const &&>::get_weak( ) {
		return weak_shared_function<R( Params... ) const &&>( this->m_storage );
	}

	template<typename R, typename... Params>
	weak_shared_function<R( Params... ) noexcept>
	shared_function<R( Params... ) noexcept>::get_weak( ) {
		return weak_shared_function<R( Params... ) noexcept>( this->m_storage );
	}

	template<typename R, typename... Params>
	weak_shared_function<R( Params... ) const noexcept>
	shared_function<R( Params... ) const noexcept>::get_weak( ) {
		return weak_shared_function<R( Params... ) const noexcept>(
		  this->m_storage );
	}

	template<typename R, typename... Params>
	weak_shared_function<R( Params... ) & noexcept>
	shared_function<R( Params... ) & noexcept>::get_weak( ) {
		return weak_shared_function<R( Params... ) & noexcept>( this->m_storage );
	}

	template<typename R, typename... Params>
	weak_shared_function<R( Params... ) const & noexcept>
	shared_function<R( Params... ) const & noexcept>::get_weak( ) {
		return weak_shared_function<R( Params... ) const & noexcept>(
		  this->m_storage );
	}

	template<typename R, typename... Params>
	weak_shared_function<R( Params... ) && noexcept>
	shared_function<R( Params... ) && noexcept>::get_weak( ) {
		return weak_shared_function < R( Params... ) &&
		       noexcept > ( this->m_storage );
	}

	template<typename R, typename... Params>
	weak_shared_function<R( Params... ) const && noexcept>
	shared_function<R( Params... ) const && noexcept>::get_weak( ) {
		return weak_shared_function < R( Params... ) const &&
		       noexcept > ( this->m_storage );
	}

	template<typename R, typename... Params>
	class weak_shared_function<R( Params... )>
	  : sf_impl::weak_shared_function_base<sf_impl::cvref_t::None, false, R,
	                                       Params...> {
		template<typename>
		friend struct shared_function;

		using base_t = sf_impl::weak_shared_function_base<sf_impl::cvref_t::None,
		                                                  false, R, Params...>;

		weak_shared_function( shared_function<R( Params... )> const &s )
		  : base_t( s.m_storage ) {}

	public:
		shared_function<R( Params... )> lock( ) {
			return shared_function<R( Params... )>( this->m_storage );
		}
	};

	template<typename R, typename... Params>
	class weak_shared_function<R( Params... ) const>
	  : sf_impl::weak_shared_function_base<sf_impl::cvref_t::Const, false, R,
	                                       Params...> {
		template<typename>
		friend struct shared_function;

		using base_t = sf_impl::weak_shared_function_base<sf_impl::cvref_t::Const,
		                                                  false, R, Params...>;

		weak_shared_function( shared_function<R( Params... ) const> const &s )
		  : base_t( s.m_storage ) {}

	public:
		shared_function<R( Params... ) const> lock( ) {
			return shared_function<R( Params... ) const>( this->m_storage );
		}
	};

	template<typename R, typename... Params>
	class weak_shared_function<R( Params... ) &>
	  : sf_impl::weak_shared_function_base<sf_impl::cvref_t::Ref, false, R,
	                                       Params...> {
		template<typename>
		friend struct shared_function;

		using base_t = sf_impl::weak_shared_function_base<sf_impl::cvref_t::Ref,
		                                                  false, R, Params...>;

		weak_shared_function( shared_function<R( Params... ) &> const &s )
		  : base_t( s.m_storage ) {}

	public:
		shared_function<R( Params... ) &> lock( ) {
			return shared_function<R( Params... ) &>( this->m_storage );
		}
	};

	template<typename R, typename... Params>
	class weak_shared_function<R( Params... ) const &>
	  : sf_impl::weak_shared_function_base<sf_impl::cvref_t::ConstRef, false, R,
	                                       Params...> {
		template<typename>
		friend struct shared_function;

		using base_t =
		  sf_impl::weak_shared_function_base<sf_impl::cvref_t::ConstRef, false, R,
		                                     Params...>;

		weak_shared_function( shared_function<R( Params... ) const &> const &s )
		  : base_t( s.m_storage ) {}

	public:
		shared_function<R( Params... ) const &> lock( ) {
			return shared_function<R( Params... ) const &>( this->m_storage );
		}
	};

	template<typename R, typename... Params>
	class weak_shared_function<R( Params... ) &&>
	  : sf_impl::weak_shared_function_base<sf_impl::cvref_t::RefRef, false, R,
	                                       Params...> {
		template<typename>
		friend struct shared_function;

		using base_t = sf_impl::weak_shared_function_base<sf_impl::cvref_t::RefRef,
		                                                  false, R, Params...>;

		weak_shared_function( shared_function<R( Params... ) &&> const &s )
		  : base_t( s.m_storage ) {}

	public:
		shared_function<R( Params... ) &&> lock( ) {
			return shared_function<R( Params... ) &&>( this->m_storage );
		}
	};

	template<typename R, typename... Params>
	class weak_shared_function<R( Params... ) const &&>
	  : sf_impl::weak_shared_function_base<sf_impl::cvref_t::ConstRefRef, false,
	                                       R, Params...> {
		template<typename>
		friend struct shared_function;

		using base_t =
		  sf_impl::weak_shared_function_base<sf_impl::cvref_t::ConstRefRef, false,
		                                     R, Params...>;

		weak_shared_function( shared_function<R( Params... ) const &&> const &s )
		  : base_t( s.m_storage ) {}

	public:
		shared_function<R( Params... ) const &&> lock( ) {
			return shared_function<R( Params... ) const &&>( this->m_storage );
		}
	};

	template<typename R, typename... Params>
	class weak_shared_function<R( Params... ) noexcept>
	  : sf_impl::weak_shared_function_base<sf_impl::cvref_t::None, true, R,
	                                       Params...> {
		template<typename>
		friend struct shared_function;

		using base_t = sf_impl::weak_shared_function_base<sf_impl::cvref_t::None,
		                                                  true, R, Params...>;

		weak_shared_function( shared_function<R( Params... ) noexcept> const &s )
		  : base_t( s.m_storage ) {}

	public:
		shared_function<R( Params... ) noexcept> lock( ) {
			return shared_function<R( Params... ) noexcept>( this->m_storage );
		}
	};

	template<typename R, typename... Params>
	class weak_shared_function<R( Params... ) const noexcept>
	  : sf_impl::weak_shared_function_base<sf_impl::cvref_t::Const, true, R,
	                                       Params...> {
		template<typename>
		friend struct shared_function;

		using base_t = sf_impl::weak_shared_function_base<sf_impl::cvref_t::Const,
		                                                  true, R, Params...>;

		weak_shared_function(
		  shared_function<R( Params... ) const noexcept> const &s )
		  : base_t( s.m_storage ) {}

	public:
		shared_function<R( Params... ) const noexcept> lock( ) {
			return shared_function<R( Params... ) const noexcept>( this->m_storage );
		}
	};

	template<typename R, typename... Params>
	class weak_shared_function<R( Params... ) & noexcept>
	  : sf_impl::weak_shared_function_base<sf_impl::cvref_t::Ref, true, R,
	                                       Params...> {
		template<typename>
		friend struct shared_function;

		using base_t = sf_impl::weak_shared_function_base<sf_impl::cvref_t::Ref,
		                                                  true, R, Params...>;

		weak_shared_function( shared_function<R( Params... ) & noexcept> const &s )
		  : base_t( s.m_storage ) {}

	public:
		shared_function<R( Params... ) & noexcept> lock( ) {
			return shared_function<R( Params... ) & noexcept>( this->m_storage );
		}
	};

	template<typename R, typename... Params>
	class weak_shared_function<R( Params... ) const & noexcept>
	  : sf_impl::weak_shared_function_base<sf_impl::cvref_t::ConstRef, true, R,
	                                       Params...> {
		template<typename>
		friend struct shared_function;

		using base_t =
		  sf_impl::weak_shared_function_base<sf_impl::cvref_t::ConstRef, true, R,
		                                     Params...>;

		weak_shared_function(
		  shared_function<R( Params... ) const & noexcept> const &s )
		  : base_t( s.m_storage ) {}

	public:
		shared_function<R( Params... ) const & noexcept> lock( ) {
			return shared_function<R( Params... ) const & noexcept>(
			  this->m_storage );
		}
	};

	template<typename R, typename... Params>
	class weak_shared_function<R( Params... ) && noexcept>
	  : sf_impl::weak_shared_function_base<sf_impl::cvref_t::RefRef, true, R,
	                                       Params...> {
		template<typename>
		friend struct shared_function;

		using base_t = sf_impl::weak_shared_function_base<sf_impl::cvref_t::RefRef,
		                                                  true, R, Params...>;

		weak_shared_function( shared_function<R( Params... ) && noexcept> const &s )
		  : base_t( s.m_storage ) {}

	public:
		shared_function<R( Params... ) && noexcept> lock( ) {
			return shared_function < R( Params... ) && noexcept > ( this->m_storage );
		}
	};

	template<typename R, typename... Params>
	class weak_shared_function<R( Params... ) const && noexcept>
	  : sf_impl::weak_shared_function_base<sf_impl::cvref_t::ConstRefRef, true, R,
	                                       Params...> {
		template<typename>
		friend struct shared_function;

		using base_t =
		  sf_impl::weak_shared_function_base<sf_impl::cvref_t::ConstRefRef, true, R,
		                                     Params...>;

		weak_shared_function(
		  shared_function<R( Params... ) const && noexcept> const &s )
		  : base_t( s.m_storage ) {}

	public:
		shared_function<R( Params... ) const && noexcept> lock( ) {
			return shared_function < R( Params... ) const &&
			       noexcept > ( this->m_storage );
		}
	};
} // namespace daw
