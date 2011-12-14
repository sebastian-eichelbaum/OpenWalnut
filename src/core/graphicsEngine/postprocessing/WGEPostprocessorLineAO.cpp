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
#include "../shaders/WGEShaderPropertyDefine.h"

#include "WGEPostprocessorLineAO.h"

WGEPostprocessorLineAO::WGEPostprocessorLineAO():
    WGEPostprocessor( "LineAO",
                      "LineAO is a special ambient occlusion technique optimized for dense line and tube rendering." )
{
}

WGEPostprocessorLineAO::WGEPostprocessorLineAO( osg::ref_ptr< WGEOffscreenRenderNode > offscreen,
                                                          const WGEPostprocessor::PostprocessorInput& gbuffer ):
    WGEPostprocessor( offscreen, gbuffer,
                      "LineAO",
                      "LineAO is a special ambient occlusion technique optimized for dense line and tube rendering." )
{
    // the LineAO algorithm has some parameters. Provide these parameters to the user
    WPropInt lineaoSamples = m_properties->addProperty( "Samples", "The number of samples to take in screen-space. Higher values produce better "
                                                                 "quality but can reduce FPS dramatically.", 32 );
    lineaoSamples->setMin( 1 );
    lineaoSamples->setMax( 128 );

    WPropInt lineaoScalers = m_properties->addProperty( "Hemispheres", "The number of hemispheres to sample around each pixel. Higher values "
                                                                       "produce better quality but can reduce FPS dramatically.", 3 );
    lineaoScalers->setMin( 1 );
    lineaoScalers->setMax( 8 );

    WPropDouble lineaoRadiusSS = m_properties->addProperty( "Radius", "The radius around the pixel to sample for occluders in pixels.", 2.5 );
    lineaoRadiusSS->setMin( 0.0 );
    lineaoRadiusSS->setMax( 100.0 );

    WPropDouble lineaoTotalStrength = m_properties->addProperty( "Total Strength", "The strength of the effect. Higher values emphasize the effect.",
                                                                                 2.5 );
    lineaoTotalStrength->setMin( 0.0 );
    lineaoTotalStrength->setMax( 10.0 );

    WPropDouble lineaoDensityWeight = m_properties->addProperty( "Density Weight", "The strength of the occluder influence in relation to the "
                                                                                   "geometry density. The higher the value, the larger the "
                                                                                   "influence. Low values remove the drop-shadow effect. "
                                                                                   "This defines the influence of one occluder to the overall "
                                                                                   "AO effect.", 1.0 );
    lineaoDensityWeight->setMin( 0.0 );
    lineaoDensityWeight->setMax( 2.0 );


    // Use the standard postprocessor uber-shader
    WGEShader::RefPtr s = new WGEShader( "WGEPostprocessor" );
    s->setDefine( "WGE_POSTPROCESSOR_LINEAO" );

    // also add the m_effectOnly property as shader preprocessor
    s->addPreprocessor( m_effectOnlyPreprocessor );
    s->addPreprocessor( WGEShaderPreprocessor::SPtr(
        new WGEShaderPropertyDefine< WPropInt >( "WGE_POSTPROCESSOR_LINEAO_SCALERS", lineaoScalers ) )
    );
    s->addPreprocessor( WGEShaderPreprocessor::SPtr(
        new WGEShaderPropertyDefine< WPropInt >( "WGE_POSTPROCESSOR_LINEAO_SAMPLES", lineaoSamples ) )
    );

    // create the rendering pass
    osg::ref_ptr< WGEOffscreenTexturePass > pass = offscreen->addTextureProcessingPass( s, "LineAO" );
    pass->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropDouble >( "u_lineaoDensityWeight", lineaoDensityWeight ) );
    pass->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropDouble >( "u_lineaoTotalStrength", lineaoTotalStrength ) );
    pass->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropDouble >( "u_lineaoRadiusSS", lineaoRadiusSS ) );

    // attach color0 output
    m_resultTexture = pass->attach( osg::Camera::COLOR_BUFFER0, GL_RGB );

    // provide the Gbuffer input
    size_t gBufUnitOffset = gbuffer.bind( pass );

    // this effect needs some additional noise texture:
    const size_t size = 64;
    osg::ref_ptr< WGETexture2D > randTex = wge::genWhiteNoiseTexture( size, size, 3 );
    pass->bind( randTex, gBufUnitOffset );
}

WGEPostprocessorLineAO::~WGEPostprocessorLineAO()
{
    // cleanup
}

WGEPostprocessor::SPtr WGEPostprocessorLineAO::create( osg::ref_ptr< WGEOffscreenRenderNode > offscreen,
    const WGEPostprocessor::PostprocessorInput& gbuffer ) const
{
    return WGEPostprocessor::SPtr( new WGEPostprocessorLineAO( offscreen, gbuffer ) );
}
