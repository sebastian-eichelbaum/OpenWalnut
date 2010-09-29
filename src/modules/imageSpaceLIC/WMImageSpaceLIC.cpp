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
#include "../../graphicsEngine/callbacks/WGENodeMaskCallback.h"
#include "../../graphicsEngine/callbacks/WGEShaderAnimationCallback.h"
#include "../../graphicsEngine/WGEGeodeUtils.h"
#include "../../graphicsEngine/WShader.h"
#include "../../graphicsEngine/WGEOffscreenRenderPass.h"
#include "../../graphicsEngine/WGEOffscreenRenderNode.h"
#include "../../graphicsEngine/WGEPropertyUniform.h"

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

    // show hud?
    m_showHUD        = m_licGroup->addProperty( "Show HUD", "Check to enable the debugging texture HUD.", false );

    m_useLight       = m_licGroup->addProperty( "Use Light", "Check to enable lightning using the Phong model.", false );

    m_useEdges       = m_licGroup->addProperty( "Edges", "Check to enable blending in edges.", true );

    m_numIters     = m_licGroup->addProperty( "Number of Iterations", "How much iterations along a streamline should be done per frame.", 30 );
    m_numIters->setMin( 1 );
    m_numIters->setMax( 100 );

    m_cmapRatio    = m_licGroup->addProperty( "Ratio Colormap to LIC", "Blending ratio between LIC and colormap.", 0.5 );
    m_cmapRatio->setMin( 0.0 );
    m_cmapRatio->setMax( 1.0 );

    // call WModule's initialization
    WModule::properties();
}

void WMImageSpaceLIC::initOSG( boost::shared_ptr< WGridRegular3D > grid, boost::shared_ptr< WTriangleMesh2 > mesh )
{
    // remove the old slices
    m_output->clear();

    if ( mesh && !m_useSlices->get( true ) )
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
        surfaceGeometry->setNormalArray( mesh->getVertexNormalArray() );
        surfaceGeometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );

        // render
        surfaceGeode->addDrawable( surfaceGeometry );
        m_output->insert( surfaceGeode );
    }
    else
    {
        m_useSlices->set( true );   // there is no mesh -> use slices

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
}

osg::ref_ptr< osg::Image > genWhiteNoise( size_t resX )
{
    std::srand( time( 0 ) );

    osg::ref_ptr< osg::Image > randImage = new osg::Image();
    randImage->allocateImage( resX, resX, 1, GL_LUMINANCE, GL_UNSIGNED_BYTE );
    unsigned char *randomLuminance = randImage->data();  // should be 4 megs
    for( unsigned int x = 0; x < resX; x++ )
    {
        for( unsigned int y = 0; y < resX; y++ )
        {
            // - stylechecker says "use rand_r" but I am not sure about portability.
            unsigned char r = ( unsigned char )( std::rand() % 255 );  // NOLINT
            randomLuminance[ ( y * resX ) + x ] = r;
        }
    }

    return randImage;
}

  /**
   * Calculates a reaction diffusion texture using turings method.
   *
   * \param tileWidth   the width of one tile
   * \param tileHeight  the height of one tile
   * \param width       the width of the target mem
   * \param height      the height of the target mem
   * \param iterations  the number of iterations to use
   * \param spotSize    the size of the spots - [0,1]
   * \param spotIrregularity  value specifying irregularity of the spots - [0,1]
   */
