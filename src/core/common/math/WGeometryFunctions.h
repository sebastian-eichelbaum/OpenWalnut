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

#ifndef WGEOMETRYFUNCTIONS_H
#define WGEOMETRYFUNCTIONS_H

#include <map>
#include <utility>
#include <vector>

#include "../WAssert.h"
#include "linearAlgebra/WLinearAlgebra.h"

namespace utility
{
    /**
     * \class Edge
     *
     * A helper class that is used to store edges as pairs of vertex indices. The indices are sorted.
     */
    class Edge : public std::pair< unsigned int, unsigned int >
    {
    public:
        /**
         * Constructor that sorts the input indices.
         *
         * \param i the first index.
         * \param j The second index.
         */
        Edge( unsigned int i, unsigned int j )
            : std::pair< unsigned int, unsigned int >( i < j ? i : j, i < j ? j : i )
        {
        }

        /**
         * Compare two edges. This operator defines a weak ordering on the edges.
         *
         * \param e The edge to compare to.
         * \return True, iff this edge is 'smaller' than the given edge.
         */
        bool operator < ( Edge const& e ) const
        {
            return first < e.first || ( first == e.first && second < e.second );
        }

        /**
         * Compare two edges for equality.
         *
         * \param e The edge to compare to.
         * \return True, iff this edge has the same vertex indices as the given edge.
         */
        bool operator == ( Edge const& e ) const
        {
            return first == e.first && second == e.second;
        }
    };

} // namespace utility

/**
 * Tesselates an icosahedron in order to generate a triangle-based approximation of a sphere.
 * The content of the provided vectors will be cleared and replaced. Triangle vertices are stored as
 * successive values in the triangle vector.
 *
 * \param[out] vertices The vertices of the mesh.
 * \param[out] triangles The resulting triangles as a list of indices into the vertex vector.
 * \param level The tesselation level.
 */
void tesselateIcosahedron( std::vector< WVector3d >* vertices, std::vector< unsigned int >* triangles, unsigned int level );

#endif  // WGEOMETRYFUNCTIONS_H
