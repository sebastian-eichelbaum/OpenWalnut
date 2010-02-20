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

#include <boost/shared_ptr.hpp>
#include <boost/signals2/signal.hpp>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Group>

#include "../../dataHandler/WDataSet.h"
#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/WDataSetSingle.h"
#include "../../dataHandler/WDataTexture3D.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../../dataHandler/WSubject.h"
#include "../../dataHandler/WValueSet.h"
#include "../../graphicsEngine/WShader.h"
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
    m_textureChanged( true )
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
    return "Navigation Slice Module";
}

const std::string WMNavSlices::getDescription() const
{
    return "This module shows 3 orthogonal navigation slices.";
}

void WMNavSlices::connectors()
{
    // initialize connectors
    // XXX to add a new connector and to offer it, these simple steps need to be done
    // initialize it first
    m_input= boost::shared_ptr<WModuleInputData< WDataSetSingle > >(
            new WModuleInputData< WDataSetSingle >( shared_from_this(),
                "in1", "List of datasets to show on the slices." )
    );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_input );

    // call WModules initialization
    WModule::connectors();
}

void WMNavSlices::properties()
{
    // NOTE: the appropriate type of addProperty is chosen by the type of the specified initial value.
    // So if you specify a bool as initial value, addProperty will create a WPropBool.
    m_showAxial      = m_properties2->addProperty( "showAxial",      "Determines whether the axial slice should be visible.", true, true );
    m_showCoronal    = m_properties2->addProperty( "showCoronal",    "Determines whether the coronal slice should be visible.", true, true );
    m_showSagittal   = m_properties2->addProperty( "showSagittal",   "Determines whether the sagittal slice should be visible.", true, true );

    m_axialPos       = m_properties2->addProperty( "axialPos",       "Position of axial slice.",    80 );
    m_axialPos->setMin( 0 );
    m_axialPos->setMax( 160 );
    m_coronalPos     = m_properties2->addProperty( "coronalPos",     "Position of coronal slice.", 100 );
    m_coronalPos->setMin( 0 );
    m_coronalPos->setMax( 160 );
    m_sagittalPos    = m_properties2->addProperty( "sagittalPos",    "Position of sagittal slice.", 80 );
    m_sagittalPos->setMin( 0 );
    m_sagittalPos->setMax( 160 );

    m_maxAxial       = m_properties2->addProperty( "maxAxial",       "Max position of axial slice.",    160, true );
    m_maxCoronal     = m_properties2->addProperty( "maxCoronal",     "Max position of coronal slice.",  200, true );
    m_maxSagittal    = m_properties2->addProperty( "maxSagittal",    "Max position of sagittal slice.", 160, true );
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
    // NOTE: ALLAWAYS remove your osg nodes!
    WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "axial" )->getScene()->remove( m_xSliceNode );
    WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "sagittal" )->getScene()->remove( m_ySliceNode );
    WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "coronal" )->getScene()->remove( m_zSliceNode );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );

    // deregister from WSubject's change condition
    con.disconnect();
}

void WMNavSlices::create()
{
    m_rootNode = osg::ref_ptr< WGEGroupNode >( new WGEGroupNode() );

    m_xSliceNode = osg::ref_ptr<osg::Geode>( new osg::Geode() );
    m_xSliceNode->setName( "X-Slice" );
    m_ySliceNode = osg::ref_ptr<osg::Geode>( new osg::Geode() );
    m_ySliceNode->setName( "Y-Slice" );
    m_zSliceNode = osg::ref_ptr<osg::Geode>( new osg::Geode() );
    m_zSliceNode->setName( "Z-Slice" );

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
    m_rootNode->setUserData( this );
    m_xSliceNode->setUserData( this );
    m_ySliceNode->setUserData( this );
    m_zSliceNode->setUserData( this );
    m_rootNode->addUpdateCallback( new sliceNodeCallback );

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );


    WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "axial" )->getScene()->insert( m_xSliceNode );
    WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "sagittal" )->getScene()->insert( m_ySliceNode );
    WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "coronal" )->getScene()->insert( m_zSliceNode );
}

