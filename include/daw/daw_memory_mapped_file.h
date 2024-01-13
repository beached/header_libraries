// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "daw_string_view.h"
#include "daw_traits.h"
#include "daw_unique_resource.h"
#include "traits/daw_traits_conditional.h"

#include <cstddef>
#include <cstdio>
#include <string_view>
#include <utility>

#if not defined( _MSC_VER ) and not defined( __MINGW32__ )
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <type_traits>
#include <unistd.h>
#else
#include <algorithm>
#include <cstdio>
#include <tchar.h>
#include <windows.h>
#endif

namespace daw::filesystem {
	enum class open_mode : bool { read, read_write };

#if not defined( _MSC_VER ) and not defined( __MINGW32__ )
	/// @brief A RAII Memory Mapped File object
	template<typename T = char>
	struct memory_mapped_file_t {
		using value_type = T;
		using reference = T &;
		using const_reference =
		  conditional_t<std::is_const_v<T>, T, T const> &;
		using pointer = T *;
		using const_pointer = conditional_t<std::is_const_v<T>, T, T const> *;
		using size_type = size_t;

	private:
		struct fdata_t {
			int file = -1;
			pointer ptr = nullptr;
			size_type size = 0;
		};

		struct cleanup_t {
			constexpr void operator( )( fdata_t &d ) noexcept {
				if( auto *p = std::exchange( d.ptr, nullptr ); p ) {
					munmap( p, d.size );
				}
				d.size = 0;
				if( auto fid = std::exchange( d.file, -1 ); fid >= 0 ) {
					close( fid );
				}
			}
		};
		daw::unique_resource<fdata_t, cleanup_t> m_fdata;

	public:
		explicit memory_mapped_file_t( ) = default;

		/// @brief Open memory mapped file with file path supplied
		/// @pre *file_path.end( ) == '\0'
		explicit memory_mapped_file_t( daw::string_view file_path,
		                               open_mode mode = open_mode::read ) noexcept {

			(void)open( file_path, mode );
		}

		/// @brief Open memory mapped file with file path supplied
		/// @pre *file.end( ) == '\0'
		[[nodiscard]] bool open( daw::string_view file,
		                         open_mode mode = open_mode::read ) noexcept {

			m_fdata.emplace( );
			m_fdata->file =
			  ::open( file.data( ), mode == open_mode::read ? O_RDONLY : O_RDWR );
			if( m_fdata->file < 0 ) {
				return false;
			}
			{
				auto const fsz = lseek( m_fdata->file, 0, SEEK_END );
				lseek( m_fdata->file, 0, SEEK_SET );
				if( fsz <= 0 ) {
					m_fdata.reset( );
					return false;
				}
				m_fdata->size = static_cast<size_type>( fsz );
			}
			m_fdata->ptr = static_cast<pointer>(
			  mmap( nullptr, m_fdata->size,
			        mode == open_mode::read ? PROT_READ : PROT_READ | PROT_WRITE,
			        MAP_SHARED, m_fdata->file, 0 ) );

			if( m_fdata->ptr == MAP_FAILED ) {
				m_fdata.reset( );
				return false;
			}
			return true;
		}

		[[nodiscard]] reference operator[]( size_type pos ) noexcept {
			return m_fdata->ptr[pos];
		}

		[[nodiscard]] const_reference operator[]( size_t pos ) const noexcept {
			return m_fdata->ptr[pos];
		}

		[[nodiscard]] constexpr pointer data( ) noexcept {
			return m_fdata->ptr;
		}

		[[nodiscard]] constexpr const_pointer data( ) const noexcept {
			return m_fdata->ptr;
		}

		[[nodiscard]] constexpr pointer begin( ) noexcept {
			return m_fdata->ptr;
		}

		[[nodiscard]] constexpr const_pointer begin( ) const noexcept {
			return m_fdata->ptr;
		}

		[[nodiscard]] constexpr const_pointer cbegin( ) const noexcept {
			return m_fdata->ptr;
		}

		[[nodiscard]] constexpr pointer end( ) noexcept {
			return m_fdata->ptr + static_cast<ptrdiff_t>( m_fdata->size );
		}

		[[nodiscard]] constexpr const_pointer end( ) const noexcept {
			return m_fdata->ptr + static_cast<ptrdiff_t>( m_fdata->size );
		}

		[[nodiscard]] constexpr const_pointer cend( ) const noexcept {
			return m_fdata->ptr + static_cast<ptrdiff_t>( m_fdata->size );
		}

		[[nodiscard]] constexpr size_type size( ) const noexcept {
			return m_fdata->size;
		}

		[[nodiscard]] constexpr bool empty( ) const noexcept {
			return not( *this ) or m_fdata->size == 0;
		}

		constexpr explicit operator bool( ) const noexcept {
			return m_fdata->file >= 0 and m_fdata->ptr != nullptr;
		}

		template<typename Traits>
		operator std::basic_string_view<T, Traits>( ) const {
			return { data( ), size( ) };
		}
	};
#else
	namespace mapfile_impl {
		static constexpr long CreateFileMode( open_mode m ) {
			if( m == open_mode::read ) {
				return GENERIC_READ;
			}
			return GENERIC_READ | GENERIC_WRITE;
		}

		static constexpr long PageMode( open_mode m ) {
			if( m == open_mode::read ) {
				return PAGE_READONLY;
			}
			return PAGE_READWRITE;
		}

		static constexpr long MapMode( open_mode m ) {
			if( m == open_mode::read ) {
				return FILE_MAP_READ;
			}
			return FILE_MAP_WRITE;
		}
	} // namespace mapfile_impl

