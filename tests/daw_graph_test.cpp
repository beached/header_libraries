
// The MIT License (MIT)
//
// Copyright (c) 2018 Darrell Wright
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

#include "daw/daw_graph.h"
#include "daw/daw_benchmark.h"

int main( ) {
	{
		daw::graph_t<int> graph{};
		auto n0_id = graph.add_node(5);
		daw::expecting(graph.has_node(n0_id));

		auto node = graph.get_node(n0_id);
		daw::expecting( node.value( ), 5 );

		graph.remove_node(n0_id);
		daw::expecting(!graph.has_node(n0_id));
	}
	daw::graph_t<int> graph{};
	auto n0_id = graph.add_node( 5 );
	auto n1_id = graph.add_node( 6 );
	graph.add_directed_edge( n0_id, n1_id );

	auto n2_id = graph.add_node( );
	auto n3_id = graph.add_node( );
	graph.add_undirected_edge( n2_id, n3_id );
	graph.add_node( 1000 );

	auto roots = graph.find( []( auto const & node ) {
		if( node.incoming_edges( ).empty( ) ) {
			return true;
		}
		return false;
	} );
	daw::expecting( roots.size( ), 2ULL );

	auto nodes = graph.find_by_value( 1000 );
	daw::expecting( nodes.size( ), 1ULL );
	daw::expecting( graph.has_node( nodes.front( ) ) );
	daw::expecting( graph.get_node( nodes.front( ) ).value( ), 1000 );
	return static_cast<int>( n0_id.value );
}
