// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

/// Create comparison operators for a class that defines compare( rhs )
/// and returns an int where 0 means equal <0 means less than...etc
#define create_comparison_operators( cls_name )                 \
	bool operator==( cls_name const &lhs, cls_name const &rhs ) { \
		return lhs.compare( rhs ) == 0;                             \
	}                                                             \
                                                                \
	bool operator!=( cls_name const &lhs, cls_name const &rhs ) { \
		return lhs.compare( rhs ) != 0;                             \
	}                                                             \
                                                                \
	bool operator<( cls_name const &lhs, cls_name const &rhs ) {  \
		return lhs.compare( rhs ) < 0;                              \
	}                                                             \
                                                                \
	bool operator>( cls_name const &lhs, cls_name const &rhs ) {  \
		return lhs.compare( rhs ) > 0;                              \
	}                                                             \
                                                                \
	bool operator<=( cls_name const &lhs, cls_name const &rhs ) { \
		return lhs.compare( rhs ) <= 0;                             \
	}                                                             \
                                                                \
	bool operator>=( cls_name const &lhs, cls_name const &rhs ) { \
		return lhs.compare( rhs ) >= 0;                             \
	}

#define create_friend_comparison_operators( cls_name )                 \
	friend bool operator==( cls_name const &lhs, cls_name const &rhs ) { \
		return lhs.compare( rhs ) == 0;                                    \
	}                                                                    \
                                                                       \
	friend bool operator!=( cls_name const &lhs, cls_name const &rhs ) { \
		return lhs.compare( rhs ) != 0;                                    \
	}                                                                    \
                                                                       \
	friend bool operator<( cls_name const &lhs, cls_name const &rhs ) {  \
		return lhs.compare( rhs ) < 0;                                     \
	}                                                                    \
                                                                       \
	friend bool operator>( cls_name const &lhs, cls_name const &rhs ) {  \
		return lhs.compare( rhs ) > 0;                                     \
	}                                                                    \
                                                                       \
	friend bool operator<=( cls_name const &lhs, cls_name const &rhs ) { \
		return lhs.compare( rhs ) <= 0;                                    \
	}                                                                    \
                                                                       \
	friend bool operator>=( cls_name const &lhs, cls_name const &rhs ) { \
		return lhs.compare( rhs ) >= 0;                                    \
	}