	/// @brief A RAII Memory Mapped File object
	template<typename T = char>
	struct memory_mapped_file_t {
		using value_type = T;
		using reference = T &;
		using const_reference =
		  typename std::conditional<std::is_const_v<T>, T, T const>::type &;
		using pointer = T *;
		using const_pointer =
		  typename std::conditional<std::is_const_v<T>, T, T const>::type *;
		using size_type = size_t;

	private:
		struct fdata_t {
			HANDLE handle = nullptr;
			size_t size = 0;
			pointer ptr = nullptr;
		};

		struct cleanup_t {
			constexpr void operator( )( fdata_t &d ) noexcept {
				d.size = 0;
				if( auto tmp = std::exchange( d.ptr, nullptr ); tmp ) {
					::UnmapViewOfFile( static_cast<LPVOID>( tmp ) );
				}
				if( auto tmp = std::exchange( d.handle, nullptr ); tmp ) {
					::CloseHandle( d.handle );
				}
			}
		};

		daw::unique_resource<fdata_t, cleanup_t> m_fdata;

	public:
		explicit memory_mapped_file_t( ) = default;

		/// @brief Open memory mapped file with file path supplied
		/// @pre *file_path.end( ) == '\0'
		explicit memory_mapped_file_t( daw::string_view file_path,
		                               open_mode mode = open_mode::read ) noexcept {

			(void)open( file_path, mode );
		}

		/// @brief Open memory mapped file with file path supplied
		/// @pre *file_path.end( ) == '\0'
		explicit memory_mapped_file_t( daw::wstring_view file_path,
		                               open_mode mode = open_mode::read ) noexcept {

			(void)open( file_path, mode );
		}

		/// @brief Open memory mapped file with file path supplied
		/// @pre *file.end( ) == '\0'
		[[nodiscard]] bool open( daw::string_view file,
		                         open_mode mode = open_mode::read ) noexcept {

			m_fdata.emplace( );
			{
				HANDLE file_handle = ::CreateFileA(
				  file.data( ), mapfile_impl::CreateFileMode( mode ), 0, nullptr,
				  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr );
				if( file_handle == INVALID_HANDLE_VALUE ) {
					return false;
				}
				LARGE_INTEGER fsz;
				if( not ::GetFileSizeEx( file_handle, &fsz ) or fsz.QuadPart <= 0 ) {
					m_fdata.reset( );
					return false;
				}
				m_fdata->size = static_cast<size_t>( fsz.QuadPart );
				m_fdata->handle = ::CreateFileMapping(
				  file_handle, nullptr, mapfile_impl::PageMode( mode ), fsz.u.HighPart,
				  fsz.u.LowPart, nullptr );
				if( m_fdata->handle == nullptr ) {
					m_fdata.reset( );
					return false;
				}
				CloseHandle( file_handle );
			}
			auto ptr = MapViewOfFile( m_fdata->handle, mapfile_impl::MapMode( mode ),
			                          0, 0, 0 );
			if( ptr == nullptr ) {
				m_fdata.reset( );
				return false;
			}
			m_fdata->ptr = static_cast<pointer>( ptr );
			return true;
		}

		/// @brief Open memory mapped file with file path supplied
		/// @pre *file.end( ) == '\0'
		[[nodiscard]] bool open( daw::wstring_view file,
		                         open_mode mode = open_mode::read ) noexcept {

			{
				HANDLE file_handle = ::CreateFileW(
				  file.data( ), mapfile_impl::CreateFileMode( mode ), 0, nullptr,
				  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr );
				if( file_handle == INVALID_HANDLE_VALUE ) {
					return false;
				}
				LARGE_INTEGER fsz;
				if( not ::GetFileSizeEx( file_handle, &fsz ) or fsz.QuadPart <= 0 ) {
					m_fdata.reset( );
					return false;
				}
				m_fdata->size = static_cast<size_t>( fsz.QuadPart );
				m_fdata->handle = ::CreateFileMapping(
				  file_handle, nullptr, mapfile_impl::PageMode( mode ), fsz.u.HighPart,
				  fsz.u.LowPart, nullptr );
				if( m_fdata->handle == nullptr ) {
					m_fdata.reset( );
					return false;
				}
				CloseHandle( file_handle );
			}
			auto ptr = MapViewOfFile( m_fdata->handle, mapfile_impl::MapMode( mode ),
			                          0, 0, 0 );
			if( ptr == nullptr ) {
				m_fdata.reset( );
				return false;
			}
			m_fdata->ptr = static_cast<pointer>( ptr );
			return true;
		}

		[[nodiscard]] reference operator[]( size_type pos ) noexcept {
			return m_fdata->ptr[pos];
		}

		[[nodiscard]] const_reference operator[]( size_t pos ) const noexcept {
			return m_fdata->ptr[pos];
		}

		[[nodiscard]] constexpr pointer data( ) noexcept {
			return m_fdata->ptr;
		}

		[[nodiscard]] constexpr const_pointer data( ) const noexcept {
			return m_fdata->ptr;
		}

		[[nodiscard]] constexpr size_type size( ) const noexcept {
			return m_fdata->size;
		}

		[[nodiscard]] constexpr bool empty( ) const noexcept {
			return not( *this ) or m_fdata->size == 0;
		}

		constexpr explicit operator bool( ) const noexcept {
			return m_fdata->size == 0 or m_fdata->ptr == nullptr or
			       m_fdata->handle == nullptr;
		}

		operator std::basic_string_view<T>( ) const {
			return { data( ), size( ) };
		}

		operator std::basic_string_view<T>( ) {
			return { data( ), size( ) };
		}
	};
#endif
} // namespace daw::filesystem
