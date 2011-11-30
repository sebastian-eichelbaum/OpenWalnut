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

#include "../common/math/WLinearAlgebraFunctions.h"
#include "../graphicsEngine/WGEViewer.h"
#include "../graphicsEngine/WGEZoomTrackballManipulator.h"
#include "../graphicsEngine/WGraphicsEngine.h"
#include "../graphicsEngine/WPickHandler.h"
#include "WKernel.h"

#include "WSelectionManager.h"


WSelectionManager::WSelectionManager() :
    m_paintMode( PAINTMODE_NONE ),
    m_textureOpacity( 1.0 ),
    m_useTexture( false )
{
    m_crosshair = boost::shared_ptr< WCrosshair >( new WCrosshair() );

    m_sliceGroup = boost::shared_ptr< WProperties >( new WProperties( "Slice Properties",
                    "Properties relating to the Axial,Coronal and Sagittal Slices." ) );

    // create dummy properties for slices. Get updated by modules.
    m_axialPos = m_sliceGroup->addProperty( "Axial Position", "Slice X position.", 0.0, true );
    m_coronalPos = m_sliceGroup->addProperty( "Coronal Position", "Slice Y position.", 0.0, true );
    m_sagittalPos = m_sliceGroup->addProperty( "Sagittal Position", "Slice Z position.", 0.0, true );

    // visibility flags
    m_axialShow = m_sliceGroup->addProperty( "Show Axial Slice", "Slice visible?", true, true );
    m_coronalShow = m_sliceGroup->addProperty( "Show Coronal Slice", "Slice visible?", true, true );
    m_sagittalShow = m_sliceGroup->addProperty( "Show Sagittal Slice", "Slice visible?", true, true );

    // until now, no bbox information is available.
    m_axialPos->setMin( 0.0 );
    m_coronalPos->setMin( 0.0 );
    m_sagittalPos->setMin( 0.0 );
    m_axialPos->setMax( 0.0 );
    m_coronalPos->setMax( 0.0 );
    m_sagittalPos->setMax( 0.0 );

    m_axialUpdateConnection = m_axialPos->getUpdateCondition()->subscribeSignal(
        boost::bind( &WSelectionManager::updateCrosshairPosition, this )
    );
    m_coronalUpdateConnection = m_coronalPos->getUpdateCondition()->subscribeSignal(
        boost::bind( &WSelectionManager::updateCrosshairPosition, this )
    );
    m_sagittalUpdateConnection = m_sagittalPos->getUpdateCondition()->subscribeSignal(
        boost::bind( &WSelectionManager::updateCrosshairPosition, this )
    );
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
    dots[0] = dot( v2, view );

    v2[2] = -1;
    dots[1] = dot( v2, view );

    v2[1] = -1;
    dots[2] = dot( v2, view );

    v2[2] = 1;
    dots[3] = dot( v2, view );

    v2[0] = -1;
    dots[4] = dot( v2, view );

    v2[2] = -1;
    dots[5] = dot( v2, view );

    v2[1] = 1;
    dots[6] = dot( v2, view );

    v2[2] = 1;
    dots[7] = dot( v2, view );

    float max = 0.0;
    int quadrant = 0;
    for( int i = 0; i < 8; ++i )
    {
        if( dots[i] > max )
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
    if( value < 0.0 )
    {
        value = 0.0;
    }
    if( value > 1.0 )
    {
        value = 1.0;
    }
    m_textureOpacity = value;
}

WPropDouble WSelectionManager::getPropAxialPos()
{
    return m_axialPos;
}

WPropDouble WSelectionManager::getPropCoronalPos()
{
    return m_coronalPos;
}

WPropDouble WSelectionManager::getPropSagittalPos()
{
    return m_sagittalPos;
}

WPropBool WSelectionManager::getPropAxialShow()
{
    return m_axialShow;
}

WPropBool WSelectionManager::getPropCoronalShow()
{
    return m_coronalShow;
}

WPropBool WSelectionManager::getPropSagittalShow()
{
    return m_sagittalShow;
}

void WSelectionManager::setShader( int shader )
{
    m_shader = shader;
}

int WSelectionManager::getShader()
{
    return m_shader;
}

void WSelectionManager::updateCrosshairPosition()
{
    m_crosshair->setPosition( WPosition( m_sagittalPos->get(), m_coronalPos->get(), m_axialPos->get() ) );
}

