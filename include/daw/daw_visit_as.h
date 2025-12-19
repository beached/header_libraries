// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include <daw/daw_bit_cast.h>
#include <daw/daw_move.h>

#include <functional>


#if defined( DAW_CX_BIT_CAST )
#define DAW_CX_IF_BIT_CAST constexpr
#else
#define DAW_CX_IF_BIT_CAST
#endif

namespace daw {
	/**
	 * @brief Invokes a visitor on a reinterpretation of the provided value, allowing
	 *        modification of the value via the provided visitor, and optionally
	 *        returning a value from the visitor's result.
	 *
	 * @tparam VisitedT The type to reinterpreted `value` as when invoking the visitor.
	 * @tparam OrigT The original type of the `value` being modified or accessed.
	 * @tparam Visitor The invocable type that will be applied to the value.
	 *
	 * @param value A reference to the value being visited and potentially modified.
	 * @param visitor A invocable that processes the value once reinterpreted as
	 *                type `VisitedT`.
	 *
	 * @return The result of invoking the visitor if the visitor returns a non-void
	 *         value; otherwise, nothing is returned.
	 *
	 * @details The function performs a bit cast to reinterpret `value` as type
	 *          `VisitedT`. It then applies the visitor to this reinterpreted value.
	 *          After the visitor processes the value, the function updates the
	 *          original value with the potentially modified value. If the visitor
	 *          returns a result, it is also returned by this function.
	 */
	template<typename VisitedT, typename OrigT, typename Visitor>
DAW_CX_IF_BIT_CAST auto visit_as( OrigT &value, Visitor &&visitor ) {
		auto tmp = daw::bit_cast<VisitedT>( value );
		if constexpr( std::is_void_v<std::invoke_result_t<Visitor, VisitedT &>> ) {
			std::invoke( DAW_FWD( visitor ), tmp );
			value = daw::bit_cast<OrigT>( tmp );
		} else {
			auto result = std::invoke( DAW_FWD( visitor ), tmp );
			value = daw::bit_cast<OrigT>( tmp );
			return result;
		}
	}


	/**
	 * @brief Invokes a visitor on a reinterpretation of the provided value.
	 *
	 * @tparam VisitedT The type to reinterpret `value` as when invoking the visitor.
	 * @tparam OrigT The original type of the `value`.
	 * @tparam Visitor The callable type that will be applied to the reinterpreted value.
	 *
	 * @param value A constant reference to the value being reinterpreted and visited.
	 * @param visitor A callable object that operates on the reinterpreted value.
	 *
	 * @return The result of invoking the visitor with the reinterpreted value.
	 *
	 * @details This function performs a bit-cast of `value` to the type `VisitedT` and
	 *          then applies the visitor to the reinterpreted value. The result of the
	 *          visitor is returned if it produces a value.
	 */
	template<typename VisitedT, typename OrigT, typename Visitor>
	DAW_CX_IF_BIT_CAST auto visit_as( OrigT const &value, Visitor &&visitor ) {
		auto const tmp = daw::bit_cast<VisitedT>( value );
		return std::invoke( DAW_FWD( visitor ), tmp );
	}
} // namespace daw
