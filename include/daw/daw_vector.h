// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "ciso646.h"
#include "daw_algorithm.h"
#include "daw_check_exceptions.h"
#include "daw_cxmath.h"
#include "daw_likely.h"
#include "daw_move.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <iterator>
#include <memory>
#include <numeric>
#if not( defined( _WIN32 ) or defined( _WIN64 ) )
#include <sys/mman.h>
#endif
#include <sys/types.h>
#include <type_traits>
#if __has_include( <unistd.h> )
#include <unistd.h>
#endif

#if defined( _WIN32 ) or defined( _WIN64 )
#include <malloc.h>
#define MALLOC_SIZE( ... ) _msize( __VA_ARGS__ )
#elif defined( __linux__ )
#include <malloc.h>
#define MALLOC_SIZE( ... ) malloc_usable_size( __VA_ARGS__ )
#else
#include <malloc/malloc.h>
#define MALLOC_SIZE( ... ) malloc_size( __VA_ARGS__ )
#endif

#if defined( __cpp_constexpr_dynamic_alloc )
#define DAW_CPP20CXDTOR constexpr
#else
#define DAW_CPP20CXDTOR
#endif

namespace daw {
	template<typename T, typename U, typename = void>
	struct has_op_minus : std::false_type {};

	template<typename T, typename U>
	struct has_op_minus<
	  T, U, std::void_t<decltype( std::declval<T>( ) - std::declval<U>( ) )>>
	  : std::true_type {};

	template<typename T, typename = void>
	struct is_iterator : std::false_type {};

	template<typename T>
	struct is_iterator<
	  T, std::void_t<decltype( std::add_pointer_t<typename std::iterator_traits<
	                             T>::value_type>{ } )>> : std::true_type {};

	template<typename T, typename = void>
	struct has_reallocate : std::false_type {};

	template<typename T>
	struct has_reallocate<T, std::void_t<typename T::has_realloc>>
	  : std::true_type {};

	namespace vector_details {
		inline std::size_t round_to_page_size( std::size_t sz ) {
			static std::size_t const page_size = [] {
				int res = ::getpagesize( );
				if( res <= 0 ) {
					res = 4096U;
				}
				return static_cast<std::size_t>( res );
			}( );
			auto result =
			  static_cast<std::size_t>( std::round(
			    static_cast<double>( sz ) / static_cast<double>( page_size ) +
			    0.5 ) ) *
			  page_size;
			return result;
		}
	} // namespace vector_details

	template<typename T>
	struct MMapAlloc {
		using value_type = T;
		using pointer = T *;
		using size_type = std::size_t;
		using has_realloc = void;

	private:
		[[nodiscard]] static inline pointer allocate_raw( size_type count ) {
			T *result =
			  reinterpret_cast<T *>( ::mmap( nullptr, count, PROT_READ | PROT_WRITE,
			                                 MAP_ANONYMOUS | MAP_PRIVATE, -1, 0 ) );
			if( DAW_UNLIKELY( not result ) ) {
				DAW_THROW_OR_TERMINATE_NA( std::bad_alloc );
			}
			return result;
		}

	public:
		constexpr MMapAlloc( ) = default;

		[[nodiscard]] static inline pointer allocate( size_type count ) {
			return allocate_raw(
			  vector_details::round_to_page_size( sizeof( T ) * count ) );
		}

		[[nodiscard]] static inline size_type alloc_size( T * ) {
			return 0U;
		}
		// Attempt to reallocate if it can be done within the current block
		// The ptr returned should
		[[nodiscard]] static inline pointer
		reallocate( T *old_ptr, size_type old_size, size_type new_size ) {
			new_size = vector_details::round_to_page_size( new_size * sizeof( T ) );
#if defined( MREMAP_FIXED )
			old_size = vector_details::round_to_page_size( old_size * sizeof( T ) );
			if( old_ptr == nullptr ) {
				return allocate_raw( new_size );
			}
			T *new_ptr = reinterpret_cast<T *>(
			  ::mremap( old_ptr, old_size, new_size, MREMAP_FIXED ) );
			if( new_ptr == nullptr ) {
				DAW_THROW_OR_TERMINATE( std::bad_alloc );
			}
			return new_ptr;
#else
			(void)old_ptr;
			(void)old_size;
			return allocate_raw( new_size );
#endif
		}

		static inline void deallocate( T *ptr, size_type sz ) {
			::munmap( ptr, vector_details::round_to_page_size( sz ) );
		}
	};

	template<typename T>
	struct MallocAlloc {
		using value_type = T;
		using pointer = T *;
		using size_type = std::size_t;
		using has_realloc = void;

