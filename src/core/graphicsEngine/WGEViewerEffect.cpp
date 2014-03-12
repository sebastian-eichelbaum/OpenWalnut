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

#include <osg/Depth>

#include "WGEGeodeUtils.h"
#include "shaders/WGEShader.h"
#include "callbacks/WGENodeMaskCallback.h"

#include "WGEViewerEffect.h"

WGEViewerEffect::WGEViewerEffect( std::string name, std::string description, const char** icon ):
    WObjectNDIP( name, description, icon )
{
    // setup camera
    setClearMask( GL_DEPTH_BUFFER_BIT );
    setRenderOrder( WGECamera::POST_RENDER );
    setReferenceFrame( osg::Transform::ABSOLUTE_RF );
    setProjectionMatrixAsOrtho2D( 0.0, 1.0, 0.0, 1.0 );
    setViewMatrix( osg::Matrixd::identity() );

    // some state options
    m_state = getOrCreateStateSet();
    m_state->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );
    m_state->setMode( GL_LIGHTING, osg::StateAttribute::PROTECTED );
    m_state->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    m_state->setMode( GL_BLEND, osg::StateAttribute::PROTECTED );
    m_state->setMode( GL_BLEND, osg::StateAttribute::ON );

    osg::Depth* depth = new osg::Depth;
    depth->setWriteMask( false );
    m_state->setAttributeAndModes( depth, osg::StateAttribute::ON );

    m_geode = wge::genFinitePlane( osg::Vec3( 0.0, 0.0, 0.0 ),
                                   osg::Vec3( 1.0, 0.0, 0.0 ),
                                   osg::Vec3( 0.0, 1.0, 0.0 ) );

    // add the slice to the geode
    addChild( m_geode );

    // Configure properties
    m_active = m_properties->addProperty( "Active", "Activate this effect?", false );
    // let this control the nodemask
    addUpdateCallback( new WGENodeMaskCallback( m_active ) );
}

WGEViewerEffect::~WGEViewerEffect()
{
    // cleanup
}

bool WGEViewerEffect::isEnabled() const
{
    return m_active->get();
}

void WGEViewerEffect::setEnabled( bool enable )
{
    m_active->set( enable );
}
