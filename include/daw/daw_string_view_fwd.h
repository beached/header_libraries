// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/header_libraries
//

#pragma once

#include "daw_string_view_version.h"

#if DAW_STRINGVIEW_VERSION == 1
#include "daw_string_view1_fwd.h"
#else
#include "daw_string_view2_fwd.h"
#endif
