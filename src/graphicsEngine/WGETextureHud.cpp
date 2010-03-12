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

#include <osg/Camera>
#include <osg/Geode>
#include <osg/Geometry>

#include "WGETextureHud.h"

WGETextureHud::WGETextureHud():
    osg::Projection()
{
    // Initialize the projection matrix for viewing everything we
    // will add as descendants of this node. Use screen coordinates
    // to define the horizontal and vertical extent of the projection
    // matrix. Positions described under this node will equate to
    // pixel coordinates.
    setMatrix( osg::Matrix::ortho2D( 0, 1024, 0, 768 ) );
    getOrCreateStateSet()->setRenderBinDetails( 1000, "RenderBin" );
}

WGETextureHud::~WGETextureHud()
{
    // cleanup
}

void WGETextureHud::addTexture( osg::ref_ptr< WGETextureHudEntry > texture )
{

}

void WGETextureHud::removeTexture( osg::ref_ptr< WGETextureHudEntry > texture )
{

}

WGETextureHud::WGETextureHudEntry::WGETextureHudEntry( osg::ref_ptr< osg::Texture2D > texture ):
    osg::Geode()
{
    // Set up geometry for the HUD and add it to the HUD
    osg::ref_ptr< osg::Geometry > HUDBackgroundGeometry = new osg::Geometry();

    osg::ref_ptr< osg::Vec3Array > HUDBackgroundVertices = new osg::Vec3Array;
    HUDBackgroundVertices->push_back( osg::Vec3( 0, 0, -1 ) );
    HUDBackgroundVertices->push_back( osg::Vec3( 1, 0, -1 ) );
    HUDBackgroundVertices->push_back( osg::Vec3( 1, 1, -1 ) );
    HUDBackgroundVertices->push_back( osg::Vec3( 0, 1, -1 ) );

    osg::ref_ptr< osg::Vec3Array > HUDBackgroundTex = new osg::Vec3Array;
    HUDBackgroundTex->push_back( osg::Vec3( 0, 0, 0 ) );
    HUDBackgroundTex->push_back( osg::Vec3( 1, 0, 0 ) );
    HUDBackgroundTex->push_back( osg::Vec3( 1, 1, 0 ) );
    HUDBackgroundTex->push_back( osg::Vec3( 0, 1, 0 ) );

    osg::ref_ptr< osg::DrawElementsUInt > HUDBackgroundIndices = new osg::DrawElementsUInt( osg::PrimitiveSet::POLYGON, 0 );
    HUDBackgroundIndices->push_back( 0 );
    HUDBackgroundIndices->push_back( 1 );
    HUDBackgroundIndices->push_back( 2 );
    HUDBackgroundIndices->push_back( 3 );

    osg::ref_ptr< osg::Vec4Array > HUDcolors = new osg::Vec4Array;
    HUDcolors->push_back( osg::Vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );

    osg::ref_ptr< osg::Vec3Array > HUDnormals = new osg::Vec3Array;
    HUDnormals->push_back( osg::Vec3( 0.0f, 0.0f, 1.0f ) );
    HUDBackgroundGeometry->setNormalArray( HUDnormals );
    HUDBackgroundGeometry->setNormalBinding( osg::Geometry::BIND_OVERALL );
    HUDBackgroundGeometry->addPrimitiveSet( HUDBackgroundIndices );
    HUDBackgroundGeometry->setVertexArray( HUDBackgroundVertices );
    HUDBackgroundGeometry->setColorArray( HUDcolors );
    HUDBackgroundGeometry->setTexCoordArray( 0, HUDBackgroundTex );
    HUDBackgroundGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    addDrawable( HUDBackgroundGeometry );

    // Create and set up a state set using the texture from above
    getOrCreateStateSet()->setTextureAttributeAndModes( 0, texture, osg::StateAttribute::ON );
}

WGETextureHud::WGETextureHudEntry::~WGETextureHudEntry()
{
    // cleanup
}

