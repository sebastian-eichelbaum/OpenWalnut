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

#include <osg/MixinVector>

#include "WPosition.h"

// we need this to find the WLineTest class which is not inside wmath namespace
class WLineTest;

namespace wmath
{
    /**
     * A line is an ordered sequence of WPositions.
     */
    typedef osg::MixinVector< WPosition > WLine;

    /**
     * Writes every mixin vector to an output stream such as cout, if its
     * elements have an output operator defined.
     *
     * \param os The output stream where the elements are written to
     * \param v Vector containing the elements
     * \return The output stream again.
     */
    template< class T > std::ostream& operator<<( std::ostream& os, const osg::MixinVector< T >& v )
    {
        std::stringstream result;
        result << "[" << std::scientific << std::setprecision( 16 );
        std::copy( v.begin(), v.end(), std::ostream_iterator< T >( result, ", " ) );
        os << string_utils::rTrim( result.str(), ", " ) << "]";
        return os;
    }
} // end of namespace
#endif  // WLINE_H
