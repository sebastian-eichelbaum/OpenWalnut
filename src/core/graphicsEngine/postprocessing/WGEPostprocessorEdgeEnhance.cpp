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
#include "../shaders/WGEShaderPropertyDefineOptions.h"

#include "WGEPostprocessorEdgeEnhance.h"

WGEPostprocessorEdgeEnhance::WGEPostprocessorEdgeEnhance():
    WGEPostprocessor( "Edge Enhance",
                      "Edge detection filter to emphasize edges in complex geometry." )
{
}

WGEPostprocessorEdgeEnhance::WGEPostprocessorEdgeEnhance( osg::ref_ptr< WGEOffscreenRenderNode > offscreen,
                                                          const WGEPostprocessor::PostprocessorInput& gbuffer ):
    WGEPostprocessor( "Edge Enhance",
                      "Edge detection filter to emphasize edges in complex geometry." )
{
    // we also provide a property
    WPropBool whiteEdge = m_properties->addProperty( "White Edge", "If set, the edge is drawn in white instead of black.", false );
    WPropDouble edgeThresholdL = m_properties->addProperty( "Edge Lower Threshold", "Define the edge threshold. Filters way \"weak\" edges.", 0.0 );
    WPropDouble edgeThresholdU = m_properties->addProperty( "Edge Upper Threshold", "Define the edge threshold. Filters way \"weak\" edges.", 1.0 );
    edgeThresholdL->setMin( 0.0 );
    edgeThresholdL->setMax( 1.0 );
    edgeThresholdU->setMin( 0.0 );
    edgeThresholdU->setMax( 1.0 );

    // Use the standard postprocessor uber-shader
    WGEShader::RefPtr s = new WGEShader( "WGEPostprocessor" );
    s->setDefine( "WGE_POSTPROCESSOR_EDGE" );

    // also add the m_effectOnly property as shader preprocessor
    s->addPreprocessor( m_effectOnlyPreprocessor );
    s->addPreprocessor( WGEShaderPreprocessor::SPtr(
        new WGEShaderPropertyDefineOptions< WPropBool >( whiteEdge, "WGE_POSTPROCESSOR_EDGE_BLACKEDGE", "WGE_POSTPROCESSOR_EDGE_WHITEEDGE" ) )
    );

    // create the rendering pass
    osg::ref_ptr< WGEOffscreenTexturePass > pass = offscreen->addTextureProcessingPass( s, "Edge Detection" );
    pass->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropDouble >( "u_edgeEdgeThresholdUpper", edgeThresholdU ) );
    pass->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropDouble >( "u_edgeEdgeThresholdLower", edgeThresholdL ) );

    // attach color0 output
    m_resultTextures.push_back( pass->attach( WGECamera::COLOR_BUFFER0, GL_RGB ) );

    // provide the Gbuffer input
    gbuffer.bind( pass );
}

WGEPostprocessorEdgeEnhance::~WGEPostprocessorEdgeEnhance()
{
    // cleanup
}

WGEPostprocessor::SPtr WGEPostprocessorEdgeEnhance::create( osg::ref_ptr< WGEOffscreenRenderNode > offscreen,
    const WGEPostprocessor::PostprocessorInput& gbuffer ) const
{
    return WGEPostprocessor::SPtr( new WGEPostprocessorEdgeEnhance( offscreen, gbuffer ) );
}
