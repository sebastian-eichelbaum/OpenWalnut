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

#include <map>
#include <string>
#include <vector>

#include <osg/Array>
#include <osgUtil/DelaunayTriangulator>

#include "../common/math/linearAlgebra/WLinearAlgebra.h"
#include "WGEGeometryUtils.h"
#include "WGEUtils.h"
#include "WTriangleMesh.h"
#include "exceptions/WGEException.h"

osg::ref_ptr< osg::Vec3Array > wge::generateCuboidQuads( const std::vector< WPosition >& corners )
{
    osg::ref_ptr< osg::Vec3Array > vertices = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );

    // Surfaces
    vertices->push_back( corners[0] );
    vertices->push_back( corners[1] );
    vertices->push_back( corners[2] );
    vertices->push_back( corners[3] );

    vertices->push_back( corners[1] );
    vertices->push_back( corners[5] );
    vertices->push_back( corners[6] );
    vertices->push_back( corners[2] );

    vertices->push_back( corners[5] );
    vertices->push_back( corners[4] );
    vertices->push_back( corners[7] );
    vertices->push_back( corners[6] );

    vertices->push_back( corners[4] );
    vertices->push_back( corners[0] );
    vertices->push_back( corners[3] );
    vertices->push_back( corners[7] );

    vertices->push_back( corners[3] );
    vertices->push_back( corners[2] );
    vertices->push_back( corners[6] );
    vertices->push_back( corners[7] );

    vertices->push_back( corners[0] );
    vertices->push_back( corners[1] );
    vertices->push_back( corners[5] );
    vertices->push_back( corners[4] );
    return vertices;
}

osg::Vec3 wge::getQuadNormal( const WPosition& a,
                              const WPosition& b,
                              const WPosition& c )
{
    WVector3d vec1 = a - b;
    WVector3d vec2 = c - b;
    WVector3d normal = cross( vec2, vec1 );
    return normalize( normal );
}

osg::ref_ptr< osg::Vec3Array > wge::generateCuboidQuadNormals( const std::vector< WPosition >& corners )
{
    osg::ref_ptr< osg::Vec3Array > vertices = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );

    vertices->push_back( getQuadNormal( corners[0], corners[1], corners[2] ) );
    vertices->push_back( getQuadNormal( corners[1], corners[5], corners[6] ) );
    vertices->push_back( getQuadNormal( corners[5], corners[4], corners[7] ) );
    vertices->push_back( getQuadNormal( corners[4], corners[0], corners[3] ) );
    vertices->push_back( getQuadNormal( corners[3], corners[2], corners[6] ) );
    vertices->push_back( getQuadNormal( corners[0], corners[1], corners[5] ) );
    return vertices;
}

WTriangleMesh::SPtr wge::triangulate( const std::vector< WPosition >& points, double transformationFactor )
{
    WAssert( points.size() > 2, "The Delaunay triangulation needs at least 3 vertices!" );

    osg::ref_ptr< osg::Vec3Array > osgPoints = wge::osgVec3Array( points );

    if( transformationFactor != 0.0 )
    {
        // Transform the points as described in the Doxygen description of
        // this function.
        osg::Vec3 centroid;
        for( std::size_t pointID = 0; pointID < osgPoints->size(); ++pointID )
        {
            centroid += (*osgPoints)[pointID];
        }
        centroid /= osgPoints->size();

        for( std::size_t pointID = 0; pointID < osgPoints->size(); ++pointID )
        {
            const double factor = ( (*osgPoints)[pointID].z() - centroid.z() ) * transformationFactor + 1.0;
            (*osgPoints)[pointID].x() = ( (*osgPoints)[pointID].x() - centroid.x() ) * factor + centroid.x();
            (*osgPoints)[pointID].y() = ( (*osgPoints)[pointID].y() - centroid.y() ) * factor + centroid.y();
        }
    }

    // The osg triangulator sorts the points and returns the triangles with
    // the indizes of the sorted points. Since we don't want to change the
    // sequence of the points, we have to save the original index of each
    // point.
    std::map< osg::Vec3, size_t > map;
    for( size_t index = 0; index < osgPoints->size(); ++index )
    {
        map[ (*osgPoints)[index] ] = index;
    }

    osg::ref_ptr< osgUtil::DelaunayTriangulator > triangulator( new osgUtil::DelaunayTriangulator( osgPoints ) );

    bool triangulationResult = triangulator->triangulate();

    WAssert( triangulationResult, "Something went wrong in triangulation." );

    osg::ref_ptr< const osg::DrawElementsUInt > osgTriangles( triangulator->getTriangles() );
    size_t nbTriangles = osgTriangles->size() / 3;
    std::vector<  size_t > triangles( osgTriangles->size()  );
    for( size_t triangleID = 0; triangleID < nbTriangles; ++triangleID )
    {
        // Convert the new index of the osgTriangle to the original
        // index stored in map.
        size_t vertID = triangleID * 3;
        triangles[vertID + 0] = map[ (*osgPoints)[ (*osgTriangles)[vertID + 0] ] ];
        triangles[vertID + 1] = map[ (*osgPoints)[ (*osgTriangles)[vertID + 1] ] ];
        triangles[vertID + 2] = map[ (*osgPoints)[ (*osgTriangles)[vertID + 2] ] ];
    }

    // I needed this reconversion using osgVec3Array because the triangulator changed my positions somehow.
    return WTriangleMesh::SPtr( new WTriangleMesh( wge::osgVec3Array( points ), triangles ) );
}
