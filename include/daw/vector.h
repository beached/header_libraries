#pragma once

#include "compressed_pair.h"
#include "daw_concepts.h"
#include "daw_likely.h"
#include "daw_move.h"
#include "split_buffer.h"
#include "wrap_iter.h"

#include <algorithm>
#include <cassert>
#include <climits>
#include <compare>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <limits>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <version>

namespace daw::impl {
	template<typename Alloc, typename Traits = std::allocator_traits<Alloc>>
	constexpr bool noexcept_move_assign_container_v =
	  Traits::propagate_on_container_move_assignment::value or
	  Traits::is_always_equal::value;

	template<typename Alloc, typename Ptr>
	constexpr void
	construct_backward_with_exception_guarantees( Alloc &a, Ptr begin1, Ptr end1,
	                                              Ptr &end2 ) {
		static_assert( is_move_insertable_v<Alloc>,
		               "The specified type does not meet the requirements of "
		               "Cpp17MoveInsertable" );
		using traits = std::allocator_traits<Alloc>;
		while( end1 != begin1 ) {
			traits::construct( a, std::to_address( end2 - 1 ),
#if defined( DAW_USE_EXCEPTIONS )
			                   std::move_if_noexcept( *--end1 )
#else
			                   std::move( *--end1 )
#endif
			);
			--end2;
		}
	}

	template<typename Alloc, typename Ptr>
	constexpr void
	construct_forward_with_exception_guarantees( Alloc &a, Ptr begin1, Ptr end1,
	                                             Ptr &begin2 ) {
		static_assert( is_move_insertable_v<Alloc>,
		               "The specified type does not meet the requirements of "
		               "Cpp17MoveInsertable" );
		using traits_t = std::allocator_traits<Alloc>;
		for( ; begin1 != end1; ++begin1, (void)++begin2 ) {
			traits_t::construct( a, std::to_address( begin2 ),
#if defined( DAW_USE_EXCEPTIONS )
			                     std::move_if_noexcept( *begin1 )
#else
			                     std::move( *begin1 )
#endif
			);
		}
	}

	template<typename Alloc, typename Iter, typename Ptr>
	constexpr void construct_range_forward( Alloc &a, Iter begin1, Iter end1,
	                                        Ptr &begin2 ) {
		using traits_t = std::allocator_traits<Alloc>;
		for( ; begin1 != end1; ++begin1, (void)++begin2 ) {
			traits_t::construct( a, std::to_address( begin2 ), *begin1 );
		}
	}
} // namespace daw::impl

namespace daw {
	struct do_resize_and_overwrite_t {};
	inline constexpr auto do_resize_and_overwrite = do_resize_and_overwrite_t{ };

	template<typename T, typename size_type, typename pointer, typename alloc>
	concept ResizeAndOverwriteOperationAlloc =
	  invocable_result<T, size_type, pointer, size_type, alloc &>;

	template<typename T, typename size_type, typename pointer, typename alloc>
	concept ResizeAndOverwriteOperation =
	  invocable_result<T, size_type, pointer, size_type> and not
	ResizeAndOverwriteOperationAlloc<T, size_type, pointer, alloc>;

	template<typename T>
	inline constexpr bool has_slow_distance_v =
	  requires
	{
		typename T::slow_distance;
	};

	template<typename T, typename Allocator = std::allocator<T>>
	struct vector {
		using value_type = T;
		using allocator_type = Allocator;
		using alloc_traits = std::allocator_traits<allocator_type>;
		using reference = value_type &;
		using const_reference = value_type const &;
		using size_type = typename alloc_traits::size_type;
		using difference_type = typename alloc_traits::difference_type;
		using pointer = typename alloc_traits::pointer;
		using const_pointer = typename alloc_traits::const_pointer;
		using iterator = wrap_iter<pointer, vector>;
		using const_iterator = wrap_iter<const_pointer, vector>;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;
		using supports_slow_distance = void;

		static_assert(
		  std::is_same_v<typename allocator_type::value_type, value_type>,
		  "Allocator::value_type must be same type as value_type" );

	private:
		pointer m_begin = nullptr;
		pointer m_end = nullptr;
		compressed_pair<pointer, allocator_type> m_endcap_ =
		  compressed_pair<pointer, allocator_type>( nullptr, default_init_tag( ) );

	public:
		explicit vector( ) = default;

		explicit constexpr vector( allocator_type const &a ) //
		  noexcept( std::is_nothrow_copy_constructible<allocator_type>::value )
		  : m_endcap_( nullptr, a ) {}

		explicit constexpr vector( size_type n ) {
			if( n == 0 ) {
				return;
			}
			vallocate( n );
			construct_at_end( n );
		}

		explicit constexpr vector( size_type n, allocator_type const &a )
		  : m_endcap_( nullptr, a ) {
			if( n > 0 ) {
				vallocate( n );
				construct_at_end( n );
			}
		}

		explicit constexpr vector( size_type n, value_type const &x ) {

			if( n > 0 ) {
				vallocate( n );
				construct_at_end( n, x );
			}
		}

		explicit constexpr vector( size_type n, value_type const &x,
		                           allocator_type const &a )
		  : m_endcap_( nullptr, a ) {

			if( n > 0 ) {
				vallocate( n );
				construct_at_end( n, x );
			}
		}

