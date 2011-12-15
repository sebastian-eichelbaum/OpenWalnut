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

#include "WGEPostprocessorGauss.h"

WGEPostprocessorGauss::WGEPostprocessorGauss():
    WGEPostprocessor( "Gauss Filtering",
                      "Gauss filter all specified input textures." )
{
}

WGEPostprocessorGauss::WGEPostprocessorGauss( osg::ref_ptr< WGEOffscreenRenderNode > offscreen,
                                              osg::ref_ptr< osg::Texture2D > tex0,
                                              osg::ref_ptr< osg::Texture2D > tex1,
                                              osg::ref_ptr< osg::Texture2D > tex2,
                                              osg::ref_ptr< osg::Texture2D > tex3,
                                              osg::ref_ptr< osg::Texture2D > tex4,
                                              osg::ref_ptr< osg::Texture2D > tex5,
                                              osg::ref_ptr< osg::Texture2D > tex6,
                                              osg::ref_ptr< osg::Texture2D > tex7 ):
    WGEPostprocessor( "Gauss Filtering",
                      "Gauss filter all specified input textures." )
{
    // Use the standard postprocessor uber-shader
    WGEShader::RefPtr s = new WGEShader( "WGEPostprocessor" );
    s->setDefine( "WGE_POSTPROCESSOR_GAUSS" );

    // also add the m_effectOnly property as shader preprocessor
    s->addPreprocessor( m_effectOnlyPreprocessor );

    // create the rendering pass
    osg::ref_ptr< WGEOffscreenTexturePass > pass = offscreen->addTextureProcessingPass( s, "Gauss Filter" );

    // for each of the textures do:

    // attach color0 output and bind tex0
    m_resultTextures.push_back( pass->attach( osg::Camera::COLOR_BUFFER0, GL_RGBA ) );
    pass->bind( tex0, 0 );
    s->setDefine( "WGE_POSTPROCESSOR_GAUSS_UNIT0" );

    // attach color1 output and bind tex1
    if( tex1 )
    {
        m_resultTextures.push_back( pass->attach( osg::Camera::COLOR_BUFFER1, GL_RGBA ) );
        pass->bind( tex1, 1 );
        s->setDefine( "WGE_POSTPROCESSOR_GAUSS_UNIT1" );
    }
    // attach color2 output and bind tex2
    if( tex2 )
    {
        m_resultTextures.push_back( pass->attach( osg::Camera::COLOR_BUFFER2, GL_RGBA ) );
        pass->bind( tex2, 2 );
        s->setDefine( "WGE_POSTPROCESSOR_GAUSS_UNIT2" );
    }
    // attach color3 output and bind tex3
    if( tex3 )
    {
        m_resultTextures.push_back( pass->attach( osg::Camera::COLOR_BUFFER3, GL_RGBA ) );
        pass->bind( tex3, 3 );
        s->setDefine( "WGE_POSTPROCESSOR_GAUSS_UNIT3" );
    }
    // attach color4 output and bind tex4
    if( tex4 )
    {
        m_resultTextures.push_back( pass->attach( osg::Camera::COLOR_BUFFER4, GL_RGBA ) );
        pass->bind( tex4, 4 );
        s->setDefine( "WGE_POSTPROCESSOR_GAUSS_UNIT4" );
    }
    // attach color5 output and bind tex5
    if( tex5 )
    {
        m_resultTextures.push_back( pass->attach( osg::Camera::COLOR_BUFFER5, GL_RGBA ) );
        pass->bind( tex5, 5 );
        s->setDefine( "WGE_POSTPROCESSOR_GAUSS_UNIT5" );
    }
    // attach color6 output and bind tex6
    if( tex6 )
    {
        m_resultTextures.push_back( pass->attach( osg::Camera::COLOR_BUFFER6, GL_RGBA ) );
        pass->bind( tex6, 6 );
        s->setDefine( "WGE_POSTPROCESSOR_GAUSS_UNIT6" );
    }
    // attach color7 output and bind tex7
    if( tex7 )
    {
        m_resultTextures.push_back( pass->attach( osg::Camera::COLOR_BUFFER7, GL_RGBA ) );
        pass->bind( tex7, 7 );
        s->setDefine( "WGE_POSTPROCESSOR_GAUSS_UNIT7" );
    }
}

WGEPostprocessorGauss::~WGEPostprocessorGauss()
{
    // cleanup
}

WGEPostprocessor::SPtr WGEPostprocessorGauss::create( osg::ref_ptr< WGEOffscreenRenderNode > offscreen,
    const WGEPostprocessor::PostprocessorInput& gbuffer ) const
{
    return WGEPostprocessor::SPtr( new WGEPostprocessorGauss( offscreen, gbuffer.m_colorTexture ) );
}

