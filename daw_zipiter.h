#pragma once
// The MIT License ( MIT )
//
// Copyright ( c ) 2013-2015 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <tuple>

namespace daw {
	template<typename... T>
	struct ZipIter {
		using types_t = std::tuple<std::remove_cv_t<T>...>;
	private:
		types_t m_values;

		template <class... Ts, std::size_t... Is>
		void increment( std::tuple<Ts...>& tpl, std::index_sequence<Is...> ) {
			using expander = int[];
			expander { 0,
				(void(
				++std::get<Is>( tpl )
				), 0)...
			};
		}

		template <class... Ts>
		void increment( std::tuple<Ts...>& tpl ) {
			increment( tpl, std::index_sequence_for<Ts...>{} );
		}

		template <class... Ts, std::size_t... Is>
		void decrement( std::tuple<Ts...>& tpl, std::index_sequence<Is...> ) {
			using expander = int[];
			expander { 0,
				(void(
				--std::get<Is>( tpl )
				), 0)...
			};
		}

		template <class... Ts>
		void decrement( std::tuple<Ts...>& tpl ) {
			decrement( tpl, std::index_sequence_for<Ts...>{} );
		}

		template <class... Ts, std::size_t... Is>
		void advance( std::tuple<Ts...>& tpl, std::index_sequence<Is...>, intmax_t n ) {
			using expander = int[];
			expander { 0, (void( std::advance( std::get<Is>( tpl ), n ) ), 0)... };
		}

		template <class... Ts>
		void advance( std::tuple<Ts...>& tpl, intmax_t n ) {
			advance( tpl, std::index_sequence_for<Ts...>{}, n );
		}


	public:
		ZipIter( ) = delete;
		~ZipIter( ) = default;
		ZipIter( ZipIter const & ) = default;
		ZipIter( ZipIter && ) = default;
		ZipIter & operator=( ZipIter const & ) = default;
		ZipIter & operator=( ZipIter && ) = default;

		ZipIter( T... args ): m_values( std::move( args )... ) { }

		types_t & as_tuple( ) {
			return m_values;
		}

		types_t const & as_tuple( ) const {
			return m_values;
		}

		ZipIter & operator++( ) {
			increment( m_values );
			return *this;
		}

		ZipIter operator++( int ) {
			auto tmp = *this;
			++(*this);
			return tmp;
		}

		ZipIter & operator--( ) {
			decrement( m_values );
			return *this;
		}

		ZipIter operator--( int ) {
			auto tmp = *this;
			--(*this);
			return tmp;
		}

		void advance( intmax_t n ) {
			advance( m_values, n );
		}

		template<size_t pos>
		auto & get( ) noexcept {
			return std::get<pos>( m_values );
		}

		constexpr static size_t size( ) {
			return std::tuple_size<T...>::value;
		}
	};	// struct ZipIter

	template<typename... T>
	bool operator==( ZipIter<T...> const & lhs, ZipIter<T...> const & rhs ) {
		return lhs.as_tuple( ) == rhs.as_tuple( );
	}

	template<typename... T>
	bool operator!=( ZipIter<T...> const & lhs, ZipIter<T...> const & rhs ) {
		return lhs.as_tuple( ) != rhs.as_tuple( );
	}

	template<typename... T>
	bool operator<=( ZipIter<T...> const & lhs, ZipIter<T...> const & rhs ) {
		return lhs.as_tuple( ) <= rhs.as_tuple( );
	}

	template<typename... T>
	bool operator>=( ZipIter<T...> const & lhs, ZipIter<T...> const & rhs ) {
		return lhs.as_tuple( ) >= rhs.as_tuple( );
	}

	template<typename... T>
	bool operator<( ZipIter<T...> const & lhs, ZipIter<T...> const & rhs ) {
		return lhs.as_tuple( ) < rhs.as_tuple( );
	}

	template<typename... T>
	bool operator>( ZipIter<T...> const & lhs, ZipIter<T...> const & rhs ) {
		return lhs.as_tuple( ) > rhs.as_tuple( );
	}

	template<typename... T>
	ZipIter<T...> make_zipiter( T... args ) {
		return ZipIter<T...>( args... );
	}
}	// namespace daw

