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

#ifndef WGEGEODEUTILS_H
#define WGEGEODEUTILS_H

#include <string>
#include <vector>

#include <osg/Array>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/Vec3>
#include <osgText/Text>

#include "../common/math/WLine.h"
#include "../common/math/WPlane.h"
#include "../common/math/WPosition.h"
#include "../common/WColor.h"
#include "WExportWGE.h"
#include "WGEGeometryUtils.h"
#include "WGESubdividedPlane.h"
#include "WGEUtils.h"
#include "WTriangleMesh.h"

namespace wge
{
    /**
     * Generates an OSG geode for the bounding box.
     *
     * \param bb The axis aligned bounding box to generate a geode from.
     * \param color The color in which the bounding box should be generated
     *
     * \return The OSG geode containing the 12 edges of the box.
     */
    osg::ref_ptr< osg::Geode > WGE_EXPORT generateBoundingBoxGeode( const WBoundingBox& bb, const WColor& color );

    /**
     * Generates an OSG node for the specified bounding box. It uses solid faces. This actually returns a MatrixTransform node and is especially
     * useful for shader based raytracing.
     *
     * \param bb The axis aligned bounding box
     * \param color The color in which the bounding box should be generated
     * \param threeDTexCoords True if 3D texture coordinates should be created.
     *
     * \return The OSG node containing the 12 edges of the box.
     */
    osg::ref_ptr< osg::Node > WGE_EXPORT generateSolidBoundingBoxNode( const WBoundingBox& bb, const WColor& color, bool threeDTexCoords = true );

    /**
     * Creates a osg::Geometry containing an unit cube, having 3D texture coordinates.
     *
     * \param color the color to set for all vertices
     *
     * \return the geometry
     */
    osg::ref_ptr< osg::Geometry > WGE_EXPORT createUnitCube( const WColor& color );

    /**
     * Creates a osg::Geometry containing an unit cube as line-strips, having 3D texture coordinates.
     *
     * \param color the color to set for all vertices
     *
     * \return the geometry
     */
    osg::ref_ptr< osg::Geometry > WGE_EXPORT createUnitCubeAsLines( const WColor& color );

    /**
     * Extract the vertices and triangles from a WTriangleMesh and save them
     * into an osg::Geometry.
     *
     * \param mesh the WTriangleMesh used as input
     * \param includeNormals When true, calculate the vertex normals and include
     *                       them into the geometry.
     * \return an osg::Geometry containing the mesh
     */
    osg::ref_ptr< osg::Geometry > WGE_EXPORT convertToOsgGeometry( WTriangleMesh* mesh, bool includeNormals = false );

    /**
     * Generates a line geode with thickness and color as parameters.
     *
     * \param line sequence of points
     * \param thickness How thick the line strip should be
     * \param color If present this color is used for the whole line, otherwise local coloring is used
     *
     * \return The new assembled geode for this line
     */
    osg::ref_ptr< osg::Geode > WGE_EXPORT generateLineStripGeode( const WLine& line,
                                                                  const float thickness = 3.0f,
                                                                  const WColor& color = WColor( 0, 0, 0, 0 ) );

    /**
     * helper function to add a label somewhere
     *
     * \param position position of the label
     * \param text text
     * \return a positionattitudetransfom object containing the label
     */
    osg::ref_ptr< osg::PositionAttitudeTransform > WGE_EXPORT addLabel( osg::Vec3 position, std::string text );

    /**
     * helper function to add a label with it's position vector
     *
     * \param position position of the label
     * \return a positionattitudetransfom object containing the label
     */
    osg::ref_ptr< osg::PositionAttitudeTransform > WGE_EXPORT vector2label( osg::Vec3 position );

    /**
     * Generates a geode out of a Plane with a fixed size in direction of the vectors which span that plane.
     *
     * \param xSize how far the plane from its center along the x-axis should be drawn (both directions)
     * \param ySize how far the plane from its center along the y-axis should be drawn (both directions)
     * \param p The plane instance
     * \param color The color of the plane
     * \param border If true than a border around each plane is drawn in inverse color of the plane
     *
     * \return The new assembled geode for this plane
     */
    osg::ref_ptr< osg::Geode > WGE_EXPORT genFinitePlane( double xSize,
                                                          double ySize,
                                                          const WPlane& p,
                                                          const WColor& color = WColor( 0.0, 0.7, 0.7, 1.0 ),
                                                          bool border = false );

