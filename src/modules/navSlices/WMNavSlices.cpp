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

#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <utility>

#include <boost/shared_ptr.hpp>
#include <boost/signals2/signal.hpp>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Group>

#include "../../common/WAssert.h"
#include "../../common/math/WVector3D.h"
#include "../../dataHandler/WDataSet.h"
#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/WDataSetSingle.h"
#include "../../dataHandler/WDataTexture3D.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../../dataHandler/WSubject.h"
#include "../../dataHandler/WValueSet.h"
#include "../../graphicsEngine/WShader.h"
#include "../../graphicsEngine/WGraphicsEngine.h"
#include "../../graphicsEngine/WGEUtils.h"
#include "../../kernel/WKernel.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleConnector.h"
#include "../../kernel/WModuleInputData.h"
#include "../data/WMData.h"
#include "WMNavSlices.h"
#include "navslices.xpm"

bool WMNavSlices::m_navsliceRunning = false;

WMNavSlices::WMNavSlices():
    WModule(),
    m_textureChanged( true ),
    m_isPicked( false ),
    m_isPickedSagittal( false ),
    m_isPickedCoronal( false ),
    m_isPickedAxial( false )
{
    // WARNING: initializing connectors inside the constructor will lead to an exception.
    // Implement WModule::initializeConnectors instead.

    // initialize members
    std::string shaderPath = WKernel::getRunningKernel()->getGraphicsEngine()->getShaderPath();
    m_shader = osg::ref_ptr< WShader > ( new WShader( "slice" ) );
}

WMNavSlices::~WMNavSlices()
{
    // cleanup
    removeConnectors();
}

boost::shared_ptr< WModule > WMNavSlices::factory() const
{
    m_navsliceRunning = true;
    return boost::shared_ptr< WModule >( new WMNavSlices() );
}

bool WMNavSlices::isRunning()
{
    return m_navsliceRunning;
}

const char** WMNavSlices::getXPMIcon() const
{
    return navslices_xpm;
}

const std::string WMNavSlices::getName() const
{
    return "Navigation Slices";
}

const std::string WMNavSlices::getDescription() const
{
    return "This module shows 3 orthogonal navigation slices.";
}

void WMNavSlices::connectors()
{
    // initialize connectors

    // call WModules initialization
    WModule::connectors();
}

void WMNavSlices::properties()
{
    // NOTE: the appropriate type of addProperty is chosen by the type of the specified initial value.
    // So if you specify a bool as initial value, addProperty will create a WPropBool.
    m_showAxial      = m_properties->addProperty( "showAxial",      "Determines whether the axial slice should be visible.", true, true );
    m_showCoronal    = m_properties->addProperty( "showCoronal",    "Determines whether the coronal slice should be visible.", true, true );
    m_showSagittal   = m_properties->addProperty( "showSagittal",   "Determines whether the sagittal slice should be visible.", true, true );

    m_axialPos       = m_properties->addProperty( "Axial Slice",       "Position of axial slice.",    80 );
    m_axialPos->setMin( 0 );
    m_axialPos->setMax( 160 );
    m_coronalPos     = m_properties->addProperty( "Coronal Slice",     "Position of coronal slice.", 100 );
    m_coronalPos->setMin( 0 );
    m_coronalPos->setMax( 160 );
    m_sagittalPos    = m_properties->addProperty( "Sagittal Slice",    "Position of sagittal slice.", 80 );
    m_sagittalPos->setMin( 0 );
    m_sagittalPos->setMax( 160 );

    m_axialPos->setHidden();
    m_coronalPos->setHidden();
    m_sagittalPos->setHidden();

    m_maxAxial       = m_properties->addProperty( "maxAxial",       "Max position of axial slice.",    160, true );
    m_maxCoronal     = m_properties->addProperty( "maxCoronal",     "Max position of coronal slice.",  200, true );
    m_maxSagittal    = m_properties->addProperty( "maxSagittal",    "Max position of sagittal slice.", 160, true );
}

void WMNavSlices::notifyDataChange( boost::shared_ptr<WModuleConnector> input,
                                               boost::shared_ptr<WModuleConnector> output )
{
    WModule::notifyDataChange( input, output );

    // in this case input==m_input
}

void WMNavSlices::notifyTextureChange()
{
    m_textureChanged = true;
}

