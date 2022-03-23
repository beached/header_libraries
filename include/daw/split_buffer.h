
#pragma once

#include "compressed_pair.h"
#include "daw_check_exceptions.h"
#include "daw_concepts.h"
#include "daw_exception.h"
#include "daw_move.h"

#include <algorithm>
#include <type_traits>

namespace daw::si_impl {
	// --- Helper for container swap --
	constexpr void swap_allocator( auto &a1, auto &a2, std::true_type ) noexcept {
		using std::swap;
		swap( a1, a2 );
	}

	constexpr void swap_allocator( auto &a1, auto &a2,
	                               std::false_type ) noexcept {}

	template<typename Alloc>
	constexpr void swap_allocator( Alloc &a1, Alloc &a2 ) noexcept {
		swap_allocator(
		  a1, a2,
		  std::bool_constant<
		    std::allocator_traits<Alloc>::propagate_on_container_swap::value>{ } );
	}
} // namespace daw::si_impl

namespace daw {
	template<typename Tp, typename Allocator = std::allocator<Tp>>
	struct split_buffer {
		using value_type = Tp;
		using allocator_type = Allocator;
		using alloc_rr = std::remove_reference_t<allocator_type>;
		using alloc_traits = std::allocator_traits<alloc_rr>;
		using reference = value_type &;
		using const_reference = value_type const &;
		using size_type = typename alloc_traits::size_type;
		using difference_type = typename alloc_traits::difference_type;
		using pointer = typename alloc_traits::pointer;
		using const_pointer = typename alloc_traits::const_pointer;
		using iterator = pointer;
		using const_iterator = const_pointer;

		using alloc_ref = std::add_lvalue_reference_t<allocator_type>;
		using alloc_const_ref = std::add_lvalue_reference_t<allocator_type>;

		pointer first_;
		pointer begin_;
		pointer end_;
		compressed_pair<pointer, allocator_type> end_cap_;

	public:
		[[nodiscard]] constexpr alloc_rr &alloc( ) noexcept {
			return end_cap_.second( );
		}

		[[nodiscard]] constexpr alloc_rr const &alloc( ) const noexcept {
			return end_cap_.second( );
		}

		[[nodiscard]] constexpr pointer &end_cap( ) noexcept {
			return end_cap_.first( );
		}

		[[nodiscard]] constexpr const pointer &end_cap( ) const noexcept {
			return end_cap_.first( );
		}

		explicit constexpr split_buffer( size_type cap, size_type start,
		                                 alloc_rr &a )
		  : end_cap_( nullptr, a ) {
			first_ = cap != 0 ? alloc_traits::allocate( alloc( ), cap ) : nullptr;
			begin_ = end_ = first_ + start;
			end_cap( ) = first_ + cap;
		}

		explicit constexpr split_buffer( ) noexcept(
		  std::is_nothrow_default_constructible_v<allocator_type> )
		  : first_( nullptr )
		  , begin_( nullptr )
		  , end_( nullptr )
		  , end_cap_( nullptr, default_init_tag( ) ) {}

		split_buffer( split_buffer const & ) = delete;
		split_buffer &operator=( split_buffer const & ) = delete;

		explicit constexpr split_buffer( alloc_rr &a )
		  : first_( nullptr )
		  , begin_( nullptr )
		  , end_( nullptr )
		  , end_cap_( nullptr, a ) {}

		explicit constexpr split_buffer( alloc_rr const &a )
		  : first_( nullptr )
		  , begin_( nullptr )
		  , end_( nullptr )
		  , end_cap_( nullptr, a ) {}

		constexpr ~split_buffer( ) {
			clear( );
			if( first_ ) {
				alloc_traits::deallocate( alloc( ), first_, capacity( ) );
			}
		}

		explicit constexpr split_buffer( split_buffer &&c ) //
		  noexcept( std::is_nothrow_move_constructible_v<allocator_type> )
		  : first_( DAW_MOVE( c.first_ ) )
		  , begin_( DAW_MOVE( c.begin_ ) )
		  , end_( DAW_MOVE( c.end_ ) )
		  , end_cap_( DAW_MOVE( c.end_cap_ ) ) {
			c.first_ = nullptr;
			c.begin_ = nullptr;
			c.end_ = nullptr;
			c.end_cap( ) = nullptr;
		}

		explicit constexpr split_buffer( split_buffer &&c, alloc_rr const &a )
		  : end_cap_( nullptr, a ) {
			if( a == c.alloc( ) ) {
				first_ = std::exchange( c.first_, nullptr );
				begin_ = std::exchange( c.begin_, nullptr );
				end_ = std::exchange( c.end_, nullptr );
				end_cap( ) = std::exchange( c.end_cap( ), nullptr );
			} else {
				auto cap = c.size( );
				first_ = alloc_traits::allocate( alloc( ), cap );
				begin_ = end_ = first_;
				end_cap( ) = first_ + cap;
				using move_it_t = std::move_iterator<iterator>;
				construct_at_end( move_it_t( c.begin( ) ), move_it_t( c.end( ) ) );
			}
		}