    /**
     * Generates a geode out of two vectors and an origin position.
     *
     * \param base the origin position. NOT the center.
     * \param a the first vector spanning the plane
     * \param b the second vector spanning the plane
     *
     * \return the geode
     */
    osg::ref_ptr< osg::Geode > WGE_EXPORT genFinitePlane( osg::Vec3 const& base, osg::Vec3 const& a, osg::Vec3 const& b );

    /**
     * Generates a plane subdivided into quads.
     *
     * \param resX How many quads in x-direction
     * \param resY How many quads in y-direction
     * \param spacing \todo(math): Not implement yet
     *
     * \return The new uncolored plane geode
     */
    osg::ref_ptr< WGESubdividedPlane > WGE_EXPORT genUnitSubdividedPlane( size_t resX, size_t resY, double spacing = 0.01 );

    /**
     * For each points in the STL container generate small cubes.
     *
     * \param points Center point of the cubes
     * \param size The size of the cubes
     * \param color The color of the cubes
     * \tparam An STL container with WPositions as elements ( don't try it with different than vector, set, list or queue )
     *
     * \return Geode with as many cubes as points in the container where each cube is around a certain position.
     */
    template< class Container > osg::ref_ptr< osg::Geode > genPointBlobs( boost::shared_ptr< Container > points,
                                                                          double size,
                                                                          const WColor& color = WColor( 1.0, 0.0, 0.0, 1.0 ) );
} // end of namespace wge

template< class Container > inline osg::ref_ptr< osg::Geode > wge::genPointBlobs( boost::shared_ptr< Container > points,
                                                                                  double size,
                                                                                  const WColor& color )
{
    osg::ref_ptr< osg::Vec3Array > vertices = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    osg::ref_ptr< osg::Vec4Array > colors   = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    osg::ref_ptr< osg::Geometry >  geometry = osg::ref_ptr< osg::Geometry >( new osg::Geometry );
    osg::ref_ptr< osg::Vec3Array > normals  = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );

    for( typename Container::const_iterator point = points->begin(); point != points->end(); ++point )
    {
        const WPosition& pos = *point;
        std::vector< WPosition > corners;
        corners.reserve( 8 );
        double halfSize = size / 2.0;
        corners.push_back( WPosition( pos[0] - halfSize, pos[1] - halfSize, pos[2] - halfSize ) );
        corners.push_back( WPosition( pos[0] + halfSize, pos[1] - halfSize, pos[2] - halfSize ) );
        corners.push_back( WPosition( pos[0] + halfSize, pos[1] - halfSize, pos[2] + halfSize ) );
        corners.push_back( WPosition( pos[0] - halfSize, pos[1] - halfSize, pos[2] + halfSize ) );
        corners.push_back( WPosition( pos[0] - halfSize, pos[1] + halfSize, pos[2] - halfSize ) );
        corners.push_back( WPosition( pos[0] + halfSize, pos[1] + halfSize, pos[2] - halfSize ) );
        corners.push_back( WPosition( pos[0] + halfSize, pos[1] + halfSize, pos[2] + halfSize ) );
        corners.push_back( WPosition( pos[0] - halfSize, pos[1] + halfSize, pos[2] + halfSize ) );

        osg::ref_ptr< osg::Vec3Array > ver = generateCuboidQuads( corners );
        vertices->insert( vertices->end(), ver->begin(), ver->end() );
        osg::ref_ptr< osg::Vec3Array > nor = generateCuboidQuadNormals( corners );
        normals->insert( normals->end(), nor->begin(), nor->end() );
        geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUADS, vertices->size() - ver->size(), ver->size() ) );
    }

    geometry->setVertexArray( vertices );
    colors->push_back( color );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_OVERALL );
    geometry->setNormalArray( normals );
    geometry->setNormalBinding( osg::Geometry::BIND_PER_PRIMITIVE );
    osg::ref_ptr< osg::Geode > geode = osg::ref_ptr< osg::Geode >( new osg::Geode );
    geode->addDrawable( geometry );
    return geode;
}
#endif  // WGEGEODEUTILS_H
