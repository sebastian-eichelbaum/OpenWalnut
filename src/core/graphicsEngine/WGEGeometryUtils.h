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

#ifndef WGEGEOMETRYUTILS_H
#define WGEGEOMETRYUTILS_H

#include <vector>

#include <osg/Array>

#include "../common/math/linearAlgebra/WPosition.h"
#include "WTriangleMesh.h"



namespace wge
{
    /**
     * Creates out of eight corner vertices QUAD vertices.
     *
     * \param corners The eight corner vertices which must be in the following order:
     *
     * \verbatim
        z-axis  y-axis
        |      /
        | h___/_g
        |/:    /|
        d_:___c |
        | :...|.|
        |.e   | f
        |_____|/ ____x-axis
       a      b
       \endverbatim
     *
     * The QUADS are generated in the following order:
     * - a,b,c,d
     * - b,f,g,c
     * - f,e,h,g
     * - e,a,d,h
     * - d,c,g,h
     * - a,b,f,e
     *
     * \return OSG vertex array where every four vertices describing a QUAD.
     */
    osg::ref_ptr< osg::Vec3Array > generateCuboidQuads( const std::vector< WPosition >& corners );

    /**
     * Generates for a QUAD given via 3 three points ( the fourth is not needed ) the
     * normal.
     *
     *\param a First point of the QUAD
     *\param b Second point of the QUAD
     *\param c Third point of the QUAD
     *
     *\return OSG Vector of the normal of the QUAD
     */
    osg::Vec3 getQuadNormal( const WPosition& a, const WPosition& b, const WPosition& c );

    /**
     * Generates for all QUADS of the Cuboid the normals in the following order:
     *
     * \verbatim
        z-axis  y-axis
        |      /
        | h___/_g
        |/:    /|
        d_:___c |
        | :...|.|
        |.e   | f
        |_____|/ ____x-axis
       a      b
       \endverbatim
     *
     * - a,b,c,d
     * - b,f,g,c
     * - f,e,h,g
     * - e,a,d,h
     * - d,c,g,h
     * - a,b,f,e
     *
     *\param corners Corner points of the cuboid.
     *
     *\return Array of normals in the order as shown above.
     */
    osg::ref_ptr< osg::Vec3Array > generateCuboidQuadNormals( const std::vector< WPosition >& corners );

    /**
     * Calculate the Delaunay Triangulation of the given points.
     *
     * If the parameter transformationFactor is not zero, the points will be
     * transformed to calcule the triangles to get a better triangulation, but
     * the returned mesh will consist of the original points.
     * Every point which lies above the centroid of all points will be put away
     * from this centroid by transformationFactor * height above the centroid.
     * Any point below the centroid will be drawn to the centroid because its
     * height is negative. This effect is inverted with a negative
     * transformationFactor.
     * This transformation is used to calculate the head surface between several
     * electrodes, were a transformationFactor of -0.005 is reasonable.
     *
     * \param points               vector of the points to triangulate
     * \param transformationFactor magnitude of the transformation before the
     *                                 triangulation
     * \return triangulation as WTriangleMesh
     */
    WTriangleMesh::SPtr triangulate( const std::vector< WPosition >& points, double transformationFactor = 0.0 );
}

#endif  // WGEGEOMETRYUTILS_H
