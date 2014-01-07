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

#include "WGEViewerEffectVignette.h"

WGEViewerEffectVignette::WGEViewerEffectVignette():
    WGEViewerEffect( "Vignette", "Provide a basic vignette effect." )
{
    // WPropDouble vignetteIntensity = m_properties->addProperty( "", "", 0.5 );
    WPropDouble vignetteIntensity = m_properties->addProperty( "Strength", "Define dominance of vignette effect.", 0.33 );
    vignetteIntensity->setMin( 0.0 );
    vignetteIntensity->setMax( 2.0 );
    m_state->addUniform( new WGEPropertyUniform< WPropDouble >( "u_vignetteIntensity", vignetteIntensity ) );

    // defaults are ok. Just set shader
    osg::ref_ptr< WGEShader > vignetteShader = new WGEShader( "WGECameraVignette" );
    vignetteShader->apply( m_geode );
}

WGEViewerEffectVignette::~WGEViewerEffectVignette()
{
    // cleanup
}

