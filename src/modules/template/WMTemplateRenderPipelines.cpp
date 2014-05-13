//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This is a tutorial on how to use the WGEOfffscreen interface. This tutorial also includes some shader files in the shaders subdirectory. You
// will be referred to them later.
//
//  You will need the knowledge of these tutorials before you can go on:
//  * WMTemplate
//  * WMTemplateShaders
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <string>

#include <osg/StateAttribute>

#include "core/kernel/WKernel.h"

#include "core/graphicsEngine/WGEManagedGroupNode.h"
#include "core/graphicsEngine/WGERequirement.h"
#include "core/graphicsEngine/offscreen/WGEOffscreenRenderNode.h"    // <- this is the awesome new header you will need
#include "core/graphicsEngine/shaders/WGEShader.h"

// Some utils for creating some demo geometry.
#include "WDemoGeometry.h"
#include "WMTemplateRenderPipelines.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// All the basic setup ... Refer to WMTemplate.cpp if you do not understand these commands.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WMTemplateRenderPipelines::WMTemplateRenderPipelines()
    : WModule()
{
}

WMTemplateRenderPipelines::~WMTemplateRenderPipelines()
{
}

boost::shared_ptr< WModule > WMTemplateRenderPipelines::factory() const
{
    // NOTE: Refer to WMTemplate.cpp if you do not understand these commands.
    return boost::shared_ptr< WModule >( new WMTemplateRenderPipelines() );
}

const std::string WMTemplateRenderPipelines::getName() const
{
    // NOTE: Refer to WMTemplate.cpp if you do not understand these commands.
    return "Template Render Pipelines";
}

const std::string WMTemplateRenderPipelines::getDescription() const
{
    // NOTE: Refer to WMTemplate.cpp if you do not understand these commands.
    return "Show how to use colormapping in your modules.";
}

void WMTemplateRenderPipelines::connectors()
{
    // NOTE: Refer to WMTemplate.cpp if you do not understand these commands.

    // We do not need any connectors. Have a look at WMTemplate.cpp if you want to know what this means.
    WModule::connectors();
}

void WMTemplateRenderPipelines::properties()
{
    // NOTE: Refer to WMTemplate.cpp if you do not understand these commands.

    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    WModule::properties();
}

void WMTemplateRenderPipelines::requirements()
{
    // NOTE: Refer to WMTemplate.cpp if you do not understand these commands.

    // We need graphics to draw anything:
    m_requirements.push_back( new WGERequirement() );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ATTENTION: now it gets interesting ...
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void WMTemplateRenderPipelines::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );

    // Now, we can mark the module ready.
    ready();

    // In this tutorial, we will show you how to develop quite complex offscreen render pipelines with ease in OpenWalnut. For this, we use a
    // rather simple example. We want to render some geometry in a cartoonish way. Although this can be done in one additional pass, we split it
    // into two additional passes here to demonstrate WGEOffscreen* a little bit more in detail.
    //
    // NOTE: Refer to WMTemplateShaders.cpp if you do not understand the next sections. It is important to understand these basics before going
    // on.

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 1. Setup some geometry.
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // So let us start by creating our geometry. This is taken from the WMTemplateShaders example_
    osg::ref_ptr< WGEGroupNode > rootNode = new WGEGroupNode();

    // Add Scene
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( rootNode );
    rootNode->setMatrix( osg::Matrixd::rotate( 1.57, 1.0, 0.0, 0.0 ) ); // First parameter is the angle in radians.

    // Now we can add your demo geometry:
    osg::ref_ptr< osg::Node > spheres = WDemoGeometry::createSphereGeometry();
    osg::ref_ptr< osg::Node > plane = WDemoGeometry::createPlaneGeometry();

    // Allow blending here? Yes. We need it later.
    plane->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
    plane->getOrCreateStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

    // To see our new geometry we need to add it to the group of ours:
    rootNode->insert( spheres );
    rootNode->insert( plane );

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 2. Setup the offscreen pipeline.
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 7. ... do stuff.
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // You already know this module loop code. You can now use your knowledge from WMTemplateShaders to implement cool features and control them
    // in you shaders of the pipelines ...

    // Now the remaining module code. In our case, this is empty.
    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();
        if( m_shutdownFlag() )
        {
            break;
        }
    }

    // Never miss to clean up. Especially remove your OSG nodes. Everything else you add to these nodes will be removed automatically.
    debugLog() << "Shutting down ...";
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( rootNode );
}

