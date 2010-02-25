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

#ifndef WLINE_H
#define WLINE_H

#include <algorithm>
#include <iostream>
#include <vector>

#include "../common/WMixinVector.h"
#include "WPosition.h"

// we need this to find the WLineTest class which is not inside wmath namespace
class WLineTest;

namespace wmath
{
    /**
     * A line is an ordered sequence of WPositions.
     */
//    typedef WMixinVector< WPosition > WLine;
    class WLine : public WMixinVector< WPosition >
    {
    public:
        /**
         * Generates a new line out of a sequence of points.
         *
         * \param points Point sequence
         */
        explicit WLine( const std::vector< WPosition > &points );

        /**
         * Creates an empty line.
         */
        WLine();
    };
} // end of namespace
#endif  // WLINE_H
