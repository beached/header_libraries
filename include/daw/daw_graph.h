
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

#include <cstddef>
#include <deque>
#include <optional>
#include <unordered_map>
#include <unordered_set>

#include "daw_exception.h"
#include "daw_utility.h"

namespace daw {
	struct node_id_t {
		static inline constexpr size_t const NO_ID = std::numeric_limits<size_t>::max( );
		size_t value = NO_ID;

		constexpr node_id_t( ) noexcept = default;
		constexpr node_id_t( size_t id ) noexcept: value( id ) {}
	};

	constexpr bool operator==( node_id_t lhs, node_id_t rhs ) noexcept {
		return lhs.value == rhs.value;
	}

	constexpr bool operator!=( node_id_t lhs, node_id_t rhs ) noexcept {
		return lhs.value != rhs.value;
	}
}

namespace std {
	template<>
	struct hash<daw::node_id_t> {
		size_t operator( )( daw::node_id_t id ) const noexcept {
			return std::hash<size_t>{}( id.value );
		}
	};
}

namespace daw {
	namespace graph_impl {
		template<typename T>
		class graph_node_impl_t {
			node_id_t m_id;
			T m_value;

		public:
			using value_type = T;
			using reference = value_type &;
			using const_reference = value_type const &;
			using edges_t = std::unordered_set<node_id_t>;

		private:
			edges_t m_incoming_edges{};
			edges_t m_outgoing_edges{};
		public:
			graph_node_impl_t( node_id_t id, T &&value ) noexcept
			  : m_id( id )
			  , m_value( std::move( value ) ) {}

			node_id_t id( ) const {
				return m_id;
			}

			reference value( ) {
				return m_value;
			}

			const_reference value( ) const {
				return m_value;
			}

			edges_t & incoming_edges( ) {
				return m_incoming_edges;
			}

			edges_t const & incoming_edges( ) const {
				return m_incoming_edges;
			}

			edges_t & outgoing_edges( ) {
				return m_outgoing_edges;
			}

			edges_t const & outgoing_edges( ) const {
				return m_outgoing_edges;
			}
		};
	} // namespace graph_impl

	class invalid_node_exception {
		std::optional<size_t> m_id = {};

	public:
		constexpr invalid_node_exception( ) noexcept = default;
		constexpr invalid_node_exception( size_t Id ) noexcept
		  : m_id( Id ) {}

		constexpr bool has_id( ) const {
			return static_cast<bool>( m_id );
		}
		constexpr size_t id( ) const {
			return *m_id;
		}
	};

	template<typename T>
	struct graph_t;

	template<typename T>
	class graph_node_t {
		graph_t<T> *m_graph;
		node_id_t m_node_id;

	public:
		using value_type = T;
		using reference = value_type &;
		using const_reference = value_type const &;
		using edges_t = typename graph_impl::graph_node_impl_t<T>::edges_t;

		graph_node_t( graph_t<T> *graph_ptr, node_id_t Id ) noexcept
		  : m_graph( graph_ptr )
		  , m_node_id( Id ) {}

		node_id_t id( ) const {
			return m_graph->get_raw_node( m_node_id ).id( );
		}

		reference value( ) {
			return m_graph->get_raw_node( m_node_id ).value( );
		}

		const_reference value( ) const {
			return m_graph->get_raw_node( m_node_id ).value( );
		}

		edges_t &incoming_edges( ) {
			return m_graph->get_raw_node( m_node_id ).incoming_edges( );
		}

		edges_t const &incoming_edges( ) const {
			return m_graph->get_raw_node( m_node_id ).incoming_edges( );
		}

		edges_t &outgoing_edges( ) {
			return m_graph->get_raw_node( m_node_id ).outgoing_edges( );
		}

		edges_t const &outgoing_edges( ) const {
			return m_graph->get_raw_node( m_node_id ).outgoing_edges( );
		}
	};

	template<typename T>
	class const_graph_node_t {
		graph_t<T> const *m_graph;
		node_id_t m_node_id;

	public:
		using value_type = T;
		using reference = value_type &;
		using const_reference = value_type const &;
		using edges_t = typename graph_impl::graph_node_impl_t<T>::edges_t;

		const_graph_node_t( graph_t<T> *graph_ptr, node_id_t Id ) noexcept
		  : m_graph( graph_ptr )
		  , m_node_id( Id ) {}

