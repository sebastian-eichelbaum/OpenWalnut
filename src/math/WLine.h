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

#include <iostream>
#include <vector>

#include "WPosition.h"
#include "../common/WStringUtils.hpp"

// we need this to find the WLineTest class which is not inside wmath namespace
class WLineTest;

namespace wmath
{
// Please notice that many small member functions are inlined for performance issues
/**
 * A line is an ordered sequence of WPositions.
 */
class WLine
{
friend class ::WLineTest;
public:
    /**
     * Constructs a new line with the given points in the given order
     */
    explicit WLine( const std::vector< WPosition > &points )
        : m_points( points )
    {
    }

    /**
     * Copy constructor
     */
    WLine( const WLine& other ) : m_points( other.m_points )
    {
    }

    /**
     * \return true if both lines have a same point vector
     */
    bool operator==( const WLine &rhs ) const
    {
        return m_points == rhs.m_points;
    }

    /**
     * \return false if both lines have a same point vector
     */
    bool operator!=( const WLine &rhs ) const
    {
        return m_points != rhs.m_points;
    }

    /**
     * Get number of points (length) the value consists of.
     */
    size_t size()
    {
        return m_points.size();
    }

    /**
     * \return Const reference to the i'th position. This is const since
     * we want an read only access.
     */
    const WPosition& operator[]( size_t index ) const
    {
        assert( index < m_points.size() );
        return m_points[index];
    }

    /**
     * Gives a meaningful representation of this object to the given
     * output stream.
     */
    friend std::ostream& operator<<( std::ostream& os, const WLine &rhs )
    {
        using string_utils::operator<<;
        os << rhs.m_points;
        return os;
    }

protected:
private:
    std::vector< WPosition > m_points;
};
}

#endif  // WLINE_H
