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

#include <cmath>
#include <cstdlib>
#include <vector>
#include <string>

#include <osg/BoundingSphere>
#include <osg/Vec3>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Drawable>

#include "core/common/WPropertyHelper.h"
#include "core/common/math/WMath.h"
#include "core/dataHandler/WDataHandler.h"
#include "core/dataHandler/WDataTexture3D.h"
#include "core/dataHandler/WGridRegular3D.h"
#include "core/graphicsEngine/WGEColormapping.h"
#include "core/graphicsEngine/WGEGeodeUtils.h"
#include "core/graphicsEngine/WGETextureUtils.h"
#include "core/graphicsEngine/callbacks/WGELinearTranslationCallback.h"
#include "core/graphicsEngine/callbacks/WGENodeMaskCallback.h"
#include "core/graphicsEngine/offscreen/WGEOffscreenRenderNode.h"
#include "core/graphicsEngine/offscreen/WGEOffscreenRenderPass.h"
#include "core/graphicsEngine/shaders/WGEPropertyUniform.h"
#include "core/graphicsEngine/shaders/WGEShader.h"
#include "core/graphicsEngine/shaders/WGEShaderDefineOptions.h"
#include "core/graphicsEngine/shaders/WGEShaderPropertyDefineOptions.h"
#include "core/kernel/WKernel.h"

#include "WMImageSpaceLIC.h"
#include "WMImageSpaceLIC.xpm"

// This line is needed by the module loader to actually find your module. You need to add this to your module too. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMImageSpaceLIC )

WMImageSpaceLIC::WMImageSpaceLIC():
    WModule()
{
}

WMImageSpaceLIC::~WMImageSpaceLIC()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMImageSpaceLIC::factory() const
{
    return boost::shared_ptr< WModule >( new WMImageSpaceLIC() );
}

const char** WMImageSpaceLIC::getXPMIcon() const
{
    return WMImageSpaceLIC_xpm;
}

const std::string WMImageSpaceLIC::getName() const
{
    return "Image Space LIC";
}

const std::string WMImageSpaceLIC::getDescription() const
{
    return "This module takes an vector dataset and creates an LIC-like texture on an arbitrary surface. You can specify the surface as input or"
           "leave it unspecified to use slices.";
}

void WMImageSpaceLIC::connectors()
{
    // vector input
    m_vectorsIn = WModuleInputData< WDataSetVector >::createAndAdd( shared_from_this(), "vectors", "The vector dataset."
                                                                                                    "Needs to be in the same grid as the mesh." );
    // scalar input
    m_scalarIn = WModuleInputData< WDataSetScalar >::createAndAdd( shared_from_this(), "scalars", "The scalar dataset."
                                                                                                    "Needs to be in the same grid as the mesh." );

    // mesh input
    m_meshIn = WModuleInputData< WTriangleMesh >::createAndAdd( shared_from_this(), "surface", "The optional surface to use." );

    // call WModule's initialization
    WModule::connectors();
}