osg::ref_ptr< osg::Image > genReactionDiffusion( unsigned int tileWidth, unsigned int tileHeight, unsigned int width, unsigned int height, unsigned int iterations, float spotSize, float spotIrregularity )
{

    /////////////////////////////////////////////////////////////////////////////////////////////
    // 1: get memory
    /////////////////////////////////////////////////////////////////////////////////////////////

    // at first create some mem
    // FIXME clean up this mess. There may be a way to reduce memory cost
    float grid1[tileWidth][tileHeight];
    float grid1Min= 3.0;
    float grid1Max= 5.0;
    float grid1Base= 4.0;

    float grid2[tileWidth][tileHeight];
    float grid2Base= 4.0;

    float delta1[tileWidth][tileHeight];
    float delta2[tileWidth][tileHeight];

    float noise[tileWidth][tileHeight];

    float noiseRangeMin=0.1;
    float noiseRangeMax=5.0;

    float noiseRange= noiseRangeMin + ((noiseRangeMax - noiseRangeMin)*spotIrregularity); // the highter the more irregular "spots"
    float noiseBase=12.0;

    /////////////////////////////////////////////////////////////////////////////////////////////
    // 2: init the grids and create random seed used during turing iteration
    /////////////////////////////////////////////////////////////////////////////////////////////

    // init grids
    std::srand( time( 0 ) );
    for (unsigned int y=0; y<tileHeight; y++)
      for( unsigned int x=0; x<tileWidth; x++ )
      {
        grid1[x][y]=grid1Base;
        grid2[x][y]=grid2Base;
        noise[x][y]=noiseBase + (-noiseRange + ( std::rand() * 2.0 * noiseRange));
        delta1[x][y]=0.0;
        delta2[x][y]=0.0;
      }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // 3: turing iteration
    /////////////////////////////////////////////////////////////////////////////////////////////

    float kaMin = 0.02;
    float kaMax = 0.6;
    float ka = (kaMin + ((kaMax - kaMin) * (1.0-spotSize))) / 15.0;        // size of spots
    float d1= 0.25 / 2.0;
    float d2= 0.0625 / 2.0;
    float speed = 1.0;

    for (unsigned int iteration = 0; iteration<iterations; iteration++)
    {
      // go through each cell in grid
      for (unsigned int i = 0; i < tileWidth; i++)
      {
        // this ensures we have an "endless" plane -> creates seamless textures
        unsigned int iPrev = (i + tileWidth - 1) % tileWidth;
        unsigned int iNext = (i + 1) % tileWidth;

        for (unsigned int j = 0; j < tileHeight; j++)
        {
          // this ensures we have an "endless" plane -> creates seamless textures
          unsigned int jPrev = (j + tileHeight - 1) % tileHeight;
          unsigned int jNext = (j + 1) % tileHeight;

          /*
          // try the other laplacian filter
          float laplacian1=  grid1[iPrev][jPrev] +      grid1[i][jPrev]  + grid1[iNext][jPrev] +
                             grid1[iPrev][j]     - (8.0*grid1[i][j])     + grid1[iNext][j] +
                             grid1[iPrev][jNext] +      grid1[i][jNext]  + grid1[iNext][jNext];

          float laplacian2=  grid2[iPrev][jPrev] +      grid2[i][jPrev]  + grid2[iNext][jPrev] +
                             grid2[iPrev][j]     - (8.0*grid2[i][j])     + grid2[iNext][j] +
                             grid2[iPrev][jNext] +      grid2[i][jNext]  + grid2[iNext][jNext];
          */

          // apply laplace filter around current grid point
          float laplacian1=grid1[i][jPrev] + grid1[i][jNext] + grid1[iPrev][j] + grid1[iNext][j] - 4.0 * grid1[i][j];
          float laplacian2=grid2[i][jPrev] + grid2[i][jNext] + grid2[iPrev][j] + grid2[iNext][j] - 4.0 * grid2[i][j];

          // diffusion reaction
          delta1[i][j] = ka * (16 - grid1[i][j] * grid2[i][j]) + d1 * laplacian1;
          delta2[i][j] = ka * (grid1[i][j] * grid2[i][j] - grid2[i][j] - noise[i][j]) + d2 * laplacian2;
        }
      }

      // apply delta and find min and max value
      grid1Min= 1e20;
      grid1Max=-1e20;

      for (unsigned int i = 0; i < tileWidth; i++)
        for (unsigned int j = 0; j < tileHeight; j++)
        {
          grid1[i][j]+=(speed * delta1[i][j]);
          grid2[i][j]+=(speed * delta2[i][j]);
          if (grid2[i][j] < 0)
            grid2[i][j] = 0;

          if (grid1[i][j] < grid1Min)
            grid1Min=grid1[i][j];
          if (grid1[i][j] > grid1Max)
            grid1Max=grid1[i][j];
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // 4: scale grid and copy to target
    /////////////////////////////////////////////////////////////////////////////////////////////

    osg::ref_ptr< osg::Image > randImage = new osg::Image();
    randImage->allocateImage( width, width, 1, GL_LUMINANCE, GL_UNSIGNED_BYTE );
    unsigned char *randomLuminance = randImage->data();  // should be 4 megs

    for (unsigned int x = 0; x < tileWidth; x++)
    {
      for (unsigned int y = 0; y < tileHeight; y++)
      {
        randomLuminance[ ( y * width ) + x ] =255.0*(grid1[x][y] - grid1Min) / (grid1Max - grid1Min);

      }
    }
    return randImage;

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
    osg::ref_ptr< osg::Texture2D > randTexture = new osg::Texture2D();
    randTexture->setFilter( osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST );
    randTexture->setFilter( osg::Texture2D::MAG_FILTER, osg::Texture2D::NEAREST );
    //randTexture->setImage( genWhiteNoise( resX ) );
    randTexture->setImage( genReactionDiffusion( 100, 100, resX, resX, 1000, 2.0, 2.0 ) );
    randTexture->setResizeNonPowerOfTwoHint( false );

    // done.
    ready();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Preparation 2: initialize offscreen renderer and hardwire it
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    // create the root node for all the geometry
    m_output = osg::ref_ptr< WGEManagedGroupNode > ( new WGEManagedGroupNode( m_active ) );

    setupTexturing();

    // the WGEOffscreenRenderNode manages each of the render-passes for us
    osg::ref_ptr< WGEOffscreenRenderNode > offscreen = new WGEOffscreenRenderNode(
        WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getCamera()
    );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( offscreen );

    // allow en-/disabling the HUD:
    offscreen->getTextureHUD()->addUpdateCallback( new WGENodeMaskCallback( m_showHUD ) );

    // setup all the passes needed for image space advection
    osg::ref_ptr< WShader > transformationShader = new WShader( "WMImageSpaceLIC-Transformation", m_localPath );
    osg::ref_ptr< WGEOffscreenRenderPass > transformation = offscreen->addGeometryRenderPass(
        m_output,
        transformationShader,
        "Transformation"
    );
    osg::ref_ptr< WGEOffscreenRenderPass > edgeDetection =  offscreen->addTextureProcessingPass(
        new WShader( "WMImageSpaceLIC-Edge", m_localPath ),
        "Edge Detection"
    );

    // we use two advection passes per frame as the input A of the first produces the output B whereas the second pass uses B as input and
    // produces A as output. This way we can use A as input for the next step (clipping and blending).
    osg::ref_ptr< WGEOffscreenRenderPass > advection =  offscreen->addTextureProcessingPass(
        new WShader( "WMImageSpaceLIC-Advection", m_localPath ),
        "Advection"
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
    osg::ref_ptr< osg::Texture2D > transformationColormapped  = transformation->attach( osg::Camera::COLOR_BUFFER1 );
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
    osg::ref_ptr< osg::Texture2D > advectionOutA  = advection->attach( osg::Camera::COLOR_BUFFER0, GL_LUMINANCE );
    advection->bind( transformationOut1, 0 );
    advection->bind( edgeDetectionOut1,  1 );

    // advection needs some uniforms controlled by properties
    osg::ref_ptr< osg::Uniform > numIters = new WGEPropertyUniform< WPropInt >( "u_numIter", m_numIters );
    advection->addUniform( numIters );

    // Final clipping and blending phase, needs Advected Noise, Edges, Depth and Light
    clipBlend->bind( advectionOutA, 0 );
    clipBlend->bind( edgeDetectionOut1, 1 );
    clipBlend->bind( transformationColormapped, 2 );
    // final pass needs some uniforms controlled by properties
    clipBlend->addUniform( new WGEPropertyUniform< WPropBool >( "u_useEdges", m_useEdges ) );
    clipBlend->addUniform( new WGEPropertyUniform< WPropBool >( "u_useLight", m_useLight ) );
    clipBlend->addUniform( new WGEPropertyUniform< WPropDouble >( "u_cmapRatio", m_cmapRatio ) );

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
        bool dataUpdated = m_vectorsIn->handledUpdate() || m_scalarIn->handledUpdate() || m_meshIn->handledUpdate();
        bool propertyUpdated = m_useSlices->changed();
        boost::shared_ptr< WDataSetVector > dataSetVec = m_vectorsIn->getData();
        boost::shared_ptr< WDataSetScalar > dataSetScal = m_scalarIn->getData();
        boost::shared_ptr< WTriangleMesh2 > mesh = m_meshIn->getData();

        bool dataValid = ( dataSetVec || dataSetScal );
        if ( !dataValid || ( dataValid && !dataUpdated && !propertyUpdated ) )
        {
            continue;
        }

        // prefer vector dataset if existing
        if ( dataSetVec )
        {
            debugLog() << "Using vector data";

            // get grid and prepare OSG
            boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( dataSetVec->getGrid() );
            m_xPos->setMax( grid->getNbCoordsX() - 1 );
            m_yPos->setMax( grid->getNbCoordsY() - 1 );
            m_zPos->setMax( grid->getNbCoordsZ() - 1 );
            initOSG( grid, mesh );

            // prepare offscreen render chain
            edgeDetection->bind( randTexture, 1 );
            transformationShader->eraseDefine( "SCALARDATA" );
            transformationShader->setDefine( "VECTORDATA" );
            transformation->bind( dataSetVec->getTexture()->getTexture(), 0 );
        }
        else if ( dataSetScal )
        {
            debugLog() << "Using scalar data";

            // get grid and prepare OSG
            boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( dataSetScal->getGrid() );
            m_xPos->setMax( grid->getNbCoordsX() - 1 );
            m_yPos->setMax( grid->getNbCoordsY() - 1 );
            m_zPos->setMax( grid->getNbCoordsZ() - 1 );
            initOSG( grid, mesh );

            // prepare offscreen render chain
            edgeDetection->bind( randTexture, 1 );
            transformationShader->eraseDefine( "VECTORDATA" );
            transformationShader->setDefine( "SCALARDATA" );
            transformation->bind( dataSetScal->getTexture()->getTexture(), 0 );
            transformation->bind( randTexture, 1 );
        }

        debugLog() << "Done";
    }

    // clean up
    offscreen->clear();
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( offscreen );
}