void WMNavSlices::moduleMain()
{
    boost::shared_ptr< WGraphicsEngine > ge = WGraphicsEngine::getGraphicsEngine();
    WAssert( ge, "No graphics engine present." );

    m_viewer = ge->getViewerByName( "main" );
    WAssert( m_viewer, "Requested viewer not found." );
    m_viewer->getPickHandler()->getPickSignal()->connect( boost::bind( &WMNavSlices::setSlicePosFromPick, this, _1 ) );

    // signal ready state
    ready();

    // now, to watch changing/new textures use WSubject's change condition
    boost::signals2::connection con = WDataHandler::getDefaultSubject()->getChangeCondition()->subscribeSignal(
            boost::bind( &WMNavSlices::notifyTextureChange, this )
    );

    create();

    // Since the modules run in a separate thread: wait
    waitForStop();

    // clean up stuff
    // NOTE: ALAWAYS remove your osg nodes!
    // Please, please always check for NULL
    boost::shared_ptr< WGEViewer > viewer;
    viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "axial" );
    if( viewer )
    {
        viewer->getScene()->remove( m_zSliceNode );
    }

    viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "sagittal" );
    if( viewer )
    {
        viewer->getScene()->remove( m_ySliceNode );
    }

    viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "coronal" );
    if( viewer )
    {
        viewer->getScene()->remove( m_xSliceNode );
    }

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );

    // deregister from WSubject's change condition
    con.disconnect();
}

void WMNavSlices::create()
{
    m_rootNode = osg::ref_ptr< WGEGroupNode >( new WGEGroupNode() );

    m_xSliceNode = osg::ref_ptr<osg::Geode>( new osg::Geode() );
    m_xSliceNode->setName( "Sagittal Slice" );
    m_ySliceNode = osg::ref_ptr<osg::Geode>( new osg::Geode() );
    m_ySliceNode->setName( "Coronal Slice" );
    m_zSliceNode = osg::ref_ptr<osg::Geode>( new osg::Geode() );
    m_zSliceNode->setName( "Axial Slice" );

    m_xSliceNode->addDrawable( createGeometry( 0 ) );
    m_ySliceNode->addDrawable( createGeometry( 1 ) );
    m_zSliceNode->addDrawable( createGeometry( 2 ) );

    m_rootNode->insert( m_xSliceNode );
    m_rootNode->insert( m_ySliceNode );
    m_rootNode->insert( m_zSliceNode );

    m_shader->apply( m_rootNode );
    m_shader->apply( m_xSliceNode );
    m_shader->apply( m_ySliceNode );
    m_shader->apply( m_zSliceNode );

    osg::StateSet* rootState = m_rootNode->getOrCreateStateSet();
    initUniforms( rootState );

    rootState->setMode( GL_BLEND, osg::StateAttribute::ON );

    m_rootNode->setUserData( this );
    m_xSliceNode->setUserData( this );
    m_ySliceNode->setUserData( this );
    m_zSliceNode->setUserData( this );
    m_rootNode->addUpdateCallback( new sliceNodeCallback );

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );

    // Please, please always check for NULL
    boost::shared_ptr< WGEViewer > viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "axial" );
    if( viewer )
    {
        viewer->getScene()->insert( m_zSliceNode );
    }

    viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "sagittal" );
    if( viewer )
    {
        viewer->getScene()->insert( m_xSliceNode );
    }

    viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "coronal" );
    if( viewer )
    {
        viewer->getScene()->insert( m_ySliceNode );
    }
}

namespace //anonymous name space
{
    osg::Vec3 wv3D2ov3( wmath::WVector3D v ) // WVector3D to osg::Vec3 conversion
    {
        return osg::Vec3( v[0], v[1], v[2] );
    }
}

