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

#include "../shaders/WGEPropertyUniform.h"

#include "WGEPostprocessorCelShading.h"

WGEPostprocessorCelShading::WGEPostprocessorCelShading():
    WGEPostprocessor( "Cel Shading",
                      "This postprocessor reduces color by binning colors." )
{
}

WGEPostprocessorCelShading::WGEPostprocessorCelShading( osg::ref_ptr< WGEOffscreenRenderNode > offscreen,
                                                        const WGEPostprocessor::PostprocessorInput& gbuffer ):
    WGEPostprocessor( offscreen, gbuffer, "Cel Shading",
                      "This postprocessor reduces color by binning colors." )
{
    // we also provide a property
    WPropInt bins = m_properties->addProperty( "Number of Bins", "The number of color bins used for cel shading.", 10 );
    bins->setMin( 1 );
    bins->setMax( 100 );

    // hide m_effectOnly since this is not useful for Cel Shading
    m_effectOnly->setHidden( true );

    // construct pipeline

    // Use the standard postprocessor uber-shader
    WGEShader::RefPtr s = new WGEShader( "WGEPostprocessor" );
    s->setDefine( "WGE_POSTPROCESSOR_CEL" );

    // also add the m_effectOnly property as shader preprocessor
    s->addPreprocessor( m_effectOnlyPreprocessor );

    // create the rendering pass
    osg::ref_ptr< WGEOffscreenTexturePass > pass = offscreen->addTextureProcessingPass( s, "Cel Shading" );
    pass->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropInt >( "u_celShadingBins", bins ) );

    // attach color0 output
    m_resultTexture = pass->attach( osg::Camera::COLOR_BUFFER0, GL_RGBA );

    // provide the Gbuffer input
    gbuffer.bind( pass );
}

WGEPostprocessorCelShading::~WGEPostprocessorCelShading()
{
    // cleanup
}

WGEPostprocessor::SPtr WGEPostprocessorCelShading::create( osg::ref_ptr< WGEOffscreenRenderNode > offscreen,
    const WGEPostprocessor::PostprocessorInput& gbuffer ) const
{
    return WGEPostprocessor::SPtr( new WGEPostprocessorCelShading( offscreen, gbuffer ) );
}
