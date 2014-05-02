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
#include "core/graphicsEngine/WGETexture.h"
#include "core/graphicsEngine/WGEImage.h"
#include "core/graphicsEngine/WGETextureUtils.h"
#include "core/graphicsEngine/WGEUtils.h"
#include "core/graphicsEngine/shaders/WGEPropertyUniform.h"
#include "core/graphicsEngine/shaders/WGEShader.h"
#include "core/graphicsEngine/shaders/WGEShaderDefineOptions.h"
#include "core/graphicsEngine/shaders/WGEShaderPropertyDefine.h"
#include "core/graphicsEngine/shaders/WGEShaderPropertyDefineOptions.h"
#include "core/graphicsEngine/callbacks/WGEShaderAnimationCallback.h"

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
    m_sphereScaler = m_properties->addProperty( "Sphere Scale", "Scale the spheres.", 1.0 );
    m_planeColor = m_properties->addProperty( "Plane Color", "Color of the plane", WColor( 90, 95, 100, 255 ) / 255.0 );
    m_spheresColor = m_properties->addProperty( "Sphere Color", "Color of the plane", WColor( 230, 43, 48, 255 ) / 255.0 );

    m_aWeight = m_properties->addProperty( "Light Intensity", "The intensity of light on the spheres.", 1.0 );
    m_aWeight->setMin( 0 );
    m_aWeight->setMax( 10 );

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

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 1. Setup some geometry.
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Lets start. We begin by defining some geometry. We use standard OSG functionality to create an awesome
    // visualization of some spheres on a plane:
    // -> create a group node in which we will place our demo geometry.
    osg::ref_ptr< WGEGroupNode > rootNode = new WGEGroupNode();
    // We add this to the global scene. If you are unfamiliar with this, please refer to WMTemplate.cpp
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( rootNode );
    // Show plane directly. By default, the user views along the Y axis -> the plane is on the XY plane -> rotate to make it visible by default:
    rootNode->setMatrix( osg::Matrixd::rotate( 1.57, 1.0, 0.0, 0.0 ) ); // First parameter is the angle in radians.

    // Now we can add your demo geometry:
    osg::ref_ptr< osg::Node > spheres = createSphereGeometry();
    osg::ref_ptr< osg::Node > plane = createPlaneGeometry();

    // Allow blending here? Yes. We need it later.
    plane->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
    plane->getOrCreateStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

    // To see our new geometry we need to add it to the group of ours:
    rootNode->insert( spheres );
    rootNode->insert( plane );

    // OK. Let us summarize the rather uninteresting part: We created a root node (a group) that will contain our demo geometry. We created the
    // geometry and added it to the group.

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 2. Setup shaders
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // TIP: you do not understand this GLSL-shader-OpenGL-geode-node-OSG-stuff? You should probably first learn the basics of OpenGL and its
    // Shading system. A good point to start is http://www.lighthouse3d.com/opengl/glsl/

    // Now it gets really interesting. We load a shader:
    osg::ref_ptr< WGEShader > sphereShader(
        new WGEShader(
                        "WMTemplateShaders-Spheres",    // shader name prefix
                        m_localPath                     // where to search?
                     )
        );

    // This line creates a new shader object and loads the code from some GLSL files. But which files? The first parameter of the WGEShader
    // constructor is a string denoting the name prefix for the shader files. The second parameter determines where to search. Luckily, the
    // WModule::m_localPath variable is automatically set for each module and points to the binary directory of your module. You can use other
    // directories heres, add subdirectories and similar. It is even allowed to skip this path. In this case, the OpenWalnut search path is used.
    // This is practical for loading OpenWalnut's own shaders directly. Never the less, the WGEShader constructor now looks inside this directory
    // for the shader files that start with our prefix string:
    //
    // * WMTemplateShaders-Spheres-vertex.glsl
    // * WMTemplateShaders-Spheres-fragment.glsl
    // * WMTemplateShaders-Spheres-geometry.glsl
    //
    // This makes clear, what the prefix means. WGEShader automatically adds the "-vertex.glsl", "-fragment.glsl", and "-geometry.glsl"
    // extensions. WGEShader now tries to load them. According to the OpenGL standard, not all shaders are needed. Usually, you provide at least
    // a vertex and a fragment shader.
    //
    // NOTE: a small note on shader paths. As mentioned above, when loading shaders, OpenWalnut searches inside a given set of Paths. This is
    // similar to your C++ compiler, which searches certain header files in a given set of paths. Besides the system paths, you specify additional
    // search paths for your local header files. This is conceptually the same in OpenWalnut's WGEShader. There is a system path of OpenWalnut.
    // There, all OpenWalnut shaders reside. When you specify your own path, WGEShader tries to fiend the above filenames in the following order:
    //  1) specified_path/WMTemplateShaders-Spheres-vertex.glsl
    //  2) specified_path/shaders/WMTemplateShaders-Spheres-vertex.glsl
    //  3) WPathHelper::getShaderPath()/WMTemplateShaders-Spheres-vertex.glsl   <-- this is the global shader path
    //
    // NOTE on naming: The above lines tell you where to place your shaders. In OpenWalnut, we usually put them into a subdirectory called "shaders".
    // We additionally name shaders as the module. This makes it easier for developers to identify the shaders of your specific module.
    //
    // After this line, the shader object exists but it has not yet loaded the code, nor is it bound to any part of the graphics pipeline. A
    // shader is always bound to something which draws something ... somehow ... like osg::Geodes. Luckily, we have two geodes we would like to shade:
    sphereShader->apply( spheres );
    // This now applied the shader to our node. There are different ways to apply a shader. What we have shown you here is the more comfortable
    // way OpenWalnut provides. Always use this. It is thread-safe and handles shader-reload properly.

    // Lets load another shader for the plane and apply it:
    osg::ref_ptr< WGEShader > planeShader( new WGEShader( "WMTemplateShaders-Plane", m_localPath ) );
    planeShader->apply( plane );

    // Some side-note: To de-activate a shader again, use
    // planeShader->deactivate( plane );
    // You might ask why you need to specify the node once again. This is because a shader might be applied to multiple nodes at the same time.

    // OK. Let us summarize. We made some geometry and loaded two different shaders. We applied it to the geometry. If we compile and start the
    // module now, OpenWalnut will tell us that it could not load our shaders. Because we did not write them yet. BUT this is not the next step
    // for now. At least in this tutorial :-).

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 2. Setup parameters for shaders
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Now that we have shaders, we want to define some uniforms and learn about compile-time definitions.

    // First start with uniforms. Uniforms can be seen as dynamic input variables that do not change during ONE drawing of ONE osg::Node. The
    // value is globally set for the program, unlike attributes, whose value changes for primitives, vertex and so on. The vertex color is a nice
    // example for attributes.
    // This, of course, is a little bit too simplified, but in most cases, it is exactly that: an input variable of one GLSL program that can be
    // changed per rendered frame.
    //
    // Uniforms can have different types. Floats, ints and so on. OSG already supports these possible types in its osg::Uniform class and  the
    // intended way to define and set an uniform is:
    osg::Uniform* animationTime(
        new osg::Uniform(
            "u_animation",         // a name. We always use "u_someName" to easily identify uniforms via the "u_".
            0                      // initial value, type of this parameter defines the uniform type in GLSL!
        )
    );
    spheres->getOrCreateStateSet()->addUniform( animationTime ); // Add this uniform to the spheres as variable for the GLSL program.
    plane->getOrCreateStateSet()->addUniform( animationTime ); // It is possible to add one uniform to multiple nodes.
    // You now might ask how to modify these uniforms? Well, as we already have added the uniforms to a state set
    // -> whose node is already added to a group -> which already is added to a scene -> which already is visible we cannot set the values
    // directly. This might cause threading problems. As usual in OSG, always use update callbacks.

    // This looks nice but has some disadvantages:
    //  * The OSG doc is not that explicit in telling you how thread-safe it is to change a value from a thread that is not the rendering thread.
    //  * Coupling uniforms with properties can produce a lot of boiler-plate code (register update callback to each OSG node, query property in
    //  there (due to thread-safety), set, ... ).
    //
    // So this is the reason why we introduced WGEPropertyUniform. Properties are so damn similar to uniforms. Why not coupling them:
    spheres->getOrCreateStateSet()->addUniform( // <-- adding a uniform is always done this way
        new WGEPropertyUniform< WPropDouble >(  // <-- define the type of property
            "u_sphereScaler",                   // a name as above.
            m_sphereScaler                      // the property whose value should be used for the uniform.
        )
    );
    // If the property now updates its value, the uniform is updated too. This is can be very comfortable to allow the user several parameters of
    // your shader-based visualization. You can do this for all property types as long as you can somehow cast them to a GLSL type. This means:
    //  * you cannot use WPropString as GLSL does not know strings
    //  * you can use WPropSelection as they get cast to ints for example
    // For details, review core/graphicsEngine/shaders/WGEUniformTypeTraits.h. Internally, OpenWalnut uses template function wge::toUniformType
    // to cast an input property type to an uniform type. This means, you can specialize this template for a specific type if you like.

    // Add a color for the plane.
    plane->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropColor >( "u_planeColor", m_planeColor ) );
    spheres->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropColor >( "u_spheresColor", m_spheresColor ) );

    // Thats it for uniforms. It is trivial and further example will not show any more cool features. Now we should get to shader-compile-time
    // definitions. You all know this from C/C++ ... good old #define. This works in GLSL too and WGEShader provides a nice API for this.

    // Add a define. They are shader specific. First we add a PI constant to the WMTemplateShaders-Spheres that we will use in its vertex shader:
    WGEShaderDefine< double >::SPtr somewhatLikePI = sphereShader->setDefine( "PI", 3.14 );
    // After creating them, you can set them as you like. This works from any thread (in contrast to pure osg::Uniforms)
    somewhatLikePI->setValue( 3.1416 ); // more "exact" PI
    // Thats it. Now you can simply use the value "PI" in the shader. No need for an extra definition or something similar. This works because
    // WGEShader directly inserts the text "#define PI 3.1416" to the shader. But this also means that, when changing the value, the shader
    // re-compiles. So you will have to make a trade-off between compile time definitions and uniforms. You now might ask why it might be useful
    // to define changeable compile-time constants. Assume you have a for-loop in your GLSL code. GLSL (depending on the version) will unroll the
    // loop during compilation, forcing the programmer to specify a number of iterations during compile-time. But you might want to change this
    // depending on your input dataset for example.

    // Besides simple compile-time values, the GLSL preprocessor also supports #ifdef/#else/#endif blocks. This is very handy, when you want to
    // switch between different types of rendering or want to en-/dis-able features without using large if-blocks (btw it is not recommended in
    // GLSL in any way)

    // TODO(ebaum): write

    // Similar to the property-coupled uniforms, we provide property-coupled compile-time definitions. These compile-time definitions are
    // implemented as code-preprocessors. Hence the call:
    sphereShader->addPreprocessor(
        WGEShaderPreprocessor::SPtr(        // create a preprocessor which is coupled to a property
            new WGEShaderPropertyDefine< WPropDouble >(
                "LIGHT_INTENSITY",          // The name of the compile-time definition
                m_aWeight                   // The coupled property
            )
        )
    );

    // This is very handy, as you can, for
    // example, turn on features in your shader (code that is embedded in #ifdef ... #endif blocks).



    // TODO(ebaum): write
    // WGEShaderDefineOptions,  WGEShaderPropertyDefineOptions
    // WGEShaderDefineSwitch::SPtr gradTexEnableDefine = m_shader->setDefine( "GRADIENTTEXTURE_ENABLED" );






    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 4. Textures
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // We need some textures. You could use the default OSG functionality via osg::Texture* but OpenWalnut has extended it to
    // provide you a comfortable interface. WGETexture keeps track of texture sizes, updates shader uniforms automatically and more. It even
    // has properties! This way, you can just bind a texture, and your shader knows about them, their sizes and so on via uniforms.
    // In our demo, we use a 3d white noise texture generated by the WGE utilities:
    const size_t size = 64;
    WGETexture3D::SPtr randTex = wge::genWhiteNoiseTexture( size, size, size, // dimensions
                                                            1                 // number if channels: 1 -> gray scale, 3 -> RGB, 4 -> RGBA
                                                                    ); // quite self-explanatory
    wge::bindTexture(
        spheres,        // Where to bind
        randTex,        // The texture to bind
        0,              // The texture unit
        "u_noise"       // Name-prefix for the texture specific uniforms. Optional. If not specified, it will be "u_textureX" where X is the unit
    );
    // This ensures you will have the following uniforms defined too:
    //  * - u_noiseUnit: the unit number (useful for accessing correct gl_TexCoord and so on)
    //  * - u_noiseSampler: the needed sampler
    //  * - u_noiseSizeX: width of the texture in pixels
    //  * - u_noiseSizeY: height of the texture in pixels
    //  * - u_noiseSizeZ: depth of the texture in pixels
    // Besides these, you will also get the uniforms for the properties in WGETexture::getProperties(). Awesome isn't it? This is especially
    // useful when loading textures from data:
    // boost::shared_ptr< WDataSetVector > gradients = ....;
    // osg::ref_ptr< WDataTexture3D > gradTexture3D = gradients->getTexture();
    // wge::bindTexture( cube, gradTexture3D, 2, "u_gradients" );
    //
    // Some of the property uniforms will then be:
    /*
    //!< For un-scaling the data: the minimum.
    uniform float u_gradientsMin = 0;

    //!< For un-scaling the data: the scaling factor.
    uniform float u_gradientsScale = 1;

    //!< The alpha value for this colormap. Is in [0,1].
    uniform float u_gradientsAlpha;

    //!< Flag denoting whether to clip the zero value.
    uniform bool u_gradientsClipZeroEnabled = false;

    //!< The threshold value for this colormap. Is in [Min,Scale+Min]
    uniform float u_gradientsThresholdLower;

    //!< The threshold value for this colormap. Is in [Min,Scale+Min]
    uniform float u_gradientsThresholdUpper;

    //!< Flag denoting whether to use the threshold value for clipping or not.
    uniform bool u_gradientsThresholdEnabled = false;

    //!< Flag denoting whether to use the windowing values for scaling or not.
    uniform bool u_gradientsWindowEnabled = false;

    //!< Contains the lower and upper window level.
    uniform vec2 u_gradientsWindow = vec2( 0.0, 1.0 );

    //!< The index of the colormap to use
    uniform int u_gradientsColormap;

    //!< True if the colormap is active.
    uniform bool u_gradientsActive = false;
    */

    // We can also utilize WGEImage and load textures from a file, whose path is stored in our META file. Refer to WMTemplate and WMTemplateUI
    // for more details about this.
    WGEImage::SPtr planeImage = WGEImage::createFromFile( m_localPath / getMetaInformation()->query< std::string >( "common/amazingTexture" ) );
    // Create a texture out of this:
    WGETexture2D::SPtr planeTexture( new WGETexture2D( planeImage ) );
    wge::bindTexture(
        plane,          // Where to bind
        planeTexture,   // The texture to bind
        0,              // The texture unit
        "u_texture"     // Name-prefix for the texture specific uniforms. Optional. If not specified, it will be "u_textureX" where X is the unit
    );

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 5. Utilities and helpers
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // We want some nice animations. We already have an uniform which contains the animation time. But who increases it?
    // OpenWalnut provides several useful callbacks for several purpose. Have a look at core/graphicsEngine/caollbacks for more.
    // Here, we use a animation callback:;
    animationTime->setUpdateCallback( new WGEShaderAnimationCallback() );

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 6. Write the shaders
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Now we can write the shader code. Please open the "WMTemplateShaders-Spheres-*" files in your shaders subdirectory (sub of the directory
    // THIS file resides in). Start reading WMTemplateShaders-Spheres-vertex.glsl.

    // TIP: As a shader developer, you most probably want to re-compile and test your shader very often. OpenWalnut supports you in this. All
    // loaded shaders can be reloaded using the dot-key. Simply focus your 3D view and press the dot-key. This reloads the bound shaders. Yours
    // too.
    //
    // TIP: In addition, your CMake Build-setup allows you to set the option "OW_LINK_SHADERS". On systems supporting soft-links, this ensures
    // that your shader is not copied to the build directory but linked. So you can change the code in your source directory and reload it
    // directly while OpenWalnut is running.

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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ATTENTION: now it gets interesting ...
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

osg::ref_ptr< osg::Node > WMTemplateShaders::createSphereGeometry()
{
    // Create some spheres using the OSG functionality.  To understand the following code, we would like to refer you to the OpenSceneGraph
    // documentation.

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
    // Create a nice plane. We use a pre-defined function in OpenWalnut's graphics engine:
    return wge::genFinitePlane( osg::Vec3( 0.0, 0.0, 0.0 ),   // base
                                osg::Vec3( 100.0, 0.0, 0.0 ), // spanning vector a
                                osg::Vec3( 0.0, 100.0, 0.0 ), // spanning vector b
                                WColor( 0.5, 0.5, 0.5, 1.0 )  // a color.
           );
}
