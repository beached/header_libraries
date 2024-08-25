// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_graph.h"
#include "daw/daw_graph_algorithm.h"

#include "daw/daw_algorithm.h"
#include "daw/daw_benchmark.h"

#include <iostream>
#include <iterator>
#include <string>
#include <unordered_map>
#include <vector>

void test_topoligical_walk_001( daw::graph_t<char> &graph ) {
	std::string result{ };
	daw::topological_sorted_walk(
	  graph,
	  [&result]( auto const &node ) {
		  result.push_back( node.value( ) );
	  },
	  []( auto const &lhs, auto const &rhs ) {
		  return lhs.value( ) < rhs.value( );
	  } );

	daw::expecting( "CABDFE", result );
}

void test_topoligical_walk_002( daw::graph_t<char> const &graph ) {
	std::string result{ };
	daw::topological_sorted_walk(
	  graph,
	  [&result]( auto const &node ) {
		  result.push_back( node.value( ) );
	  },
	  []( auto const &lhs, auto const &rhs ) {
		  return lhs.value( ) < rhs.value( );
	  } );

	daw::expecting( "CABDFE", result );
}

void test_topoligical_walk_003( ) {
	daw::graph_t<char> graph{ };
	auto n0 = graph.add_node( '0' );
	auto n1 = graph.add_node( '1' );
	auto n2 = graph.add_node( '2' );
	auto n3 = graph.add_node( '3' );
	auto n4 = graph.add_node( '4' );
	auto n5 = graph.add_node( '5' );
	graph.add_directed_edge( n2, n3 );
	graph.add_directed_edge( n3, n1 );
	graph.add_directed_edge( n4, n0 );
	graph.add_directed_edge( n4, n1 );
	graph.add_directed_edge( n5, n0 );
	graph.add_directed_edge( n5, n2 );

	std::string result{ };
	daw::topological_sorted_walk(
	  graph,
	  [&result]( auto const &node ) {
		  result.push_back( node.value( ) );
	  },
	  []( auto const &lhs, auto const &rhs ) {
		  return lhs.value( ) < rhs.value( );
	  } );

	daw::expecting( "542310", result );
}

void test_topoligical_range_001( ) {
	daw::graph_t<char> graph{ };
	auto n0 = graph.add_node( '0' );
	auto n1 = graph.add_node( '1' );
	auto n2 = graph.add_node( '2' );
	auto n3 = graph.add_node( '3' );
	auto n4 = graph.add_node( '4' );
	auto n5 = graph.add_node( '5' );
	graph.add_directed_edge( n2, n3 );
	graph.add_directed_edge( n3, n1 );
	graph.add_directed_edge( n4, n0 );
	graph.add_directed_edge( n4, n1 );
	graph.add_directed_edge( n5, n0 );
	graph.add_directed_edge( n5, n2 );

	std::string result{ };
	auto rng = daw::make_topological_sorted_range(
	  graph, []( auto const &lhs, auto const &rhs ) {
		  return lhs.value( ) < rhs.value( );
	  } );
	result.reserve( rng.size( ) );
	daw::algorithm::transform(
	  rng.begin( ), rng.end( ), std::back_inserter( result ), []( auto &&node ) {
		  return node.value( );
	  } );
	daw::expecting( "542310", result );
}

void test_bfs_walk_001( daw::graph_t<char> const &graph,
                        daw::node_id_t root_id ) {
	std::string result{ };
	daw::bfs_walk(
	  graph,
	  root_id,
	  [&result]( auto &&node ) {
		  result.push_back( node.value( ) );
	  },
	  std::less<void>{ } );
	daw::expecting( "CAFBDEE", result );
}

void test_dfs_walk_001( daw::graph_t<char> graph, daw::node_id_t root_id ) {
	std::string result{ };
	daw::dfs_walk(
	  graph,
	  root_id,
	  [&result]( auto &&node ) {
		  result.push_back( node.value( ) );
	  },
	  std::less<void>{ } );
	daw::expecting( "CABEDF", result );
}

void test_dfs_walk_002( daw::graph_t<char> const &graph,
                        daw::node_id_t root_id ) {
	std::string result{ };
	daw::dfs_walk(
	  graph,
	  root_id,
	  [&result]( auto &&node ) {
		  result.push_back( node.value( ) );
	  },
	  std::less<void>{ } );
	daw::expecting( "CABEDF", result );
}

void test_mst_001( daw::graph_t<char> graph, daw::node_id_t ) {
	std::string result{ };
	daw::mst( graph );

	daw::topological_sorted_walk(
	  graph,
	  [&result]( auto const &node ) {
		  result.push_back( node.value( ) );
	  },
	  []( auto const &lhs, auto const &rhs ) {
		  return lhs.value( ) < rhs.value( );
	  } );
	daw::expecting( "CABDEF", result );
}

int main( ) {
	daw::graph_t<char> graph{ };
	auto nA = graph.add_node( 'A' );
	auto nB = graph.add_node( 'B' );
	auto nC = graph.add_node( 'C' );
	auto nD = graph.add_node( 'D' );
	auto nE = graph.add_node( 'E' );
	auto nF = graph.add_node( 'F' );
	graph.add_directed_edge( nC, nA );
	graph.add_directed_edge( nC, nF );
	graph.add_directed_edge( nA, nB );
	graph.add_directed_edge( nA, nD );
	graph.add_directed_edge( nB, nE );
	graph.add_directed_edge( nF, nE );

	std::cout << "Starting test_topoligical_range_00\n";
	test_topoligical_range_001( );
	std::cout << "Starting test_topoligical_walk_003\n";
	test_topoligical_walk_003( );
	// TODO: failing
	// test_topoligical_walk_001( graph );
	// test_topoligical_walk_002( graph );
	std::cout << "Starting test_bfs_walk_001\n";
	test_bfs_walk_001( graph, nC );
	// TODO: failing
	test_dfs_walk_001( graph, nC );
	test_dfs_walk_002( graph, nC );
	// test_mst_001( graph, nC );
}