void WMImageSpaceLIC::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_geometryGroup   = m_properties->addPropertyGroup( "Geometry",  "Selection of used geometry to apply LIC to." );

    m_useSlices       = m_geometryGroup->addProperty( "Use Slices", "Show vectors on slices.", true, m_propCondition );

    m_sliceGroup      = m_geometryGroup->addPropertyGroup( "Slices",  "Slice based LIC." );

    // enable slices
    // Flags denoting whether the glyphs should be shown on the specific slice
    m_showonX        = m_sliceGroup->addProperty( "Show Sagittal", "Show vectors on sagittal slice.", true );
    m_showonY        = m_sliceGroup->addProperty( "Show Coronal", "Show vectors on coronal slice.", true );
    m_showonZ        = m_sliceGroup->addProperty( "Show Axial", "Show vectors on axial slice.", true );

    // The slice positions. These get update externally.
    // TODO(all): this should somehow be connected to the nav slices.
    m_xPos           = m_sliceGroup->addProperty( "Sagittal Position", "Slice X position.", 80 );
    m_yPos           = m_sliceGroup->addProperty( "Coronal Position", "Slice Y position.", 100 );
    m_zPos           = m_sliceGroup->addProperty( "Axial Position", "Slice Z position.", 80 );
    m_xPos->setMin( 0 );
    m_xPos->setMax( 159 );
    m_yPos->setMin( 0 );
    m_yPos->setMax( 199 );
    m_zPos->setMin( 0 );
    m_zPos->setMax( 159 );

    m_licGroup      = m_properties->addPropertyGroup( "LIC",  "LIC properties." );

    m_useLight       = m_licGroup->addProperty( "Use Light", "Check to enable lightning using the Phong model.", false );
    m_lightIntensity = m_licGroup->addProperty( "Light Intensity", "Define how intense the light should be.", 1.0 );
    m_lightIntensity->setMin( 0.0 );
    m_lightIntensity->setMax( 10.0 );

    m_useEdges        = m_licGroup->addProperty( "Edges", "Check to enable blending in edges.", true );
    m_useEdgesColor   = m_licGroup->addProperty( "Edge Color", "Define the color of the edges.", defaultColor::WHITE );
    m_useEdgesStep    = m_licGroup->addProperty( "Edge Step", "Define the steepness of the blend function between color and edge color.", 1.0 );
    m_useEdgesStep->setMin( 0.0 );
    m_useEdgesStep->setMax( 10.0 );

    m_useHighContrast = m_licGroup->addProperty( "High Contrast", "Use an extremely increased contrast.", false );
    m_useDepthCueing  = m_licGroup->addProperty( "Depth Cueing", "Use depth as additional cue? Mostly useful for isosurfaces.",
                                                false );

    m_cmapRatio    = m_licGroup->addProperty( "Ratio Colormap to LIC", "Blending ratio between LIC and colormap.", 0.5 );
    m_cmapRatio->setMin( 0.0 );
    m_cmapRatio->setMax( 1.0 );

    m_advancedLicGroup      = m_properties->addPropertyGroup( "Advanced",  "More advanced LIC properties." );
    // show hud?
    m_showHUD        = m_advancedLicGroup->addProperty( "Show HUD", "Check to enable the debugging texture HUD.", false );
    m_3dNoise        = m_advancedLicGroup->addProperty( "Use 3D noise", "Use 3D noise? This provides better coherence during transformation of "
                                                        "the geometry but might introduce resolution problems.", true );
    m_3dNoiseRes     = m_advancedLicGroup->addProperty( "3D Noise Resolution", "The 3D noise is of 128^3 pixels size. This scaler allows "
                                                        "modification of this size.", 1.5 );
    m_3dNoiseRes->setMin( 0 );
    m_3dNoiseRes->setMax( 10 );

    m_3dNoiseAutoRes = m_advancedLicGroup->addProperty( "3D Noise Auto-Resolution", "If checked, the resolution of the 3D noise gets calculated "
                                                       "automatically according to the screen size. If disabled, the user can zoom into the LIC.",
                                                       true );

    m_numIters     = m_advancedLicGroup->addProperty( "Number of Iterations", "How much iterations along a streamline should be done per frame.",
                                                      30 );
    m_numIters->setMin( 1 );
    m_numIters->setMax( 1000 );

    m_projectionAngleThreshold = m_advancedLicGroup->addProperty( "Projection Angle Threshold", "This defines the threshold of the angle between "
            "tangential plane of the surface and the vector which is going to be projected. You can adjust how steep a vector can be before it is "
            "clipped and NOT projected. Note: all vectors with an angle below this threshold are projected but linearly reduced in influence "
            "depending on the angle.", 90.0 );
    m_projectionAngleThreshold->setMin( 0.0 );
    m_projectionAngleThreshold->setMax( 90.0 );


    // call WModule's initialization
    WModule::properties();
}

