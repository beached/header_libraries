// The MIT License (MIT)
//
// Copyright (c) 2014-2015 Darrell Wright
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

#pragma once

#include <algorithm>
#include <iterator>
#include <numeric>
#include <random>
#include <utility>
#include <vector>
#include "daw_traits.h"
#include "daw_algorithm.h"
#include "daw_reference_range.h"

#define DAW_RANGE_GENERATE_UPRED_CLAUSE( clause_name )\
namespace daw {\
	namespace range {\
		namespace details {\
			template<typename Container, typename UnaryPredicate>\
			auto clause_name( Container const & container, UnaryPredicate predicate ) {\
				return make_ref_range( container ).clause_name( predicate );\
			}\
			template<typename Iterator, typename UnaryPredicate>\
			auto clause_name( ReferenceRange<Iterator> const & rng, UnaryPredicate predicate ) {\
				return rng.clause_name( predicate );\
			}\
			template<typename UnaryPredicate>\
			struct clause_name##_t {\
				UnaryPredicate predicate;\
				clause_name##_t( UnaryPredicate pred ):predicate( pred ) { }\
				template<typename Collection>\
				auto operator()( Collection const & collection ) const {\
					return clause_name( collection, predicate );\
				}\
			};\
		}\
		template<typename UnaryPredicate>\
		auto clause_name( UnaryPredicate predicate ) {\
			return ::daw::range::details::clause_name##_t<UnaryPredicate>( predicate );\
		}\
	}\
}\
template<typename Collection, typename F>\
auto operator<<( Collection const & collection, ::daw::range::details::clause_name##_t<F> const & predicate ) {\
	return predicate( collection );\
}\

//DAW_RANGE_GENERATE_UPRED_CLAUSE( where );

#undef DAW_RANGE_GENERATE_UPRED_CLAUSE

#define DAW_RANGE_GENERATE_CLAUSE( clause_name )\
namespace daw {\
	namespace range {\
		namespace details {\
			template<typename Container, typename UnaryPredicate>\
			auto clause_name( Container const & container ) {\
				return make_ref_range( container ).clause_name( );\
			}\
			template<typename Iterator>\
			auto clause_name( ReferenceRange<Iterator> const & rng ) {\
				return rng.clause_name( );\
			}\
			struct clause_name##_t {\
				clause_name##_t( ) = default;\
				template<typename Collection>\
				auto operator()( Collection & collection ) {\
					return clause_name( collection );\
				}\
				template<typename Collection>\
				auto operator()( Collection const & collection ) const {\
					return clause_name( collection );\
				}\
			};\
		}\
		auto clause_name( ) {\
			return ::daw::range::details::clause_name##_t( );\
		}\
	}\
}\
template<typename Collection>\
auto operator<<( Collection const & collection, ::daw::range::details::clause_name##_t const & predicate ) {\
	return predicate( collection );\
}\

DAW_RANGE_GENERATE_CLAUSE( sort );
DAW_RANGE_GENERATE_CLAUSE( unique );

#undef DAW_RANGE_GENERATE_CLAUSE


#define DAW_RANGE_GENERATE_VCLAUSE( clause_name )\
namespace daw {\
	namespace range {\
		namespace details {\
			template<typename Container, typename... Args>\
			auto clause_name( Container const & container, Args&&... clause_args ) {\
				return make_ref_range( container ).clause_name( std::forward<Args>( clause_args )... );\
			}\
			\
			template<typename Iterator, typename... Args>\
			auto clause_name( ReferenceRange<Iterator> const & rng, Args&&... clause_args ) {\
				return rng.clause_name( std::forward<Args>( clause_args )... );\
			}\
			\
			template<typename... Args>\
			struct clause_name##_t {\
				::std::tuple<Args...> clause_args;\
				clause_name##_t( Args&&... args ):clause_args( ::std::forward<Args>( clause_args )... ) { }\
				template<typename Collection>\
				auto operator()( Collection const & collection ) const {\
					return delayed_dispatch( collection );\
				}\
				\
				template<int ...>\
				struct seq { };\
				\
				template<int N, int ...S>\
				struct gens: gens<N-1, N-1, S...> { };\
				\
				template<int ...S>\
				struct gens<0, S...> {\
					typedef seq<S...> type;\
				};\
				\
				template<typename Collection>\
				auto delayed_dispatch( Collection const & collection ) const {\
					return callFunc( collection, typename gens<sizeof...(Args)>::type( ) );\
				}\
				\
				template<typename Collection, int ...S>\
				auto callFunc( Collection const & collection, seq<S...> ) const {\
					return clause_name( collection, std::get<S>( clause_args )... );\
				}\
			};\
		}\
		\
		template<typename... Args>\
		auto clause_name( Args&&... clause_args ) {\
			return ::daw::range::details::clause_name##_t<Args...>( ::std::forward<Args>( clause_args )... );\
		}\
	}\
}\
template<typename Collection, typename... Args>\
auto operator<<( Collection const & collection, ::daw::range::details::clause_name##_t<Args...> const & predicate ) {\
	return predicate( collection );\
}

//DAW_RANGE_GENERATE_VCLAUSE( where );

#undef DAW_RANGE_GENERATE_VCLAUSE

namespace daw {
	namespace range {
		namespace details {
			template<typename Container, typename... Args>
			auto where( Container const & container, Args&&... clause_args ) {
				return make_ref_range( container ).where( std::forward<Args>( clause_args )... );
			}
			
			template<typename Iterator, typename... Args>
			auto where( ReferenceRange<Iterator> const & rng, Args&&... clause_args ) {
				return rng.where( std::forward<Args>( clause_args )... );
			}
			
			template<typename... Args>
			struct where_t {
				::std::tuple<Args...> where_args;

				where_t( Args&&... args ): where_args( ::std::forward<Args>( where_args )... ) { }

				template<typename Collection>
				auto operator()( Collection const & collection ) const {
					return delayed_dispatch( collection );
				}
				
				template<int ...>
				struct seq { };
				
				template<int N, int ...S>
				struct gens: gens<N-1, N-1, S...> { };
				
				template<int ...S>
				struct gens<0, S...> {
					typedef seq<S...> type;
				};
				
				template<typename Collection>
				auto delayed_dispatch( Collection const & collection ) const {
					return call_where( collection, typename gens<sizeof...(Args)>::type( ) );
				}
				
				template<typename Collection, int ...S>
				auto call_where( Collection const & collection, seq<S...> ) const {
					return where( collection, std::get<S>( where_args )... );
				}
			};
		}
		
		template<typename... Args>
		auto where( Args&&... where_args ) {
			return ::daw::range::details::where_t<Args...>( ::std::forward<Args>( where_args )... );
		}
	}
}
template<typename Collection, typename... Args>\
auto operator<<( Collection const & collection, ::daw::range::details::where_t<Args...> const & predicate ) {\
	return predicate( collection );\
}









namespace daw {
	namespace range {
		template<typename... Args>
		auto from( Args&&... args ) {
			return make_range( std::forward<Args>( args )... );
		}
	}	// namespace range
}	// namespace daw

