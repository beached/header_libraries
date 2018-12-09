
// The MIT License (MIT)
//
// Copyright (c) 2018 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
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
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "daw_graph.h"
#include "cpp_17.h"

namespace daw {
	namespace graph_alg_impl {
		struct NoSort {};

		template<typename Graph, typename Node>
		auto get_child_nodes( Graph &&graph, Node &&node ) {
			using node_t =
			  std::remove_reference_t<decltype( graph.get_node( node.id( ) ) )>;

			std::vector<node_t> result{};
			for( auto m_id : node.outgoing_edges( ) ) {
				result.push_back( graph.get_node( m_id ) );
			}
			return result;
		}
	} // namespace graph_alg_impl

	template<typename T, typename Function, typename Compare = daw::graph_alg_impl::NoSort>
	void topological_sorted_for_each( graph_t<T> &graph, Function &&func,
	                                  Compare comp = Compare{} ) {

		constexpr bool perform_sort_v = !std::is_same_v<Compare, daw::graph_alg_impl::NoSort>;
		// Find all nodes that do not have incoming entries
		std::vector<daw::graph_node_t<T>> root_nodes = [&graph]( ) {
			auto root_node_ids = graph.find_roots( );
			std::vector<daw::graph_node_t<T>> result{};
			result.reserve( root_node_ids.size( ) );
			std::transform( begin( root_node_ids ), end( root_node_ids ),
			                std::back_inserter( result ),
			                [&]( node_id_t id ) { return graph.get_node( id ); } );
		}

		if constexpr( perform_sort_v ) {
			std::sort( begin( root_nodes ), end( root_nodes ),
			           [&]( auto &&... args ) {
				           return !daw::invoke(
				             comp, std::forward<decltype( args )>( args )... );
			           } );
		}

		std::unordered_map<node_id_t, std::vector<node_id_t>> excluded_edges{};

		auto const exclude_edge = [&]( node_id_t from, node_id_t to ) {
			auto &child = excluded_edges[to];
			auto pos = std::find( begin( child ), end( child ), from );
			if( pos == end( child ) ) {
				child.push_back( from );
			}
		};

		auto const has_parent_nodes = [&]( node_id_t n_id ) {
			auto incoming = graph.get_raw_node( n_id ).incoming_edges( );
			if( incoming.empty( ) ) {
				return false;
			}
			if( excluded_edges.count( n_id ) == 0 ) {
				return true;
			}
			for( auto ex_id : excluded_edges[n_id] ) {
				incoming.erase( ex_id );
			}
			return !incoming.empty( );
		};

		while( !root_nodes.empty( ) ) {
			auto node = root_nodes.back( );
			root_nodes.pop_back( );
			daw::invoke( func, node );

			{
				auto child_nodes = graph_alg_impl::get_child_nodes( graph, node );
				if constexpr( perform_sort_v ) {
					std::sort( begin( child_nodes ), end( child_nodes ),
					           [&]( auto &&... args ) {
						           return !daw::invoke(
						             comp, std::forward<decltype( args )>( args )... );
					           } );
				}
				for( auto child : child_nodes ) {
					exclude_edge( node.id( ), child.id( ) );
					if( !has_parent_nodes( child.id( ) ) ) {
						root_nodes.push_back( child );
					}
				}
			}
			if constexpr( perform_sort_v ) {
				std::sort( begin( root_nodes ), end( root_nodes ),
				           [&]( auto &&... args ) {
					           return !daw::invoke(
					             comp, std::forward<decltype( args )>( args )... );
				           } );
			}
		}
	}

	template<typename T, typename Function, typename Compare = daw::graph_alg_impl::NoSort>
	void topological_sorted_for_each( graph_t<T> const &graph, Function &&func,
	                                  Compare comp = Compare{} ) {

		constexpr bool perform_sort_v = !std::is_same_v<Compare, daw::graph_alg_impl::NoSort>;
		// Find all nodes that do not have incoming entries
		std::vector<daw::const_graph_node_t<T>> root_nodes = [&graph]( ) {
			auto root_node_ids = graph.find_roots( );
			std::vector<daw::const_graph_node_t<T>> result{};
			result.reserve( root_node_ids.size( ) );
			std::transform( begin( root_node_ids ), end( root_node_ids ),
			                std::back_inserter( result ),
			                [&]( node_id_t id ) { return graph.get_node( id ); } );
		}
			
		std::unordered_map<node_id_t, std::vector<node_id_t>> excluded_edges{};

		auto const exclude_edge = [&]( node_id_t from, node_id_t to ) {
			auto &child = excluded_edges[to];
			auto pos = std::find( begin( child ), end( child ), from );
			if( pos == end( child ) ) {
				child.push_back( from );
			}
		};

		auto const has_parent_nodes = [&]( node_id_t n_id ) {
			auto incoming = graph.get_raw_node( n_id ).incoming_edges( );
			if( incoming.empty( ) ) {
				return false;
			}
			if( excluded_edges.count( n_id ) == 0 ) {
				return true;
			}
			for( auto ex_id : excluded_edges[n_id] ) {
				incoming.erase( ex_id );
			}
			return !incoming.empty( );
		};

		while( !root_nodes.empty( ) ) {
			auto node = root_nodes.back( );
			root_nodes.pop_back( );
			daw::invoke( func, node );

			{
				auto child_nodes = graph_alg_impl::get_child_nodes( graph, node );
				if constexpr( perform_sort_v ) {
					std::sort( begin( child_nodes ), end( child_nodes ),
					           [&]( auto &&... args ) {
						           return !daw::invoke(
						             comp, std::forward<decltype( args )>( args )... );
					           } );
				}
				for( auto child : child_nodes ) {
					exclude_edge( node.id( ), child.id( ) );
					if( !has_parent_nodes( child.id( ) ) ) {
						root_nodes.push_back( child );
					}
				}
			}
			if constexpr( perform_sort_v ) {
				std::sort( begin( root_nodes ), end( root_nodes ),
				           [&]( auto &&... args ) {
					           return !daw::invoke(
					             comp, std::forward<decltype( args )>( args )... );
				           } );
			}
		}
	}
} // namespace daw