		node_id_t id( ) const {
			return m_graph->get_raw_node( m_node_id ).id( );
		}

		const_reference value( ) const {
			return m_graph->get_raw_node( m_node_id ).value( );
		}

		edges_t const &incoming_edges( ) const {
			return m_graph->get_raw_node( m_node_id ).incoming_edges( );
		}

		edges_t const &outgoing_edges( ) const {
			return m_graph->get_raw_node( m_node_id ).outgoing_edges( );
		}
	};

	template<typename T>
	struct graph_t {
		using raw_node_t = graph_impl::graph_node_impl_t<T>;

	private:
		size_t cur_id = 0;
		std::unordered_map<size_t, raw_node_t> m_nodes{};

	public:
		graph_t( ) noexcept = default;

		template<typename... Args>
		node_id_t add_node( Args &&... args ) {
			auto const id = cur_id++;
			m_nodes.emplace( std::make_pair(
			  id, graph_impl::graph_node_impl_t<T>(
			        node_id_t{ id }, daw::construct_a<T>{}( std::forward<Args>( args )... ) ) ) );

			return node_id_t{ id };
		}

		bool has_node( node_id_t id ) const {
			return m_nodes.count( id.value ) > 0;
		}

		// Be very careful when storing as the node
		// can be removed
		raw_node_t &get_raw_node( node_id_t id ) {
			daw::exception::dbg_precondition_check( has_node( id ) );
			auto pos = m_nodes.find( id.value );
			return pos->second;
		}

		// Be very careful when storing as the node
		// can be removed
		raw_node_t const &get_raw_node( node_id_t id ) const {
			daw::exception::dbg_precondition_check( has_node( id ) );
			auto pos = m_nodes.find( id.value );
			return pos->second;
		}

		const_graph_node_t<T> get_node( node_id_t id ) const {
			daw::exception::dbg_precondition_check( has_node( id ) );
			return graph_node_t<T>( this, id );
		}

		graph_node_t<T> get_node( node_id_t id ) {
			daw::exception::dbg_precondition_check( has_node( id ) );
			return graph_node_t<T>( this, id );
		}

		size_t size( ) const {
			return m_nodes.size( );
		}

		void add_directed_edge( node_id_t from, node_id_t to ) {
			daw::exception::dbg_precondition_check( has_node( from ) );
			daw::exception::dbg_precondition_check( has_node( to ) );

			auto & n_from = get_raw_node( from );
			auto & n_to = get_raw_node( to );

			n_from.outgoing_edges( ).insert( to );
			n_to.incoming_edges( ).insert( from );
		}

		void add_undirected_edge( node_id_t node0, node_id_t node1 ) {
			daw::exception::dbg_precondition_check( has_node( node0 ) );
			daw::exception::dbg_precondition_check( has_node( node1 ) );

			auto & n_0 = get_raw_node( node0 );
			auto & n_1 = get_raw_node( node1 );

			n_0.outgoing_edges( ).insert( node1 );
			n_0.incoming_edges( ).insert( node1 );
			n_1.outgoing_edges( ).insert( node0 );
			n_1.incoming_edges( ).insert( node0 );
		}

		void remove_node( node_id_t id ) {
			daw::exception::dbg_precondition_check( has_node( id ) );

			auto & node = get_raw_node( id );
			for( auto n_id : node.incoming_edges( ) ) {
				get_node( n_id ).outgoing_edges( ).erase( id );
			}
			for( auto n_id : node.outgoing_edges( ) ) {
				get_node( n_id ).incoming_edges( ).erase( id );
			}
			m_nodes.erase( id.value );
		}

		template<typename Compare = std::equal_to<>>
		std::vector<node_id_t> find_by_value( T const & value, Compare compare = Compare{} ) const {
			std::vector<node_id_t> result{};
			for( auto const & node: m_nodes ) {
				if( daw::invoke( compare, node.second.value( ), value ) ) {
					result.push_back( node.second.id( ) );
				}
			}
			return result;
		}

		template<typename Predicate>
		std::vector<node_id_t> find( Predicate&& pred ) const {
			static_assert( std::is_invocable_v<Predicate, raw_node_t const &>, "Predicate must accept a node as argument" );
			std::vector<node_id_t> result{};
			for( auto const & node: m_nodes ) {
				if( daw::invoke( pred, node.second ) ) {
					result.push_back( node.second.id( ) );
				}
			}
			return result;
		}
	};

} // namespace daw