		constexpr split_buffer &operator=( split_buffer &&c ) //
		  noexcept(
		    (alloc_traits::propagate_on_container_move_assignment::value and
		     std::is_nothrow_move_assignable_v<allocator_type>) or
		    not alloc_traits::propagate_on_container_move_assignment::value ) {
			clear( );
			shrink_to_fit( );
			first_ = c.first_;
			begin_ = c.begin_;
			end_ = c.end_;
			end_cap( ) = c.end_cap( );
			move_assign_alloc(
			  c, integral_constant<
			       bool,
			       alloc_traits::propagate_on_container_move_assignment::value>( ) );
			c.first_ = nullptr;
			c.begin_ = nullptr;
			c.end_ = nullptr;
			c.end_cap( ) = nullptr;
			return *this;
		}

		[[nodiscard]] constexpr iterator begin( ) noexcept {
			return begin_;
		}

		[[nodiscard]] constexpr const_iterator begin( ) const noexcept {
			return begin_;
		}

		[[nodiscard]] constexpr iterator end( ) noexcept {
			return end_;
		}

		[[nodiscard]] constexpr const_iterator end( ) const noexcept {
			return end_;
		}

		constexpr void clear( ) noexcept {
			destruct_at_end( begin_ );
		}

		[[nodiscard]] constexpr size_type size( ) const {
			return static_cast<size_type>( end_ - begin_ );
		}

		[[nodiscard]] constexpr bool empty( ) const {
			return end_ == begin_;
		}

		[[nodiscard]] constexpr size_type capacity( ) const {
			return static_cast<size_type>( end_cap( ) - first_ );
		}

		[[nodiscard]] constexpr size_type front_spare( ) const {
			return static_cast<size_type>( begin_ - first_ );
		}

		[[nodiscard]] constexpr size_type back_spare( ) const {
			return static_cast<size_type>( end_cap( ) - end_ );
		}

		[[nodiscard]] constexpr reference front( ) {
			return *begin_;
		}

		[[nodiscard]] constexpr const_reference front( ) const {
			return *begin_;
		}

		[[nodiscard]] constexpr reference back( ) {
			return *( end_ - 1 );
		}

		[[nodiscard]] constexpr const_reference back( ) const {
			return *( end_ - 1 );
		}

		constexpr void reserve( size_type n ) {
			if( n < capacity( ) ) {
				auto t = split_buffer<value_type, alloc_rr &>( n, 0, alloc( ) );
				t.construct_at_end( move_iterator<pointer>( begin_ ),
				                    move_iterator<pointer>( end_ ) );
				std::swap( first_, t.first_ );
				std::swap( begin_, t.begin_ );
				std::swap( end_, t.end_ );
				std::swap( end_cap( ), t.end_cap( ) );
			}
		}

		constexpr void shrink_to_fit( ) noexcept {
			if( capacity( ) > size( ) ) {
#if defined( DAW_USE_EXCEPTIONS )
				try {
#endif
					auto t = split_buffer<value_type, alloc_rr &>( size( ), 0, alloc( ) );
					t.construct_at_end( std::move_iterator<pointer>( begin_ ),
					                    std::move_iterator<pointer>( end_ ) );
					t.end_ = t.begin_ + ( end_ - begin_ );
					std::swap( first_, t.first_ );
					std::swap( begin_, t.begin_ );
					std::swap( end_, t.end_ );
					std::swap( end_cap( ), t.end_cap( ) );
#if defined( DAW_USE_EXCEPTIONS )
				} catch( ... ) {}
#endif
			}
		}

		constexpr void push_front( const_reference x ) {
			if( begin_ == first_ ) {
				if( end_ < end_cap( ) ) {
					difference_type d = end_cap( ) - end_;
					d = ( d + 1 ) / 2;
					begin_ = std::move_backward( begin_, end_, end_ + d );
					end_ += d;
				} else {
					size_type c =
					  max<size_type>( 2 * static_cast<size_t>( end_cap( ) - first_ ), 1 );
					auto t =
					  split_buffer<value_type, alloc_rr &>( c, ( c + 3 ) / 4, alloc( ) );
					t.construct_at_end( std::move_iterator<pointer>( begin_ ),
					                    std::move_iterator<pointer>( end_ ) );
					std::swap( first_, t.first_ );
					std::swap( begin_, t.begin_ );
					std::swap( end_, t.end_ );
					std::swap( end_cap( ), t.end_cap( ) );
				}
			}
			alloc_traits::construct( alloc( ), std::to_address( begin_ - 1 ), x );
			--begin_;
		}

