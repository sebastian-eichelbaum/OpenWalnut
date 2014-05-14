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

#include "core/graphicsEngine/callbacks/WGENodeMaskCallback.h"
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

    // show hud?
    m_showHUD       = m_properties->addProperty( "Show HUD", "Check to enable the debugging texture HUD.", true );

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
    //
    // First, a short overview on offscreen rendering. Nowadays in OpenGL and OSG, offscreen rendering is done using framebuffer objects (FBO).
    // You can attach textures to certain targets on the FBO. For example, your shader wants to output a color, a normal and you will need the
    // depth. Then you attach the first two color buffers and the depth buffer. So you get three textures. After the draw call (done internally
    // by OSG), the textures will be filled with a color, normals and your requested depth buffer. You can then bind these textures to another
    // state and re-use them. To write a per-pixel processing, you will use a full-screen quad and bind the mentioned textures to it. Then, you
    // can utilize a fragment shader to process each input texture texel which matches a pixel. To write your resulting image to the on-screen
    // framebuffer, you de-activate (unbind in OpenGL terms) the FBO and render a full-screen quad. Your fragment shader then defines the final
    // pixel color and depth.
    //
    // OpenWalnut's offscreen render mechanism also follows this principles. What we have done is providing a comfortable interface to OSG
    // cameras (they represent the FBO) and a comfortable way to bind and attach textures. This way, you are able to write complex pipelines
    // somehow like you would write module pipelines: define a render pass, bind its inputs, define its outputs. In this tutorial, you will learn
    // how to do this. Remember that you can also use the mechanisms shown in WMTemplateShaders to define complex parameters and switches for the
    // shaders you use in each pass.
    //
    // Now lets get started ...
    //

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 1. Setup some geometry.
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Create a module root node.
    osg::ref_ptr< WGEGroupNode > rootNode = new WGEGroupNode();
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( rootNode );

    // So let us start by creating our geometry. This is taken from the WMTemplateShaders example.
    osg::ref_ptr< osg::MatrixTransform > sceneNode = new osg::MatrixTransform();
    sceneNode->setMatrix( osg::Matrixd::rotate( 1.57, 1.0, 0.0, 0.0 ) ); // First parameter is the angle in radians.

    // Now we can add your demo geometry:
    osg::ref_ptr< osg::Node > spheres = WDemoGeometry::createSphereGeometry();
    osg::ref_ptr< osg::Node > plane = WDemoGeometry::createPlaneGeometry();

    // Allow blending here? Yes. We need it later.
    plane->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
    plane->getOrCreateStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

    // To see our new geometry we need to add it to the group of ours:
    sceneNode->addChild( spheres );
    sceneNode->addChild( plane );

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 2. Setup the offscreen pipeline.
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // First, is to create a node in the scene-graph, which will encapsulate our render pipeline:
    osg::ref_ptr< WGEOffscreenRenderNode > offscreen(
        new WGEOffscreenRenderNode(
            // Specify a reference camera all the offscreen passes will use
            WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getCamera()
        )
    );
    // The reference you have to specify is important to keep certain states in sync with it when you add further render passes.

    // Now, we create all the passes we need. Remember the above description about what we are going to do? We will create two passes now that do
    // a simple edge detection and a color-reduction (cel-shading alike). The results have to be merged again and shown on screen. This will be
    // the final on-screen render pass.

    // First pass: render the geometry to textures. This can be done by addGeometryRenderPass:
    osg::ref_ptr< WGEOffscreenRenderPass > renderToTexture = offscreen->addGeometryRenderPass(
        sceneNode,                                                        // The node to render
        new WGEShader( "WMTemplateRenderPipelines-Render", m_localPath ), // The shader to use
        "Render"                                                          // A name. Useful for debugging
    );
    // You might now ask why we need the shader for the creation of the pass? Well it is not needed. There are methods available which do not
    // require them. It simply applies your shader to the render pass itself.

    // Edge Detection:
    osg::ref_ptr< WGEOffscreenRenderPass > edgeDetection =  offscreen->addTextureProcessingPass(
        new WGEShader( "WMTemplateRenderPipelines-Edge", m_localPath ),
        "Edge Detection"
    );

    // Cel Shader:
    osg::ref_ptr< WGEOffscreenRenderPass > celShading =  offscreen->addTextureProcessingPass(
        new WGEShader( "WMTemplateRenderPipelines-Cel", m_localPath ),
        "Cel Shading"
    );

    // Finally, we want to merge the results on screen:
    osg::ref_ptr< WGEOffscreenRenderPass > finalPass = offscreen->addFinalOnScreenPass(
        new WGEShader( "WMTemplateRenderPipelines-Merge", m_localPath ),
        "Merge"
    );

    // Thats it. We now render our geometry to a texture. But to use this rendering result in both processing passes, we have to connect the
    // output of the first geometry render pass to the processing passes:

    // So we need to tell the offscreen pass to attach the according outputs for us:
    osg::ref_ptr< osg::Texture2D > geometryColor = renderToTexture->attach(
        WGECamera::COLOR_BUFFER0,   // Which output?
        GL_RGBA                     // Its pixel format.
    );
    osg::ref_ptr< osg::Texture2D > geometryDepth = renderToTexture->attach( WGECamera::DEPTH_BUFFER );

    // The reason for choosing RGBA as pixel format is simple: we need to transport which pixel was occupied by a fragment. The default clear
    // color of all passes is vec4(0,0,0,0). This means, no fragment -> alpha == 0.

    // ... and bind these textures to both processing passes:
    edgeDetection->bind( geometryDepth, 0 );
    celShading->bind( geometryColor, 0 );

    // That was easy, right? Please keep in mind that each render pass is basically an OSG Camera and works as an framebuffer object.

    // Lets go on and connect the output of the processing passes to the final on-screen pass. But first? Yes! Attach the outputs:
    osg::ref_ptr< osg::Texture2D > edges = edgeDetection->attach(
        WGECamera::COLOR_BUFFER0,   // Which output?
        GL_LUMINANCE                // Only gray colors are needed
    );

    osg::ref_ptr< osg::Texture2D > colors = celShading->attach(
        WGECamera::COLOR_BUFFER0,   // Which output?
        GL_RGBA                     // RGBA
    );

    // Now we can bind them. Just like textures in a stateset (and in fact, it is not more than that).
    finalPass->bind( colors, 0 );
    finalPass->bind( edges, 1 );

    // The final pass should also blend properly:
    finalPass->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );

    // Finally, just like every other node, ADD to the node.
    rootNode->insert( offscreen );

    // IMPORTANT!
    //
    // OSG automatically culls invisible geometry and modifies nested camera clipping settings. This means for us that we have to tell OSG the
    // real size of our geometry. Or else, OSG will think we are a thin plane, thus changing near-far clipping for the camera rendering the
    // geometry which isn't just a thin plane. Thats somehow a bug or a feature. We do not know. To avoid this, we can add a cull proxy to our
    // scene. If you know how to properly fix this (might even be located in the WGEOffscreen classes), please tell us.

    // You usually get this from your dataset grid. We hardcode it.
    WBoundingBox bbox( -100.0, -100.0, -100.0, 200.0, 200.0, 200.0 );
    // Cull proxy. Updated on dataset change
    osg::ref_ptr< osg::Node > cullProxy = wge::generateCullProxy( bbox );
    rootNode->insert( cullProxy );

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 4. Debugging
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Offscreen rendering has a drawback. You usually cannot see the intermediate results (textures). For this, OpenWalnut provides us with a
    // little helper: the texture HUD.

    // The texture hud is already a part of WGEOffscreenRenderNode. Your task is to activate it. It basically is an osg::Node and thus, can be
    // activated by its node mask, a callback and a property:
    offscreen->getTextureHUD()->addUpdateCallback( new WGENodeMaskCallback( m_showHUD ) );

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 5. Shaders!
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Now head over to the shaders. Start with WMTemplateRenderPipelines-Render-vertex.glsl. These shaders are trivial. There is nothing special
    // to take care of. Especially the vertex shaders are boring. The fragment shaders can be used to process each pixel of your textures ...

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 6. ... do stuff.
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