		explicit constexpr vector( do_resize_and_overwrite_t, size_type n,
		                           auto operation ) {
			if( n == 0 ) {
				return;
			}
			resize_and_overwrite( n, DAW_MOVE( operation ) );
		}

		explicit constexpr vector( do_resize_and_overwrite_t, size_type n,
		                           auto operation, allocator_type const &a )
		  : m_endcap_( nullptr, a ) {
			if( n == 0 ) {
				return;
			}
			resize_and_overwrite( n, DAW_MOVE( operation ) );
		}

		template<input_iterator InputIterator>
		  requires( constructible_from<value_type,
		                               iter_reference_type<InputIterator>> ) //
		explicit constexpr vector( InputIterator first, InputIterator last ) {

			for( ; first != last; ++first ) {
				emplace_back( *first );
			}
		}

		template<input_iterator InputIterator>
		  requires( constructible_from<value_type,
		                               iter_reference_type<InputIterator>> ) //
		explicit constexpr vector( InputIterator first, InputIterator last,
		                           allocator_type const &a )
		  : m_endcap_( nullptr, a ) {

			for( ; first != last; ++first ) {
				emplace_back( *first );
			}
		}

		template<forward_iterator ForwardIterator>
		  requires( not has_slow_distance_v<ForwardIterator> and
		            constructible_from<value_type,
		                               iter_reference_type<ForwardIterator>> ) //
		explicit constexpr vector( ForwardIterator first, ForwardIterator last ) {

			auto const n = static_cast<size_type>( std::distance( first, last ) );
			if( n > 0 ) {
				vallocate( n );
				construct_at_end( first, last, n );
			}
		}

		template<forward_iterator ForwardIterator>
		  requires( not has_slow_distance_v<ForwardIterator> and
		            constructible_from<value_type,
		                               iter_reference_type<ForwardIterator>> ) //
		explicit constexpr vector( ForwardIterator first, ForwardIterator last,
		                           allocator_type const &a )
		  : m_endcap_( nullptr, a ) {
			auto const n = static_cast<size_type>( std::distance( first, last ) );
			if( n > 0 ) {
				vallocate( n );
				construct_at_end( first, last, n );
			}
		}

		constexpr ~vector( ) {
			if( m_begin != nullptr ) {
				clear( );
				alloc_traits::deallocate( alloc( ), m_begin, capacity( ) );
			}
		}

		constexpr vector( vector const &x )
		  : m_endcap_( nullptr, alloc_traits::select_on_container_copy_construction(
		                          x.alloc( ) ) ) {

			size_type const n = x.size( );
			if( n > 0 ) {
				vallocate( n );
				construct_at_end( x.m_begin, x.m_end, n );
			}
		}

		explicit constexpr vector( vector const &x,
		                           std::type_identity_t<allocator_type> const &a )
		  : m_endcap_( nullptr, a ) {

			size_type const n = x.size( );
			if( n > 0 ) {
				vallocate( n );
				construct_at_end( x.m_begin, x.m_end, n );
			}
		}

		constexpr vector &operator=( vector const &x ) {
			if( this != std::addressof( x ) ) {
				copy_assign_alloc( x );
				assign( x.m_begin, x.m_end );
			}
			return *this;
		}

		explicit constexpr vector( std::initializer_list<value_type> il,
		                           allocator_type const &a )
		  : m_endcap_( nullptr, a ) {

			if( il.size( ) > 0 ) {
				vallocate( il.size( ) );
				construct_at_end( il.begin( ), il.end( ), il.size( ) );
			}
		}

		constexpr vector( vector &&x ) noexcept
		  : m_endcap_( nullptr, DAW_MOVE( x.alloc( ) ) ) {
			m_begin = x.m_begin;
			m_end = x.m_end;
			endcap( ) = x.endcap( );
			x.m_begin = nullptr;
			x.m_end = nullptr;
			x.endcap( ) = nullptr;
		}

		explicit constexpr vector( vector &&x,
		                           std::type_identity_t<allocator_type> const &a )
		  : m_endcap_( nullptr, a ) {

			if( a == x.alloc( ) ) {
				m_begin = x.m_begin;
				m_end = x.m_end;
				endcap( ) = x.endcap( );
				x.m_begin = nullptr;
				x.m_end = nullptr;
				x.endcap( ) = nullptr;
			} else {
				using move_it_t = std::move_iterator<iterator>;
				assign( move_it_t( x.begin( ) ), move_it_t( x.end( ) ) );
			}
		}

		constexpr vector &operator=( std::initializer_list<value_type> il ) {
			assign( il.begin( ), il.end( ) );
			return *this;
		}
		constexpr vector &operator=( vector &&x ) //
		  noexcept(
		    impl::noexcept_move_assign_container_v<Allocator, alloc_traits> ) {

			move_assign(
			  x, std::bool_constant<
			       alloc_traits::propagate_on_container_move_assignment::value>{ } );

			return *this;
		}

		template<input_iterator InputIterator>
		  requires( constructible_from<value_type,
		                               iter_reference_type<InputIterator>> ) //
		void assign( InputIterator first, InputIterator last ) {
			clear( );
			for( ; first != last; ++first )
				emplace_back( *first );
		}