void WMNavSlices::setSlicePosFromPick( WPickInfo pickInfo )
{
    if ( pickInfo.getName() == "Axial Slice"
         ||  pickInfo.getName() == "Coronal Slice"
         ||  pickInfo.getName() == "Sagittal Slice" )
    {
        boost::unique_lock< boost::shared_mutex > lock;
        lock = boost::unique_lock< boost::shared_mutex >( m_updateLock );

        wmath::WVector3D normal = pickInfo.getPickNormal();

        std::pair< float, float > newPixelPos( pickInfo.getPickPixelPosition() );
        if ( m_isPicked )
        {
            osg::Vec3 startPosScreen( m_oldPixelPosition.first, m_oldPixelPosition.second, 0.0 );
            osg::Vec3 endPosScreen( newPixelPos.first, newPixelPos.second, 0.0 );

            osg::Vec3 startPosWorld = wge::unprojectFromScreen( startPosScreen, m_viewer->getCamera() );
            osg::Vec3 endPosWorld = wge::unprojectFromScreen( endPosScreen, m_viewer->getCamera() );

            osg::Vec3 moveDirWorld = endPosWorld - startPosWorld;
            float diff = wv3D2ov3( normal ) * moveDirWorld;

            // recognize also small values.
            if( diff < 0 && diff > -1 )
            {
                diff = -1;
            }
            if( diff > 0 && diff < 1 )
            {
                diff = 1;
            }

            if ( pickInfo.getName() == "Axial Slice" )
            {
                m_axialPos->set( m_axialPos->get() + diff );
            }
            if ( pickInfo.getName() == "Coronal Slice" )
            {
                m_coronalPos->set( m_coronalPos->get() + diff );
            }
            if ( pickInfo.getName() == "Sagittal Slice" )
            {
                m_sagittalPos->set( m_sagittalPos->get() + diff );
            }

            lock.unlock();
        }
        m_oldPixelPosition = newPixelPos;
        m_isPicked |= true;

        if ( pickInfo.getName() == "Axial Slice" )
        {
            m_isPickedAxial |= true;
        }
        if ( pickInfo.getName() == "Coronal Slice" )
        {
            m_isPickedCoronal |= true;
        }
        if ( pickInfo.getName() == "Sagittal Slice" )
        {
            m_isPickedSagittal |= true;
        }
    }
    else
    {
        m_isPicked &= false;
        m_isPickedCoronal &= false;
        m_isPickedAxial &= false;
        m_isPickedSagittal &= false;
    }
}

osg::ref_ptr<osg::Geometry> WMNavSlices::createGeometry( int slice )
{
    const size_t nbVerts = 4;
    float maxDim = 255.0;

    float xSlice = static_cast< float >( m_sagittalPos->get() );
    float ySlice = static_cast< float >( m_coronalPos->get() );
    float zSlice = static_cast< float >( m_axialPos->get() );

    float xPos = xSlice + 0.5f;
    float yPos = ySlice + 0.5f;
    float zPos = zSlice + 0.5f;

    osg::ref_ptr<osg::Geometry> sliceGeometry = osg::ref_ptr<osg::Geometry>( new osg::Geometry() );

    osg::Vec3Array* sliceVertices = new osg::Vec3Array;

    // grab a list of data textures
    std::vector< boost::shared_ptr< WDataTexture3D > > tex = WDataHandler::getDefaultSubject()->getDataTextures( true );

    if ( tex.size() > 0 )
    {
        switch ( slice )
        {
            case 0:
            {
                std::vector< wmath::WPosition > vertices;
                vertices.push_back( wmath::WPosition( xPos, 0,      0      ) );
                vertices.push_back( wmath::WPosition( xPos, 0,      maxDim ) );
                vertices.push_back( wmath::WPosition( xPos, maxDim, maxDim ) );
                vertices.push_back( wmath::WPosition( xPos, maxDim, 0      ) );
                for( size_t i = 0; i < nbVerts; ++i )
                {
                    sliceVertices->push_back( wv3D2ov3( vertices[i] ) );
                }

                sliceGeometry->setVertexArray( sliceVertices );

                int c = 0;
                for ( std::vector< boost::shared_ptr< WDataTexture3D > >::const_iterator iter = tex.begin(); iter != tex.end(); ++iter )
                {
                    boost::shared_ptr< WGridRegular3D > grid = ( *iter )->getGrid();

                    osg::Vec3Array* texCoords = new osg::Vec3Array;
                    for( size_t i = 0; i < nbVerts; ++i )
                    {
                        texCoords->push_back( wv3D2ov3( grid->worldCoordToTexCoord( vertices[i] ) ) );
                    }
                    sliceGeometry->setTexCoordArray( c, texCoords );
                    ++c;
                }
                break;
            }
            case 1:
            {
                std::vector< wmath::WPosition > vertices;
                vertices.push_back( wmath::WPosition( 0,      yPos, 0      ) );
                vertices.push_back( wmath::WPosition( maxDim, yPos, 0      ) );
                vertices.push_back( wmath::WPosition( maxDim, yPos, maxDim ) );
                vertices.push_back( wmath::WPosition( 0,      yPos, maxDim ) );
                for( size_t i = 0; i < nbVerts; ++i )
                {
                    sliceVertices->push_back( wv3D2ov3( vertices[i] ) );
                }
                sliceGeometry->setVertexArray( sliceVertices );

                int c = 0;
                for ( std::vector< boost::shared_ptr< WDataTexture3D > >::const_iterator iter = tex.begin(); iter != tex.end(); ++iter )
                {
                    boost::shared_ptr< WGridRegular3D > grid = ( *iter )->getGrid();

                    osg::Vec3Array* texCoords = new osg::Vec3Array;
                    for( size_t i = 0; i < nbVerts; ++i )
                    {
                        texCoords->push_back( wv3D2ov3( grid->worldCoordToTexCoord( vertices[i] ) ) );
                    }
                    sliceGeometry->setTexCoordArray( c, texCoords );
                    ++c;
                }
                break;
            }
            case 2:
            {
                std::vector< wmath::WPosition > vertices;
                vertices.push_back( wmath::WPosition(      0,      0, zPos      ) );
                vertices.push_back( wmath::WPosition(      0, maxDim, zPos      ) );
                vertices.push_back( wmath::WPosition( maxDim, maxDim, zPos      ) );
                vertices.push_back( wmath::WPosition( maxDim,      0, zPos      ) );
                for( size_t i = 0; i < nbVerts; ++i )
                {
                    sliceVertices->push_back( wv3D2ov3( vertices[i] ) );
                }

                sliceGeometry->setVertexArray( sliceVertices );

                int c = 0;
                for ( std::vector< boost::shared_ptr< WDataTexture3D > >::const_iterator iter = tex.begin(); iter != tex.end(); ++iter )
                {
                    boost::shared_ptr< WGridRegular3D > grid = ( *iter )->getGrid();

                    osg::Vec3Array* texCoords = new osg::Vec3Array;

                    for( size_t i = 0; i < nbVerts; ++i )
                    {
                        texCoords->push_back( wv3D2ov3( grid->worldCoordToTexCoord( vertices[i] ) ) );
                    }
                    sliceGeometry->setTexCoordArray( c, texCoords );
                    ++c;
                }
                break;
            }
        }

        osg::DrawElementsUInt* quad = new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 0 );
        quad->push_back( 3 );
        quad->push_back( 2 );
        quad->push_back( 1 );
        quad->push_back( 0 );
        sliceGeometry->addPrimitiveSet( quad );
    }
    WKernel::getRunningKernel()->getSelectionManager()->getCrosshair()->setPosition(
            wmath::WPosition( m_sagittalPos->get(), m_coronalPos->get(), m_axialPos->get() ) );

    return sliceGeometry;
}

