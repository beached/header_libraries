// The MIT License (MIT)
//
// Copyright (c) 2013-2015 Darrell Wright
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

#include <iostream>
#include <memory>
#include <string>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/iostreams/device/mapped_file.hpp>

namespace daw {
	namespace filesystem {
		template<typename T>
		class MemoryMappedFile {
		private:
			//TODO remove size_t const m_max_buff_size = 1048576;
			boost::filesystem::path m_file_path;
			boost::iostreams::mapped_file_params m_mf_params;
			boost::iostreams::mapped_file m_mf_file;
		public:
			using iterator = T *;
			using const_iterator = T const *;
			using value_type = T;
			using reference = T &;
			using const_reference = T const &;

			MemoryMappedFile( std::string const & filename, bool const readonly = true ) : m_file_path( filename ), m_mf_params( filename ) {
				m_mf_params.flags = boost::iostreams::mapped_file::mapmode::readwrite;
				if( readonly ) {
					//FIXME: seems to crash	m_mf_params.flags = boost::iostreams::mapped_file::mapmode::readonly;
				}
				m_mf_params.offset = 0;
				try {
					m_mf_file.open( m_mf_params );
				} catch( const std::exception& ex ) {
					std::cerr << "Error Opening memory mapped file '" << filename << "': " << ex.what( ) << std::endl;
					throw ex;
				}
			}

			MemoryMappedFile( ) = delete;

			MemoryMappedFile( MemoryMappedFile&& other ) :
				m_file_path( std::move( other.m_file_path ) ),
				m_mf_params( std::move( other.m_mf_params ) ),
				m_mf_file( std::move( other.m_mf_file ) ) { }

			MemoryMappedFile& operator=(MemoryMappedFile&& rhs) {
				if( &rhs != this ) {
					m_file_path = std::move( rhs.m_file_path );
					m_mf_params = std::move( rhs.m_mf_params );
					m_mf_file = std::move( rhs.m_mf_file );
				}
				return *this;
			}

			MemoryMappedFile( const MemoryMappedFile& ) = delete;
			MemoryMappedFile& operator=(const MemoryMappedFile&) = delete;

			void close( ) {
				if( m_mf_file.is_open( ) ) {
					m_mf_file.close( );
				}
			}

			~MemoryMappedFile( ) {
				try {
					close( );
				} catch( ... ) {
					std::cerr << "Exception while closing memory mapped file" << std::endl;
				}
			}

			bool is_open( ) const {
				return m_mf_file.is_open( );
			}

			reference operator[]( size_t const & position ) {
				return m_mf_file.data( )[position];
			}

			const_reference operator[]( size_t const & position ) const {
				return m_mf_file.data( )[position];
			}

			const_iterator data( size_t const & position = 0 ) const {
				return m_mf_file.data( ) + static_cast<boost::iostreams::stream_offset>(position);
			}

			void swap( MemoryMappedFile& other ) {
				using std::swap;
				swap( m_mf_params, other.m_mf_params );
				swap( m_mf_file, other.m_mf_file );
			}

			size_t size( ) const {
				return m_mf_file.size( );
			}

			iterator begin( ) {
				return m_mf_file.begin( );
			}

			const_iterator begin( ) const {
				return m_mf_file.begin( );
			}

			iterator end( ) {
				return m_mf_file.end( );
			}

			const_iterator end( ) const {
				return m_mf_file.end( );
			}

			const_iterator cbegin( ) const {
				return m_mf_file.begin( );
			}

			const_iterator cend( ) const {
				return m_mf_file.end( );	
			}

		};

		template<typename T>
		void swap( MemoryMappedFile<T>& lhs, MemoryMappedFile<T>& rhs ) noexcept {
			lhs.swap( rhs );
		}

	}	// namespace filesystem
}	// namespace daw

