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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This is a tutorial on how to use the WGEColormapping interface. This tutorial also includes some shader files in the shaders subdirectory. You
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
#include "core/graphicsEngine/WGETexture.h"
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

    // This tutorial is about applying the global colormapping to your geometry. So, if you write a module which creates some geometry, you might
    // want to be able to map other datasets onto this surface. The WGEColormapping interface is made for exactly this. It consists of a C++ part
    // and a quite complex GLSL part. But before we start, here are some important facts.
    //
    // In OpenWalnut, most 3D scalar or vectorial datasets can be used as colormaps. The data module automatically registers compatible datasets
    // to the global colormapper. If you create data for your own, you can use the Colormapping module to register data. Each supported
    // WDataSetSingle instance has a method getTexture() that returns the data as OpenGL compatible texture. As these texture were derived from
    // WGETexture they already provide a lot of comfortable features. You have learned them in the WMTemplateShaders tutorial. These textures
    // also store their transformation matrix and set them automatically to the right texture unit as texture matrix. So, the colormapper ensures
    // that the colormap only appears on the surface parts that intersect with the bounding volume of the original dataset!
    //
    // This means, you can use either global colormapping, which usually is recommended or you can use an input dataset's texture and bind it to
    // your state. This tutorial shows the global method as binding a texture was already shown in the shader tutorial. In GLSL, we then use the
    // GLSL colormapping implementation to actually color each fragment.
    // To run this module, you need to create a colormap (or load a dataset). An easy way is to add a Scalar Dataset Creator module and attach a
    // Colormapper module to it! Ensure that the colormap-data overlaps with our geometry ... i.e. somewhere in the box defined by
    // 0,0,0 to 100,100,50.

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 1. Setup some geometry.
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    osg::ref_ptr< WGEGroupNode > rootNode = new WGEGroupNode();

    // Add Scene
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( rootNode );

    // Now we can add your demo geometry:
    osg::ref_ptr< osg::Node > spheres = WDemoGeometry::createSphereGeometry();
    osg::ref_ptr< osg::Node > plane = WDemoGeometry::createPlaneGeometry();

    // Allow blending here? Yes. We need it later.
    plane->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
    plane->getOrCreateStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

    // To see our new geometry we need to add it to the group of ours:
    rootNode->insert( spheres );
    rootNode->insert( plane );

    // Allow blending, since colormaps might contain transparency.
    rootNode->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );

    // We use some shaders too:
    osg::ref_ptr< WGEShader > globalShader(
        new WGEShader(
                        "WMTemplateColormapping",    // shader name prefix
                        m_localPath                  // where to search?
                     )
    );
    globalShader->apply( rootNode );

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 2. Setup some the colormapper
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Colormapping? One call!
    WGEColormapping::apply( rootNode,           // Apply to all our nodes
                            globalShader        // Tell the colormapper which shader is bound to the node
                          );

    // This was easy! You tell the colormapper on which node you want to bind it and the corresponding shader. The shader is needed as the
    // colormapper sets several defines to the shader.

    // Now we can switch over to the shader:  WMTemplateColormapping-vertex.glsl!

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 3. ... do stuff.
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // You already know this module loop code.

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