void WMNavSlices::updateGeometry()
{
    boost::shared_lock<boost::shared_mutex> slock;
    slock = boost::shared_lock<boost::shared_mutex>( m_updateLock );

    osg::ref_ptr<osg::Geometry> xSliceGeometry = createGeometry( 0 );
    osg::ref_ptr<osg::Geometry> ySliceGeometry = createGeometry( 1 );
    osg::ref_ptr<osg::Geometry> zSliceGeometry = createGeometry( 2 );

    osg::ref_ptr<osg::Drawable> oldx = osg::ref_ptr<osg::Drawable>( m_xSliceNode->getDrawable( 0 ) );
    m_xSliceNode->replaceDrawable( oldx, xSliceGeometry );
    osg::ref_ptr<osg::Drawable> oldy = osg::ref_ptr<osg::Drawable>( m_ySliceNode->getDrawable( 0 ) );
    m_ySliceNode->replaceDrawable( oldy, ySliceGeometry );
    osg::ref_ptr<osg::Drawable> oldz = osg::ref_ptr<osg::Drawable>( m_zSliceNode->getDrawable( 0 ) );
    m_zSliceNode->replaceDrawable( oldz, zSliceGeometry );

    std::vector< boost::shared_ptr< WDataTexture3D > > tex = WDataHandler::getDefaultSubject()->getDataTextures( true );
    bool noSlices = ( tex.size() == 0 ) || !m_active->get();

    if ( m_showAxial->get() && !noSlices )
    {
        m_zSliceNode->setNodeMask( 0xFFFFFFFF );
    }
    else
    {
        m_zSliceNode->setNodeMask( 0x0 );
    }

    if ( m_showCoronal->get() && !noSlices )
    {
        m_ySliceNode->setNodeMask( 0xFFFFFFFF );
    }
    else
    {
        m_ySliceNode->setNodeMask( 0x0 );
    }

    if ( m_showSagittal->get() && !noSlices )
    {
        m_xSliceNode->setNodeMask( 0xFFFFFFFF );
    }
    else
    {
        m_xSliceNode->setNodeMask( 0x0 );
    }

    slock.unlock();
}


