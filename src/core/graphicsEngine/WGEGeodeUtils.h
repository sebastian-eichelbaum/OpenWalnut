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
#include "../common/math/linearAlgebra/WPosition.h"
#include "../common/WColor.h"
#include "../common/datastructures/WColoredVertices.h"

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
    osg::ref_ptr< osg::Geode > generateBoundingBoxGeode( const WBoundingBox& bb, const WColor& color );

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
    osg::ref_ptr< osg::Node > generateSolidBoundingBoxNode( const WBoundingBox& bb, const WColor& color, bool threeDTexCoords = true );

    /**
     * Creates a osg::Geometry containing an unit cube, having 3D texture coordinates.
     *
     * \param color the color to set for all vertices
     *
     * \return the geometry
     */
    osg::ref_ptr< osg::Geometry > createUnitCube( const WColor& color );

    /**
     * Creates a osg::Geometry containing an unit cube as line-strips, having 3D texture coordinates.
     *
     * \param color the color to set for all vertices
     *
     * \return the geometry
     */
    osg::ref_ptr< osg::Geometry > createUnitCubeAsLines( const WColor& color );

    /**
     * Extract the vertices and triangles from a WTriangleMesh and save them
     * into an osg::Geometry. It can use the normals and per-vertex colors of the mesh.
     *
     * \param mesh the WTriangleMesh used as input
     * \param includeNormals When true, calculate the vertex normals and include
     *                       them into the geometry.
     * \param defaultColor This color is used in case the useMeshColor parameter is false or no colors are defined in the mesh.
     * \param lighting if true, a standard lighting is activated for this geometry
     * \param useMeshColor if true, the mesh color is used. If false, the defaultColor is used.
     * \return an osg::Geometry containing the mesh
     * \note mesh cannot be const since osg::Geometry needs non-const pointers to the contained arrays
     */
    osg::ref_ptr< osg::Geometry > convertToOsgGeometry( WTriangleMesh::SPtr mesh,
                                                        const WColor& defaultColor = WColor( 1.0, 1.0, 1.0, 1.0 ),
                                                        bool includeNormals = false,
                                                        bool lighting = false,
                                                        bool useMeshColor = true );

    /**
     * Extract the vertices and triangles from a WTriangleMesh and save them
     * into an osg::Geometry. It can use the normals and per-vertex colors of the mesh.
     * This method additionally uses the specified vertexID-color map to provide additional coloring.
     *
     * \param mesh the WTriangleMesh used as input
     * \param colorMap the map from vertex to color.
     * \param includeNormals When true, calculate the vertex normals and include
     *                       them into the geometry.
     * \param defaultColor This color is used in case the colorMap does not provide a color for a vertex
     * \param lighting if true, a standard lighting is activated for this geometry*
     * \return an osg::Geometry containing the mesh
     * \note mesh cannot be const since osg::Geometry needs non-const pointers to the contained arrays
     */
    osg::ref_ptr< osg::Geometry > convertToOsgGeometry( WTriangleMesh::SPtr mesh, const WColoredVertices& colorMap,
                                                                   const WColor& defaultColor = WColor( 1.0, 1.0, 1.0, 1.0 ),
                                                                   bool includeNormals = false,
                                                                   bool lighting = false
                                                                   );

    /**
     * Convert triangle mesh to lines representing it. Draws lines twice (ATM).
     *
     * \param mesh The WTriangleMesh used as input.
     * \param defaultColor This color is used in case the useMeshColor parameter is false or no colors are defined in the mesh.
     * \param useMeshColor If true, the mesh color is used. If false, the defaultColor is used.
     *
     * \return an osg::Geometry containing the mesh as lines
     */
    osg::ref_ptr< osg::Geometry > convertToOsgGeometryLines( WTriangleMesh::SPtr mesh,
                                                             const WColor& defaultColor = WColor( 1.0, 1.0, 1.0, 1.0 ),
                                                             bool useMeshColor = true );

    /**
     * helper function to add a label somewhere
     *
     * \param position position of the label
     * \param text text
     * \return a positionattitudetransfom object containing the label
     */
    osg::ref_ptr< osg::PositionAttitudeTransform > addLabel( osg::Vec3 position, std::string text );

    /**
     * helper function to add a label with it's position vector
     *
     * \param position position of the label
     * \return a positionattitudetransfom object containing the label
     */
    osg::ref_ptr< osg::PositionAttitudeTransform > vector2label( osg::Vec3 position );

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
    osg::ref_ptr< osg::Geode > genFinitePlane( double xSize,
                                                          double ySize,
                                                          const WPlane& p,
                                                          const WColor& color = WColor( 0.0, 0.7, 0.7, 1.0 ),
                                                          bool border = false );

    /**
     * Create a coordinate system. The coordinate system is build from cylinders and cones and includes a positive-to-negative
     * color gradient.
     *
     * \param middle osg::Vec3( middleX, middleY, middleZ ) middle points of X, Y, Z
     * \param sizeX whole lenght of X
     * \param sizeY whole lenght of Y
     * \param sizeZ whole lenght of Z
     *
     * \return Group Node
     */
    osg::ref_ptr< osg::Group > creatCoordinateSystem(
        osg::Vec3 middle,
        double sizeX,
        double sizeY,
        double sizeZ
    );

    /**
     * Generates a geode out of two vectors and an origin position.
     *
     * \param base the origin position. NOT the center.
     * \param a the first vector spanning the plane
     * \param b the second vector spanning the plane
     *
     * \return the geode
     */
    osg::ref_ptr< osg::Geode > genFinitePlane( osg::Vec3 const& base, osg::Vec3 const& a, osg::Vec3 const& b );
} // end of namespace wge

#endif  // WGEGEODEUTILS_H
