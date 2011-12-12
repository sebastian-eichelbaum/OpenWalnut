//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS
// For more information see http://www.openwalnut.org/copying
//
// This file is part of OpenWalnut.
//
// OpenWalnut is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// OpenWalnut is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with OpenWalnut. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------

#include <limits>

#include "WLimits.h"

namespace wlimits
{
    const double MAX_DOUBLE = std::numeric_limits< double >::max();
    const float MAX_FLOAT = std::numeric_limits< float >::max();
    const size_t MAX_SIZE_T = std::numeric_limits< size_t >::max();
    const int32_t MAX_INT32_T = std::numeric_limits< int32_t >::max();
    const double MIN_DOUBLE = std::numeric_limits< double >::min();
    const double DBL_EPS = std::numeric_limits< double >::epsilon();
    const float FLT_EPS = std::numeric_limits< float >::epsilon();
}

