
// The MIT License (MIT)
//
// Copyright (c) 2018-2019 Darrell Wright
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

#include "daw/daw_benchmark.h"
#include "daw/daw_graph.h"
#include "daw/daw_graph_algorithm.h"

void test_topoligical_walk_001( daw::graph_t<char> &graph ) {
	std::string result{};
	daw::topological_sorted_walk(
	  graph, [&result]( auto const &node ) { result.push_back( node.value( ) ); },
	  []( auto const &lhs, auto const &rhs ) {
		  return lhs.value( ) < rhs.value( );
	  } );

	daw::expecting( "CABDFE", result );
}

void test_topoligical_walk_002( daw::graph_t<char> const &graph ) {
	std::string result{};
	daw::topological_sorted_walk(
	  graph, [&result]( auto const &node ) { result.push_back( node.value( ) ); },
	  []( auto const &lhs, auto const &rhs ) {
		  return lhs.value( ) < rhs.value( );
	  } );

	daw::expecting( "CABDFE", result );
}

void test_topoligical_walk_003( ) {
	daw::graph_t<char> graph{};
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

	std::string result{};
	daw::topological_sorted_walk(
	  graph, [&result]( auto const &node ) { result.push_back( node.value( ) ); },
	  []( auto const &lhs, auto const &rhs ) {
		  return lhs.value( ) < rhs.value( );
	  } );

	daw::expecting( "542310", result );
}

void test_topoligical_walk_004( ) {
	daw::graph_t<char> graph{};
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

	std::string result{};
	auto rng = daw::make_topological_sorted_range(
	  graph, [&result]( auto const &node ) { result.push_back( node.value( ) ); },
	  []( auto const &lhs, auto const &rhs ) {
		  return lhs.value( ) < rhs.value( );
	  } );
	result.reserve( rng.size( ) );
	std::copy( rng.begin( ), rng.end( ), std::back_inserter( result ) );
	daw::expecting( "542310", result );
}

void test_bfs_walk_001( daw::graph_t<char> const &graph,
                        daw::node_id_t root_id ) {
	std::string result{};
	daw::bfs_walk( graph, root_id, [&result]( auto &&node ) {
		result.push_back( node.value( ) );
	} );
	daw::expecting( "CFAEDB", result );
}

void test_bfs_walk_002( daw::graph_t<char> &graph, daw::node_id_t root_id ) {
	std::string result{};
	daw::bfs_walk( graph, root_id, [&result]( auto &&node ) {
		result.push_back( node.value( ) );
	} );
	daw::expecting( "CFAEDB", result );
}
void test_dfs_walk_001( daw::graph_t<char> graph, daw::node_id_t root_id ) {
	std::string result{};
	daw::dfs_walk( graph, root_id, [&result]( auto &&node ) {
		result.push_back( node.value( ) );
		++node.value( );
	} );
	daw::expecting( "CABEDF", result );

	result.clear( );
	daw::dfs_walk( graph, root_id, [&result]( auto &&node ) {
		result.push_back( node.value( ) );
	} );
	daw::expecting( "DBCFEG", result );
}

void test_dfs_walk_002( daw::graph_t<char> const &graph,
                        daw::node_id_t root_id ) {
	std::string result{};
	daw::dfs_walk( graph, root_id, [&result]( auto &&node ) {
		result.push_back( node.value( ) );
	} );
	daw::expecting( "CABEDF", result );
}

void test_mst_001( daw::graph_t<char> graph, daw::node_id_t ) {
	std::string result{};
	daw::mst( graph );

	daw::topological_sorted_walk(
	  graph, [&result]( auto const &node ) { result.push_back( node.value( ) ); },
	  []( auto const &lhs, auto const &rhs ) {
		  return lhs.value( ) < rhs.value( );
	  } );
	daw::expecting( "CABDEF", result );
}

int main( ) {
	daw::graph_t<char> graph{};
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

	test_topoligical_walk_004( );
	test_topoligical_walk_003( );
	test_topoligical_walk_001( graph );
	test_topoligical_walk_002( graph );
	test_bfs_walk_001( graph, nC );
	test_bfs_walk_002( graph, nC );
	test_dfs_walk_001( graph, nC );
	test_dfs_walk_002( graph, nC );
	test_mst_001( graph, nC );
}