void WMNavSlices::updateTextures()
{
    osg::StateSet* rootState = m_rootNode->getOrCreateStateSet();
    if ( m_textureChanged )
    {
        m_textureChanged = false;

        // grab a list of data textures
        std::vector< boost::shared_ptr< WDataTexture3D > > tex = WDataHandler::getDefaultSubject()->getDataTextures( true );

        if ( tex.size() > 0 )
        {
            // reset all uniforms
            for ( int i = 0; i < 10; ++i )
            {
                m_typeUniforms[i]->set( 0 );
            }

            // for each texture -> apply
            int c = 0;
            for ( std::vector< boost::shared_ptr< WDataTexture3D > >::const_iterator iter = tex.begin(); iter != tex.end(); ++iter )
            {
                osg::ref_ptr<osg::Texture3D> texture3D = ( *iter )->getTexture();

                if ( ( *iter )->isInterpolated() )
                {
                    texture3D->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR );
                    texture3D->setFilter( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
                }
                else
                {
                    texture3D->setFilter( osg::Texture::MIN_FILTER, osg::Texture::NEAREST );
                    texture3D->setFilter( osg::Texture::MAG_FILTER, osg::Texture::NEAREST );
                }
                rootState->setTextureAttributeAndModes( c, texture3D, osg::StateAttribute::ON );

                // set threshold/opacity as uniforms
                float t = ( *iter )->getThreshold() / 255.0;
                float a = ( *iter )->getAlpha();
                int cmap = ( *iter )->getSelectedColormap();

                m_typeUniforms[c]->set( ( *iter )->getDataType() );
                m_thresholdUniforms[c]->set( t );
                m_alphaUniforms[c]->set( a );
                m_cmapUniforms[c]->set( cmap );

                ++c;
            }

            // TODO(schurade): used? Not used? Replace by new Property please
            // rootState->addUniform( osg::ref_ptr<osg::Uniform>(
            //            new osg::Uniform( "useTexture", m_properties->getValue< bool >( "Use Texture" ) ) )
            // );
        }
    }

    m_highlightUniformSagittal->set( m_isPickedSagittal );
    m_highlightUniformCoronal->set( m_isPickedCoronal );
    m_highlightUniformAxial->set( m_isPickedAxial );

    m_xSliceNode->getOrCreateStateSet()->merge( *rootState );
    m_ySliceNode->getOrCreateStateSet()->merge( *rootState );
    m_zSliceNode->getOrCreateStateSet()->merge( *rootState );
}

void WMNavSlices::initUniforms( osg::StateSet* rootState )
{
    boost::shared_lock<boost::shared_mutex> slock;
    slock = boost::shared_lock<boost::shared_mutex>( m_updateLock );

    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type0", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type1", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type2", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type3", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type4", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type5", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type6", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type7", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type8", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type9", 0 ) ) );

    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha0", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha1", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha2", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha3", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha4", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha5", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha6", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha7", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha8", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha9", 1.0f ) ) );

    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold0", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold1", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold2", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold3", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold4", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold5", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold6", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold7", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold8", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold9", 0.0f ) ) );

    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex0", 0 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex1", 1 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex2", 2 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex3", 3 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex4", 4 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex5", 5 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex6", 6 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex7", 7 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex8", 8 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex9", 9 ) ) );

    m_cmapUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useCmap0", 0 ) ) );
    m_cmapUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useCmap1", 0 ) ) );
    m_cmapUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useCmap2", 0 ) ) );
    m_cmapUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useCmap3", 0 ) ) );
    m_cmapUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useCmap4", 0 ) ) );
    m_cmapUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useCmap5", 0 ) ) );
    m_cmapUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useCmap6", 0 ) ) );
    m_cmapUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useCmap7", 0 ) ) );
    m_cmapUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useCmap8", 0 ) ) );
    m_cmapUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "useCmap9", 0 ) ) );

    for ( int i = 0; i < 10; ++i )
    {
        rootState->addUniform( m_typeUniforms[i] );
        rootState->addUniform( m_thresholdUniforms[i] );
        rootState->addUniform( m_alphaUniforms[i] );
        rootState->addUniform( m_samplerUniforms[i] );
        rootState->addUniform( m_cmapUniforms[i] );
    }

    m_highlightUniformSagittal = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "highlighted", 0 ) );
    m_highlightUniformCoronal = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "highlighted", 0 ) );
    m_highlightUniformAxial = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "highlighted", 0 ) );

    m_xSliceNode->getOrCreateStateSet()->addUniform( m_highlightUniformSagittal );
    m_ySliceNode->getOrCreateStateSet()->addUniform( m_highlightUniformCoronal );
    m_zSliceNode->getOrCreateStateSet()->addUniform( m_highlightUniformAxial );

    slock.unlock();
}
