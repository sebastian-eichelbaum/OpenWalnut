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
// This is a tutorial on how to use the WGEShader interface. This tutorial also includes some shader files in the shaders subdirectory. You will
// be referred to them later.
// You do not know what shaders are? No nothing about GLSL, OpenGL or the OpenSceneGraph? Then this tutorial is probably not for you. This
// tutorial demonstrates how to comfortably integrate GLSL shaders into your modules and how to interact with them (coupling with properties).
//
// If you want to learn how to program a module, refer to WMTemplate.cpp. It is an extensive tutorial on all the details.
// In this tutorial, we assume you already know how to write modules.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <string>

#include <osg/Group>
#include <osg/StateAttribute>

#include "core/kernel/WKernel.h"

#include "core/graphicsEngine/WGEManagedGroupNode.h"
#include "core/graphicsEngine/WGERequirement.h"
#include "core/graphicsEngine/WGETexture.h"
#include "core/graphicsEngine/WGEImage.h"
#include "core/graphicsEngine/WGETextureUtils.h"
#include "core/graphicsEngine/WGEUtils.h"
#include "core/graphicsEngine/WGEColormapping.h"    // <- this is the awesome new header you will need
#include "core/graphicsEngine/shaders/WGEShader.h"

// Some utils for creating some demo geometry.
#include "WDemoGeometry.h"
#include "WMTemplateColormapping.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// All the basic setup ... Refer to WMTemplate.cpp if you do not understand these commands.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WMTemplateColormapping::WMTemplateColormapping()
    : WModule()
{
}

WMTemplateColormapping::~WMTemplateColormapping()
{
}

boost::shared_ptr< WModule > WMTemplateColormapping::factory() const
{
    // NOTE: Refer to WMTemplate.cpp if you do not understand these commands.
    return boost::shared_ptr< WModule >( new WMTemplateColormapping() );
}

const std::string WMTemplateColormapping::getName() const
{
    // NOTE: Refer to WMTemplate.cpp if you do not understand these commands.
    return "Template Colormapping";
}

const std::string WMTemplateColormapping::getDescription() const
{
    // NOTE: Refer to WMTemplate.cpp if you do not understand these commands.
    return "Show how to use colormapping in your modules.";
}

void WMTemplateColormapping::connectors()
{
    // NOTE: Refer to WMTemplate.cpp if you do not understand these commands.

    // We do not need any connectors. Have a look at WMTemplate.cpp if you want to know what this means.
    WModule::connectors();
}

void WMTemplateColormapping::properties()
{
    // NOTE: Refer to WMTemplate.cpp if you do not understand these commands.

    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    WModule::properties();
}

void WMTemplateColormapping::requirements()
{
    // NOTE: Refer to WMTemplate.cpp if you do not understand these commands.

    // We need graphics to draw anything:
    m_requirements.push_back( new WGERequirement() );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ATTENTION: now it gets interesting ...
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void WMTemplateColormapping::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );

    // Now, we can mark the module ready.
    ready();

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 1. Setup some geometry.
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    // 7. ... do stuff.
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // You already know this module loop code. You won't need to implement any further interaction between your properties and your shader.
    // This happens automatically due to OpenWalnut's comfortable shader<->property interface we set up above. But of course the options are
    // endless. Modify the geometry, change attributes and so on ...

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