osg::Vec3 wv3D2ov3( wmath::WVector3D v ) // WVector3D to osg::Vec3 conversion
{
    return osg::Vec3( v[0], v[1], v[2] );
}

osg::ref_ptr<osg::Geometry> WMNavSlices::createGeometry( int slice )
{
    float maxDim = 255.0;

    float xSlice = ( float )( m_sagittalPos->get() );
    float ySlice = ( float )( m_coronalPos->get() );
    float zSlice = ( float )( m_axialPos->get() );

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
                sliceVertices->push_back( osg::Vec3( 0,      yPos, 0      ) );
                sliceVertices->push_back( osg::Vec3( 0,      yPos, maxDim ) );
                sliceVertices->push_back( osg::Vec3( maxDim, yPos, maxDim ) );
                sliceVertices->push_back( osg::Vec3( maxDim, yPos, 0      ) );
                sliceGeometry->setVertexArray( sliceVertices );

                int c = 0;
                for ( std::vector< boost::shared_ptr< WDataTexture3D > >::const_iterator iter = tex.begin(); iter != tex.end(); ++iter )
                {
                    boost::shared_ptr< WGridRegular3D > grid = ( *iter )->getGrid();

                    float maxX = ( float )( grid->getNbCoordsX() );
                    float maxY = ( float )( grid->getNbCoordsY() );
                    float maxZ = ( float )( grid->getNbCoordsZ() );

                    //float texX = xSlice / maxX;
                    float texY = ySlice / maxY;
                    //float texZ = zSlice / maxZ;

                    float texXOff = 255.0 / maxX;
                    //float texYOff = 255.0 / maxY;
                    float texZOff = 255.0 / maxZ;

                    osg::Vec3Array* texCoords = new osg::Vec3Array;
                    texCoords->push_back( wv3D2ov3( grid->transformTexCoord( wmath::WPosition( 0.0,     texY, 0.0     ) ) ) );
                    texCoords->push_back( wv3D2ov3( grid->transformTexCoord( wmath::WPosition( 0.0,     texY, texZOff ) ) ) );
                    texCoords->push_back( wv3D2ov3( grid->transformTexCoord( wmath::WPosition( texXOff, texY, texZOff ) ) ) );
                    texCoords->push_back( wv3D2ov3( grid->transformTexCoord( wmath::WPosition( texXOff, texY, 0.0     ) ) ) );
                    sliceGeometry->setTexCoordArray( c, texCoords );
                    ++c;
                }
                break;
            }
            case 1:
            {
                sliceVertices->push_back( osg::Vec3( xPos, 0,      0      ) );
                sliceVertices->push_back( osg::Vec3( xPos, 0,      maxDim ) );
                sliceVertices->push_back( osg::Vec3( xPos, maxDim, maxDim ) );
                sliceVertices->push_back( osg::Vec3( xPos, maxDim, 0      ) );
                sliceGeometry->setVertexArray( sliceVertices );

                int c = 0;
                for ( std::vector< boost::shared_ptr< WDataTexture3D > >::const_iterator iter = tex.begin(); iter != tex.end(); ++iter )
                {
                    boost::shared_ptr< WGridRegular3D > grid = ( *iter )->getGrid();

                    float maxX = ( float )( grid->getNbCoordsX() );
                    float maxY = ( float )( grid->getNbCoordsY() );
                    float maxZ = ( float )( grid->getNbCoordsZ() );

                    float texX = xSlice / maxX;
                    //float texY = ySlice / maxY;
                    //float texZ = zSlice / maxZ;

                    //float texXOff = 255.0 / maxX;
                    float texYOff = 255.0 / maxY;
                    float texZOff = 255.0 / maxZ;

                    osg::Vec3Array* texCoords = new osg::Vec3Array;
                    texCoords->push_back( wv3D2ov3( grid->transformTexCoord( wmath::WPosition( texX, 0.0,     0.0     ) ) ) );
                    texCoords->push_back( wv3D2ov3( grid->transformTexCoord( wmath::WPosition( texX, 0.0,     texZOff ) ) ) );
                    texCoords->push_back( wv3D2ov3( grid->transformTexCoord( wmath::WPosition( texX, texYOff, texZOff ) ) ) );
                    texCoords->push_back( wv3D2ov3( grid->transformTexCoord( wmath::WPosition( texX, texYOff, 0.0     ) ) ) );
                    sliceGeometry->setTexCoordArray( c, texCoords );
                    ++c;
                }
                break;
            }
            case 2:
            {
                sliceVertices->push_back( osg::Vec3( 0,      0,      zPos ) );
                sliceVertices->push_back( osg::Vec3( 0,      maxDim, zPos ) );
                sliceVertices->push_back( osg::Vec3( maxDim, maxDim, zPos ) );
                sliceVertices->push_back( osg::Vec3( maxDim, 0,      zPos ) );
                sliceGeometry->setVertexArray( sliceVertices );

                int c = 0;
                for ( std::vector< boost::shared_ptr< WDataTexture3D > >::const_iterator iter = tex.begin(); iter != tex.end(); ++iter )
                {
                    boost::shared_ptr< WGridRegular3D > grid = ( *iter )->getGrid();


                    float maxX = ( float )( grid->getNbCoordsX() );
                    float maxY = ( float )( grid->getNbCoordsY() );
                    float maxZ = ( float )( grid->getNbCoordsZ() );

                    //float texX = xSlice / maxX;
                    //float texY = ySlice / maxY;
                    float texZ = zSlice / maxZ;

                    float texXOff = 255.0 / maxX;
                    float texYOff = 255.0 / maxY;
                    //float texZOff = 255.0 / maxZ;

                    osg::Vec3Array* texCoords = new osg::Vec3Array;

                    texCoords->push_back( wv3D2ov3( grid->transformTexCoord( wmath::WPosition( 0.0,     0.0,     texZ ) ) ) );
                    texCoords->push_back( wv3D2ov3( grid->transformTexCoord( wmath::WPosition( 0.0,     texYOff, texZ ) ) ) );
                    texCoords->push_back( wv3D2ov3( grid->transformTexCoord( wmath::WPosition( texXOff, texYOff, texZ ) ) ) );
                    texCoords->push_back( wv3D2ov3( grid->transformTexCoord( wmath::WPosition( texXOff, 0.0,     texZ ) ) ) );
                    sliceGeometry->setTexCoordArray( c, texCoords );
                    ++c;
                }
                break;
            }
        }
    }
    else
    {
        float maxX = ( float )( m_maxSagittal->get() );
        float maxY = ( float )( m_maxCoronal->get() );
        float maxZ = ( float )( m_maxAxial->get() );

        m_sagittalPos->setMax( maxX );
        m_coronalPos->setMax( maxY );
        m_axialPos->setMax( maxZ );

        float texX = xSlice / maxX;
        float texY = ySlice / maxY;
        float texZ = zSlice / maxZ;

        osg::Vec3Array* texCoords = new osg::Vec3Array;
        switch ( slice )
        {
            case 0:
                sliceVertices->push_back( osg::Vec3( 0,       yPos, 0      ) );
                sliceVertices->push_back( osg::Vec3( 0,       yPos, maxDim ) );
                sliceVertices->push_back( osg::Vec3( maxDim,  yPos, maxDim ) );
                sliceVertices->push_back( osg::Vec3( maxDim,  yPos, 0      ) );
                sliceGeometry->setVertexArray( sliceVertices );
                texCoords->push_back( osg::Vec3( 0.0, texY, 0.0 ) );
                texCoords->push_back( osg::Vec3( 0.0, texY, 1.0 ) );
                texCoords->push_back( osg::Vec3( 1.0, texY, 1.0 ) );
                texCoords->push_back( osg::Vec3( 1.0, texY, 0.0 ) );
                sliceGeometry->setTexCoordArray( 0, texCoords );
                break;
            case 1:
                sliceVertices->push_back( osg::Vec3( xPos, 0,      0      ) );
                sliceVertices->push_back( osg::Vec3( xPos, 0,      maxDim ) );
                sliceVertices->push_back( osg::Vec3( xPos, maxDim, maxDim ) );
                sliceVertices->push_back( osg::Vec3( xPos, maxDim, 0      ) );
                sliceGeometry->setVertexArray( sliceVertices );
                texCoords->push_back( osg::Vec3( texX, 0.0, 0.0 ) );
                texCoords->push_back( osg::Vec3( texX, 0.0, 1.0 ) );
                texCoords->push_back( osg::Vec3( texX, 1.0, 1.0 ) );
                texCoords->push_back( osg::Vec3( texX, 1.0, 0.0 ) );
                sliceGeometry->setTexCoordArray( 0, texCoords );
                break;
            case 2:
                sliceVertices->push_back( osg::Vec3( 0,      0,      zPos ) );
                sliceVertices->push_back( osg::Vec3( 0,      maxDim, zPos ) );
                sliceVertices->push_back( osg::Vec3( maxDim, maxDim, zPos ) );
                sliceVertices->push_back( osg::Vec3( maxDim, 0,      zPos ) );
                sliceGeometry->setVertexArray( sliceVertices );
                texCoords->push_back( osg::Vec3( 0.0, 0.0, texZ ) );
                texCoords->push_back( osg::Vec3( 0.0, 1.0, texZ ) );
                texCoords->push_back( osg::Vec3( 1.0, 1.0, texZ ) );
                texCoords->push_back( osg::Vec3( 1.0, 0.0, texZ ) );
                sliceGeometry->setTexCoordArray( 0, texCoords );
                break;
        }
    }

    osg::DrawElementsUInt* quad = new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 0 );
    quad->push_back( 3 );
    quad->push_back( 2 );
    quad->push_back( 1 );
    quad->push_back( 0 );
    sliceGeometry->addPrimitiveSet( quad );

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
        m_xSliceNode->setNodeMask( 0xFFFFFFFF );
    }
    else
    {
        m_xSliceNode->setNodeMask( 0x0 );
    }

    if ( m_showSagittal->get() && !noSlices )
    {
        m_ySliceNode->setNodeMask( 0xFFFFFFFF );
    }
    else
    {
        m_ySliceNode->setNodeMask( 0x0 );
    }

    slock.unlock();
}


