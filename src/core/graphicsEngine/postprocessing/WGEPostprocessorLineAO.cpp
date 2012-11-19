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
#include "WGEPostprocessorGauss.h"
#include "WGEPostprocessorMergeOp.h"

#include "WGEPostprocessorLineAO.h"

WGEPostprocessorLineAO::WGEPostprocessorLineAO():
    WGEPostprocessor( "LineAO",
                      "LineAO is a special ambient occlusion technique optimized for dense line and tube rendering." )
{
}

WGEPostprocessorLineAO::WGEPostprocessorLineAO( osg::ref_ptr< WGEOffscreenRenderNode > offscreen,
                                                          const WGEPostprocessor::PostprocessorInput& gbuffer ):
    WGEPostprocessor( "LineAO",
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

    WPropDouble lineaoRadiusSS = m_properties->addProperty( "Radius", "The radius around the pixel to sample for occluders in pixels.", 2.0 );
    lineaoRadiusSS->setMin( 0.01 );
    lineaoRadiusSS->setMax( 10.0 );

    WPropDouble lineaoTotalStrength = m_properties->addProperty( "Total Strength", "The strength of the effect. Higher values emphasize the effect.",
                                                                                 1.0 );
    lineaoTotalStrength->setMin( 0.0 );
    lineaoTotalStrength->setMax( 5.0 );

    WPropDouble lineaoDensityWeight = m_properties->addProperty( "Density Weight", "The strength of the occluder influence in relation to the "
                                                                                   "geometry density. The higher the value, the larger the "
                                                                                   "influence. Low values remove the drop-shadow effect. "
                                                                                   "This defines the influence of one occluder to the overall "
                                                                                   "AO effect.", 1.0 );
    lineaoDensityWeight->setMin( 0.001 );
    lineaoDensityWeight->setMax( 2.0 );

    WPropBool lineaoUseGaussedNDMap = m_properties->addProperty( "Use Gaussed ND-Map", "When enabling, the LineAO algorithm uses a scale-base "
                                                                                       "pyramid for sampling the depth and normal maps. Unlike "
                                                                                       "described in the paper, this is turned off by default to "
                                                                                       "ensure that details of far occluders are retained (the "
                                                                                       "images look crispier).", false );

    // NOTE: The paper proposes to use a gaussian pyramid of the depth and normal maps. We skip this step. Skipping this causes the AO to look
    // more crispy and more detailed at local scope.

    // use the standard postprocessor uber-shader
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

    s->addPreprocessor( WGEShaderPreprocessor::SPtr(
        new WGEShaderPropertyDefineOptions< WPropBool >( lineaoUseGaussedNDMap, "WGE_POSTPROCESSOR_LINEAO_USEDIRECTNDMAP",
                                                                                "WGE_POSTPROCESSOR_LINEAO_USEGAUSSPYRAMID" ) )
    );

    // create the LineAO rendering pass
    osg::ref_ptr< WGEOffscreenTexturePass > lineAOPass = offscreen->addTextureProcessingPass( s, "LineAO" );
    lineAOPass->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropDouble >( "u_lineaoDensityWeight", lineaoDensityWeight ) );
    lineAOPass->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropDouble >( "u_lineaoTotalStrength", lineaoTotalStrength ) );
    lineAOPass->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropDouble >( "u_lineaoRadiusSS", lineaoRadiusSS ) );

    // attach color0 output
    m_resultTextures.push_back( lineAOPass->attach( osg::Camera::COLOR_BUFFER0, GL_RGB ) );

    // provide the Gbuffer input, with several mipmap levels
    gbuffer.m_depthTexture->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR );
    gbuffer.m_normalTexture->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR );
    gbuffer.m_tangentTexture->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR );

    size_t gBufUnitOffset = gbuffer.bind( lineAOPass );

    // this effect needs some additional noise texture:
    const size_t size = 64;
    osg::ref_ptr< WGETexture2D > randTex = wge::genWhiteNoiseTexture( size, size, 3 );
    lineAOPass->bind( randTex, gBufUnitOffset );
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
