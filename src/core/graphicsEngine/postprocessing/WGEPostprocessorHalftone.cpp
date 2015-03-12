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
#include "WGEPostprocessorEdgeEnhance.h"
#include "WGEPostprocessorMergeOp.h"

#include "WGEPostprocessorHalftone.h"

WGEPostprocessorHalftone::WGEPostprocessorHalftone():
    WGEPostprocessor( "Halftone",
                      "Halftone is a dithering effect to emphasize certain structures of a surface." )
{
}

WGEPostprocessorHalftone::WGEPostprocessorHalftone( osg::ref_ptr< WGEOffscreenRenderNode > offscreen,
                                                          const WGEPostprocessor::PostprocessorInput& gbuffer ):
    WGEPostprocessor( "Halftone",
                      "Halftone is a dithering effect to emphasize certain structures of a surface." )
{
    // the Halftone algorithm has some parameters. Provide these parameters to the user
    WPropDouble ditherRotationMatrixAngle = m_properties->addProperty( "Rotation angle",
                                            "Rotate the dither effect by this angle.", 30.0 );
    ditherRotationMatrixAngle->setMin( 0 );
    ditherRotationMatrixAngle->setMax( 180 );

    WPropDouble ditherTexSize = m_properties->addProperty( "Texture Size",
                                            "Write me.", 4.0 );
    ditherTexSize->setMin( 0 );
    ditherTexSize->setMax( 10 );

    WPropDouble ditherCrossHatchThreshold = m_properties->addProperty( "Cross Hatch Threshold",
                                                            "Write me.", 0.9 );
    ditherCrossHatchThreshold->setMin( 0 );
    ditherCrossHatchThreshold->setMax( 10 );

    WPropDouble ditherClip =  m_properties->addProperty( "Dither Clip",
                                                            "Write me.", 0.6 );
    ditherClip->setMin( 0.0 );
    ditherClip->setMax( 1.0 );

    WPropColor hatchColor = m_properties->addProperty( "Hatch Color", "Write", WColor( 0.1, 0.1, 0.1, 1.0 ) );

    //////////////////////////////////////////////////////////////////
    // Use EdgeEnhance Processor

    // We need edges. Let the edge postprocessor do the work for us
    WGEPostprocessorEdgeEnhance edgeProto;
    WGEPostprocessor::SPtr edgeProcessor = edgeProto.create( offscreen, gbuffer );
    // we want mipmaping for the edge texture
    edgeProcessor->getOutput()->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR );
    edgeProcessor->getProperties()->getProperty( "Effect Only" )->toPropBool()->set( true );
    // Allow user to config our sub-processor
    m_properties->addProperty( edgeProcessor->getProperties() );

    //////////////////////////////////////////////////////////////////
    // Build our Halftone Processor

    // use the standard postprocessor uber-shader
    WGEShader::RefPtr s = new WGEShader( "WGEPostprocessor" );
    s->setDefine( "WGE_POSTPROCESSOR_HALFTONE" );

    // also add the m_effectOnly property as shader preprocessor
    s->addPreprocessor( m_effectOnlyPreprocessor );

    // create the Halftone rendering pass
    osg::ref_ptr< WGEOffscreenTexturePass > htPass = offscreen->addTextureProcessingPass( s, "Halftone" );
    htPass->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropDouble >( "u_ditherRotMatrixAngle", ditherRotationMatrixAngle ) );
    htPass->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropDouble >( "u_ditherTexSize", ditherTexSize ) );
    htPass->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropDouble >( "u_ditherCrossHatchThresh", ditherCrossHatchThreshold ) );
    htPass->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropDouble >( "u_ditherClip", ditherClip ) );
    htPass->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropColor >( "u_hatchColor", hatchColor ) );

    //////////////////////////////////////////////////////////////////
    // Build a Gauss Filter for  shader


    // attach color0 output
    m_resultTextures.push_back( htPass->attach( osg::Camera::COLOR_BUFFER0, GL_RGBA ) );

    // provide the Gbuffer input, with several mipmap levels
    gbuffer.m_depthTexture->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR );
    gbuffer.m_normalTexture->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR );
    gbuffer.m_tangentTexture->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR );

    size_t gBufUnitOffset = gbuffer.bind( htPass );

    // this effect needs some additional noise texture:
    const size_t size = 64;
    osg::ref_ptr< WGETexture2D > randTex = wge::genWhiteNoiseTexture( size, size, 3 );
    htPass->bind( randTex, gBufUnitOffset );
    htPass->bind( edgeProcessor->getOutput(), gBufUnitOffset + 1 );

    // also utilize the pre-blend scale
    m_effectScale->setHidden( false );
    htPass->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropDouble >( "u_effectPreBlendScale", m_effectScale ) );
}

WGEPostprocessorHalftone::~WGEPostprocessorHalftone()
{
    // cleanup
}

WGEPostprocessor::SPtr WGEPostprocessorHalftone::create( osg::ref_ptr< WGEOffscreenRenderNode > offscreen,
    const WGEPostprocessor::PostprocessorInput& gbuffer ) const
{
    return WGEPostprocessor::SPtr( new WGEPostprocessorHalftone( offscreen, gbuffer ) );
}
