// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw/daw_cpp_feature_check.h"
#include "daw/daw_restrict.h"
#include "daw/daw_string_view.h"
#include "daw/daw_unique_ptr.h"

#include <cstdio>

namespace daw {
	namespace cfile_impl {
		struct cfile_deleter {
			cfile_deleter( ) = default;

			DAW_CPP23_STATIC_CALL_OP inline void
			operator( )( std::FILE *fp ) DAW_CPP23_STATIC_CALL_OP_CONST noexcept {
				std::fclose( fp );
			}
		};
	} // namespace cfile_impl

	using unique_file_ptr = daw::unique_ptr<std::FILE, cfile_impl::cfile_deleter>;

	[[nodiscard]] inline unique_file_ptr
	open_cfile( daw::string_view const path, char const *DAW_RESTRICT modes ) {
		return unique_file_ptr( std::fopen( path.get_c_str( ).c_str( ), modes ) );
	}
} // namespace daw
