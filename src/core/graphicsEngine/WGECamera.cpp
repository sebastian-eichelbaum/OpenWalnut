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

#include <iostream>
#include <string>

#include "exceptions/WGEInitFailed.h"
#include "WGECamera.h"

WGECamera::WGECamera( int width, int height, ProjectionMode projectionMode )
    : osg::Camera(),
      m_DefProjMode( projectionMode )
{
    setViewport( 0, 0, width, height );
    setClearColor( osg::Vec4( 0.9, 0.9, 0.9, 1.0 ) );

    // disable all culling
    setCullingActive( false );
    setCullingMode( osg::CullSettings::NO_CULLING );

    // near-far computation is done using the bounding volumes
    setComputeNearFarMode(
        osg::CullSettings::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES
        // osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR
        // osg::CullSettings::COMPUTE_NEAR_FAR_USING_PRIMITIVES
    );
    reset();
}

WGECamera::~WGECamera()
{
    // cleanup
}

void WGECamera::setDefaultProjectionMode( WGECamera::ProjectionMode mode )
{
    m_DefProjMode = mode;
}

WGECamera::ProjectionMode WGECamera::getDefaultProjectionMode()
{
    return m_DefProjMode;
}

void WGECamera::reset()
{
    switch( m_DefProjMode )
    {
        case ORTHOGRAPHIC:
            setProjectionMatrixAsOrtho( -120.0 * getViewport()->aspectRatio(), 120.0 * getViewport()->aspectRatio(),
                -120.0, 120.0, -1000.0, +1000.0 );
            setProjectionResizePolicy( HORIZONTAL );
            break;
        case PERSPECTIVE:
            setProjectionMatrixAsPerspective( 30.0, getViewport()->aspectRatio(), 1.0, 1000.0 );
            setProjectionResizePolicy( osg::Camera::HORIZONTAL );
            break;
        case TWO_D:
            resize();
            setProjectionResizePolicy( osg::Camera::FIXED );
            break;
        case TWO_D_UNIT:
            resize();
            setProjectionResizePolicy( osg::Camera::FIXED );
            break;
        default:
            throw WGEInitFailed( std::string( "Unknown projection mode." ) );
    }
}

void WGECamera::resize()
{
    if( m_DefProjMode == TWO_D )
    {
        setProjectionMatrixAsOrtho2D( 0.0, getViewport()->width(), 0.0, getViewport()->height() );
    }
    else if( m_DefProjMode == TWO_D_UNIT )
    {
        double aspectWH = static_cast< double >( getViewport()->width() ) / static_cast< double >( getViewport()->height() );
        double aspectHW = 1.0 / aspectWH;

        double w = aspectWH > aspectHW ? aspectWH : 1.0;
        double h = aspectWH > aspectHW ? 1.0 : aspectHW;

        w *= 0.5;
        h *= 0.5;
        setProjectionMatrixAsOrtho( -w, w, -h, h, 0.0, 1.0 );
    }
}
