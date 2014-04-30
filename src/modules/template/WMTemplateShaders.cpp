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
//
// If you want to learn how to program a module, refer to WMTemplate.cpp. It is an extensive tutorial on all the details.
// In this tutorial, we assume you already know how to write modules.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <string>

#include <osg/Geode>
#include <osg/Group>
#include <osg/Material>
#include <osg/ShapeDrawable>
#include <osg/StateAttribute>

#include "core/kernel/WKernel.h"
// #include "core/common/WPathHelper.h"

#include "core/graphicsEngine/WGEGeodeUtils.h"
#include "core/graphicsEngine/WGEManagedGroupNode.h"
#include "core/graphicsEngine/WGERequirement.h"
#include "core/graphicsEngine/WGETextureUtils.h"
#include "core/graphicsEngine/WGEUtils.h"
#include "core/graphicsEngine/shaders/WGEPropertyUniform.h"
#include "core/graphicsEngine/shaders/WGEShader.h"
#include "core/graphicsEngine/shaders/WGEShaderDefineOptions.h"
#include "core/graphicsEngine/shaders/WGEShaderPropertyDefine.h"
#include "core/graphicsEngine/shaders/WGEShaderPropertyDefineOptions.h"

#include "WMTemplateShaders.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// All the basic setup ... Refer to WMTemplate.cpp if you do not understand these commands.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WMTemplateShaders::WMTemplateShaders()
    : WModule()
{
}

WMTemplateShaders::~WMTemplateShaders()
{
}

boost::shared_ptr< WModule > WMTemplateShaders::factory() const
{
    // NOTE: Refer to WMTemplate.cpp if you do not understand these commands.
    return boost::shared_ptr< WModule >( new WMTemplateShaders() );
}

const std::string WMTemplateShaders::getName() const
{
    // NOTE: Refer to WMTemplate.cpp if you do not understand these commands.
    return "Template Shaders";
}

const std::string WMTemplateShaders::getDescription() const
{
    // NOTE: Refer to WMTemplate.cpp if you do not understand these commands.
    return "Show some shader programming examples.";
}

void WMTemplateShaders::connectors()
{
    // NOTE: Refer to WMTemplate.cpp if you do not understand these commands.

    // We do not need any connectors. Have a look at WMTemplate.cpp if you want to know what this means.
    WModule::connectors();
}

void WMTemplateShaders::properties()
{
    // NOTE: Refer to WMTemplate.cpp if you do not understand these commands.

    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    // We create some preferences here to use in our shader later:


    WModule::properties();
}

void WMTemplateShaders::requirements()
{
    // NOTE: Refer to WMTemplate.cpp if you do not understand these commands.

    // We need graphics to draw anything:
    m_requirements.push_back( new WGERequirement() );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ATTENTION: now it gets interesting ...
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void WMTemplateShaders::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );

    // Now, we can mark the module ready.
    ready();

    // We begin as in nearly all modules: create a group node in which we will place our demo geometry.
    osg::ref_ptr< WGEGroupNode > rootNode = new WGEGroupNode();
    // We add this to the global scene. If you are unfamiliar with this, please refer to WMTemplate.cpp
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( rootNode );

    // Now we can add your demo geometry:
    osg::ref_ptr< osg::Node > spheres = createSphereGeometry();
    osg::ref_ptr< osg::Node > plane = createPlaneGeometry();

    // To see our new geometry we need to add it to the group of ours:
    rootNode->insert( spheres );
    rootNode->insert( plane );

    // OK. Let us summarize the rather uninteresting part: We created a root node (a group) that will contain our demo geometry. We created the
    // geometry and added it to the group.

    // Now it gets really interesting. We load a shader:


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

osg::ref_ptr< osg::Node > WMTemplateShaders::createSphereGeometry()
{
    // create a nice plane and some spheres on top of it. This is just an example. To understand the following code, we would like to refer you
    // to the OpenSceneGraph documentation.

    // add a bunch of spheres to a group and return it. Normals are set but no color.
    osg::ref_ptr< osg::Group > group( new osg::Group );

    // add 5 spheres with increasing size and add them along the X axs
    float x = 2.0;
    for( size_t i = 0; i < 5; ++i )
    {
        // Create a sphere.
        osg::Geode* sphereGeode = new osg::Geode;
        sphereGeode->addDrawable(
            new osg::ShapeDrawable(
                new osg::Sphere(
                    osg::Vec3d( x, 50.0, 2 + i * 3 ),   // moving center
                    2 + i * 3                           // increasing radius
                )
            )
        );

        // move the spheres along the X axis a bit each time
        x += 2 + 2 * i * 3 + 10.0;

        // Add to group.
        group->addChild( sphereGeode );
    }
    return group;
}

osg::ref_ptr< osg::Node > WMTemplateShaders::createPlaneGeometry()
{
    // create a nice plane and some spheres on top of it. This is just an example. To understand the following code, we would like to refer you
    // to the OpenSceneGraph documentation.

    // for creating a plane, we fortunately have a WGE function:
    return wge::genFinitePlane( osg::Vec3( 0.0, 0.0, 0.0 ),   // base
                                osg::Vec3( 100.0, 0.0, 0.0 ), // spanning vector a
                                osg::Vec3( 0.0, 100.0, 0.0 ), // spanning vector b
                                WColor( 0.5, 0.5, 0.5, 1.0 )  // a color.
           );
}