	private:
		[[nodiscard]] static inline pointer allocate_raw( size_type count ) {
			count = vector_details::round_to_page_size( count );
			auto result = reinterpret_cast<T *>( std::malloc( count ) );
			if( not result ) {
				DAW_THROW_OR_TERMINATE( std::bad_alloc );
			}
			return result;
		}

	public:
		constexpr MallocAlloc( ) = default;

		[[nodiscard]] static inline pointer allocate( size_type count ) {
			return allocate_raw( sizeof( T ) * count );
		}

		[[nodiscard]] static inline size_type alloc_size( T *ptr ) {
#if defined( MALLOC_SIZE )
			return MALLOC_SIZE( ptr );
#else
			return 0U;
#endif
		}
		// Attempt to reallocate if it can be done within the current block
		// The ptr returned should
		[[nodiscard]] static inline pointer
		reallocate( T *old_ptr, size_type /*old_size*/, size_type new_size ) {
			auto const max_sz = alloc_size( old_ptr );
			if( new_size < ( max_sz / sizeof( T ) ) ) {
				pointer new_ptr = reinterpret_cast<pointer>(
				  realloc( old_ptr, sizeof( T ) * new_size ) );
				assert( new_ptr == old_ptr );
				return new_ptr;
			}
			return allocate_raw( sizeof( T ) * new_size );
		}

		static inline void deallocate( T *ptr, size_type ) {
			std::free( ptr );
		}
	};

	template<typename T, typename... Args>
	constexpr T *construct_at( T *p, Args &&...args ) {
		if constexpr( std::is_aggregate_v<T> ) {
			return ::new( static_cast<void *>( p ) ) T{ DAW_FWD2( Args, args )... };
		} else {
			return ::new( static_cast<void *>( p ) ) T( DAW_FWD2( Args, args )... );
		}
	}

	template<typename Iterator, typename OutputIterator>
	constexpr OutputIterator move_n( Iterator first, std::size_t sz,
	                                 OutputIterator out_it ) noexcept {
		return std::uninitialized_move_n( first, sz, out_it ).second;
	}

	template<typename Iterator, typename OutputIterator>
	constexpr OutputIterator copy_n( Iterator first, std::size_t sz,
	                                 OutputIterator out_it ) noexcept {
		return std::uninitialized_copy_n( first, sz, out_it );
	}

	template<typename Allocator>
	struct AllocDeleter : Allocator {
		using pointer = typename std::allocator_traits<Allocator>::pointer;

	private:
		std::size_t alloc_size = 0;

	public:
		constexpr AllocDeleter( ) = default;

		constexpr AllocDeleter( std::size_t sz, Allocator a = Allocator{ } )
		  : Allocator{ a }
		  , alloc_size( sz ) {}

		template<typename T>
		constexpr void operator( )( T *p ) const {
			this->deallocate( p, alloc_size );
		}

		constexpr void set_capacity( std::size_t new_cap ) {
			alloc_size = new_cap;
		}

		constexpr std::size_t get_capacity( ) const {
			return alloc_size;
		}
	};

	struct sized_for_overwrite_t {};
	inline constexpr sized_for_overwrite_t sized_for_overwrite =
	  sized_for_overwrite_t{ };

	template<typename T, typename Alloc = MallocAlloc<T>>
	struct Vector : private Alloc {
		using value_type = T;
		using allocator_type = Alloc;
		using pointer = value_type *;
		using const_pointer = value_type const *;
		using reference = value_type &;
		using const_reference = value_type const &;
		using rvalue_reference = value_type &&;
		using iterator = value_type *;
		using const_iterator = value_type const *;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

	private:
		using uptr_del = AllocDeleter<allocator_type>;
		using uptr_t = std::unique_ptr<value_type[], uptr_del>;
		pointer m_first = nullptr;
		pointer m_size = nullptr;
		pointer m_capacity = nullptr;

		uptr_t make_copy( Vector const &other ) {
			auto result =
			  uptr_t( this->allocate( other.size( ) ), uptr_del{ other.size( ) } );
			copy_n( other.data( ), other.size( ), result.get( ) );
			return result;
		}

		explicit constexpr Vector( sized_for_overwrite_t, size_type sz )
		  : m_first( this->allocate( vector_details::round_to_page_size( sz ) ) )
		  , m_size( m_first + static_cast<difference_type>( sz ) )
		  , m_capacity( m_first + vector_details::round_to_page_size( sz ) ) {}