		template<forward_iterator ForwardIterator>
		  requires( not has_slow_distance_v<ForwardIterator> and
		            constructible_from<value_type,
		                               iter_reference_type<ForwardIterator>> ) //
		constexpr void assign( ForwardIterator first, ForwardIterator last ) {
			auto const new_size =
			  static_cast<size_type>( std::distance( first, last ) );
			if( new_size <= capacity( ) ) {
				ForwardIterator mid = last;
				bool growing = false;
				if( new_size > size( ) ) {
					growing = true;
					mid = first;
					std::advance( mid, size( ) );
				}
				pointer m = std::copy( first, mid, m_begin );
				if( growing )
					construct_at_end( mid, last, new_size - size( ) );
				else
					destruct_at_end( m );
			} else {
				vdeallocate( );
				vallocate( recommend( new_size ) );
				construct_at_end( first, last, new_size );
			}
		}

		constexpr void assign( size_type n, const_reference u ) {
			if( n <= capacity( ) ) {
				size_type s = size( );
				std::fill_n( m_begin, std::min( n, s ), u );
				if( n > s )
					construct_at_end( n - s, u );
				else
					destruct_at_end( m_begin + n );
			} else {
				vdeallocate( );
				vallocate( recommend( static_cast<size_type>( n ) ) );
				construct_at_end( n, u );
			}
		}

		constexpr void assign( std::initializer_list<value_type> il ) {
			assign( il.begin( ), il.end( ) );
		}

		[[nodiscard]] constexpr allocator_type get_allocator( ) const noexcept {
			return alloc( );
		}

		[[nodiscard]] constexpr iterator begin( ) noexcept {
			return make_iter( m_begin );
		}

		[[nodiscard]] constexpr const_iterator begin( ) const noexcept {
			return make_iter( m_begin );
		}

		[[nodiscard]] constexpr iterator end( ) noexcept {
			return make_iter( m_end );
		}

		[[nodiscard]] constexpr const_iterator end( ) const noexcept {
			return make_iter( m_end );
		}

		[[nodiscard]] constexpr reverse_iterator rbegin( ) noexcept {
			return reverse_iterator( end( ) );
		}

		[[nodiscard]] constexpr const_reverse_iterator rbegin( ) const noexcept {
			return const_reverse_iterator( end( ) );
		}

		[[nodiscard]] constexpr reverse_iterator rend( ) noexcept {
			return reverse_iterator( begin( ) );
		}

		[[nodiscard]] constexpr const_reverse_iterator rend( ) const noexcept {
			return const_reverse_iterator( begin( ) );
		}

		[[nodiscard]] constexpr const_iterator cbegin( ) const noexcept {
			return begin( );
		}

		[[nodiscard]] constexpr const_iterator cend( ) const noexcept {
			return end( );
		}

		[[nodiscard]] constexpr const_reverse_iterator crbegin( ) const noexcept {
			return rbegin( );
		}

		[[nodiscard]] constexpr const_reverse_iterator crend( ) const noexcept {
			return rend( );
		}

		[[nodiscard]] constexpr size_type size( ) const noexcept {
			return static_cast<size_type>( m_end - m_begin );
		}

		[[nodiscard]] constexpr difference_type ssize( ) const noexcept {
			return m_end - m_begin;
		}

		[[nodiscard]] constexpr size_type capacity( ) const noexcept {
			return static_cast<size_type>( endcap( ) - m_begin );
		}

		[[nodiscard]] constexpr bool empty( ) const noexcept {
			return m_begin == m_end;
		}

		[[nodiscard]] constexpr size_type max_size( ) const noexcept {
			return std::min<size_type>(
			  alloc_traits::max_size( alloc( ) ),
			  std::numeric_limits<difference_type>::max( ) );
		}

		constexpr void reserve( size_type n ) {
			if( n > capacity( ) ) {
				if( DAW_UNLIKELY( n > max_size( ) ) ) {
					throw_length_error( );
				}
				allocator_type &a = alloc( );
				auto v = split_buffer<value_type, allocator_type &>( n, size( ), a );
				swap_out_circular_buffer( v );
			}
		}

		void shrink_to_fit( ) noexcept {
			if( capacity( ) > size( ) ) {
#if defined( DAW_USE_EXCEPTIONS )
				try {
#endif
					allocator_type &a = alloc( );
					auto v =
					  split_buffer<value_type, allocator_type &>( size( ), size( ), a );
					swap_out_circular_buffer( v );
#if defined( DAW_USE_EXCEPTIONS )
				} catch( ... ) {}
#endif
			}
		}

		[[nodiscard]] constexpr reference operator[]( size_type n ) noexcept {
			return m_begin[n];
		}

		[[nodiscard]] constexpr const_reference
		operator[]( size_type n ) const noexcept {
			return m_begin[n];
		}

		[[nodiscard]] reference at( size_type n ) {
			if( DAW_UNLIKELY( n >= size( ) ) ) {
				throw_out_of_range( );
			}
			return m_begin[n];
		}

