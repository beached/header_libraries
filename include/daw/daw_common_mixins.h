// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "daw_move.h"

#include <iterator>
#include <utility>

namespace daw::mixins {
	template<typename Derived, typename ContainerType>
	struct ContainerProxy {
		using container_type = ContainerType;
		using iterator = typename container_type::iterator;
		using const_iterator = typename container_type::const_iterator;
		using value_type = typename container_type::value_type;
		using reference = typename container_type::reference;
		using const_reference = typename container_type::const_reference;
		using size_type = typename container_type::size_type;

	private:
		Derived &derived( ) {
			return *static_cast<Derived *>( this );
		}

		Derived const &derived( ) const {
			return *static_cast<Derived const *>( this );
		}

	public:
		[[nodiscard]] iterator begin( ) {
			return derived( ).container( ).begin( );
		}

		[[nodiscard]] iterator end( ) {
			return derived( ).container( ).end( );
		}

		[[nodiscard]] const_iterator begin( ) const {
			return derived( ).container( ).begin( );
		}

		[[nodiscard]] const_iterator end( ) const {
			return derived( ).container( ).end( );
		}

		[[nodiscard]] const_iterator cbegin( ) {
			return derived( ).container( ).cbegin( );
		}

		[[nodiscard]] const_iterator cend( ) {
			return derived( ).container( ).cend( );
		}

		iterator insert( iterator where, value_type item ) {
			return derived( ).container( ).insert( where, std::move( item ) );
		}

		template<typename... Args>
		void emplace( iterator where, Args &&...args ) {
			return derived( ).container( ).emplace( where,
			                                        DAW_FWD( args )... );
		}

		[[nodiscard]] size_type size( ) const {
			return std::distance( cbegin( ), cend( ) );
		}
	};

	//////////////////////////////////////////////////////////////////////////
	/// Summary:	Provides a minimal interface for a vector like container
	///				with a member that is a container.
	///				Requires a member in Derived called container( ) that
	///				returns the container
	template<typename Derived, typename container_type>
	class VectorLikeProxy : public ContainerProxy<Derived, container_type> {
		using base_t = ContainerProxy<Derived, container_type>;

	public:
		void push_back( typename base_t::value_type &&value ) {
			this->insert( this->end( ), std::move( value ) );
		}

		void push_back( typename base_t::const_reference value ) {
			this->insert( this->end( ), value );
		}

		template<typename... Args>
		void emplace_back( Args &&...args ) {
			this->emplace( this->end( ), DAW_FWD( args )... );
		}

		[[nodiscard]] typename base_t::reference
		operator[]( typename base_t::size_type pos ) {
			return *( this->begin( ) + pos );
		}

		[[nodiscard]] typename base_t::const_reference
		operator[]( typename base_t::size_type pos ) const {
			return *( this->cbegin( ) + pos );
		}
	};

	/////////////////////////////////////11/////////////////////////////////////
	/// Summary:	Provides a minimal interface for a map like container
	///				with a member that is a container.
	///				Requires a member in Derived called container( ) that
	///				returns the container, find( ) that searches
	///				for a key and key_type and mapped_type
	template<typename Derived, typename MapType>
	struct MapLikeProxy : public ContainerProxy<Derived, MapType> {
		using key_type = typename MapType::key_type;
		using mapped_type = typename MapType::mapped_type;

	private:
		using base_t = ContainerProxy<Derived, MapType>;

	public:
		[[nodiscard]] typename base_t::reference operator[]( key_type key ) {
			return this->derived( ).find( key );
		}

		[[nodiscard]] typename base_t::const_reference
		operator[]( key_type key ) const {
			return this->derived( ).find( key );
		}
	};
} // namespace daw::mixins