		constexpr void push_back( const_reference x ) {
			if( end_ == end_cap( ) ) {
				if( begin_ > first_ ) {
					difference_type d = begin_ - first_;
					d = ( d + 1 ) / 2;
					end_ = DAW_MOVE( begin_, end_, begin_ - d );
					begin_ -= d;
				} else {
					size_type c =
					  max<size_type>( 2 * static_cast<size_t>( end_cap( ) - first_ ), 1 );
					auto t = split_buffer<value_type, alloc_rr &>( c, c / 4, alloc( ) );
					t.construct_at_end( std::move_iterator<pointer>( begin_ ),
					                    std::move_iterator<pointer>( end_ ) );
					std::swap( first_, t.first_ );
					std::swap( begin_, t.begin_ );
					std::swap( end_, t.end_ );
					std::swap( end_cap( ), t.end_cap( ) );
				}
			}
			alloc_traits::construct( alloc( ), std::to_address( end_ ), x );
			++end_;
		}

		constexpr void push_front( value_type &&x ) {
			if( begin_ == first_ ) {
				if( end_ < end_cap( ) ) {
					difference_type d = end_cap( ) - end_;
					d = ( d + 1 ) / 2;
					begin_ = std::move_backward( begin_, end_, end_ + d );
					end_ += d;
				} else {
					size_type c =
					  max<size_type>( 2 * static_cast<size_t>( end_cap( ) - first_ ), 1 );
					auto t =
					  split_buffer<value_type, alloc_rr &>( c, ( c + 3 ) / 4, alloc( ) );
					t.construct_at_end( std::move_iterator<pointer>( begin_ ),
					                    std::move_iterator<pointer>( end_ ) );
					std::swap( first_, t.first_ );
					std::swap( begin_, t.begin_ );
					std::swap( end_, t.end_ );
					std::swap( end_cap( ), t.end_cap( ) );
				}
			}
			alloc_traits::construct( alloc( ), std::to_address( begin_ - 1 ),
			                         DAW_MOVE( x ) );
			--begin_;
		}

		constexpr void push_back( value_type &&x ) {
			if( end_ == end_cap( ) ) {
				if( begin_ > first_ ) {
					difference_type d = begin_ - first_;
					d = ( d + 1 ) / 2;
					end_ = DAW_MOVE( begin_, end_, begin_ - d );
					begin_ -= d;
				} else {
					size_type c =
					  max<size_type>( 2 * static_cast<size_t>( end_cap( ) - first_ ), 1 );
					auto t = split_buffer<value_type, alloc_rr &>( c, c / 4, alloc( ) );
					t.construct_at_end( move_iterator<pointer>( begin_ ),
					                    move_iterator<pointer>( end_ ) );
					std::swap( first_, t.first_ );
					std::swap( begin_, t.begin_ );
					std::swap( end_, t.end_ );
					std::swap( end_cap( ), t.end_cap( ) );
				}
			}
			alloc_traits::construct( alloc( ), std::to_address( end_ ),
			                         DAW_MOVE( x ) );
			++end_;
		}

		template<class... Args>
		constexpr void emplace_back( Args &&...args ) {
			if( end_ == end_cap( ) ) {
				if( begin_ > first_ ) {
					difference_type d = begin_ - first_;
					d = ( d + 1 ) / 2;
					end_ = DAW_MOVE( begin_, end_, begin_ - d );
					begin_ -= d;
				} else {
					size_type c = std::max<size_type>(
					  2 * static_cast<size_t>( end_cap( ) - first_ ), 1 );
					auto t = split_buffer<value_type, alloc_rr &>( c, c / 4, alloc( ) );
					t.construct_at_end( move_iterator<pointer>( begin_ ),
					                    move_iterator<pointer>( end_ ) );
					std::swap( first_, t.first_ );
					std::swap( begin_, t.begin_ );
					std::swap( end_, t.end_ );
					std::swap( end_cap( ), t.end_cap( ) );
				}
			}
			alloc_traits::construct( alloc( ), std::to_address( end_ ),
			                         DAW_FWD2( Args, args )... );
			++end_;
		}

		constexpr void pop_front( ) {
			destruct_at_begin( begin_ + 1 );
		}
		constexpr void pop_back( ) {
			destruct_at_end( end_ - 1 );
		}

		/// @brief  Default constructs n objects starting at end_ throws if
		/// construction throws
		/// @pre n > 0
		/// @pre size() + n <= capacity()
		/// @post size() == size() + n
		constexpr void construct_at_end( size_type n ) {
			ConstructTransaction tx( &end_, n );
			while( tx.pos != tx.end ) {
				alloc_traits::construct( alloc( ), std::to_address( tx.pos ) );
				++tx.pos;
			}
		}

