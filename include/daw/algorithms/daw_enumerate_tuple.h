// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_enumerate_tuple.h"
#include "daw/cpp_17.h"
#include "daw/daw_move.h"
#include "daw/daw_constant.h"

#include <cstddef>
#include <utility>

namespace daw {
	template<typename Tp, typename Index = std::size_t>
	struct enumerated_tuple_t {
		using i_am_an_enumerated_tuple = void;
		using tuple_t = Tp;
		using index_t = Index;
		Tp m_tuple;
	};

	template<typename T, auto Idx>
	struct enumerate_tuple_element_t {
		using i_am_an_enumerated_tuple_element = void;
		using index_t = decltype( Idx );
		using value_t = T;
		static constexpr index_t index = Idx;

		value_t value;
	};

	template<typename T>
	concept EnumeratedTuple = requires
	{
		typename std::remove_cvref_t<T>::i_am_an_enumerated_tuple;
	};

	template<typename T>
	concept EnumeratedTupleElement = requires
	{
		typename std::remove_cvref_t<T>::i_am_an_enumerated_tuple_element;
	};

	template<EnumeratedTuple T>
	using enumerated_tuple_index_t = typename std::remove_cvref_t<T>::index_t;

	template<EnumeratedTupleElement T>
	using enumerated_tuple_element_index_t = typename std::remove_cvref_t<
		T>::index_t;

	template<EnumeratedTupleElement T>
	using enumerated_tuple_element_value_t = typename std::remove_cvref_t<
		T>::value_t;

	template<typename Index = std::size_t, typename Tp>
	constexpr enumerated_tuple_t<Tp, Index> enumerate_tuple( Tp &&tp ) {
		return {DAW_FWD( tp )};
	}
}

namespace std {
	template<daw::EnumeratedTuple ET>
	struct tuple_size<ET> {
		static constexpr auto value =
			std::tuple_size_v<typename std::remove_cvref_t<ET>::tuple_t>;
	};

	template<daw::EnumeratedTupleElement ETE>
	struct tuple_size<ETE> {
		static constexpr std::size_t value = 2U;
	};

	template<std::size_t I, daw::EnumeratedTuple ET>
	struct tuple_element<I, ET> {
		using index_t = daw::enumerated_tuple_index_t<ET>;
		static constexpr index_t index = static_cast<index_t>(I);
		using type_t = std::tuple_element_t<I, std::remove_cvref_t<ET>>;
		using type = daw::enumerate_tuple_element_t<type_t, index>;
	};

	template<std::size_t Idx, daw::EnumeratedTupleElement ETE>
		requires( Idx == 0 )
	struct tuple_element<Idx, ETE> {
		using type = daw::enumerated_tuple_element_index_t<ETE>;
	};

	template<std::size_t Idx, daw::EnumeratedTupleElement ETE>
		requires( Idx == 1 )
	struct tuple_element<Idx, ETE> {
		using type = daw::enumerated_tuple_element_value_t<ETE>;
	};

	template<std::size_t Idx, daw::EnumeratedTuple EnumeratedTuple>
	constexpr auto get( EnumeratedTuple &&tp ) {
		using index_t = typename std::remove_cvref_t<EnumeratedTuple>::index_t;
		static constexpr index_t index = static_cast<index_t>(Idx);
		using value_t = decltype( std::get<Idx>( DAW_FWD( tp ).m_tuple ) );
		return daw::enumerate_tuple_element_t<value_t, index>{
			std::get<Idx>( DAW_FWD( tp ).m_tuple )};
	}

	template<std::size_t I, daw::EnumeratedTupleElement EnumeratedTupleElement>
	constexpr decltype(auto) get( EnumeratedTupleElement &&element ) {
		if constexpr(I == 0) {
			return element.index;
		} else {
			return DAW_FWD( element ).value;
		}
	}

}