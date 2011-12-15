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

#include "WGEPostprocessorMergeOp.h"

WGEPostprocessorMergeOp::WGEPostprocessorMergeOp():
    WGEPostprocessor( "MergeOp",
                      "MergeOp - combines all input textures in a user defined way." )
{
}

WGEPostprocessorMergeOp::WGEPostprocessorMergeOp( osg::ref_ptr< WGEOffscreenRenderNode > offscreen,
                                              osg::ref_ptr< osg::Texture2D > tex0,
                                              osg::ref_ptr< osg::Texture2D > tex1,
                                              osg::ref_ptr< osg::Texture2D > tex2,
                                              osg::ref_ptr< osg::Texture2D > tex3,
                                              osg::ref_ptr< osg::Texture2D > tex4,
                                              osg::ref_ptr< osg::Texture2D > tex5,
                                              osg::ref_ptr< osg::Texture2D > tex6,
                                              osg::ref_ptr< osg::Texture2D > tex7 ):
    WGEPostprocessor( "MergeOp",
                      "MergeOp - combines all input textures in a user defined way." ),
    m_codeInjector( new WGEShaderCodeInjector( "WGE_POSTPROCESSOR_MERGEOP_CODE" ) )

{
    // Use the standard postprocessor uber-shader
    m_mergeOpShader = new WGEShader( "WGEPostprocessor" );
    m_mergeOpShader->setDefine( "WGE_POSTPROCESSOR_MERGEOP" );
    m_mergeOpShader->addPreprocessor( m_codeInjector );

    // also add the m_effectOnly property as shader preprocessor
    m_mergeOpShader->addPreprocessor( m_effectOnlyPreprocessor );

    // create the rendering pass
    osg::ref_ptr< WGEOffscreenTexturePass > pass = offscreen->addTextureProcessingPass( m_mergeOpShader, "MergeOp" );

    // for each of the textures do:

    // attach color0 output and bind tex0
    m_resultTextures.push_back( pass->attach( osg::Camera::COLOR_BUFFER0, GL_RGBA ) );
    pass->bind( tex0, 0 );
    m_mergeOpShader->setDefine( "WGE_POSTPROCESSOR_MERGEOP_UNIT0" );

    // attach color1 output and bind tex1
    if( tex1 )
    {
        pass->bind( tex1, 1 );
        m_mergeOpShader->setDefine( "WGE_POSTPROCESSOR_MERGEOP_UNIT1" );
    }
    // attach color2 output and bind tex2
    if( tex2 )
    {
        pass->bind( tex2, 2 );
        m_mergeOpShader->setDefine( "WGE_POSTPROCESSOR_MERGEOP_UNIT2" );
    }
    // attach color3 output and bind tex3
    if( tex3 )
    {
        pass->bind( tex3, 3 );
        m_mergeOpShader->setDefine( "WGE_POSTPROCESSOR_MERGEOP_UNIT3" );
    }
    // attach color4 output and bind tex4
    if( tex4 )
    {
        pass->bind( tex4, 4 );
        m_mergeOpShader->setDefine( "WGE_POSTPROCESSOR_MERGEOP_UNIT4" );
    }
    // attach color5 output and bind tex5
    if( tex5 )
    {
        pass->bind( tex5, 5 );
        m_mergeOpShader->setDefine( "WGE_POSTPROCESSOR_MERGEOP_UNIT5" );
    }
    // attach color6 output and bind tex6
    if( tex6 )
    {
        pass->bind( tex6, 6 );
        m_mergeOpShader->setDefine( "WGE_POSTPROCESSOR_MERGEOP_UNIT6" );
    }
    // attach color7 output and bind tex7
    if( tex7 )
    {
        pass->bind( tex7, 7 );
        m_mergeOpShader->setDefine( "WGE_POSTPROCESSOR_MERGEOP_UNIT7" );
    }
}

WGEPostprocessorMergeOp::~WGEPostprocessorMergeOp()
{
    // cleanup
}

WGEPostprocessor::SPtr WGEPostprocessorMergeOp::create( osg::ref_ptr< WGEOffscreenRenderNode > offscreen,
    const WGEPostprocessor::PostprocessorInput& gbuffer ) const
{
    return WGEPostprocessor::SPtr( new WGEPostprocessorMergeOp( offscreen, gbuffer.m_colorTexture ) );
}

void WGEPostprocessorMergeOp::setGLSLMergeCode( std::string code )
{
    m_mergeOpShader->setDefine( "WGE_POSTPROCESSOR_MERGEOP_CUSTOM" );
    m_codeInjector->setCode( code );
}