		[[nodiscard]] const_reference at( size_type n ) const {
			if( DAW_UNLIKELY( n >= size( ) ) ) {
				throw_out_of_range( );
			}
			return m_begin[n];
		}

		[[nodiscard]] constexpr reference front( ) noexcept {
			return *m_begin;
		}

		[[nodiscard]] constexpr const_reference front( ) const noexcept {
			return *m_begin;
		}

		[[nodiscard]] constexpr reference back( ) noexcept {
			return *( m_end - 1 );
		}

		[[nodiscard]] constexpr const_reference back( ) const noexcept {
			return *( m_end - 1 );
		}

		[[nodiscard]] constexpr pointer data( ) noexcept {
			return std::to_address( m_begin );
		}

		[[nodiscard]] constexpr const_pointer data( ) const noexcept {
			return std::to_address( m_begin );
		}

		[[nodiscard]] constexpr pointer data_end( ) noexcept {
			return std::to_address( m_end );
		}

		[[nodiscard]] constexpr const_pointer data_end( ) const noexcept {
			return std::to_address( m_end );
		}

		constexpr void push_back( const_reference x ) {
			if( m_end != endcap( ) ) {
				construct_one_at_end( x );
			} else
				push_back_slow_path( x );
		}

		constexpr void push_back( value_type &&x ) {
			if( m_end < endcap( ) ) {
				construct_one_at_end( DAW_MOVE( x ) );
			} else
				push_back_slow_path( DAW_MOVE( x ) );
		}

		template<typename... Args>
		constexpr reference emplace_back( Args &&...args ) {
			if( m_end < endcap( ) ) {
				construct_one_at_end( DAW_FWD2( Args, args )... );
			} else {
				emplace_back_slow_path( DAW_FWD2( Args, args )... );
			}
			return back( );
		}

		constexpr void pop_back( ) {
			destruct_at_end( m_end - 1 );
		}

		constexpr iterator insert( const_iterator position, const_reference x ) {
			pointer p = m_begin + ( position - begin( ) );
			if( m_end < endcap( ) ) {
				if( p == m_end ) {
					construct_one_at_end( x );
				} else {
					move_range( p, m_end, p + 1 );
					auto xr = std::pointer_traits<const_pointer>::pointer_to( x );
					if( p <= xr && xr < m_end ) {
						++xr;
					}
					*p = *xr;
				}
			} else {
				allocator_type &a = alloc( );
				auto v = split_buffer<value_type, allocator_type &>(
				  recommend( size( ) + 1 ), p - m_begin, a );
				v.push_back( x );
				p = swap_out_circular_buffer( v, p );
			}
			return make_iter( p );
		}

		constexpr iterator insert( const_iterator position, value_type &&x ) {
			pointer p = m_begin + ( position - begin( ) );

			if( m_end < endcap( ) ) {
				if( p == m_end ) {
					construct_one_at_end( DAW_MOVE( x ) );
				} else {
					move_range( p, m_end, p + 1 );
					*p = DAW_MOVE( x );
				}
			} else {
				allocator_type &a = alloc( );
				auto v = split_buffer<value_type, allocator_type &>(
				  recommend( size( ) + 1 ), p - m_begin, a );
				v.push_back( DAW_MOVE( x ) );
				p = swap_out_circular_buffer( v, p );
			}
			return make_iter( p );
		}

		template<typename... Args>
		constexpr iterator emplace( const_iterator position, Args &&...args ) {
			pointer p = m_begin + ( position - begin( ) );
			if( m_end < endcap( ) ) {
				if( p == m_end ) {
					construct_one_at_end( std::forward<Args>( args )... );
				} else {
					auto tmp = temp_value<value_type, Allocator>(
					  alloc( ), DAW_FWD2( Args, args )... );
					move_range( p, m_end, p + 1 );
					*p = DAW_MOVE( tmp.get( ) );
				}
			} else {
				allocator_type &a = alloc( );
				auto v = split_buffer<value_type, allocator_type &>(
				  recommend( size( ) + 1 ), p - m_begin, a );

				v.emplace_back( DAW_FWD2( Args, args )... );
				p = swap_out_circular_buffer( v, p );
			}
			return make_iter( p );
		}

		constexpr iterator insert( const_iterator position, size_type n,
		                           const_reference x ) {
			pointer p = m_begin + ( position - begin( ) );
			if( n > 0 ) {
				if( n <= static_cast<size_type>( endcap( ) - m_end ) ) {
					size_type old_n = n;
					pointer old_last = m_end;
					if( n > static_cast<size_type>( m_end - p ) ) {
						size_type cx = n - ( m_end - p );
						construct_at_end( cx, x );
						n -= cx;
					}
					if( n > 0 ) {
						move_range( p, old_last, p + old_n );
						auto xr = std::pointer_traits<const_pointer>::pointer_to( x );
						if( p <= xr && xr < m_end ) {
							xr += old_n;
						}
						std::fill_n( p, n, *xr );
					}
				} else {
					allocator_type &a = alloc( );
					auto v = split_buffer<value_type, allocator_type &>(
					  recommend( size( ) + n ), p - m_begin, a );
					v.construct_at_end( n, x );
					p = swap_out_circular_buffer( v, p );
				}
			}
			return make_iter( p );
		}

