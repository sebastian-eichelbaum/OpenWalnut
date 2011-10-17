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

#ifndef WTRACTADAPTER_H
#define WTRACTADAPTER_H

#include <vector>

#include <boost/shared_ptr.hpp>

#include "../common/math/linearAlgebra/WLinearAlgebra.h"

/**
 * Adapter
 */
class WTractAdapter
{
public:
    /**
     * Constructs a new WTract which has \ref numPoints points and a startIndex inside of the
     * given components array.
     *
     * \param pointComponents Array where the components of the tract points are inside of
     * \param startIndex The position inside of the components array of the first x coordinate.
     * \param numPoints How many points this tract has
     */
    WTractAdapter( boost::shared_ptr< const std::vector< float > > pointComponents, size_t startIndex, size_t numPoints );

    /**
     * How many positions this tract incorporates.
     *
     * \return number of points
     */
    size_t numPoints() const;

    /**
     * Constructs and returns a \ref WPosition out of the i'th position of this tract.
     *
     * \param index The index of the position of this tract. It may start at \c 0 and is always
     * smaller than \ref numPoints().
     *
     * \return The i'th position of this tract as \ref WPosition.
     */
    WPosition operator[]( size_t index ) const;

//    void reset( boost::shared_ptr< const WTractData > tracts, size_t startIndex, size_t numPoints )

protected:
private:
    /**
     * The array where the components of this tracts live. But you will need the starting position
     * and the length of the tract to access them.
     */
    boost::shared_ptr< const std::vector< float > > m_pointComponents;

    /**
     * How many \e points aka WPositions this tract consists of.
     */
    size_t m_numPoints;

    /**
     * The index of the x-component of the first point of this tract inside the \ref m_pointComponents array.
     */
    size_t m_startIndex;
};

inline size_t WTractAdapter::numPoints() const
{
    return m_numPoints;
}

#endif  // WTRACTADAPTER_H