void WMImageSpaceLIC::initOSG( boost::shared_ptr< WGridRegular3D > grid, boost::shared_ptr< WTriangleMesh > mesh )
{
    // remove the old slices
    m_output->clear();

    if( mesh && !m_useSlices->get( true ) )
    {
        // we have a mesh and want to use it
        // create geometry and geode
        osg::Geometry* surfaceGeometry = new osg::Geometry();
        osg::ref_ptr< osg::Geode > surfaceGeode = osg::ref_ptr< osg::Geode >( new osg::Geode );

        surfaceGeometry->setVertexArray( mesh->getVertexArray() );
        osg::DrawElementsUInt* surfaceElement;
        surfaceElement = new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );
        std::vector< size_t > tris = mesh->getTriangles();
        surfaceElement->reserve( tris.size() );
        for( unsigned int vertId = 0; vertId < tris.size(); ++vertId )
        {
            surfaceElement->push_back( tris[vertId] );
        }
        surfaceGeometry->addPrimitiveSet( surfaceElement );

        // normals
        surfaceGeometry->setNormalArray( mesh->getVertexNormalArray() );
        surfaceGeometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );

        // texture coordinates
        surfaceGeometry->setTexCoordArray( 0, mesh->getTextureCoordinateArray() );

        // render
        surfaceGeode->addDrawable( surfaceGeometry );
        m_output->insert( surfaceGeode );
    }
    else if( !mesh && !m_useSlices->get( true ) )
    {
        warnLog() << "No surface connected to input but surface render mode enabled. Nothing rendered.";
    }
    else
    {
        // create a new geode containing the slices
        osg::ref_ptr< osg::Node > xSlice = wge::genFinitePlane( grid->getOrigin(), grid->getNbCoordsY() * grid->getDirectionY(),
                                                                                   grid->getNbCoordsZ() * grid->getDirectionZ() );

        osg::ref_ptr< osg::Node > ySlice = wge::genFinitePlane( grid->getOrigin(), grid->getNbCoordsX() * grid->getDirectionX(),
                                                                                   grid->getNbCoordsZ() * grid->getDirectionZ() );

        osg::ref_ptr< osg::Node > zSlice =  wge::genFinitePlane( grid->getOrigin(), grid->getNbCoordsX() * grid->getDirectionX(),
                                                                                    grid->getNbCoordsY() * grid->getDirectionY() );

        // The movement of the slice is done in the shader. An alternative would be WGELinearTranslationCallback but there, the needed matrix is
        // not available in the shader
        osg::StateSet* ss = xSlice->getOrCreateStateSet();
        ss->addUniform( new WGEPropertyUniform< WPropInt >( "u_vertexShift", m_xPos ) );
        ss->addUniform( new osg::Uniform( "u_vertexShiftDirection", grid->getDirectionX().as< osg::Vec3f >() ) );  // the axis to move along
        ss = ySlice->getOrCreateStateSet();
        ss->addUniform( new WGEPropertyUniform< WPropInt >( "u_vertexShift", m_yPos ) );
        ss->addUniform( new osg::Uniform( "u_vertexShiftDirection", grid->getDirectionY().as< osg::Vec3f >() ) );  // the axis to move along
        ss = zSlice->getOrCreateStateSet();
        ss->addUniform( new WGEPropertyUniform< WPropInt >( "u_vertexShift", m_zPos ) );
        ss->addUniform( new osg::Uniform( "u_vertexShiftDirection", grid->getDirectionZ().as< osg::Vec3f >() ) );  // the axis to move along

        // set callbacks for en-/disabling the nodes
        xSlice->addUpdateCallback( new WGENodeMaskCallback( m_showonX ) );
        ySlice->addUpdateCallback( new WGENodeMaskCallback( m_showonY ) );
        zSlice->addUpdateCallback( new WGENodeMaskCallback( m_showonZ ) );

        // disable culling.
        xSlice->setCullingActive( false );
        ySlice->setCullingActive( false );
        zSlice->setCullingActive( false );

        // add the transformation nodes to the output group
        m_output->insert( xSlice );
        m_output->insert( ySlice );
        m_output->insert( zSlice );
        m_output->dirtyBound();
    }
}