		template<input_iterator InputIterator>
		  requires(
		    constructible_from<value_type, iter_reference_type<InputIterator>> ) //
		constexpr iterator insert( const_iterator position, InputIterator first,
		                           InputIterator last ) {
			difference_type off = position - begin( );
			pointer p = m_begin + off;
			allocator_type &a = alloc( );
			pointer old_last = m_end;
			for( ; m_end != endcap( ) and first != last; ++first ) {
				construct_one_at_end( *first );
			}
			auto v = split_buffer<value_type, allocator_type &>( a );
			if( first != last ) {
#if defined( DAW_USE_EXCEPTIONS )
				try {
#endif
					v.construct_at_end( first, last );
					difference_type old_size = old_last - m_begin;
					difference_type old_p = p - m_begin;
					reserve( recommend( size( ) + v.size( ) ) );
					p = m_begin + old_p;
					old_last = m_begin + old_size;
#if defined( DAW_USE_EXCEPTIONS )
				} catch( ... ) {
					erase( make_iter( old_last ), end( ) );
					throw;
				}
#endif
			}
			p = std::rotate( p, old_last, m_end );
			insert( make_iter( p ), std::make_move_iterator( v.begin( ) ),
			        std::make_move_iterator( v.end( ) ) );
			return begin( ) + off;
		}

		template<forward_iterator ForwardIterator>
		  requires( not has_slow_distance_v<ForwardIterator> and
		            constructible_from<value_type,
		                               iter_reference_type<ForwardIterator>> ) //
		constexpr iterator insert( const_iterator position, ForwardIterator first,
		                           ForwardIterator last ) {
			pointer p = m_begin + ( position - begin( ) );
			difference_type n = std::distance( first, last );
			if( n > 0 ) {
				if( n <= endcap( ) - m_end ) {
					auto const old_n = static_cast<size_type>( n );
					pointer old_last = m_end;
					ForwardIterator m = last;
					difference_type dx = m_end - p;
					if( n > dx ) {
						m = first;
						difference_type diff = m_end - p;
						std::advance( m, diff );
						construct_at_end( m, last, static_cast<size_type>( n - diff ) );
						n = dx;
					}
					if( n > 0 ) {
						move_range( p, old_last, p + old_n );
						std::copy( first, m, p );
					}
				} else {
					allocator_type &a = alloc( );
					auto v = split_buffer<value_type, allocator_type &>(
					  recommend( static_cast<size_type>( ssize( ) + n ) ),
					  static_cast<size_type>( p - m_begin ), a );
					v.construct_at_end( first, last );
					p = swap_out_circular_buffer( v, p );
				}
			}
			return make_iter( p );
		}

		constexpr iterator insert( const_iterator position,
		                           std::initializer_list<value_type> il ) {
			return insert( position, il.begin( ), il.end( ) );
		}

		constexpr iterator erase( const_iterator position ) {
			difference_type ps = position - cbegin( );
			pointer p = m_begin + ps;
			destruct_at_end( DAW_MOVE( p + 1, m_end, p ) );
			iterator r = make_iter( p );
			return r;
		}

		iterator erase( const_iterator first, const_iterator last ) {
			pointer p = m_begin + ( first - begin( ) );
			if( first != last ) {
				destruct_at_end( DAW_MOVE( p + ( last - first ), m_end, p ) );
			}
			iterator r = make_iter( p );
			return r;
		}

		constexpr void resize( size_type sz ) {
			size_type const cs = size( );
			if( cs < sz ) {
				append( sz - cs );
			} else if( cs > sz ) {
				destruct_at_end( m_begin + sz );
			}
		}

		constexpr void resize( size_type sz, const_reference x ) {
			size_type const cs = size( );
			if( cs < sz ) {
				append( sz - cs, x );
			} else if( cs > sz ) {
				destruct_at_end( m_begin + sz );
			}
		}

		template<
		  ResizeAndOverwriteOperation<size_type, pointer, allocator_type> Operation>
		constexpr void resize_and_overwrite( size_type n, Operation operation ) {
			if( capacity( ) < n ) {
				reserve( n );
			}
			pointer p = m_begin;
			auto const new_size = static_cast<size_type>( operation( p, n ) );
			assert( new_size <= n );
			auto new_end = m_begin + static_cast<difference_type>( new_size );
			if( new_size < n ) {
				destruct_at_end( new_end );
			}
			m_end = new_end;
		}

		template<
		  ResizeAndOverwriteOperationAlloc<size_type, pointer, allocator_type>
		    Operation>
		constexpr void resize_and_overwrite( size_type n, Operation operation ) {
			if( capacity( ) < n ) {
				reserve( n );
			}
			pointer p = m_begin;
			allocator_type &a = alloc( );
			auto const new_size = static_cast<size_type>( operation( p, n, a ) );
			assert( new_size <= n );
			auto new_end = m_begin + static_cast<difference_type>( new_size );
			if( new_size < n ) {
				destruct_at_end( new_end );
			}
			m_end = new_end;
		}

