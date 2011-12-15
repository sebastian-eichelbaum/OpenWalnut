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

#include "../WGETextureUtils.h"
#include "../shaders/WGEPropertyUniform.h"
#include "../shaders/WGEShaderPropertyDefine.h"

#include "WGEPostprocessorSSAO.h"

WGEPostprocessorSSAO::WGEPostprocessorSSAO():
    WGEPostprocessor( "SSAO",
                      "SSAO is a special ambient occlusion technique." )
{
}

WGEPostprocessorSSAO::WGEPostprocessorSSAO( osg::ref_ptr< WGEOffscreenRenderNode > offscreen,
                                                          const WGEPostprocessor::PostprocessorInput& gbuffer ):
    WGEPostprocessor( "SSAO",
                      "SSAO is a special ambient occlusion technique." )
{
    // the SSAO algorithm has some parameters. Provide these parameters to the user
    WPropInt ssaoSamples = m_properties->addProperty( "Samples", "The number of samples to take in screen-space. Higher values produce better "
                                                                 "quality but can reduce FPS dramatically.", 16 );
    ssaoSamples->setMin( 1 );
    ssaoSamples->setMax( 128 );

    WPropDouble ssaoRadius = m_properties->addProperty( "Radius", "The radius around the pixel to sample for occluders in pixels.", 10.0 );
    ssaoRadius->setMin( 0.0 );
    ssaoRadius->setMax( 100.0 );

    WPropDouble ssaoTotalStrength = m_properties->addProperty( "Total Strength", "The strength of the effect. Higher values emphasize the effect.",
                                                                                 2.0 );
    ssaoTotalStrength->setMin( 0.0 );
    ssaoTotalStrength->setMax( 100.0 );

    WPropDouble ssaoStrength = m_properties->addProperty( "Strength", "This defines the influence of one occluder to the overall AO effect.", 1.0 );
    ssaoStrength->setMin( 0.0 );
    ssaoStrength->setMax( 1.0 );

    WPropDouble ssaoFalloff = m_properties->addProperty( "Falloff", "Define the edge at which a depth difference between two pixels is assumed to "
                                                                    "be non-zero.", 0.0 );
    ssaoFalloff->setMin( 0.0 );
    ssaoFalloff->setMax( 1.0 );

    // Use the standard postprocessor uber-shader
    WGEShader::RefPtr s = new WGEShader( "WGEPostprocessor" );
    s->setDefine( "WGE_POSTPROCESSOR_SSAO" );

    // also add the m_effectOnly property as shader preprocessor
    s->addPreprocessor( m_effectOnlyPreprocessor );
    s->addPreprocessor( WGEShaderPreprocessor::SPtr(
        new WGEShaderPropertyDefine< WPropInt >( "WGE_POSTPROCESSOR_SSAO_SAMPLES", ssaoSamples ) )
    );

    // create the rendering pass
    osg::ref_ptr< WGEOffscreenTexturePass > pass = offscreen->addTextureProcessingPass( s, "SSAO" );
    pass->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropDouble >( "u_ssaoTotalStrength", ssaoTotalStrength ) );
    pass->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropDouble >( "u_ssaoStrength", ssaoStrength ) );
    pass->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropDouble >( "u_ssaoRadius", ssaoRadius ) );
    pass->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropDouble >( "u_ssaoFalloff", ssaoFalloff ) );

    // attach color0 output
    m_resultTextures.push_back( pass->attach( osg::Camera::COLOR_BUFFER0, GL_RGB ) );

    // provide the Gbuffer input
    size_t gBufUnitOffset = gbuffer.bind( pass );

    // this effect needs some additional noise texture:
    // some of the post-processors need some white noise, like the ssao
    const size_t size = 64;
    osg::ref_ptr< WGETexture2D > randTex = wge::genWhiteNoiseTexture( size, size, 3 );
    pass->bind( randTex, gBufUnitOffset );
}

WGEPostprocessorSSAO::~WGEPostprocessorSSAO()
{
    // cleanup
}

WGEPostprocessor::SPtr WGEPostprocessorSSAO::create( osg::ref_ptr< WGEOffscreenRenderNode > offscreen,
    const WGEPostprocessor::PostprocessorInput& gbuffer ) const
{
    return WGEPostprocessor::SPtr( new WGEPostprocessorSSAO( offscreen, gbuffer ) );
}
