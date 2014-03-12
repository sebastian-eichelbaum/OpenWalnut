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

#include "../common/WProperties.h"

#include "shaders/WGEShader.h"
#include "shaders/WGEPropertyUniform.h"

#include "WGEViewerEffectHorizon.h"

WGEViewerEffectHorizon::WGEViewerEffectHorizon():
    WGEViewerEffect( "Horizon Background", "Configurable horizon background effect." )
{
    WPropColor bottom1Color = m_properties->addProperty( "First Bottom Color", "Bottom color gradient: first color.",
                                                         WColor( 1.00, 1.00, 1.00, 1.00 ) );
    WPropColor bottom2Color = m_properties->addProperty( "Second Bottom Color", "Bottom color gradient: second color.",
                                                         WColor( 0.82, 0.82, 0.82, 1.00 ) );
    WPropColor top1Color = m_properties->addProperty( "First Top Color", "Top color gradient: first color.",
                                                      WColor( 0.85, 0.85, 0.85, 1.0 ) );
    WPropColor top2Color = m_properties->addProperty( "Second Top Color", "Top color gradient: second color.",
                                                      WColor( 0.95, 0.95, 0.95, 0.95 ) );

    WPropDouble horizonLine = m_properties->addProperty( "Horizon in %", "Horizon line in percent.", 33.0 );
    horizonLine->setMin( 0.0 );
    horizonLine->setMax( 100.0 );

    m_state->addUniform( new WGEPropertyUniform< WPropDouble >( "u_horizon", horizonLine ) );
    m_state->addUniform( new WGEPropertyUniform< WPropColor >( "u_bottom1Color", bottom1Color ) );
    m_state->addUniform( new WGEPropertyUniform< WPropColor >( "u_bottom2Color", bottom2Color ) );
    m_state->addUniform( new WGEPropertyUniform< WPropColor >( "u_top1Color", top1Color ) );
    m_state->addUniform( new WGEPropertyUniform< WPropColor >( "u_top2Color", top2Color ) );

    // default cam is a post render cam. The Horizon needs to be done BEFORE everything else
    setRenderOrder( WGECamera::NESTED_RENDER, 0 );

    osg::ref_ptr< WGEShader > horizonShader = new WGEShader( "WGECameraHorizon" );
    horizonShader->apply( m_geode );
}

WGEViewerEffectHorizon::~WGEViewerEffectHorizon()
{
    // cleanup
}

