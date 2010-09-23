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

#include <osg/Vec3>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Drawable>

#include "../../kernel/WKernel.h"
#include "../../common/WPropertyHelper.h"
#include "../../common/math/WMath.h"
#include "../../common/math/WPlane.h"
#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../../dataHandler/WDataTexture3D.h"
#include "../../graphicsEngine/callbacks/WGELinearTranslationCallback.h"
#include "../../graphicsEngine/WGEGeodeUtils.h"
#include "../../graphicsEngine/WShader.h"
#include "../../graphicsEngine/WGEOffscreenRenderPass.h"
#include "../../graphicsEngine/WGEOffscreenRenderNode.h"

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
    m_meshIn = WModuleInputData< WTriangleMesh2 >::createAndAdd( shared_from_this(), "surface", "The optional surface to use." );

    // call WModule's initialization
    WModule::connectors();
}

void WMImageSpaceLIC::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    // enable slices
    // Flags denoting whether the glyphs should be shown on the specific slice
    m_showonX        = m_properties->addProperty( "Show sagittal", "Show vectors on sagittal slice.", true );
    m_showonY        = m_properties->addProperty( "Show coronal", "Show vectors on coronal slice.", true );
    m_showonZ        = m_properties->addProperty( "Show axial", "Show vectors on axial slice.", true );

    // The slice positions. These get update externally.
    // TODO(all): this should somehow be connected to the nav slices.
    m_xPos           = m_properties->addProperty( "Sagittal position", "Slice X position.", 80 );
    m_yPos           = m_properties->addProperty( "Coronal position", "Slice Y position.", 100 );
    m_zPos           = m_properties->addProperty( "Axial position", "Slice Z position.", 80 );
    m_xPos->setMin( 0 );
    m_xPos->setMax( 159 );
    m_yPos->setMin( 0 );
    m_yPos->setMax( 199 );
    m_zPos->setMin( 0 );
    m_zPos->setMax( 159 );

    // call WModule's initialization
    WModule::properties();
}

