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

#include "WGEPostprocessorEdgeEnhance.h"
#include "WGEPostprocessorCelShading.h"

#include "WGEPostprocessor.h"

WGEPostprocessor::WGEPostprocessor( std::string name, std::string description ):
    WPrototyped(),
    m_name( name ),
    m_description( description )
{
}

WGEPostprocessor::WGEPostprocessor( osg::ref_ptr< WGEOffscreenRenderNode > /* offscreen */,
                                    const WGEPostprocessor::PostprocessorInput& gbuffer,
                                    std::string name,
                                    std::string description ):
    WPrototyped(),
    m_resultTexture( gbuffer.m_colorTexture ),
    m_depthTexture( gbuffer.m_depthTexture ),
    m_properties( boost::shared_ptr< WProperties >( new WProperties( "Settings for " + name, "Post-processing properties" ) ) ),
    m_name( name ),
    m_description( description )
{
    // there is always one property:
    m_effectOnly = m_properties->addProperty( "Effect only", "If active, the plain effect will be shown instead a combination of effect "
                                                             "and color. This settings does not affect all postprocessors.", false );

    // for convenience, also create a preprocessor for this property
    m_effectOnlyPreprocessor = WGEShaderPreprocessor::SPtr( new WGEShaderPropertyDefineOptions< WPropBool >( m_effectOnly,
        "WGE_POSTPROCESSOR_OUTPUT_COMBINE", "WGE_POSTPROCESSOR_OUTPUT_EFFECT_ONLY" ) );
}

WGEPostprocessor::~WGEPostprocessor()
{
    // cleanup
}

WPropGroup WGEPostprocessor::getProperties() const
{
    return m_properties;
}

osg::ref_ptr< osg::Texture2D > WGEPostprocessor::getOutput() const
{
    return m_resultTexture;
}

osg::ref_ptr< osg::Texture2D > WGEPostprocessor::getDepth() const
{
    return m_depthTexture;
}

WGEPostprocessor::PostprocessorInput WGEPostprocessor::PostprocessorInput::attach( osg::ref_ptr< WGEOffscreenRenderPass > from )
{
    PostprocessorInput buf;
    buf.m_colorTexture = from->attach( osg::Camera::COLOR_BUFFER0 );
    buf.m_normalTexture = from->attach( osg::Camera::COLOR_BUFFER1, GL_RGB );
    buf.m_parameterTexture = from->attach( osg::Camera::COLOR_BUFFER2, GL_LUMINANCE );
    buf.m_tangentTexture = from->attach( osg::Camera::COLOR_BUFFER3, GL_RGB );
    buf.m_depthTexture = from->attach( osg::Camera::DEPTH_BUFFER );

    return buf;
}

void WGEPostprocessor::PostprocessorInput::bind( osg::ref_ptr< WGEOffscreenRenderPass > to ) const
{
    to->bind( m_colorTexture, 0 );
    to->bind( m_normalTexture, 1 );
    to->bind( m_parameterTexture, 2 );
    to->bind( m_depthTexture, 3 );
    to->bind( m_tangentTexture, 4 );
}

WGEPostprocessor::ProcessorList WGEPostprocessor::getPostprocessors()
{
    WGEPostprocessor::ProcessorList postprocs;

    // create prototypes of the postprocessors OW knows about
    postprocs.push_back( WGEPostprocessor::SPtr( new WGEPostprocessorEdgeEnhance() ) );
    postprocs.push_back( WGEPostprocessor::SPtr( new WGEPostprocessorCelShading() ) );

    return postprocs;
}

const std::string WGEPostprocessor::getName() const
{
    return m_name;
}

const std::string WGEPostprocessor::getDescription() const
{
    return m_description;
}

