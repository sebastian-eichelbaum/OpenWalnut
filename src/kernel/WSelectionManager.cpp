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

#include <vector>

#include <osg/Matrix>

#include "WKernel.h"
#include "../common/math/WLinearAlgebraFunctions.h"

#include "../graphicsEngine/WGEZoomTrackballManipulator.h"

#include "WSelectionManager.h"

using wmath::WVector3D;
using wmath::WPosition;
using wmath::WMatrix;


WSelectionManager::WSelectionManager() :
    m_paintMode( PAINTMODE_NONE ),
    m_textureOpacity( 1.0 ),
    m_useTexture( false )
{
    m_crosshair = boost::shared_ptr< WCrosshair >( new WCrosshair() );
}

WSelectionManager::~WSelectionManager()
{
}

boost::shared_ptr< WCrosshair >WSelectionManager::getCrosshair()
{
    return m_crosshair;
}

int WSelectionManager::getFrontSector()
{
    boost::shared_ptr< WGEViewer > viewer;
    viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "main" );
    viewer->getCamera()->getViewMatrix();
    osg::Matrix rm = viewer->getCamera()->getViewMatrix();

    WMatrix< double > rotMat( 4, 4 );
    for( size_t i = 0; i < 4; ++i )
    {
        for( size_t j = 0; j < 4; ++j )
        {
            rotMat( i, j ) = rm( i, j );
        }
    }
    WPosition v1( 0, 0, 1 );
    WPosition view;
    view = transformPosition3DWithMatrix4D( rotMat, v1 );

    std::vector<float> dots( 8 );
    WPosition v2( 1, 1, 1 );
    dots[0] = v2.dotProduct( view );

    v2[2] = -1;
    dots[1] = v2.dotProduct( view );

    v2[1] = -1;
    dots[2] = v2.dotProduct( view );

    v2[2] = 1;
    dots[3] = v2.dotProduct( view );

    v2[0] = -1;
    dots[4] = v2.dotProduct( view );

    v2[2] = -1;
    dots[5] = v2.dotProduct( view );

    v2[1] = 1;
    dots[6] = v2.dotProduct( view );

    v2[2] = 1;
    dots[7] = v2.dotProduct( view );

    float max = 0.0;
    int quadrant = 0;
    for ( int i = 0; i < 8; ++i )
    {
        if ( dots[i] > max )
        {
            max = dots[i];
            quadrant = i;
        }
    }
    return quadrant;
}

void WSelectionManager::setPaintMode( WPaintMode mode )
{
    m_paintMode = mode;

    osg::static_pointer_cast<WGEZoomTrackballManipulator>(
            WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getCameraManipulator() )->setPaintMode( mode );
    WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getPickHandler()->setPaintMode( mode );
}

WPaintMode WSelectionManager::getPaintMode()
{
    return m_paintMode;
}

void WSelectionManager::setTexture( osg::ref_ptr< osg::Texture3D > texture, boost::shared_ptr< WGridRegular3D >grid )
{
    m_texture = texture;
    m_textureGrid = grid;
}


osg::ref_ptr< osg::Texture3D > WSelectionManager::getTexture()
{
    return m_texture;
}

boost::shared_ptr< WGridRegular3D >WSelectionManager::getGrid()
{
    return m_textureGrid;
}

void WSelectionManager::setUseTexture( bool flag )
{
    m_useTexture = flag;
}

bool WSelectionManager::getUseTexture()
{
    return m_useTexture;
}


float WSelectionManager::getTextureOpacity()
{
    return m_textureOpacity;
}

void WSelectionManager::setTextureOpacity( float value )
{
    if ( value < 0.0 )
    {
        value = 0.0;
    }
    if ( value > 1.0 )
    {
        value = 1.0;
    }
    m_textureOpacity = value;
}

void WSelectionManager::setPropAxialPos( WPropInt prop )
{
    m_axialPos = prop;
}

void WSelectionManager::setPropCoronalPos( WPropInt prop )
{
    m_coronalPos = prop;
}

void WSelectionManager::setPropSagittalPos( WPropInt prop )
{
    m_sagittalPos = prop;
}

WPropInt WSelectionManager::getPropAxialPos()
{
    return m_axialPos;
}

WPropInt WSelectionManager::getPropCoronalPos()
{
    return m_coronalPos;
}

WPropInt WSelectionManager::getPropSagittalPos()
{
    return m_sagittalPos;
}

void WSelectionManager::setShader( int shader )
{
    m_shader = shader;
}

int WSelectionManager::getShader()
{
    return m_shader;
}

boost::shared_ptr< std::vector< bool > > WSelectionManager::getBitField()
{
    return m_fiberSelectionBitfield;
}

void WSelectionManager::setBitField( boost::shared_ptr< std::vector< bool > > bf )
{
    m_fiberSelectionBitfield = bf;
}