void WMImageSpaceLIC::initOSG( boost::shared_ptr< WGridRegular3D > grid )
{
    // remove the old slices
    m_output->remove( m_xSlice );
    m_output->remove( m_ySlice );
    m_output->remove( m_zSlice );

    // we want the tex matrix for each slice to be modified too,
    osg::ref_ptr< osg::TexMat > texMat;

    // create all the transformation nodes
    m_xSlice = osg::ref_ptr< WGEManagedGroupNode > ( new WGEManagedGroupNode( m_showonX ) );
    m_ySlice = osg::ref_ptr< WGEManagedGroupNode > ( new WGEManagedGroupNode( m_showonY ) );
    m_zSlice = osg::ref_ptr< WGEManagedGroupNode > ( new WGEManagedGroupNode( m_showonZ ) );

    texMat = new osg::TexMat();
    m_xSlice->addUpdateCallback( new WGELinearTranslationCallback< WPropInt >( osg::Vec3( 1.0, 0.0, 0.0 ), m_xPos, texMat ) );
    m_xSlice->getOrCreateStateSet()->setTextureAttributeAndModes( 0, texMat, osg::StateAttribute::ON );

    texMat = new osg::TexMat();
    m_ySlice->addUpdateCallback( new WGELinearTranslationCallback< WPropInt >( osg::Vec3( 0.0, 1.0, 0.0 ), m_yPos, texMat ) );
    m_ySlice->getOrCreateStateSet()->setTextureAttributeAndModes( 0, texMat, osg::StateAttribute::ON );

    texMat = new osg::TexMat();
    m_zSlice->addUpdateCallback( new WGELinearTranslationCallback< WPropInt >( osg::Vec3( 0.0, 0.0, 1.0 ), m_zPos, texMat ) );
    m_zSlice->getOrCreateStateSet()->setTextureAttributeAndModes( 0, texMat, osg::StateAttribute::ON );

    // create a new geode containing the slices
    m_xSlice->addChild( wge::genFinitePlane( grid->getOrigin(), grid->getNbCoordsY() * grid->getDirectionY(),
                                                                grid->getNbCoordsZ() * grid->getDirectionZ() ) );

    m_ySlice->addChild( wge::genFinitePlane( grid->getOrigin(), grid->getNbCoordsX() * grid->getDirectionX(),
                                                                grid->getNbCoordsZ() * grid->getDirectionZ() ) );

    m_zSlice->addChild( wge::genFinitePlane( grid->getOrigin(), grid->getNbCoordsX() * grid->getDirectionX(),
                                                                grid->getNbCoordsY() * grid->getDirectionY() ) );

    // add the transformation nodes to the output group
    m_output->insert( m_xSlice );
    m_output->insert( m_ySlice );
    m_output->insert( m_zSlice );
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
    std::srand( time( 0 ) );

    osg::ref_ptr< osg::Image > randImage = new osg::Image();
    randImage->allocateImage( resX, resX, 1, GL_LUMINANCE, GL_UNSIGNED_BYTE );
    unsigned char *randomLuminance = randImage->data();  // should be 4 megs
    for( unsigned int x = 0; x < resX; x++ )
    {
        for( unsigned int y = 0; y < resX; y++ )
        {
            unsigned char r = ( unsigned char )( std::rand() % 255 );
            randomLuminance[ ( y * resX ) + x ] = r > 150 ? 255 : ( r < 100 ? 0 : r );       // NOLINT
            // - stylechecker says "use rand_r" but I am not sure about portability.
        }
    }

    // finally, create a texture from the image
    osg::ref_ptr< osg::Texture2D > randTexture = new osg::Texture2D();
    randTexture->setFilter( osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR );
    randTexture->setFilter( osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR );
    randTexture->setImage( randImage );
    randTexture->setResizeNonPowerOfTwoHint( false );

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
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( offscreen );

    // setup all the passes needed for image space advection
    osg::ref_ptr< WGEOffscreenRenderPass > transformation = offscreen->addGeometryRenderPass(
        m_output,
        new WShader( "WMImageSpaceLIC-Transformation", m_localPath ),
        "Transformation"
    );
    osg::ref_ptr< WGEOffscreenRenderPass > edgeDetection =  offscreen->addTextureProcessingPass(
        new WShader( "WMImageSpaceLIC-Edge", m_localPath ),
        "Edge Detection"
    );

    // we use two advection passes per frame as the input A of the first produces the output B whereas the second pass uses B as input and
    // produces A as output. This way we can use A as input for the next step (clipping and blending).
    osg::ref_ptr< WGEOffscreenRenderPass > advectionAB =  offscreen->addTextureProcessingPass(
        new WShader( "WMImageSpaceLIC-Advection", m_localPath ),
        "Advection AB"
    );
    osg::ref_ptr< WGEOffscreenRenderPass > advectionBA =  offscreen->addTextureProcessingPass(
        new WShader( "WMImageSpaceLIC-Advection", m_localPath ),
        "Advection BA"
    );

    // finally, put it back on screen, clip it, color it and apply depth buffer to on-screen buffer
    osg::ref_ptr< WGEOffscreenRenderPass > clipBlend =  offscreen->addFinalOnScreenPass(
        new WShader( "WMImageSpaceLIC-ClipBlend", m_localPath ),
        "Clip & Blend"
    );

    // hardwire the textures to use for each pass:

    // Transformation Pass, needs Geometry
    //  * Creates 2D projected Vectors in RG
    //  * Lighting in B
    //  * Depth
    osg::ref_ptr< osg::Texture2D > transformationOut1  = transformation->attach( osg::Camera::COLOR_BUFFER0 );
    osg::ref_ptr< osg::Texture2D > transformationDepth = transformation->attach( osg::Camera::DEPTH_BUFFER );

    // Edge Detection Pass, needs Depth as input
    //  * Edges in R
    //  * Depth in G
    //  * Un-advected Noise in B
    osg::ref_ptr< osg::Texture2D > edgeDetectionOut1 = edgeDetection->attach( osg::Camera::COLOR_BUFFER0 );
    edgeDetection->bind( transformationDepth, 0 );
    edgeDetection->bind( randTexture,         1 );

    // Advection Pass, needs edges and projected vectors as well as noise texture
    //  * Advected noise in luminance channel
    osg::ref_ptr< osg::Texture2D > advectionOutA  = advectionAB->attach( osg::Camera::COLOR_BUFFER0, GL_LUMINANCE );
    advectionAB->bind( transformationOut1, 0 );
    advectionAB->bind( edgeDetectionOut1,  1 );
    osg::ref_ptr< osg::Texture2D > advectionOutB  = advectionBA->attach( osg::Camera::COLOR_BUFFER0, GL_LUMINANCE );
    advectionBA->bind( transformationOut1, 0 );
    advectionBA->bind( edgeDetectionOut1,  1 );
    advectionBA->bind( advectionOutA,      2 );
    advectionAB->bind( advectionOutB,      2 );

    // Final clipping and blending phase, needs Advected Noise, Edges, Depth and Light
    clipBlend->bind( advectionOutB, 0 );
    clipBlend->bind( edgeDetectionOut1, 1 );

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Main loop
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    // main loop
    while ( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish?
        if ( m_shutdownFlag() )
        {
            break;
        }

        // To query whether an input was updated, simply ask the input:
        bool dataUpdated = m_vectorsIn->handledUpdate();
        boost::shared_ptr< WDataSetVector > dataSet = m_vectorsIn->getData();
        bool dataValid = ( dataSet );
        if ( !dataValid || ( dataValid && !dataUpdated ) )
        {
            continue;
        }

        // get grid and prepare OSG
        boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( dataSet->getGrid() );
        m_xPos->setMax( grid->getNbCoordsX() - 1 );
        m_yPos->setMax( grid->getNbCoordsY() - 1 );
        m_zPos->setMax( grid->getNbCoordsZ() - 1 );
        initOSG( grid );

        // prepare offscreen render chain
        transformation->bind( dataSet->getTexture()->getTexture() );

        debugLog() << "Done";
    }

    // clean up
    offscreen->clear();
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( offscreen );
}