		constexpr size_type calc_new_size( size_type sz ) noexcept {
			if constexpr( has_reallocate<allocator_type>::value ) {
				auto const avail = allocator_type::alloc_size( data( ) );
				if( sz <= avail ) {
					return avail;
				}
			}
			auto result =
			  vector_details::round_to_page_size( std::max( sz, capacity( ) * 2U ) );
			return result;
		}

		static constexpr void relocate( pointer source, size_type sz,
		                                pointer destination ) {
			if constexpr( std::is_trivially_copyable_v<value_type> ) {
#if DAW_HAS_BUILTIN( __builtin_memcpy )
				(void)__builtin_memcpy( destination, source, sz * sizeof( T ) );
#else
				(void)std::memcpy( destination, source, sz * sizeof( T ) );
#endif
			} else if constexpr( std::is_nothrow_move_constructible_v<T> ) {
				(void)move_n( source, sz, destination );
			} else {
				(void)copy_n( source, sz, destination );
			}
		}

		static constexpr void overlapped_relocate( pointer source,
		                                           size_type where_idx,
		                                           size_type range_size,
		                                           size_type insert_size ) {
			assert( where_idx <= range_size );
			if( insert_size == 0 ) {
				return;
			}
			size_type const reloc_size = range_size - insert_size;
			auto dfirst =
			  static_cast<difference_type>( ( range_size + insert_size ) - 1U );
#if not defined( NDEBUG )
			auto dlast = static_cast<difference_type>( ( range_size + insert_size ) -
			                                           reloc_size );
#endif
			auto sfirst = static_cast<difference_type>( range_size - 1U );
			auto slast = static_cast<difference_type>( range_size - reloc_size );

			while( sfirst >= slast ) {
				if constexpr( std::is_nothrow_move_constructible_v<value_type> ) {
					construct_at( source + dfirst, std::move( source[sfirst] ) );
				} else {
					construct_at( source + dfirst, source[sfirst] );
				}
				--dfirst;
				--sfirst;
			}
			assert( dfirst < dlast );
		}

		constexpr void resize_impl( size_type sz ) noexcept {
			pointer const old_ptr = data( );
			if( sz <= size( ) ) {
				if constexpr( std::is_trivially_destructible_v<value_type> ) {
					m_size = m_first + static_cast<difference_type>( sz );
					return;
				} else {
					if( empty( ) ) {
						return;
					}
					while( m_size > ( m_first + static_cast<difference_type>( sz ) ) ) {
						std::destroy_at( --m_size );
					}
				}
			} else {
				auto const old_cap = capacity( );
				if( sz >= old_cap ) {
					size_type new_size = calc_new_size( sz );
					pointer const new_ptr = [&] {
						if constexpr( has_reallocate<allocator_type>::value ) {
							return this->reallocate( old_ptr, size( ), new_size );
						} else {
							return this->allocate( new_size );
						}
					}( );
					if( new_ptr != old_ptr ) {
						auto const old_sz = size( );
						relocate( old_ptr, old_sz, new_ptr );
						clear( );
						this->deallocate( m_first, capacity( ) );
						m_first = new_ptr;
						m_size = m_first + static_cast<difference_type>( old_sz );
					}
					m_capacity = m_first + new_size;
				}
			}
		}

	public:
		/// @brief Construct an empty Vector
		/// @post empty( ) == true
		constexpr Vector( ) = default;
		/// @brief Construct an empty Vector with a specific instance of allocator
		/// @post empty( ) == true
		constexpr Vector( allocator_type const &alloc )
		  : allocator_type{ alloc } {}

		constexpr Vector( Vector &&other ) noexcept
		  : allocator_type( std::move( other ) )
		  , m_first( std::exchange( other.m_first, nullptr ) )
		  , m_size( std::exchange( other.m_size, nullptr ) )
		  , m_capacity( std::exchange( other.m_capacity, nullptr ) ) {}

		constexpr Vector &operator=( Vector &&rhs ) noexcept {
			if( this != &rhs ) {
				clear( );
				allocator_type::operator=( std::move( rhs ) );
				m_first = std::exchange( rhs.m_first, nullptr );
				m_size = std::exchange( rhs.m_size, nullptr );
				m_capacity = std::exchange( rhs.m_capacity, nullptr );
			}
			return *this;
		}

		template<typename Iterator, std::enable_if_t<is_iterator<Iterator>::value,
		                                             std::nullptr_t> = nullptr>
		explicit constexpr Vector( Iterator p, size_type sz )
		  : m_first( this->allocate( vector_details::round_to_page_size( sz ) ) )
		  , m_capacity( m_first + static_cast<difference_type>(
		                            vector_details::round_to_page_size( sz ) ) ) {

			m_size = daw::algorithm::copy_n( p, m_first, sz ).output;
		}

