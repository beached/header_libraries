// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#include "daw/daw_graph.h"

#include "daw/daw_benchmark.h"

#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

int main( ) {
	{
		constexpr daw::node_id_t null_id{ };
		constexpr daw::graph_node_t<int> null_node{ };

		daw::graph_t<int> graph{ };
		auto n0_id = graph.add_node( 5 );
		daw::expecting( n0_id != null_id );

		daw::expecting( graph.has_node( n0_id ) );

		daw::expecting( graph.get_node( n0_id ) != null_node );

		auto node = graph.get_node( n0_id );
		daw::expecting( node.value( ), 5 );

		graph.remove_node( n0_id );
		daw::expecting( !graph.has_node( n0_id ) );
	}
	{
		daw::graph_t<int> graph{ };
		auto n0_id = graph.add_node( 5 );
		auto n1_id = graph.add_node( 6 );
		graph.add_directed_edge( n0_id, n1_id );

		auto n2_id = graph.add_node( );
		auto n3_id = graph.add_node( );
		graph.add_undirected_edge( n2_id, n3_id );
		graph.add_node( 1000 );

		auto roots = graph.find( []( auto const &node ) {
			return node.incoming_edges( ).empty( );
		} );
		daw::expecting( roots.size( ), 2ULL );

		auto nodes = graph.find_by_value( 1000 );
		daw::expecting( nodes.size( ), 1ULL );
		daw::expecting( graph.has_node( nodes.front( ) ) );
		daw::expecting( graph.get_node( nodes.front( ) ).value( ), 1000 );
	}
	{
		daw::graph_t<int> graph{ };
		auto n0 = graph.add_node( 0 );
		auto n1 = graph.add_node( 1 );
		auto n2 = graph.add_node( 2 );
		graph.add_undirected_edge( n0, n1 );
		graph.add_directed_edge( n0, n2 );
		daw::expecting( graph.get_raw_node( n0 ).incoming_edges( ).count( n1 ),
		                1ULL );
		daw::expecting( graph.get_raw_node( n1 ).incoming_edges( ).count( n0 ),
		                1ULL );
		daw::expecting( graph.get_raw_node( n0 ).outgoing_edges( ).count( n1 ),
		                1ULL );
		daw::expecting( graph.get_raw_node( n1 ).outgoing_edges( ).count( n0 ),
		                1ULL );
		daw::expecting( graph.get_raw_node( n2 ).incoming_edges( ).count( n0 ),
		                1ULL );
		daw::expecting( graph.get_raw_node( n0 ).outgoing_edges( ).count( n2 ),
		                1ULL );

		graph.remove_edges( n0, n1 );
		daw::expecting( graph.get_raw_node( n0 ).incoming_edges( ).count( n1 ),
		                0U );
		daw::expecting( graph.get_raw_node( n1 ).incoming_edges( ).count( n0 ),
		                0U );
		daw::expecting( graph.get_raw_node( n0 ).outgoing_edges( ).count( n1 ),
		                0U );
		daw::expecting( graph.get_raw_node( n1 ).outgoing_edges( ).count( n0 ),
		                0U );
		daw::expecting( graph.get_raw_node( n2 ).incoming_edges( ).count( n0 ),
		                1ULL );
		daw::expecting( graph.get_raw_node( n0 ).outgoing_edges( ).count( n2 ),
		                1ULL );
		graph.remove_directed_edge( n0, n2 );
	}
}
