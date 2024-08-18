// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_move.h"
#include "daw_visit.h"

#include <memory>

namespace daw::algorithm {
	namespace alg_details {
		template<typename T>
		struct incrementor {
			constexpr void operator( )( T &value ) const {
				++value;
			}
		};
	} // namespace alg_details
	template<typename T, typename Comp = std::less<>>
	struct iota_op {
		T m_first = 0;
		T m_step = 1;

		explicit iota_op( ) = default;

		template<typename U>
		explicit constexpr iota_op( U &&first )
		  : m_first{ DAW_FWD( first ) } {}

		template<typename U>
		explicit constexpr iota_op( U &&first, U &&step )
		  : m_first{ DAW_FWD( first ) }
		  , m_step{ DAW_FWD( step ) } {}

		template<typename SizeType, typename Allocator>
		constexpr SizeType
		operator( )( T *ptr, SizeType sz, Allocator &alloc ) const {
			DAW_UNSAFE_BUFFER_FUNC_START
			auto value = m_first;
			auto const last = m_first + static_cast<T>( sz ) * m_step;
			while( Comp{ }( value, last ) ) {
				std::allocator_traits<Allocator>::construct( alloc, ptr, value );
				++ptr;
				value += m_step;
			}
			DAW_UNSAFE_BUFFER_FUNC_STOP
			return sz;
		}
	};

	template<typename T>
	iota_op( T ) -> iota_op<T>;

	template<typename T>
	iota_op( T, T ) -> iota_op<T>;
} // namespace daw::algorithm
