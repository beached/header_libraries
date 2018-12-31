
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

#include "cpp_17.h"
#include "daw_graph.h"

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

		template<typename Node, typename T, typename Graph, typename Function,
		         typename Compare>
		void topological_sorted_walk( Graph &&graph, Function &&func,
		                              Compare comp = Compare{} ) {

			constexpr bool perform_sort_v =
			  !std::is_same_v<Compare, daw::graph_alg_impl::NoSort>;
			// Find all nodes that do not have incoming entries
			std::vector<Node> root_nodes = [&graph]( ) {
				auto root_node_ids = graph.find_roots( );
				std::vector<Node> result{};
				result.reserve( root_node_ids.size( ) );
				std::transform( std::begin( root_node_ids ), std::end( root_node_ids ),
				                std::back_inserter( result ),
				                [&]( node_id_t id ) { return graph.get_node( id ); } );
				return result;
			}( );

			if constexpr( perform_sort_v ) {
				std::sort( std::begin( root_nodes ), std::end( root_nodes ),
				           [&]( auto &&... args ) {
					           return !daw::invoke(
					             comp, std::forward<decltype( args )>( args )... );
				           } );
			}

			std::unordered_map<node_id_t, std::vector<node_id_t>> excluded_edges{};

			auto const exclude_edge = [&]( node_id_t from, node_id_t to ) {
				auto &child = excluded_edges[to];
				auto pos = std::find( std::begin( child ), std::end( child ), from );
				if( pos == std::end( child ) ) {
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
						std::sort( std::begin( child_nodes ), std::end( child_nodes ),
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
					std::sort( std::begin( root_nodes ), std::end( root_nodes ),
					           [&]( auto &&... args ) {
						           return !daw::invoke(
						             comp, std::forward<decltype( args )>( args )... );
					           } );
				}
			}
		}

		template<typename T, typename Graph, typename Function>
		void bfs_walk( Graph &&graph, daw::node_id_t start_node_id,
		               Function &&func ) {
			std::unordered_set<daw::node_id_t> visited{};
			std::deque<daw::node_id_t> path{};
			path.push_back( start_node_id );

			while( !path.empty( ) ) {
				auto current_node = graph.get_node( path.front( ) );
				path.pop_front( );
				daw::invoke( func, current_node );
				visited.insert( current_node.id( ) );
				std::copy_if( std::begin( current_node.outgoing_edges( ) ),
				              std::end( current_node.outgoing_edges( ) ),
				              std::back_inserter( path ), [&]( auto const &n_id ) {
					              return visited.count( n_id ) == 0;
				              } );
			}
		}

		template<typename T, typename Graph, typename Function>
		void dfs_walk( Graph &&graph, daw::node_id_t start_node_id,
		               Function &&func ) {
			std::unordered_set<daw::node_id_t> visited{};
			std::vector<daw::node_id_t> path{};
			path.push_back( start_node_id );

			while( !path.empty( ) ) {
				auto current_node = graph.get_node( path.back( ) );
				path.pop_back( );
				daw::invoke( func, current_node );
				visited.insert( current_node.id( ) );
				std::copy_if( std::begin( current_node.outgoing_edges( ) ),
				              std::end( current_node.outgoing_edges( ) ),
				              std::back_inserter( path ), [&]( auto const &n_id ) {
					              return visited.count( n_id ) == 0;
				              } );
			}
		}
	} // namespace graph_alg_impl
	template<typename T, typename Compare = daw::graph_alg_impl::NoSort>
	void mst( daw::graph_t<T> &graph, Compare comp = Compare{} ) {
		auto root_ids = graph.find_roots( );
		for( auto start_node_id : root_ids ) {
			std::unordered_set<daw::node_id_t> visited{};
			std::vector<std::pair<std::optional<daw::node_id_t>, daw::node_id_t>>
			  path{};
			path.push_back( {std::nullopt, start_node_id} );

			while( !path.empty( ) ) {
				auto current_id = path.back( );
				path.pop_back( );
				if( visited.count( current_id.second ) > 0 ) {
					daw::exception::precondition_check(
					  current_id.first.has_value( ),
					  "I assumed that the root would have been first visited" );
					graph.remove_directed_edge( *current_id.first, current_id.second );
					continue;
				}
				visited.insert( current_id.second );
				auto current_node = graph.get_node( current_id.second );
				std::transform(
				  std::begin( current_node.outgoing_edges( ) ),
				  std::end( current_node.outgoing_edges( ) ),
				  std::back_inserter( path ), [parent = current_id.second]( auto id ) {
					  return std::pair<std::optional<daw::node_id_t>, node_id_t>( parent,
					                                                              id );
				  } );
			}
		}
	} // namespace daw

	template<typename T, typename Function,
	         typename Compare = daw::graph_alg_impl::NoSort>
	void topological_sorted_walk( daw::graph_t<T> const &graph, Function &&func,
	                              Compare comp = Compare{} ) {

		using Node = std::remove_reference_t<decltype(
		  graph.get_node( std::declval<daw::node_id_t>( ) ) )>;

		graph_alg_impl::topological_sorted_walk<Node, T>(
		  graph, std::forward<Function>( func ), std::move( comp ) );
	}

	template<typename T, typename Function,
	         typename Compare = daw::graph_alg_impl::NoSort>
	void topological_sorted_walk( daw::graph_t<T> &graph, Function &&func,
	                              Compare comp = Compare{} ) {

		using Node = std::remove_reference_t<decltype(
		  graph.get_node( std::declval<daw::node_id_t>( ) ) )>;

		graph_alg_impl::topological_sorted_walk<Node, T>(
		  graph, std::forward<Function>( func ), std::move( comp ) );
	}

	template<typename T, typename Function>
	void bfs_walk( daw::graph_t<T> const &graph, daw::node_id_t start_node_id,
	               Function &&func ) {

		graph_alg_impl::bfs_walk<T>( graph, start_node_id,
		                             std::forward<Function>( func ) );
	}

	template<typename T, typename Function>
	void bfs_walk( daw::graph_t<T> &graph, daw::node_id_t start_node_id,
	               Function &&func ) {

		graph_alg_impl::bfs_walk<T>( graph, start_node_id,
		                             std::forward<Function>( func ) );
	}

	template<typename T, typename Function>
	void dfs_walk( daw::graph_t<T> const &graph, daw::node_id_t start_node_id,
	               Function &&func ) {

		graph_alg_impl::dfs_walk<T>( graph, start_node_id,
		                             std::forward<Function>( func ) );
	}

	template<typename T, typename Function>
	void dfs_walk( daw::graph_t<T> &graph, daw::node_id_t start_node_id,
	               Function &&func ) {

		graph_alg_impl::dfs_walk<T>( graph, start_node_id,
		                             std::forward<Function>( func ) );
	}

} // namespace daw
