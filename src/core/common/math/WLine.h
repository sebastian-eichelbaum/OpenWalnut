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

#include "../WBoundingBox.h"

#include "../WMixinVector.h"
#include "linearAlgebra/WLinearAlgebra.h"

// forward declarations
class WLineTest;

/**
 * A line is an ordered sequence of WPositions.
 */
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

    /**
     * Resample this line so it has a number of given points afterwards.
     * \warning This changes your line!
     *
     * \param numPoints Number of sampling points.
     */
    void resampleByNumberOfPoints( size_t numPoints );

    /**
     *
     *
     * \warning This may elongate your line at max. by the newSegmentLength
     *
     * \param newSegementLength
     */
    void resampleBySegmentLength( double newSegementLength );

    /**
     * Reverses the order of the points. (mirroring)
     */
    void reverseOrder();

    /**
     * Collapse samplepoints which are equal and neighboured.
     */
    void removeAdjacentDuplicates();

    /**
     * Put the line into reverse ordering if the reverse ordering would have a
     * similar direction to the given line. That means if the start point (or
     * multiple selected sample points) of the given line will better match to
     * end point (or multiple selected sample points) of this line (in term of
     * direction) the line is reordered.
     *
     * \param other The line giving the direction to align this line to.
     */
    void unifyDirectionBy( const WLine& other );
};

// Some convinience functions as non-member non-friend functions

/**
 * Computes a AABB (axis aligned bounding box) for all positions inside this line.
 *
 * \param line The line to compute the bounding box for.
 *
 * \return The AABB for this line.
 */
WBoundingBox computeBoundingBox( const WLine& line );

/**
 * Computes the length of a line in terms of accumulated segment lengths.
 *
 * \param line The line which used for computations
 *
 * \return Sum of all line segment lengths
 */
double pathLength( const WLine& line );

/**
 * Returns the point in the middle of a line. In case of an even sized
 * line the mid point is the same as if there were only size()-1 many
 * elements present.
 *
 * \param line The line to compute the mid point for.
 *
 * \throws WOutOfBounds In case its called on an empty line
 *
 * \return Const reference to the midpoint element.
 */
const WPosition& midPoint( const WLine& line );

/**
 * Compares two lines with each other point wise upto a given delta.
 *
 * \param line The first line
 * \param other The other line
 * \param delta Specifying the environment upto this two points are considered to be the same
 *
 * \return -1 in case of the two fibers are considered equal, otherwise the first position on which they differ is returned.
 */
int equalsDelta( const WLine& line, const WLine& other, double delta );

/**
 * Compute the maximal segment length of all segements of a line. If there are no segements meaning
 * zero or one point, zero is returned.
 *
 * \param line The line used for computation of the max segment length
 *
 * \return Max segement length or zero if there aren't any.
 */
double maxSegmentLength( const WLine& line );

/**
 * Boolean predicate indicating that the first line has more points then
 * the second one.
 *
 * \param first First line
 * \param second Second line
 * \return True if the first line has more points than the second
 */
bool hasMorePointsThen( const WLine& first, const WLine& second );

inline bool hasMorePointsThen( const WLine& first, const WLine& second )
{
    return first.size() > second.size();
}

#endif  // WLINE_H
