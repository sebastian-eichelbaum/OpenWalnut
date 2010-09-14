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

#include "../WMixinVector.h"
#include "WPosition.h"
#include "../WExportCommon.h"

// we need this to find the WLineTest class which is not inside wmath namespace
class WLineTest;

namespace wmath
{
    /**
     * A line is an ordered sequence of WPositions.
     */
//    typedef WMixinVector< WPosition > WLine;
    class OWCOMMON_EXPORT WLine : public WMixinVector< WPosition >
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

        /**
         * Resample this line so it has a number of given points afterwards.
         * \warning This changes your line!
         *
         * \param numPoints Number of sampling points.
         */
        void resample( size_t numPoints );

        /**
         * Reverses the order of the points. (mirroring)
         */
        void reverseOrder();

        /**
         * Computes the length of the line not in terms of points but in terms
         * of accumulated segment lengths.
         *
         * \return Sum of all line segment lengths
         */
        double pathLength() const;

        /**
         * Returns the point in the middle of the line. In case of an even sized
         * line the mid point is the same as if there were only size()-1 many
         * elements present.
         *
         * \throws WOutOfBounds In case its called on an empty line
         *
         * \return Const reference to the midpoint element.
         */
        const wmath::WPosition& midPoint() const;

        /**
         * Compares this line with another line point wise upto a given delta.
         *
         * \param other The other line
         * \param delta Specifying the environment upto this two points are considered to be the same
         *
         * \return -1 in case of the two fibers are considered equal, otherwise the first position on which they differ is returned.
         */
        int equalsDelta( const wmath::WLine& other, double delta ) const;
    };
} // end of namespace
#endif  // WLINE_H