		constexpr void clear( ) {
			while( m_size != m_first ) {
				std::destroy_at( --m_size );
			}
		}

		DAW_CPP20CXDTOR ~Vector( ) {
			clear( );
			if( m_first ) {
				this->deallocate( m_first, capacity( ) );
			}
		}

		constexpr Vector( Vector const &other )
		  : allocator_type( other )
		  , m_first( make_copy( other ).release( ) )
		  , m_size( m_first + static_cast<difference_type>( other.size( ) ) )
		  , m_capacity( m_first +
		                static_cast<difference_type>( other.capacity( ) ) ) {}

		constexpr Vector &operator=( Vector const &rhs ) {
			if( this != &rhs ) {
				clear( );
				allocator_type::operator=( rhs );
				m_first = make_copy( rhs ).release( );
				m_size = m_first + static_cast<difference_type>( rhs.size( ) );
				m_capacity = m_first + static_cast<difference_type>( rhs.capacity( ) );
			}
			return *this;
		}

		template<typename Operation>
		explicit constexpr Vector( sized_for_overwrite_t, size_type sz,
		                           Operation op )
		  : m_first( this->allocate( vector_details::round_to_page_size( sz ) ) )
		  , m_size( m_first + static_cast<difference_type>( sz ) )
		  , m_capacity( m_first + static_cast<difference_type>(
		                            vector_details::round_to_page_size( sz ) ) ) {

			m_size = m_first + static_cast<difference_type>( op( m_first, size( ) ) );
			assert( size( ) <= sz );
		}

		template<size_type N>
		explicit constexpr Vector( value_type const ( &ary )[N] )
		  : Vector( ary, N ) {}

		template<size_type N>
		explicit constexpr Vector( value_type ( &&ary )[N] )
		  : Vector( sized_for_overwrite, N ) {

			pointer const p = move_n( ary, N, m_first );
			m_size = p;
		}

		template<typename IteratorF, typename IteratorL,
		         std::enable_if_t<( has_op_minus<IteratorF, IteratorL>::value and
		                            is_iterator<IteratorF>::value ),
		                          std::nullptr_t> = nullptr>
		constexpr Vector( IteratorF first, IteratorL last )
		  : Vector( first, static_cast<size_type>( last - first ) ) {}

		template<
		  typename IteratorF, typename IteratorL,
		  std::enable_if_t<( not has_op_minus<IteratorF, IteratorL>::value and
		                     is_iterator<IteratorF>::value ),
		                   std::nullptr_t> = nullptr>
		constexpr Vector( IteratorF first, IteratorL last )
		  : Vector( ) {
			while( first != last ) {
				push_back( *first );
				++first;
			}
		}

		template<typename Operation>
		constexpr void resize_for_overwrite( size_type sz, Operation op ) noexcept {
			resize_impl( sz );
			m_size = m_first + static_cast<difference_type>( op( data( ), sz ) );
		}

		[[nodiscard]] constexpr reference back( ) {
			return m_size[-1];
		}

		[[nodiscard]] constexpr const_reference back( ) const {
			return m_size[-1];
		}

		[[nodiscard]] constexpr reference front( ) {
			return *m_first;
		}

		[[nodiscard]] constexpr const_reference front( ) const {
			return *m_first;
		}

		constexpr void pop_back( ) {
			if( not std::is_trivially_destructible_v<value_type> ) {
				std::destroy_at( --m_size );
			} else {
				--m_size;
			}
		}

		constexpr void resize( size_type sz ) noexcept {
			resize_for_overwrite( sz, [&]( pointer p, size_type ) {
				if( sz < size( ) ) {
					while( size( ) > sz ) {
						pop_back( );
					}
				} else if( sz > size( ) ) {
					(void)std::fill_n( p + size( ), sz - size( ), value_type{ } );
				}
				return sz;
			} );
		}

		template<typename U>
		constexpr void resize( size_type sz, U const &value ) noexcept {
			resize_for_overwrite( sz, [&]( pointer p, size_type ) {
				if( size( ) > sz ) {
					do {
						pop_back( );
					} while( size( ) > sz );
				} else if( size( ) < sz ) {
					(void)std::fill_n( p + size( ), sz - size( ), value );
				}
			} );
		}

		[[nodiscard]] constexpr pointer data( ) {
			return m_first;
		}

		[[nodiscard]] constexpr const_pointer data( ) const {
			return m_first;
		}