		/// @brief Copy constructs n objects starting at end_ from x throws if
		/// construction throws
		/// @pre n > 0
		/// @pre size() + n <= capacity()
		/// @post size() == old size() + n
		/// @post [i] == x for all i in [size() - n, n)
		constexpr void construct_at_end( size_type n, const_reference x ) {
			ConstructTransaction tx( &end_, n );
			while( tx.pos != tx.end ) {
				alloc_traits::construct( alloc( ), std::to_address( tx.pos ), x );
				++tx.pos;
			}
		}

		template<input_iterator InputIter>
		constexpr void construct_at_end( InputIter first, InputIter last ) {
			alloc_rr &a = alloc( );
			for( ; first != last; ++first ) {
				if( end_ == end_cap( ) ) {
					auto const old_cap = end_cap( ) - first_;
					auto const new_cap = std::max<size_type>( 2 * old_cap, 8 );
					split_buffer buf( new_cap, 0, a );
					for( pointer p = begin_; p != end_; ++p, ++buf.end_ ) {
						alloc_traits::construct( buf.alloc( ), std::to_address( buf.end_ ),
						                         DAW_MOVE( *p ) );
					}
					swap( buf );
				}
				alloc_traits::construct( a, std::to_address( end_ ), *first );
				++end_;
			}
		}

		template<forward_iterator ForwardIter>
		constexpr void construct_at_end( ForwardIter first, ForwardIter last ) {
			ConstructTransaction tx( &end_, std::distance( first, last ) );
			for( ; tx.pos != tx.end; ++tx.pos, ++first ) {
				alloc_traits::construct( alloc( ), std::to_address( tx.pos ), *first );
			}
		}

		constexpr void destruct_at_begin( pointer new_begin ) {
			destruct_at_begin( new_begin, is_trivially_destructible<value_type>( ) );
		}

		constexpr void destruct_at_begin( pointer new_begin, std::false_type ) {
			while( begin_ != new_begin ) {
				alloc_traits::destroy( alloc( ), std::to_address( begin_ ) );
				++begin_;
			}
		}

		constexpr void destruct_at_begin( pointer new_begin, std::true_type ) {
			begin_ = new_begin;
		}

		constexpr void destruct_at_end( pointer new_last ) noexcept {
			destruct_at_end( new_last, std::false_type( ) );
		}

		constexpr void destruct_at_end( pointer new_last,
		                                std::false_type ) noexcept {
			while( new_last != end_ )
				alloc_traits::destroy( alloc( ), std::to_address( --end_ ) );
		}

		constexpr void destruct_at_end( pointer new_last,
		                                std::true_type ) noexcept {
			end_ = new_last;
		}

		constexpr void swap( split_buffer &x ) //
		  noexcept( not alloc_traits::propagate_on_container_swap::value or
		            std::is_nothrow_swappable_v<alloc_rr> ) {
			std::swap( first_, x.first_ );
			std::swap( begin_, x.begin_ );
			std::swap( end_, x.end_ );
			std::swap( end_cap( ), x.end_cap( ) );
			si_impl::swap_allocator( alloc( ), x.alloc( ) );
		}

		[[nodiscard]] constexpr bool invariants( ) const {
			if( first_ == nullptr ) {
				if( begin_ != nullptr )
					return false;
				if( end_ != nullptr )
					return false;
				if( end_cap( ) != nullptr )
					return false;
			} else {
				if( begin_ < first_ )
					return false;
				if( end_ < begin_ )
					return false;
				if( end_cap( ) < end_ )
					return false;
			}
			return true;
		}

	private:
		constexpr void
		move_assign_alloc( split_buffer &c, std::true_type ) noexcept(
		  std::is_nothrow_move_assignable_v<allocator_type> ) {
			alloc( ) = DAW_MOVE( c.alloc( ) );
		}

		constexpr void move_assign_alloc( split_buffer &,
		                                  std::false_type ) noexcept {}

		struct ConstructTransaction {
			pointer pos;
			const pointer end;

		private:
			pointer *m_dest;

		public:
			explicit constexpr ConstructTransaction( pointer *p,
			                                         size_type n ) noexcept
			  : pos( *p )
			  , end( *p + n )
			  , m_dest( p ) {}

			constexpr ~ConstructTransaction( ) {
				*m_dest = pos;
			}
		};
	};

	template<class T, class Allocator>
	inline constexpr void
	swap( split_buffer<T, Allocator> &x,
	      split_buffer<T, Allocator> &y ) noexcept( noexcept( x.swap( y ) ) ) {
		x.swap( y );
	}
} // namespace daw