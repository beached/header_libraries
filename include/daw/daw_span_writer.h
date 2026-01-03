// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_concepts.h"
#include "daw/daw_cpp_feature_check.h"
#include "daw/daw_ensure.h"
#include "daw/daw_restrict.h"

#include <cstdlib>
#include <ranges>
#include <span>
#include <type_traits>
#include <utility>

namespace daw {

	///
	/// @tparam T Destination type
	/// @param out output span to write elements to
	/// @param values parameters that are convertible to T to write to out
	/// @return out.subspan( sizeof...( values ) )
	template<typename T>
	[[nodiscard]] constexpr std::span<T>
	span_writer( std::span<T> out,
	             daw::explicitly_convertible_to<T> auto &&...values ) {
		if constexpr( sizeof...( values ) == 0 ) {
			return out;
		}
		daw_ensure( sizeof...( values ) <= out.size( ) );
		return [&]<std::size_t... Is>( std::index_sequence<Is...> ) {
			( (void)( out[Is] = static_cast<T>( values ) ), ... );
			return out.subspan( sizeof...( values ) );
		}( std::make_index_sequence<sizeof...( values )>{ } );
	}

	///
	/// @tparam T Destination type
	/// @tparam R Range of elements convertible to T
	/// @param out output span to write elements to
	/// @param r input range values to copy write to out
	/// @return out.subspan( std::ranges::size( r ) )
	/// The two ranges cannot overlap
	template<typename T, typename R>
	requires(
	  std::ranges::contiguous_range<R>
	    and daw::explicitly_convertible_to<std::ranges::range_reference_t<R>, T> )
	  [[nodiscard]] constexpr std::span<T> span_writer( std::span<T> out,
	                                                    R const &r ) {
		auto const sz = std::ranges::size( r );
		daw_ensure( sz <= out.size( ) );
		T *DAW_RESTRICT out_ptr = out.data( );
		auto *DAW_RESTRICT in_ptr = std::ranges::data( r );
		for( std::size_t n = 0; n < sz; ++n ) {
			out_ptr[n] = static_cast<T>( in_ptr[n] );
		}
		return out.subspan( sz );
	}

	/// Write a string literal, minus trailing zero to span
	/// @tparam T Destination type
	/// @tparam N size, including trailing zero, of string literal
	/// @param out output span to write elements to
	/// @param str input string literal to write to out
	/// @return out.subspan( N-1 )
	/// The two ranges cannot overlap and str must have a trailing 0
	template<typename T, std::size_t N>
	requires( daw::explicitly_convertible_to<char, T> )
	  [[nodiscard]] constexpr std::span<T> span_writer_ntz(
	    std::span<T> out, char const ( &str )[N] ) {
		daw_ensure( str[N - 1] == '\0' );
		auto const sz = N - 1;
		daw_ensure( sz <= out.size( ) );
		T *DAW_RESTRICT out_ptr = out.data( );
		auto *DAW_RESTRICT in_ptr = str;
		for( std::size_t n = 0; n < sz; ++n ) {
			out_ptr[n] = static_cast<T>( in_ptr[n] );
		}
		return out.subspan( sz );
	}

	template<typename T>
	requires( not std::is_const_v<T> and not std::is_reference_v<T> ) //
	  struct output_span {
		using pointer = T *;

	private:
		pointer m_first = nullptr;
		std::size_t m_size = 0;

		constexpr void remove_prefix_unsafe( std::size_t n ) {
			m_first += static_cast<std::ptrdiff_t>( n );
			m_size -= n;
		}

	public:
		output_span( ) = default;

		constexpr output_span( std::ranges::contiguous_range auto &r )
		  : m_first( std::ranges::data( r ) )
		  , m_size( std::ranges::size( r ) ) {}

		constexpr pointer data( ) const {
			return m_first;
		}

		constexpr std::size_t size( ) const {
			return m_size;
		}

		constexpr output_span subspan( std::size_t n ) const {
			daw_ensure( n <= m_size );
			return output_span{ .m_first = m_first + static_cast<std::ptrdiff_t>( n ),
			                    .m_size = m_size - n };
		}

		constexpr output_span &remove_prefix( std::size_t n ) {
			daw_ensure( n <= m_size );
			m_first += static_cast<std::ptrdiff_t>( n );
			m_size -= n;
			return *this;
		}

		constexpr output_span &
		write( this auto &self,
		       daw::explicitly_convertible_to<T> auto &&...values ) {
			if constexpr( sizeof...( values ) == 0 ) {
				return self;
			}
			daw_ensure( sizeof...( values ) <= self.m_size );
			[&]<std::size_t... Is>( std::index_sequence<Is...> ) {
				( (void)( self.m_first[Is] = static_cast<T>( values ) ), ... );
			}( std::make_index_sequence<sizeof...( values )>{ } );
			self.remove_prefix_unsafe( sizeof...( values ) );
			return self;
		}

		template<typename R>
		requires(
		  std::ranges::contiguous_range<R> and daw::explicitly_convertible_to<
		    std::ranges::range_reference_t<R>, T> ) //
		  constexpr output_span &write( this auto &self, R const &r ) {
			auto const sz = std::ranges::size( r );
			daw_ensure( sz <= self.m_size );
			T *DAW_RESTRICT self_ptr = self.m_first;
			auto *DAW_RESTRICT in_ptr = std::ranges::data( r );
			for( std::size_t n = 0; n < sz; ++n ) {
				self_ptr[n] = static_cast<T>( in_ptr[n] );
			}
			self.remove_prefix_unsafe( sz );
			return self;
		}

		template<std::size_t N>
		requires( daw::explicitly_convertible_to<char, T> ) //
		  constexpr output_span &write_ntz( this auto &self,
		                                    char const ( &str )[N] ) {
			daw_ensure( str[N - 1] == '\0' );
			auto const sz = N - 1;
			daw_ensure( sz <= self.m_size );
			T *DAW_RESTRICT self_ptr = self.m_first;
			auto *DAW_RESTRICT in_ptr = str;
			for( std::size_t n = 0; n < sz; ++n ) {
				self_ptr[n] = static_cast<T>( in_ptr[n] );
			}
			self.remove_prefix_unsafe( sz );
			return self;
		}
	};

	template<std::ranges::contiguous_range R>
	output_span( R ) -> output_span<std::ranges::range_value_t<R>>;

} // namespace daw