void WMImageSpaceLIC::moduleMain()
{
    // get notified about data changes
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_vectorsIn->getDataChangedCondition() );
    m_moduleState.add( m_scalarIn->getDataChangedCondition() );
    m_moduleState.add( m_meshIn->getDataChangedCondition() );
    // Remember the condition provided to some properties in properties()? The condition can now be used with this condition set.
    m_moduleState.add( m_propCondition );

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Preparation 1: create noise texture
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    // we need a noise texture with a sufficient resolution. Create it.
    const size_t resX = 1024;

    // finally, create a texture from the image
    osg::ref_ptr< osg::Texture2D > randTexture = wge::genWhiteNoiseTexture( resX, resX, 1 );

    // create a 3D texture too. This allows transformation-invariant noise but is prone to flickering artifacts due to down/upscaling
    osg::ref_ptr< osg::Texture3D > rand3DTexture = wge::genWhiteNoiseTexture( 128, 128, 128, 1 );
    rand3DTexture->setWrap( osg::Texture::WRAP_S, osg::Texture::REPEAT );
    rand3DTexture->setWrap( osg::Texture::WRAP_T, osg::Texture::REPEAT );
    rand3DTexture->setWrap( osg::Texture::WRAP_R, osg::Texture::REPEAT );
    WGEShaderPreprocessor::SPtr define3dNoise( new WGEShaderPropertyDefineOptions< WPropBool >( m_3dNoise, "NOISE3D_DISABLED", "NOISE3D_ENABLED" ) );

    // done.
    ready();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Preparation 2: initialize offscreen renderer and hardwire it
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    // create the root node for all the geometry
    m_output = osg::ref_ptr< WGEManagedGroupNode > ( new WGEManagedGroupNode( m_active ) );

    // the WGEOffscreenRenderNode manages each of the render-passes for us
    osg::ref_ptr< WGEOffscreenRenderNode > offscreen = new WGEOffscreenRenderNode(
        WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getCamera()
    );

    // allow en-/disabling the HUD:
    offscreen->getTextureHUD()->addUpdateCallback( new WGENodeMaskCallback( m_showHUD ) );

    // setup all the passes needed for image space advection
    osg::ref_ptr< WGEShader > transformationShader = new WGEShader( "WMImageSpaceLIC-Transformation", m_localPath );
    WGEShaderDefineOptions::SPtr availableDataDefines = WGEShaderDefineOptions::SPtr( new WGEShaderDefineOptions( "SCALARDATA", "VECTORDATA" ) );
    transformationShader->addPreprocessor( availableDataDefines );
    transformationShader->addPreprocessor( define3dNoise );
    transformationShader->addPreprocessor( WGEShaderPreprocessor::SPtr(
        new WGEShaderPropertyDefineOptions< WPropBool >( m_3dNoiseAutoRes, "NOISE3DAUTORES_DISABLED", "NOISE3DAUTORES_ENABLED" )
    ) );

    osg::ref_ptr< WGEOffscreenRenderPass > transformation = offscreen->addGeometryRenderPass(
        m_output,
        transformationShader,
        "Transformation"
    );
    transformation->bind( rand3DTexture, 1 );
    // apply colormapping to transformation
    WGEColormapping::apply( transformation, transformationShader, 2 );

    osg::ref_ptr< WGEShader > edgeShader = new WGEShader( "WMImageSpaceLIC-Edge", m_localPath );
    osg::ref_ptr< WGEOffscreenRenderPass > edgeDetection =  offscreen->addTextureProcessingPass(
        edgeShader,
        "Edge Detection"
    );
    edgeShader->addPreprocessor( define3dNoise );

    // we use two advection passes per frame as the input A of the first produces the output B whereas the second pass uses B as input and
    // produces A as output. This way we can use A as input for the next step (clipping and blending).
    osg::ref_ptr< WGEOffscreenRenderPass > advection =  offscreen->addTextureProcessingPass(
        new WGEShader( "WMImageSpaceLIC-Advection", m_localPath ),
        "Advection"
    );

    // finally, put it back on screen, clip it, color it and apply depth buffer to on-screen buffer
    osg::ref_ptr< WGEOffscreenRenderPass > clipBlend =  offscreen->addFinalOnScreenPass(
        new WGEShader( "WMImageSpaceLIC-ClipBlend", m_localPath ),
        "Clip & Blend"
    );

    // hardwire the textures to use for each pass:

    // Transformation Pass, needs Geometry
    //  * Creates 2D projected Vectors in RG
    //  * Lighting in B
    //  * Depth
    osg::ref_ptr< osg::Texture2D > transformationOut1  = transformation->attach( osg::Camera::COLOR_BUFFER0 );
    osg::ref_ptr< osg::Texture2D > transformationColormapped  = transformation->attach( osg::Camera::COLOR_BUFFER1 );
    osg::ref_ptr< osg::Texture2D > transformationDepth = transformation->attach( osg::Camera::DEPTH_BUFFER );
    // and some uniforms
    transformation->addUniform( new WGEPropertyUniform< WPropDouble >( "u_noise3DResoultuion", m_3dNoiseRes ) );
    transformation->addUniform( new WGEPropertyUniform< WPropDouble >( "u_projectionAngleThreshold", m_projectionAngleThreshold ) );

    // Edge Detection Pass, needs Depth as input
    //  * Edges in R
    //  * Depth in G
    //  * Un-advected Noise in B
    osg::ref_ptr< osg::Texture2D > edgeDetectionOut1 = edgeDetection->attach( osg::Camera::COLOR_BUFFER0 );
    edgeDetection->bind( transformationDepth, 0 );
    edgeDetection->bind( randTexture,         1 );
    edgeDetection->bind( transformationOut1,  2 );

    // Advection Pass, needs edges and projected vectors as well as noise texture
    //  * Advected noise in luminance channel
    osg::ref_ptr< osg::Texture2D > advectionOutA  = advection->attach( osg::Camera::COLOR_BUFFER0, GL_LUMINANCE );
    advection->bind( transformationOut1, 0 );
    advection->bind( edgeDetectionOut1,  1 );

    // advection needs some uniforms controlled by properties
    osg::ref_ptr< osg::Uniform > numIters = new WGEPropertyUniform< WPropInt >( "u_numIter", m_numIters );
    advection->addUniform( numIters );

    // provide the Gbuffer input, with several mipmap levels
    advectionOutA->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR );
    edgeDetectionOut1->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR );
    transformationColormapped->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR );

    // Final clipping and blending phase, needs Advected Noise, Edges, Depth and Light
    clipBlend->bind( advectionOutA, 0 );
    clipBlend->bind( edgeDetectionOut1, 1 );
    clipBlend->bind( transformationColormapped, 2 );

    // final pass needs some uniforms controlled by properties
    clipBlend->addUniform( new WGEPropertyUniform< WPropBool >( "u_useEdges", m_useEdges ) );
    clipBlend->addUniform( new WGEPropertyUniform< WPropColor >( "u_useEdgesColor", m_useEdgesColor ) );
    clipBlend->addUniform( new WGEPropertyUniform< WPropDouble >( "u_useEdgesStep", m_useEdgesStep ) );
    clipBlend->addUniform( new WGEPropertyUniform< WPropBool >( "u_useLight", m_useLight ) );
    clipBlend->addUniform( new WGEPropertyUniform< WPropDouble >( "u_lightIntensity", m_lightIntensity ) );
    clipBlend->addUniform( new WGEPropertyUniform< WPropBool >( "u_useDepthCueing", m_useDepthCueing ) );
    clipBlend->addUniform( new WGEPropertyUniform< WPropBool >( "u_useHighContrast", m_useHighContrast ) );
    clipBlend->addUniform( new WGEPropertyUniform< WPropDouble >( "u_cmapRatio", m_cmapRatio ) );

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Main loop
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    // main loop
    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish?
        if( m_shutdownFlag() )
        {
            break;
        }

        // To query whether an input was updated, simply ask the input:
        bool dataUpdated = m_vectorsIn->handledUpdate() || m_scalarIn->handledUpdate() || m_meshIn->handledUpdate();
        bool propertyUpdated = m_useSlices->changed();
        boost::shared_ptr< WDataSetVector > dataSetVec = m_vectorsIn->getData();
        boost::shared_ptr< WDataSetScalar > dataSetScal = m_scalarIn->getData();
        boost::shared_ptr< WTriangleMesh > mesh = m_meshIn->getData();

        bool dataValid = ( dataSetVec || dataSetScal );

        // is data valid? If not, remove graphics
        if( !dataValid )
        {
            debugLog() << "Resetting.";
            WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( offscreen );
            continue;
        }

        // something interesting for us?
        if( dataValid && !dataUpdated && !propertyUpdated )
        {
            continue;
        }

        // ensure it gets added
        WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( offscreen );
        WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( offscreen );

        // prefer vector dataset if existing
        if( dataSetVec )
        {
            debugLog() << "Using vector data";

            // get grid and prepare OSG
            boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( dataSetVec->getGrid() );
            m_xPos->setMax( grid->getNbCoordsX() - 1 );
            m_yPos->setMax( grid->getNbCoordsY() - 1 );
            m_zPos->setMax( grid->getNbCoordsZ() - 1 );
            initOSG( grid, mesh );

            // prepare offscreen render chain
            availableDataDefines->activateOption( 1 );  // vector input
            transformation->bind( dataSetVec->getTexture(), 0 );
        }
        else if( dataSetScal )
        {
            debugLog() << "Using scalar data";

            // get grid and prepare OSG
            boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( dataSetScal->getGrid() );
            m_xPos->setMax( grid->getNbCoordsX() - 1 );
            m_yPos->setMax( grid->getNbCoordsY() - 1 );
            m_zPos->setMax( grid->getNbCoordsZ() - 1 );
            initOSG( grid, mesh );

            // prepare offscreen render chain
            availableDataDefines->activateOption( 0 );  // scalar input
            transformation->bind( dataSetScal->getTexture(), 0 );
        }

        debugLog() << "Done";
    }

    // clean up
    offscreen->clear();
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( offscreen );
}