		template<
		  ResizeAndOverwriteOperation<size_type, pointer, allocator_type> Operation>
		constexpr void append_and_overwrite( size_type n, Operation operation ) {
			if( capacity( ) < size( ) + n ) {
				reserve( size( ) + n );
			}
			pointer p = m_end;
			auto const append_count = static_cast<size_type>( operation( p, n ) );
			assert( append_count <= n );
			auto new_end = m_end + static_cast<difference_type>( append_count );
			m_end = new_end;
		}

		template<
		  ResizeAndOverwriteOperationAlloc<size_type, pointer, allocator_type>
		    Operation>
		constexpr void append_and_overwrite( size_type n, Operation operation ) {

			if( capacity( ) < size( ) + n ) {
				reserve( size( ) + n );
			}
			pointer p = m_end;
			allocator_type &a = alloc( );
			auto const append_count = static_cast<size_type>( operation( p, n, a ) );
			assert( append_count <= n );
			auto new_end = m_end + static_cast<difference_type>( append_count );
			m_end = new_end;
		}

		constexpr void swap( vector &other ) noexcept {
			std::swap( m_begin, other.m_begin );
			std::swap( m_end, other.m_end );
			std::swap( endcap( ), other.endcap( ) );
			swap_allocator( alloc( ), other.alloc( ),
			                std::bool_constant<
			                  alloc_traits::propagate_on_container_swap::value>{ } );
		}

	private:
		/// @brief Allocate space for n objects
		/// @throws length_error if n > max_size()
		/// @throws (probably bad_alloc) if memory run out
		/// @pre m_begin == m_end == endcap() == 0
		/// @pre n > 0
		/// @post capacity() == n
		/// @post size() == 0
		constexpr void vallocate( size_type n ) {
			if( DAW_UNLIKELY( n > max_size( ) ) ) {
				throw_length_error( );
			}
			m_begin = m_end = alloc_traits::allocate( alloc( ), n );
			endcap( ) = m_begin + n;
		}

		constexpr void vdeallocate( ) noexcept {
			if( m_begin != nullptr ) {
				clear( );
				alloc_traits::deallocate( alloc( ), m_begin, capacity( ) );
				m_begin = m_end = endcap( ) = nullptr;
			}
		}

		///  @pre new_size > capacity()
		[[nodiscard]] constexpr size_type recommend( size_type new_size ) const {
			size_type const ms = max_size( );
			if( DAW_UNLIKELY( new_size > ms ) ) {
				throw_length_error( );
			}
			const size_type cap = capacity( );
			if( cap >= ms / 2 )
				return ms;
			return std::max<size_type>( 2 * cap, new_size );
		}

		/// @brief Default constructs n objects starting at m_end
		/// @throws if construction throws
		/// @pre n > 0
		/// @pre size() + n <= capacity()
		/// @post size() == size() + n
		constexpr void construct_at_end( size_type n ) {
			ConstructTransaction tx( *this, n );
			const_pointer const new_end = tx.new_m_end;
			for( pointer pos = tx.pos; pos != new_end; tx.pos = ++pos ) {
				alloc_traits::construct( alloc( ), std::to_address( pos ) );
			}
		}

		/// @brief Copy constructs n objects starting at m_end from x
		/// @throws if construction throws
		/// @pre n > 0
		/// @pre size() + n <= capacity()
		/// @post size() == old size() + n
		/// @post [i] == x for all i in [size() - n, n)
		constexpr void construct_at_end( size_type n, const_reference x ) {
			ConstructTransaction tx( *this, n );
			const_pointer new_end = tx.new_m_end;
			for( pointer pos = tx.pos; pos != new_end; tx.pos = ++pos ) {
				alloc_traits::construct( alloc( ), std::to_address( pos ), x );
			}
		}

		template<forward_iterator ForwardIterator>
		  requires( not has_slow_distance_v<ForwardIterator> ) //
		constexpr void construct_at_end( ForwardIterator first,
		                                 ForwardIterator last, size_type n ) {
			ConstructTransaction tx( *this, n );
			impl::construct_range_forward( alloc( ), first, last, tx.pos );
		}

		/// @brief Default constructs n objects starting at m_end
		/// Exception safety: strong.
		/// @throws if construction throws
		/// @post size() == size() + n
		constexpr void append( size_type n ) {
			if( static_cast<size_type>( endcap( ) - m_end ) >= n ) {
				construct_at_end( n );
			} else {
				allocator_type &a = alloc( );
				auto v = split_buffer<value_type, allocator_type &>(
				  recommend( size( ) + n ), size( ), a );
				v.construct_at_end( n );
				swap_out_circular_buffer( v );
			}
		}

		/// @brief Default constructs n objects starting at m_end
		/// Exception safety: strong.
		/// @throws if construction throws
		/// @post size() == size() + n
		constexpr void append( size_type n, const_reference x ) {
			if( static_cast<size_type>( endcap( ) - m_end ) >= n ) {
				construct_at_end( n, x );
			} else {
				allocator_type &a = alloc( );
				auto v = split_buffer<value_type, allocator_type &>(
				  recommend( size( ) + n ), size( ), a );
				v.construct_at_end( n, x );
				swap_out_circular_buffer( v );
			}
		}

		[[nodiscard]] constexpr iterator make_iter( pointer p ) noexcept {
			return iterator( p );
		}