void WMNavSlices::updateTextures()
{
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
            osg::StateSet* rootState = m_rootNode->getOrCreateStateSet();
            int c = 0;
            for ( std::vector< boost::shared_ptr< WDataTexture3D > >::const_iterator iter = tex.begin(); iter != tex.end(); ++iter )
            {
                osg::ref_ptr<osg::Texture3D> texture3D = ( *iter )->getTexture();
                rootState->setTextureAttributeAndModes( c, texture3D, osg::StateAttribute::ON );

                // set threshold/opacity as uniforms
                float t = ( *iter )->getThreshold() / 255.0;
                float a = ( *iter )->getAlpha();

                m_typeUniforms[c]->set( ( *iter )->getDataType() );
                m_thresholdUniforms[c]->set( t );
                m_alphaUniforms[c]->set( a );

                ++c;
            }
            m_xSliceNode->setStateSet( rootState );
            m_ySliceNode->setStateSet( rootState );
            m_zSliceNode->setStateSet( rootState );
        }
    }
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

    for ( int i = 0; i < 10; ++i )
    {
        rootState->addUniform( m_typeUniforms[i] );
        rootState->addUniform( m_thresholdUniforms[i] );
        rootState->addUniform( m_alphaUniforms[i] );
        rootState->addUniform( m_samplerUniforms[i] );
    }
    slock.unlock();
}
