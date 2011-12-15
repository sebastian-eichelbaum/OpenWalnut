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

#include <string>

#include <osg/Camera>

#include "../../common/WLogger.h"

#include "../WGETextureUtils.h"
#include "../shaders/WGEPropertyUniform.h"
#include "../shaders/WGEShaderPropertyDefine.h"

#include "WGEPostprocessorGauss.h"
#include "WGEPostprocessorSSAO.h"
#include "WGEPostprocessorMergeOp.h"

#include "WGEPostprocessorMRSSAO.h"

WGEPostprocessorMRSSAO::WGEPostprocessorMRSSAO():
    WGEPostprocessor( "MRSSAO",
                      "MRSSAO is a special ambient occlusion technique which evaluates SSAO at different resolution levels." )
{
}

WGEPostprocessorMRSSAO::WGEPostprocessorMRSSAO( osg::ref_ptr< WGEOffscreenRenderNode > offscreen,
                                                          const WGEPostprocessor::PostprocessorInput& gbuffer ):
    WGEPostprocessor( "MRSSAO",
                      "MRSSAO is a special ambient occlusion technique which evaluates SSAO at different resolution levels." )

{
    // abbreviate processor input as GBuffer
    typedef WGEPostprocessor::PostprocessorInput GBuffer;
    // also create some prototypes of the needed postprocessors
    WGEPostprocessorSSAO::SPtr ssaoProto = WGEPostprocessorSSAO::SPtr( new WGEPostprocessorSSAO() );

    // Construct the pipeline:

    // first step create a gaussed version of the GBuffer, ... we only need gaussed normals and depths
    WGEPostprocessor::SPtr g1( new WGEPostprocessorGauss( offscreen, gbuffer.m_normalTexture, gbuffer.m_depthTexture ) );
    // SSAO the result of gauss 1: (note: we use the input GBuffer but change depth and normal texture)
    WGEPostprocessor::SPtr ssaoOfG1 = ssaoProto->create( offscreen, GBuffer( gbuffer.m_colorTexture,
                                                                               g1->getOutput( 0 ),
                                                                               gbuffer.m_parameterTexture,
                                                                               gbuffer.m_tangentTexture,
                                                                               g1->getOutput( 1 ) ) );
    ssaoOfG1->getProperties()->getProperty( "Effect Only" )->toPropBool()->set( true );
    ssaoOfG1->getProperties()->getProperty( "Radius" )->toPropDouble()->set( 20 );

    // second step: gauss the gauss 1 result again
    WGEPostprocessor::SPtr g2( new WGEPostprocessorGauss( offscreen, g1->getOutput( 0 ), g1->getOutput( 1 ) ) );
    // SSAO the result of gauss 2: (note: we use the input GBuffer but change depth and normal texture)
    WGEPostprocessor::SPtr ssaoOfG2 = ssaoProto->create( offscreen, GBuffer( gbuffer.m_colorTexture,
                                                                               g2->getOutput( 0 ),
                                                                               gbuffer.m_parameterTexture,
                                                                               gbuffer.m_tangentTexture,
                                                                               g2->getOutput( 1 ) ) );
    ssaoOfG2->getProperties()->getProperty( "Effect Only" )->toPropBool()->set( true );
    ssaoOfG2->getProperties()->getProperty( "Radius" )->toPropDouble()->set( 40 );

    // we also need a SSAO of the original buffer:
    WGEPostprocessor::SPtr ssaoOfOriginal = ssaoProto->create( offscreen, gbuffer );
    ssaoOfOriginal->getProperties()->getProperty( "Effect Only" )->toPropBool()->set( true );
    ssaoOfOriginal->getProperties()->getProperty( "Radius" )->toPropDouble()->set( 10 );


    WGEPostprocessor::SPtr gOfssaoOfG1( new WGEPostprocessorGauss( offscreen, ssaoOfG1->getOutput() ) );
    WGEPostprocessor::SPtr gOfssaoOfG2( new WGEPostprocessorGauss( offscreen, ssaoOfG2->getOutput() ) );
    WGEPostprocessor::SPtr gOfssaoOfGOrg( new WGEPostprocessorGauss( offscreen, ssaoOfOriginal->getOutput() ) );


    // merge the 3 SSAO images together
    WGEPostprocessorMergeOp::SPtr merge( new WGEPostprocessorMergeOp( offscreen, gbuffer.m_colorTexture,
                                                                                 ssaoOfOriginal->getOutput(),
                                                                                 gOfssaoOfG1->getOutput(),
                                                                                 gOfssaoOfG2->getOutput()
                ) );
    // set some custom merge code:
    std::string mergecode=
        "float ssaoO = texture2D( u_texture1Sampler, pixelCoord ).r;\n"
        "float ssao0 = texture2D( u_texture2Sampler, pixelCoord ).r;\n"
        "float ssao1 = texture2D( u_texture3Sampler, pixelCoord ).r;\n"
        "float combinedSSAO = min( min( ssaoO, ssao0 ), ssao1 );\n"
        "#ifdef WGE_POSTPROCESSOR_OUTPUT_COMBINE\n"
        "color = vec4( ( texture2D( u_texture0Sampler, pixelCoord ) * combinedSSAO ).rgb, 1.0 );\n"
        "#else\n"
        "color = vec4( vec3( combinedSSAO ), 1.0 );\n"
        "#endif\n";
    merge->setGLSLMergeCode( mergecode );

    // thats it. output result
    m_resultTextures.push_back( merge->getOutput() );
}

WGEPostprocessorMRSSAO::~WGEPostprocessorMRSSAO()
{
    // cleanup
}

WGEPostprocessor::SPtr WGEPostprocessorMRSSAO::create( osg::ref_ptr< WGEOffscreenRenderNode > offscreen,
    const WGEPostprocessor::PostprocessorInput& gbuffer ) const
{
    return WGEPostprocessor::SPtr( new WGEPostprocessorMRSSAO( offscreen, gbuffer ) );
}
