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
#include "../common/WStringUtils.h"

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
// The following friend IS documented in this file
// \cond
friend std::ostream& operator<<( std::ostream& os, const WLine &rhs );
// \endcond

public:
    /**
     * Constructs a new line with the given points in the given order
     *
     * \param points The points this line consists of
     */
    explicit WLine( const std::vector< WPosition > &points );

    /**
     * \param rhs Right hand side operand
     * \return true if both lines have a same point vector
     */
    bool operator==( const WLine &rhs ) const;

    /**
     * \param rhs Right hand side operand
     * \return false if both lines have a same point vector
     */
    bool operator!=( const WLine &rhs ) const;

    /**
     * Get number of points (length) the value consists of.
     */
    size_t size() const;

    /**
     * \param index Index for the i'th point of this line
     * \return Const reference to the i'th position. This is const since
     * we want an read only access.
     */
    const WPosition& operator[]( size_t index ) const;


protected:
private:
    std::vector< WPosition > m_points; //!< stores the points of this line
};

inline size_t WLine::size() const
{
    return m_points.size();
}

inline bool WLine::operator==( const WLine &rhs ) const
{
    return m_points == rhs.m_points;
}

inline bool WLine::operator!=( const WLine &rhs ) const
{
    return m_points != rhs.m_points;
}

inline const WPosition& WLine::operator[]( size_t index ) const
{
    assert( index < m_points.size() );
    return m_points[index];
}

/**
 * Gives a meaningful representation of this object to the given
 * output stream.
 *
 * \param os The outputstream
 * \param rhs Right hand side operand
 */
inline std::ostream& operator<<( std::ostream& os, const WLine &rhs )
{
    using string_utils::operator<<;
    return os << rhs.m_points;
}

} // end of namespace
#endif  // WLINE_H