		[[nodiscard]] constexpr const_iterator
		make_iter( const_pointer p ) const noexcept {
			return const_iterator( p );
		}

		constexpr void
		swap_out_circular_buffer( split_buffer<value_type, allocator_type &> &v ) {
			impl::construct_backward_with_exception_guarantees( alloc( ), m_begin,
			                                                    m_end, v.begin_ );
			std::swap( m_begin, v.begin_ );
			std::swap( m_end, v.end_ );
			std::swap( endcap( ), v.end_cap( ) );
			v.first_ = v.begin_;
		}

		[[nodiscard]] constexpr pointer
		swap_out_circular_buffer( split_buffer<value_type, allocator_type &> &v,
		                          pointer p ) {
			pointer r = v.begin_;
			impl::construct_backward_with_exception_guarantees( alloc( ), m_begin, p,
			                                                    v.begin_ );
			impl::construct_forward_with_exception_guarantees( alloc( ), p, m_end,
			                                                   v.end_ );
			std::swap( m_begin, v.begin_ );
			std::swap( m_end, v.end_ );
			std::swap( endcap( ), v.end_cap( ) );
			v.first_ = v.begin_;
			return r;
		}

		void move_range( pointer from_s, pointer from_e, pointer to ) {
			pointer old_last = m_end;
			difference_type n = old_last - to;
			{
				pointer i = from_s + n;
				ConstructTransaction tx( *this, static_cast<size_type>( from_e - i ) );
				for( pointer pos = tx.pos; i < from_e;
				     ++i, (void)++pos, tx.pos = pos ) {
					alloc_traits::construct( alloc( ), std::to_address( pos ),
					                         DAW_MOVE( *i ) );
				}
			}
			std::move_backward( from_s, from_s + n, old_last );
		}

		void move_assign( vector &c,
		                  std::false_type ) //
		  noexcept( alloc_traits::is_always_equal::value ) {
			if( alloc( ) != c.alloc( ) ) {
				using move_it_t = std::move_iterator<iterator>;
				assign( move_it_t( c.begin( ) ), move_it_t( c.end( ) ) );
			} else {
				move_assign( c, std::true_type( ) );
			}
		}

		void move_assign( vector &c, std::true_type ) //
		  noexcept( std::is_nothrow_move_assignable_v<allocator_type> ) {
			vdeallocate( );
			move_assign_alloc( c ); // this can throw
			m_begin = c.m_begin;
			m_end = c.m_end;
			endcap( ) = c.endcap( );
			c.m_begin = nullptr;
			c.m_end = nullptr;
			c.endcap( ) = nullptr;
		}

		constexpr void destruct_at_end( pointer new_last ) noexcept {
			base_destruct_at_end( new_last );
		}

		template<typename U>
		constexpr inline void push_back_slow_path( U &&x ) {
			allocator_type &a = alloc( );
			auto v = split_buffer<value_type, allocator_type &>(
			  recommend( size( ) + 1 ), size( ), a );
			// v.push_back(std::forward<_Up>(x));
			alloc_traits::construct( a, std::to_address( v.end_ ), DAW_FWD2( U, x ) );
			++v.end_;
			swap_out_circular_buffer( v );
		}

		template<typename... Args>
		constexpr void emplace_back_slow_path( Args &&...args ) {
			allocator_type &a = alloc( );
			auto v = split_buffer<value_type, allocator_type &>(
			  recommend( size( ) + 1 ), size( ), a );
			//    v.emplace_back(std::forward<Args>(args)...);
			alloc_traits::construct( a, std::to_address( v.end_ ),
			                         DAW_FWD2( Args, args )... );
			++v.end_;
			swap_out_circular_buffer( v );
		}

		struct ConstructTransaction {
			vector &v;
			pointer pos;
			const_pointer const new_m_end;

			explicit constexpr ConstructTransaction( vector &ve, size_type n )
			  : v( ve )
			  , pos( v.m_end )
			  , new_m_end( v.m_end + n ) {}

			constexpr ~ConstructTransaction( ) {
				v.m_end = pos;
			}

			ConstructTransaction( ConstructTransaction && ) = delete;
			ConstructTransaction &operator=( ConstructTransaction && ) = delete;
			ConstructTransaction( ConstructTransaction const & ) = delete;
			ConstructTransaction &operator=( ConstructTransaction const & ) = delete;
		};

		template<typename... Args>
		constexpr void construct_one_at_end( Args &&...args ) {
			ConstructTransaction tx( *this, 1 );
			alloc_traits::construct( alloc( ), std::to_address( tx.pos ),
			                         DAW_FWD2( Args, args )... );
			++tx.pos;
		}

		[[nodiscard]] constexpr allocator_type &alloc( ) noexcept {
			return m_endcap_.second( );
		}

		[[nodiscard]] constexpr allocator_type const &alloc( ) const noexcept {
			return m_endcap_.second( );
		}

		[[nodiscard]] constexpr pointer &endcap( ) noexcept {
			return m_endcap_.first( );
		}

		[[nodiscard]] constexpr const pointer &endcap( ) const noexcept {
			return m_endcap_.first( );
		}

		constexpr void clear( ) noexcept {
			base_destruct_at_end( m_begin );
		}

