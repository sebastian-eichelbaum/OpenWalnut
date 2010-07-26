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

#include <osg/Geode>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osgText/Text>

#include "../common/datastructures/WTriangleMesh.h"
#include "../common/WColor.h"
#include "../common/math/WPosition.h"
#include "../common/math/WLine.h"


namespace wge
{
    /**
     * Generates an OSG geode for the bounding box.
     *
     * \param pos1 Front lower left corner
     * \param pos2 Back upper right corner
     * \param color The color in which the bounding box should be generated
     *
     * \return The OSG geode containing the 12 edges of the box.
     */
    osg::ref_ptr< osg::Geode > generateBoundingBoxGeode( const wmath::WPosition& pos1, const wmath::WPosition& pos2, const WColor& color );

    /**
     * Generates an OSG node for the specified bounding box. It uses solid faces. This actually returns a MatrixTransform node and is especially
     * useful for shader based raytracing.
     *
     * \param pos1 Front lower left corner
     * \param pos2 Back upper right corner
     * \param color The color in which the bounding box should be generated
     * \param threeDTexCoords True if 3D texture coordinates should be created.
     *
     * \return The OSG node containing the 12 edges of the box.
     */
    osg::ref_ptr< osg::Node > generateSolidBoundingBoxNode( const wmath::WPosition& pos1, const wmath::WPosition& pos2, const WColor& color,
                                                            bool threeDTexCoords = true );

    /**
     * Creates a osg::Geometry containing an unit cube, having 3D texture coordinates.
     *
     * \param color the color to set for all vertices
     *
     * \return the geometry
     */
    osg::ref_ptr< osg::Geometry > createUnitCube( const WColor& color );

    /**
     * Extract the vertices and triangles from a WTriangleMesh and save them
     * into an osg::Geometry.
     *
     * \param mesh the WTriangleMesh used as input
     * \param includeNormals When true, calculate the vertex normals and include
     *                       them into the geometry.
     * \return an osg::Geometry containing the mesh
     */
    osg::ref_ptr< osg::Geometry > convertToOsgGeometry( WTriangleMesh* mesh, bool includeNormals = false );

    /**
     * Generates a line geode with thickness and color as parameters.
     *
     * \param line sequence of points
     * \param thickness How thick the line strip should be
     * \param color If present this color is used for the whole line, otherwise local coloring is used
     *
     * \return The new assembled geode for this line
     */
    osg::ref_ptr< osg::Geode > generateLineStripGeode( const wmath::WLine& line,
                                                       const float thickness = 3.0f,
                                                       const WColor& color = WColor( 0, 0, 0, 0 ) );

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

} // end of namespace wge

#endif  // WGEGEODEUTILS_H
