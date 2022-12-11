// Originally from the LLVM Project, under the Apache v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// Since modified by Darrell Wright
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_concepts.h"
#include "daw_move.h"
#include "daw_traits.h"

#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace daw {
	struct default_init_tag {};
	struct value_init_tag {};

	template<typename Tp, int Idx,
	         bool /*Can Be Empty Base*/ =
	           std::is_empty_v<Tp> and not std::is_final_v<Tp>>
	struct compressed_pair_elem {
		using value_type = Tp;
		using reference = Tp &;
		using const_reference = Tp const &;

	private:
		value_type m_value;

	public:
		explicit constexpr compressed_pair_elem( default_init_tag ) {}

		explicit constexpr compressed_pair_elem( value_init_tag )
		  : m_value( ) {}

		explicit constexpr compressed_pair_elem(
		  not_decay_of<compressed_pair_elem> auto &&u )
		  : m_value( DAW_FWD( u ) ) {}

		template<class... Args, size_t... Indexes>
		explicit constexpr compressed_pair_elem( std::piecewise_construct_t,
		                                         std::tuple<Args...> args,
		                                         std::index_sequence<Indexes...> )
		  : m_value( DAW_FWD2( Args, std::get<Indexes>( args ) )... ) {}

		constexpr reference get( ) noexcept {
			return m_value;
		}

		constexpr const_reference get( ) const noexcept {
			return m_value;
		}
	};

	template<class Tp, int Idx>
	struct compressed_pair_elem<Tp, Idx, true> : private Tp {
		using value_type = Tp;
		using reference = Tp &;
		using const_reference = Tp const &;

		explicit compressed_pair_elem( ) = default;

		explicit constexpr compressed_pair_elem( default_init_tag ) {}

		explicit constexpr compressed_pair_elem( value_init_tag )
		  : value_type( ) {}

		explicit constexpr compressed_pair_elem(
		  not_decay_of<compressed_pair_elem> auto &&u )
		  : value_type( DAW_FWD( u ) ) {}

		template<class... Args, size_t... Indexes>
		explicit constexpr compressed_pair_elem( std::piecewise_construct_t,
		                                         std::tuple<Args...> args,
		                                         std::index_sequence<Indexes...> )
		  : value_type( DAW_FWD2( Args, std::get<Indexes>( args ) )... ) {}

		constexpr reference get( ) noexcept {
			return *this;
		}

		constexpr const_reference get( ) const noexcept {
			return *this;
		}
	};

	template<typename T1, typename T2>
	struct compressed_pair : private compressed_pair_elem<T1, 0>,
	                         private compressed_pair_elem<T2, 1> {
		// NOTE: This static assert should never fire because compressed_pair
		// is *almost never* used in a scenario where it's possible for T1 == T2.
		// (The exception is std::function where it is possible that the function
		//  object and the allocator have the same type).
		static_assert(
		  not same_as<T1, T2>,
		  "compressed_pair cannot be instantiated when T1 and T2 are the same "
		  "type; "
		  "The current implementation is NOT ABI-compatible with the previous "
		  "implementation for this configuration" );

		using Base1 = compressed_pair_elem<T1, 0>;
		using Base2 = compressed_pair_elem<T2, 1>;

		template<bool D = true>
		  requires( std::is_default_constructible_v<T1> and
		            std::is_default_constructible_v<T2> ) //
		explicit constexpr compressed_pair( )
		  : Base1( value_init_tag( ) )
		  , Base2( value_init_tag( ) ) {}

		template<typename U1, typename U2>
		explicit constexpr compressed_pair( U1 &&t1, U2 &&t2 )
		  : Base1( DAW_FWD2( U1, t1 ) )
		  , Base2( DAW_FWD2( U2, t2 ) ) {}

		template<class... Args1, class... Args2>
		explicit constexpr compressed_pair( std::piecewise_construct_t pc,
		                                    std::tuple<Args1...> first_args,
		                                    std::tuple<Args2...> second_args )
		  : Base1( pc, DAW_MOVE( first_args ),
		           std::make_index_sequence<sizeof...( Args1 )>{ } )
		  , Base2( pc, DAW_MOVE( second_args ),
		           std::make_index_sequence<sizeof...( Args2 )>{ } ) {}

		[[nodiscard]] constexpr typename Base1::reference first( ) noexcept {
			return static_cast<Base1 &>( *this ).get( );
		}

		[[nodiscard]] constexpr typename Base1::const_reference
		first( ) const noexcept {
			return static_cast<Base1 const &>( *this ).get( );
		}

		[[nodiscard]] constexpr typename Base2::reference second( ) noexcept {
			return static_cast<Base2 &>( *this ).get( );
		}

		[[nodiscard]] constexpr typename Base2::const_reference
		second( ) const noexcept {
			return static_cast<Base2 const &>( *this ).get( );
		}

		[[nodiscard]] static constexpr Base1 *
		get_first_base( compressed_pair *pair ) noexcept {
			return static_cast<Base1 *>( pair );
		}

		[[nodiscard]] static constexpr Base2 *
		get_second_base( compressed_pair *pair ) noexcept {
			return static_cast<Base2 *>( pair );
		}

		constexpr void swap( compressed_pair &x ) noexcept(
		  std::is_nothrow_swappable_v<T1> and std::is_nothrow_swappable_v<T2> ) {
			using std::swap;
			swap( first( ), x.first( ) );
			swap( second( ), x.second( ) );
		}
	};

	template<class T1, class T2>
	constexpr void
	swap( compressed_pair<T1, T2> &x, compressed_pair<T1, T2> &y ) noexcept(
	  std::is_nothrow_swappable_v<T1> and std::is_nothrow_swappable_v<T2> ) {
		x.swap( y );
	}
} // namespace daw