		constexpr void base_destruct_at_end( pointer new_last ) noexcept {
			pointer soon_to_be_end = m_end;
			while( new_last != soon_to_be_end ) {
				alloc_traits::destroy( alloc( ), std::to_address( --soon_to_be_end ) );
			}
			m_end = new_last;
		}

		constexpr void copy_assign_alloc( vector const &c ) {
			copy_assign_alloc(
			  c, std::bool_constant<
			       alloc_traits::propagate_on_container_copy_assignment::value>{ } );
		}

		constexpr void move_assign_alloc( vector &c ) noexcept(
		  not alloc_traits::propagate_on_container_move_assignment::value or
		  std::is_nothrow_move_assignable_v<allocator_type> ) {
			move_assign_alloc(
			  c, std::bool_constant<
			       alloc_traits::propagate_on_container_move_assignment::value>{ } );
		}

		[[noreturn]] void throw_length_error( ) const {
			::daw::exception::throw_length_error( "vector" );
		}

		[[noreturn]] void throw_out_of_range( ) const {
			::daw::exception::throw_out_of_range( "vector" );
		}

		constexpr void copy_assign_alloc( vector const &c, std::true_type ) {
			if( alloc( ) != c.alloc( ) ) {
				clear( );
				alloc_traits::deallocate( alloc( ), m_begin, capacity( ) );
				m_begin = m_end = endcap( ) = nullptr;
			}
			alloc( ) = c.alloc( );
		}

		constexpr void copy_assign_alloc( vector const &, std::false_type ) {}

		constexpr void move_assign_alloc( vector &c, std::true_type ) //
		  noexcept( std::is_nothrow_move_assignable_v<allocator_type> ) {
			alloc( ) = DAW_MOVE( c.alloc( ) );
		}

		constexpr void move_assign_alloc( vector &, std::false_type ) noexcept {}

		[[nodiscard]] friend constexpr bool operator==( vector const &x,
		                                                vector const &y ) {
			size_type const sz = x.size( );
			return sz == y.size( ) and
			       std::equal( x.data( ), x.data_end( ), y.data( ) );
		}

		[[nodiscard]] friend constexpr bool operator!=( vector const &x,
		                                                vector const &y ) {
			return not( x == y );
		}

		[[nodiscard]] friend constexpr bool operator<( vector const &x,
		                                               vector const &y ) {
			return std::lexicographical_compare( x.data( ), x.data_end( ), y.data( ),
			                                     y.data_end( ) );
		}

		[[nodiscard]] friend constexpr bool operator>( vector const &x,
		                                               vector const &y ) {
			return y < x;
		}

		[[nodiscard]] friend constexpr bool operator>=( vector const &x,
		                                                vector const &y ) {
			return not( x < y );
		}

		[[nodiscard]] friend constexpr bool operator<=( vector const &x,
		                                                vector const &y ) {
			return not( y < x );
		}
	};

	template<input_iterator InputIterator>
	vector( InputIterator, InputIterator )
	  -> vector<iter_value_type<InputIterator>,
	            std::allocator<iter_value_type<InputIterator>>>;

	template<input_iterator InputIterator, Allocators Alloc>
	vector( InputIterator, InputIterator, Alloc )
	  -> vector<iter_value_type<InputIterator>, Alloc>;

	template<typename T, typename Allocator>
	constexpr void
	swap( vector<T, Allocator> &x,
	      vector<T, Allocator> &y ) noexcept( noexcept( x.swap( y ) ) ) {
		x.swap( y );
	}

	template<typename T, typename Allocator>
	constexpr typename vector<T, Allocator>::size_type
	erase( vector<T, Allocator> &c, auto const &v ) {
		auto old_size = c.size( );
		c.erase( std::remove( c.begin( ), c.end( ), v ), c.end( ) );
		return old_size - c.size( );
	}

	template<typename T, typename Allocator>
	constexpr typename vector<T, Allocator>::size_type
	erase_if( vector<T, Allocator> &c, auto pred ) {
		auto old_size = c.size( );
		c.erase( std::remove_if( c.begin( ), c.end( ), pred ), c.end( ) );
		return old_size - c.size( );
	}

	template<typename T, typename Allocator = std::allocator<T>,
	         input_iterator InputIterator>
	constexpr void block_append_from( vector<T, Allocator> &v,
	                                  InputIterator first, InputIterator last,
	                                  std::size_t block_size = 512 ) {
		auto cur_size = v.size( );
		while( first != last ) {
			v.resize_and_overwrite( cur_size + block_size,
			                        [&]( T *ptr, std::size_t sz ) {
				                        ptr += cur_size;
				                        cur_size += block_size;
				                        while( cur_size < sz and first != last ) {
					                        *ptr = *first;
					                        ++ptr;
					                        ++first;
				                        }
				                        return cur_size;
			                        } );
		}
	}

	template<typename T, typename Allocator = std::allocator<T>,
	         input_iterator InputIterator>
	[[nodiscard]] constexpr vector<T, Allocator>
	block_create_from( InputIterator first, InputIterator last,
	                   std::size_t block_size = 512 ) {
		auto v = vector<T, Allocator>{ };
		block_append_from( v, first, last, block_size );
		return v;
	}
} // namespace daw