		[[nodiscard]] constexpr size_type size( ) const {
			return static_cast<size_type>( m_size - m_first );
		}

		[[nodiscard]] static size_type max_size( ) {
			return static_cast<size_type>(
			  std::numeric_limits<difference_type>::max( ) );
		}

		[[nodiscard]] constexpr bool empty( ) const {
			return m_size == m_first;
		}

		[[nodiscard]] constexpr size_type capacity( ) const {
			return static_cast<size_type>( m_capacity - m_first );
		}

		constexpr void reserve( size_type n ) {
			if( n > capacity( ) ) {
				resize_impl( n );
			}
		}
		constexpr reference push_back( const_reference v ) {
			if( m_size >= m_capacity ) {
				resize_impl( size( ) + 1 );
			}
			return *construct_at( m_size++, v );
		}

		constexpr reference push_back( rvalue_reference v ) {
			if( m_size >= m_capacity ) {
				resize_impl( size( ) + 1 );
			}
			return *construct_at( m_size++, std::move( v ) );
		}

		template<typename... Args>
		constexpr reference emplace_back( Args &&...args ) {
			if( m_size >= m_capacity ) {
				resize_impl( m_size + 1 );
			}
			return *construct_at( m_size++, DAW_FWD2( Args, args )... );
		}

		[[nodiscard]] iterator begin( ) {
			return iterator{ m_first };
		}

		[[nodiscard]] const_iterator begin( ) const {
			return const_iterator{ m_first };
		}

		[[nodiscard]] const_iterator cbegin( ) const {
			return const_iterator{ m_first };
		}

		[[nodiscard]] iterator end( ) {
			return iterator{ m_size };
		}

		[[nodiscard]] const_iterator end( ) const {
			return const_iterator{ m_size };
		}

		[[nodiscard]] const_iterator cend( ) const {
			return const_iterator{ m_size };
		}

		[[nodiscard]] constexpr const_reference operator[]( size_type idx ) const {
			return m_first[idx];
		}

		[[nodiscard]] constexpr reference operator[]( size_type idx ) {
			return m_first[idx];
		}

		constexpr iterator insert( const_iterator where, const_reference value ) {
			auto const *first = &value;
			return insert( where, first, first + 1 );
		}

		template<
		  typename IteratorF, typename IteratorL,
		  std::enable_if_t<( not has_op_minus<IteratorF, IteratorL>::value and
		                     is_iterator<IteratorF>::value ),
		                   std::nullptr_t> = nullptr>
		constexpr iterator insert( const_iterator where, IteratorF first,
		                           IteratorL last ) {
			auto idx = static_cast<size_type>( where - data( ) );

			while( first != last ) {
				insert( data( )[idx], *first );
				++idx;
				++first;
			}
		}

		template<typename IteratorF, typename IteratorL,
		         std::enable_if_t<( has_op_minus<IteratorF, IteratorL>::value and
		                            is_iterator<IteratorF>::value ),
		                          std::nullptr_t> = nullptr>
		constexpr iterator insert( const_iterator where, IteratorF first,
		                           IteratorL last ) {
			size_type const old_size = size( );
			size_type const where_idx = static_cast<size_type>( where - data( ) );
			size_type const insert_size = static_cast<size_type>( last - first );
			auto const needed_size = insert_size + old_size;

			if( capacity( ) <= needed_size ) {
				resize_impl( needed_size );
			}
			if( where_idx < old_size ) {
				overlapped_relocate( m_first, where_idx, old_size, insert_size );
			}
			pointer pos = m_first + static_cast<difference_type>( where_idx );
			while( first != last ) {
				*pos = *first;
				++pos;
				++first;
			}
			m_size = m_first + needed_size;
			return m_first + where_idx;
		}

		template<typename U, std::size_t N>
		iterator insert( const_iterator where, U const ( &values )[N] ) {
			return insert( where, values, values + N );
		}

		template<typename Alloc2>
		bool operator==( Vector<value_type, Alloc2> const &rhs ) const noexcept {
			if( size( ) != rhs.size( ) ) {
				return false;
			}
			return std::equal( m_first, m_size, rhs.m_first );
		}

		template<typename Alloc2>
		bool operator!=( Vector<value_type, Alloc2> const &rhs ) const noexcept {
			if( size( ) == rhs.size( ) ) {
				return false;
			}

			return std::equal( m_first, m_size, rhs.m_first,
			                   std::not_equal_to<value_type>{ } );
		}
	};

	template<typename T, std::size_t N>
	Vector( T const ( & )[N] ) -> Vector<T>;
} // namespace daw
