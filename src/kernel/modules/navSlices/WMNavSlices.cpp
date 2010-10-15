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
#include <algorithm>

#include <boost/shared_ptr.hpp>
#include <boost/signals2/signal.hpp>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Group>
#include <osg/LineWidth>

#include "../../../common/WAssert.h"
#include "../../../common/WLimits.h"
#include "../../../common/math/WVector3D.h"
#include "../../../dataHandler/WDataSet.h"
#include "../../../dataHandler/WDataHandler.h"
#include "../../../dataHandler/WDataSetSingle.h"
#include "../../../dataHandler/WDataSetScalar.h"
#include "../../../dataHandler/WDataTexture3D.h"
#include "../../../dataHandler/WGridRegular3D.h"
#include "../../../dataHandler/WSubject.h"
#include "../../../dataHandler/WValueSet.h"
#include "../../../graphicsEngine/WShader.h"
#include "../../../graphicsEngine/WGraphicsEngine.h"
#include "../../../graphicsEngine/WGEUtils.h"
#include "../../WKernel.h"
#include "../../WModule.h"
#include "../../WModuleConnector.h"
#include "../../WModuleInputData.h"
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
    m_shader = osg::ref_ptr< WShader > ( new WShader( "WMNavSlices" ) );
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
    m_coronalPos->setMax( 200 );
    m_sagittalPos    = m_properties->addProperty( "Sagittal Slice",    "Position of sagittal slice.", 80 );
    m_sagittalPos->setMin( 0 );
    m_sagittalPos->setMax( 160 );
    m_showComplete = m_properties->addProperty( "Show complete", "Slice should be drawn complete even if the texture value is zero.", false );

    m_axialPos->setHidden();
    m_coronalPos->setHidden();
    m_sagittalPos->setHidden();

    WKernel::getRunningKernel()->getSelectionManager()->setPropAxialPos( m_axialPos );
    WKernel::getRunningKernel()->getSelectionManager()->setPropCoronalPos( m_coronalPos );
    WKernel::getRunningKernel()->getSelectionManager()->setPropSagittalPos( m_sagittalPos );

    // Print some nice output: the current nav slice position
    m_currentPosition = m_infoProperties->addProperty( "Position", "Current position of the navigation slices.",
            wmath::WPosition( m_axialPos->get(), m_coronalPos->get(), m_sagittalPos->get() ) );
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

    m_viewer = ge->getViewerByName( "Axial View" );
    if( m_viewer )
    {
        m_viewer->getPickHandler()->getPickSignal()->connect( boost::bind( &WMNavSlices::setSlicePosFromPick, this, _1 ) );
    }

    m_viewer = ge->getViewerByName( "Sagittal View" );
    if( m_viewer )
    {
        m_viewer->getPickHandler()->getPickSignal()->connect( boost::bind( &WMNavSlices::setSlicePosFromPick, this, _1 ) );
    }

    m_viewer = ge->getViewerByName( "Coronal View" );
    if( m_viewer )
    {
        m_viewer->getPickHandler()->getPickSignal()->connect( boost::bind( &WMNavSlices::setSlicePosFromPick, this, _1 ) );
    }

    m_viewer = ge->getViewerByName( "main" );

    // signal ready state
    ready();

    // now, to watch changing/new textures use WSubject's change condition
    boost::signals2::connection con = WDataHandler::getDefaultSubject()->getChangeCondition()->subscribeSignal(
            boost::bind( &WMNavSlices::notifyTextureChange, this )
    );

    setMaxMinFromBoundingBox();
    m_sagittalPos->set( 0.5 * ( m_bb.first[0] + m_bb.second[0] ) );
    m_coronalPos->set( 0.5 * (  m_bb.first[1] + m_bb.second[1] ) );
    m_axialPos->set( 0.5 * (  m_bb.first[2] + m_bb.second[2] ) );

    create();

    // Since the modules run in a separate thread: wait
    waitForStop();

    // clean up stuff
    // NOTE: ALAWAYS remove your osg nodes!
    // Please, please always check for NULL
    boost::shared_ptr< WGEViewer > viewer;
    viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Axial View" );
    if( viewer )
    {
        viewer->getScene()->remove( m_zSliceNode );
        viewer->getScene()->remove( m_zCrossNode );
    }

    viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Sagittal View" );
    if( viewer )
    {
        viewer->getScene()->remove( m_xSliceNode );
        viewer->getScene()->remove( m_xCrossNode );
    }

    viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Coronal View" );
    if( viewer )
    {
        viewer->getScene()->remove( m_ySliceNode );
        viewer->getScene()->remove( m_yCrossNode );
    }

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_slicesNode );

    // deregister from WSubject's change condition
    con.disconnect();
}

void WMNavSlices::create()
{
    m_rootNode = osg::ref_ptr< WGEGroupNode >( new WGEGroupNode() );
    m_rootNode->setDataVariance( osg::Object::DYNAMIC );
    m_slicesNode = osg::ref_ptr< WGEGroupNode >( new WGEGroupNode() );
    m_slicesNode->setDataVariance( osg::Object::DYNAMIC );
    m_slicesSwitchNode = osg::ref_ptr< osg::Switch >( new osg::Switch() );
    m_slicesSwitchNode->setDataVariance( osg::Object::DYNAMIC );

    m_xSliceNode = osg::ref_ptr<osg::Geode>( new osg::Geode() );
    m_xSliceNode->setDataVariance( osg::Object::DYNAMIC );
    m_xSliceNode->setName( "Sagittal Slice" );
    m_ySliceNode = osg::ref_ptr<osg::Geode>( new osg::Geode() );
    m_ySliceNode->setDataVariance( osg::Object::DYNAMIC );
    m_ySliceNode->setName( "Coronal Slice" );
    m_zSliceNode = osg::ref_ptr<osg::Geode>( new osg::Geode() );
    m_zSliceNode->setDataVariance( osg::Object::DYNAMIC );
    m_zSliceNode->setName( "Axial Slice" );

    m_xSliceNode->addDrawable( createGeometry( 0 ) );
    m_ySliceNode->addDrawable( createGeometry( 1 ) );
    m_zSliceNode->addDrawable( createGeometry( 2 ) );

    m_xCrossNode = osg::ref_ptr<osg::Geode>( new osg::Geode() );
    m_yCrossNode = osg::ref_ptr<osg::Geode>( new osg::Geode() );
    m_zCrossNode = osg::ref_ptr<osg::Geode>( new osg::Geode() );
    m_xCrossNode->setDataVariance( osg::Object::DYNAMIC );
    m_yCrossNode->setDataVariance( osg::Object::DYNAMIC );
    m_zCrossNode->setDataVariance( osg::Object::DYNAMIC );
    m_xCrossNode->addDrawable( createCrossGeometry( 0 ) );
    m_yCrossNode->addDrawable( createCrossGeometry( 1 ) );
    m_zCrossNode->addDrawable( createCrossGeometry( 2 ) );

    m_rootNode->insert( m_xCrossNode );
    m_rootNode->insert( m_yCrossNode );
    m_rootNode->insert( m_zCrossNode );

    m_rootNode->insert( m_slicesNode );

    m_slicesSwitchNode->addChild( m_xSliceNode );
    m_slicesSwitchNode->addChild( m_ySliceNode );
    m_slicesSwitchNode->addChild( m_zSliceNode );

    m_slicesNode->insert( m_slicesSwitchNode );

    m_shader->apply( m_slicesNode );

    osg::StateSet* rootState = m_slicesNode->getOrCreateStateSet();
    initUniforms( rootState );

    rootState->setMode( GL_BLEND, osg::StateAttribute::ON );

    osg::ref_ptr< userData > usrData = osg::ref_ptr< userData >(
        new userData( boost::shared_dynamic_cast< WMNavSlices >( shared_from_this() ) )
        );

    m_slicesNode->setUserData( usrData );
    m_slicesNode->addUpdateCallback( new sliceNodeCallback );

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_slicesNode );

    // Please, please always check for NULL
    boost::shared_ptr< WGEViewer > viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Axial View" );
    if( viewer )
    {
        viewer->getScene()->insert( m_zSliceNode );
        viewer->getScene()->insert( m_zCrossNode );
        viewer->setCameraManipulator( 0 );

        m_axialWidgetWidth = static_cast< int >( viewer->getCamera()->getViewport()->width() );
        m_axialWidgetHeight = static_cast< int >( viewer->getCamera()->getViewport()->height() );
    }

    viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Sagittal View" );
    if( viewer )
    {
        viewer->getScene()->insert( m_xSliceNode );
        viewer->getScene()->insert( m_xCrossNode );
        viewer->setCameraManipulator( 0 );

        m_sagittalWidgetWidth = static_cast< int >( viewer->getCamera()->getViewport()->width() );
        m_sagittalWidgetHeight = static_cast< int >( viewer->getCamera()->getViewport()->height() );
    }

    viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Coronal View" );
    if( viewer )
    {
        viewer->getScene()->insert( m_ySliceNode );
        viewer->getScene()->insert( m_yCrossNode );
        viewer->setCameraManipulator( 0 );

        m_coronalWidgetWidth = static_cast< int >( viewer->getCamera()->getViewport()->width() );
        m_coronalWidgetHeight = static_cast< int >( viewer->getCamera()->getViewport()->height() );
    }
}

void WMNavSlices::setSlicePosFromPick( WPickInfo pickInfo )
{
    // handle the pick information on the slice views
    if ( pickInfo.getViewerName() != "main" && pickInfo.getViewerName() != "" )
    {
        // this uses fixed windows size of 150x150 pixel
        boost::unique_lock< boost::shared_mutex > lock;
        lock = boost::unique_lock< boost::shared_mutex >( m_updateLock );

        double x = static_cast< double >( pickInfo.getPickPixelPosition().first );
        double y = static_cast< double >( pickInfo.getPickPixelPosition().second );
        double xPos = 0.0;
        double yPos = 0.0;
        double width;
        double height;
        double left, top;
        double aspectR;
        double scale;
        double MStPX;
        double MStPY;
        double scale1;
        double scale2;
        double m1;
        double m2;
        double sizeX;
        double sizeY;
        // z slice
        if ( pickInfo.getViewerName() == "Axial View" )
        {
            width = m_bb.second[0] - m_bb.first[0] + 1;
            height = m_bb.second[1] - m_bb.first[1] + 1;
            left = m_bb.first[0];
            top = m_bb.first[1];

            aspectR = static_cast< double >( m_axialWidgetWidth ) / static_cast< double >( m_axialWidgetHeight );
            {
                MStPX = 240.0 * aspectR / static_cast< double >( m_axialWidgetWidth );
                MStPY = 240.0 / static_cast< double >( m_axialWidgetHeight );
                scale1 = 240.0 * aspectR / width;
                scale2 = 240.0 / height;
                m1 = height * scale1 / MStPY;
                m2 = width  * scale2 / MStPX;

                if( m1 <= static_cast< double >( m_axialWidgetHeight ) && m2 <= static_cast< double >( m_axialWidgetWidth ) )
                    scale = std::max( scale1, scale2 );
                else
                    if ( m2 > static_cast< double >( m_axialWidgetWidth ) )
                        scale = scale1;
                    else
                        scale = scale2;

                sizeX = width * scale / MStPX;
                sizeY = height * scale / MStPY;
                xPos = ( ( x - ( static_cast< double >( m_axialWidgetWidth ) - sizeX ) / 2 ) / sizeX ) * width + left;
                yPos = ( ( y - ( static_cast< double >( m_axialWidgetHeight ) - sizeY ) / 2 ) / sizeY ) * height + top;
            }

            xPos = xPos < m_bb.first[0] ? m_bb.first[0] : xPos;
            xPos = xPos > m_bb.second[0] ? m_bb.second[0] : xPos;
            yPos = yPos < m_bb.first[1] ? m_bb.first[1] : yPos;
            yPos = yPos > m_bb.second[1] ? m_bb.second[1] : yPos;

            m_sagittalPos->set( xPos );
            m_coronalPos->set( yPos );
        }

        // x slice
        if ( pickInfo.getViewerName() == "Sagittal View" )
        {
            width = m_bb.second[1] - m_bb.first[1] + 1;
            height = m_bb.second[2] - m_bb.first[2] + 1;
            left = m_bb.first[1];
            top = m_bb.first[2];

            aspectR = static_cast< double >( m_sagittalWidgetWidth ) / static_cast< double >( m_sagittalWidgetHeight );
            {
                MStPX = 240.0 * aspectR / static_cast< double >( m_sagittalWidgetWidth );
                MStPY = 240.0 / static_cast< double >( m_sagittalWidgetHeight );
                scale1 = 240.0 * aspectR / width;
                scale2 = 240.0 / height;
                m1 = height * scale1 / MStPY;
                m2 = width  * scale2 / MStPX;

                if( m1 <= static_cast< double >( m_sagittalWidgetHeight ) && m2 <= static_cast< double >( m_sagittalWidgetWidth ) )
                    scale = std::max( scale1, scale2 );
                else
                    if ( m2 > static_cast< double >( m_sagittalWidgetWidth ) )
                        scale = scale1;
                    else
                        scale = scale2;

                sizeX = width * scale / MStPX;
                sizeY = height * scale / MStPY;
                xPos = ( ( x - ( static_cast< double >( m_sagittalWidgetWidth ) - sizeX ) / 2 ) / sizeX ) * width + left;
                yPos = ( ( y - ( static_cast< double >( m_sagittalWidgetHeight ) - sizeY ) / 2 ) / sizeY ) * height + top;
            }
            xPos = m_bb.second[1] - xPos + left;
            xPos = xPos < m_bb.first[1] ? m_bb.first[1] : xPos;
            xPos = xPos > m_bb.second[1] ? m_bb.second[1] : xPos;
            yPos = yPos < m_bb.first[2] ? m_bb.first[2] : yPos;
            yPos = yPos > m_bb.second[2] ? m_bb.second[2] : yPos;

            m_coronalPos->set( xPos );
            m_axialPos->set( yPos );
        }

        // y slice
        if ( pickInfo.getViewerName() == "Coronal View" )
        {
            width = m_bb.second[0] - m_bb.first[0] + 1;
            height = m_bb.second[2] - m_bb.first[2] + 1;
            left = m_bb.first[0];
            top = m_bb.first[2];

            aspectR = static_cast< double >( m_coronalWidgetWidth ) / static_cast< double >( m_coronalWidgetHeight );
            {
                MStPX = 240.0 * aspectR / static_cast< double >( m_coronalWidgetWidth );
                MStPY = 240.0 / static_cast< double >( m_coronalWidgetHeight );
                scale1 = 240.0 * aspectR / width;
                scale2 = 240.0 / height;
                m1 = height * scale1 / MStPY;
                m2 = width  * scale2 / MStPX;

                if( m1 <= static_cast< double >( m_sagittalWidgetHeight ) && m2 <= static_cast< double >( m_coronalWidgetWidth ) )
                    scale = std::max( scale1, scale2 );
                else
                    if ( m2 > static_cast< double >( m_coronalWidgetWidth ) )
                        scale = scale1;
                    else
                        scale = scale2;

                sizeX = width * scale / MStPX;
                sizeY = height * scale / MStPY;
                xPos = ( ( x - ( static_cast< double >( m_coronalWidgetWidth ) - sizeX ) / 2 ) / sizeX ) * width + left;
                yPos = ( ( y - ( static_cast< double >( m_coronalWidgetHeight ) - sizeY ) / 2 ) / sizeY ) * height + top;
            }
            xPos = xPos < m_bb.first[0] ? m_bb.first[0] : xPos;
            xPos = xPos > m_bb.second[0] ? m_bb.second[0] : xPos;
            yPos = yPos < m_bb.first[2] ? m_bb.first[2] : yPos;
            yPos = yPos > m_bb.second[2] ? m_bb.second[2] : yPos;

            m_sagittalPos->set( xPos );
            m_axialPos->set( yPos );
        }

        return;
    }

    if ( ( pickInfo.getName() == "Axial Slice"
         ||  pickInfo.getName() == "Coronal Slice"
         ||  pickInfo.getName() == "Sagittal Slice" ) && pickInfo.getMouseButton() == WPickInfo::MOUSE_RIGHT )
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
            float diff = normal * moveDirWorld;

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
                m_coronalPos->set( m_coronalPos->get() - diff ); // minus here because of the order of the points of the slice
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

void WMNavSlices::setMaxMinFromBoundingBox()
{
    // grab a list of data textures
    std::vector< boost::shared_ptr< WDataTexture3D > > tex; // = WDataHandler::getDefaultSubject()->getDataTextures( true );

    if ( tex.size() > 0 )
    {
        std::pair< wmath::WPosition, wmath::WPosition > bb = tex[0]->getGrid()->getBoundingBox();

        for( size_t i = 1; i < tex.size(); ++i )
        {
            std::pair< wmath::WPosition, wmath::WPosition > bbTmp = tex[i]->getGrid()->getBoundingBox();
            bb.first[0] = bb.first[0] < bbTmp.first[0] ? bb.first[0] : bbTmp.first[0];
            bb.first[1] = bb.first[1] < bbTmp.first[1] ? bb.first[1] : bbTmp.first[1];
            bb.first[2] = bb.first[2] < bbTmp.first[2] ? bb.first[2] : bbTmp.first[2];
            bb.second[0] = bb.second[0] > bbTmp.second[0] ? bb.second[0] : bbTmp.second[0];
            bb.second[1] = bb.second[1] > bbTmp.second[1] ? bb.second[1] : bbTmp.second[1];
            bb.second[2] = bb.second[2] > bbTmp.second[2] ? bb.second[2] : bbTmp.second[2];
        }

        m_bb = bb;

        m_sagittalPos->setMin( bb.first[0] );
        m_sagittalPos->setMax( bb.second[0] );
        m_coronalPos->setMin( bb.first[1] );
        m_coronalPos->setMax( bb.second[1] );
        m_axialPos->setMin( bb.first[2] );
        m_axialPos->setMax( bb.second[2] );
    }
}

osg::ref_ptr<osg::Geometry> WMNavSlices::createGeometry( int slice )
{
    const size_t nbVerts = 4;

    float xSlice = static_cast< float >( m_sagittalPos->get( true ) );
    float ySlice = static_cast< float >( m_coronalPos->get( true ) );
    float zSlice = static_cast< float >( m_axialPos->get( true ) );

    float xPos = xSlice;
    float yPos = ySlice;
    float zPos = zSlice;

    osg::ref_ptr<osg::Geometry> sliceGeometry = osg::ref_ptr<osg::Geometry>( new osg::Geometry() );

    osg::Vec3Array* sliceVertices = new osg::Vec3Array;

    // grab a list of data textures
    std::vector< boost::shared_ptr< WDataTexture3D > > tex; // = WDataHandler::getDefaultSubject()->getDataTextures( true );


    if ( tex.size() > 0 )
    {
        setMaxMinFromBoundingBox();

        switch ( slice )
        {
            case 0:
            {
                std::vector< wmath::WPosition > vertices;
                vertices.push_back( wmath::WPosition( xPos, m_bb.first[1],  m_bb.first[2]   ) );
                vertices.push_back( wmath::WPosition( xPos, m_bb.first[1],  m_bb.second[2]  ) );
                vertices.push_back( wmath::WPosition( xPos, m_bb.second[1], m_bb.second[2]  ) );
                vertices.push_back( wmath::WPosition( xPos, m_bb.second[1], m_bb.first[2]   ) );
                for( size_t i = 0; i < nbVerts; ++i )
                {
                    sliceVertices->push_back( vertices[i] );
                }

                sliceGeometry->setVertexArray( sliceVertices );

                int c = 0;
                //////////////////////////////////////////////////////////////////////////////////////////////////
                if ( WKernel::getRunningKernel()->getSelectionManager()->getUseTexture() )
                {
                    boost::shared_ptr< WGridRegular3D > grid = WKernel::getRunningKernel()->getSelectionManager()->getGrid();
                    osg::Vec3Array* texCoords = new osg::Vec3Array;
                    for( size_t i = 0; i < nbVerts; ++i )
                    {
                        texCoords->push_back( grid->worldCoordToTexCoord( vertices[i] ) );
                    }
                    sliceGeometry->setTexCoordArray( c, texCoords );
                    ++c;
                }
                //////////////////////////////////////////////////////////////////////////////////////////////////

                for ( std::vector< boost::shared_ptr< WDataTexture3D > >::const_iterator iter = tex.begin(); iter != tex.end(); ++iter )
                {
                    boost::shared_ptr< WGridRegular3D > grid = ( *iter )->getGrid();

                    osg::Vec3Array* texCoords = new osg::Vec3Array;
                    for( size_t i = 0; i < nbVerts; ++i )
                    {
                        texCoords->push_back( grid->worldCoordToTexCoord( vertices[i] ) );
                    }
                    sliceGeometry->setTexCoordArray( c, texCoords );
                    ++c;
                }

                break;
            }
            case 1:
            {
                std::vector< wmath::WPosition > vertices;
                vertices.push_back( wmath::WPosition( m_bb.first[0],   yPos, m_bb.first[2]  ) );
                vertices.push_back( wmath::WPosition( m_bb.first[0],   yPos, m_bb.second[2] ) );
                vertices.push_back( wmath::WPosition( m_bb.second[0],  yPos, m_bb.second[2] ) );
                vertices.push_back( wmath::WPosition( m_bb.second[0],  yPos, m_bb.first[2]  ) );
                for( size_t i = 0; i < nbVerts; ++i )
                {
                    sliceVertices->push_back( vertices[i] );
                }
                sliceGeometry->setVertexArray( sliceVertices );

                int c = 0;
                //////////////////////////////////////////////////////////////////////////////////////////////////
                if ( WKernel::getRunningKernel()->getSelectionManager()->getUseTexture() )
                {
                    boost::shared_ptr< WGridRegular3D > grid = WKernel::getRunningKernel()->getSelectionManager()->getGrid();
                    osg::Vec3Array* texCoords = new osg::Vec3Array;
                    for( size_t i = 0; i < nbVerts; ++i )
                    {
                        texCoords->push_back( grid->worldCoordToTexCoord( vertices[i] ) );
                    }
                    sliceGeometry->setTexCoordArray( c, texCoords );
                    ++c;
                }
                //////////////////////////////////////////////////////////////////////////////////////////////////

                for ( std::vector< boost::shared_ptr< WDataTexture3D > >::const_iterator iter = tex.begin(); iter != tex.end(); ++iter )
                {
                    boost::shared_ptr< WGridRegular3D > grid = ( *iter )->getGrid();

                    osg::Vec3Array* texCoords = new osg::Vec3Array;
                    for( size_t i = 0; i < nbVerts; ++i )
                    {
                        texCoords->push_back( grid->worldCoordToTexCoord( vertices[i] ) );
                    }
                    sliceGeometry->setTexCoordArray( c, texCoords );
                    ++c;
                }
                break;
            }
            case 2:
            {
                std::vector< wmath::WPosition > vertices;
                vertices.push_back( wmath::WPosition( m_bb.first[0],  m_bb.first[1],  zPos ) );
                vertices.push_back( wmath::WPosition( m_bb.first[0],  m_bb.second[1], zPos ) );
                vertices.push_back( wmath::WPosition( m_bb.second[0], m_bb.second[1], zPos ) );
                vertices.push_back( wmath::WPosition( m_bb.second[0], m_bb.first[1],  zPos ) );
                for( size_t i = 0; i < nbVerts; ++i )
                {
                    sliceVertices->push_back( vertices[i] );
                }

                sliceGeometry->setVertexArray( sliceVertices );

                int c = 0;
                //////////////////////////////////////////////////////////////////////////////////////////////////
                if ( WKernel::getRunningKernel()->getSelectionManager()->getUseTexture() )
                {
                    boost::shared_ptr< WGridRegular3D > grid = WKernel::getRunningKernel()->getSelectionManager()->getGrid();
                    osg::Vec3Array* texCoords = new osg::Vec3Array;
                    for( size_t i = 0; i < nbVerts; ++i )
                    {
                        texCoords->push_back( grid->worldCoordToTexCoord( vertices[i] ) );
                    }
                    sliceGeometry->setTexCoordArray( c, texCoords );
                    ++c;
                }
                //////////////////////////////////////////////////////////////////////////////////////////////////

                for ( std::vector< boost::shared_ptr< WDataTexture3D > >::const_iterator iter = tex.begin(); iter != tex.end(); ++iter )
                {
                    boost::shared_ptr< WGridRegular3D > grid = ( *iter )->getGrid();

                    osg::Vec3Array* texCoords = new osg::Vec3Array;

                    for( size_t i = 0; i < nbVerts; ++i )
                    {
                        texCoords->push_back( grid->worldCoordToTexCoord( vertices[i] ) );
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

osg::ref_ptr<osg::Geometry> WMNavSlices::createCrossGeometry( int slice )
{
    float xSlice = static_cast< float >( m_sagittalPos->get( true ) );
    float ySlice = static_cast< float >( m_coronalPos->get( true ) );
    float zSlice = static_cast< float >( m_axialPos->get( true ) );

    float xPos = xSlice + 0.5f;
    float yPos = ySlice + 0.5f;
    float zPos = zSlice + 0.5f;

    osg::ref_ptr<osg::Geometry> geometry = osg::ref_ptr<osg::Geometry>( new osg::Geometry() );

    osg::Vec3Array* crossVertices = new osg::Vec3Array;
    osg::Vec3Array* colorArray = new osg::Vec3Array;

    float minx = m_bb.first[0];
    float miny = m_bb.first[1];
    float minz = m_bb.first[2];
    float maxx = m_bb.second[0];
    float maxy = m_bb.second[1];
    float maxz = m_bb.second[2];

    std::vector< wmath::WPosition > vertices;

    switch ( slice )
    {
    case 0:
        {
            vertices.push_back( wmath::WPosition( xPos - 1.0f, miny, zPos ) );
            vertices.push_back( wmath::WPosition( xPos - 1.0f, maxy, zPos ) );
            vertices.push_back( wmath::WPosition( xPos - 1.0f, yPos, minz ) );
            vertices.push_back( wmath::WPosition( xPos - 1.0f, yPos, maxz ) );
            break;
        }
    case 1:
        {
            vertices.push_back( wmath::WPosition( minx, yPos - 1.0f, zPos ) );
            vertices.push_back( wmath::WPosition( maxx, yPos - 1.0f, zPos ) );
            vertices.push_back( wmath::WPosition( xPos, yPos - 1.0f, minz ) );
            vertices.push_back( wmath::WPosition( xPos, yPos - 1.0f, maxz ) );
            break;
        }
    case 2:
        {
            vertices.push_back( wmath::WPosition( minx, yPos, zPos + 1.0f ) );
            vertices.push_back( wmath::WPosition( maxx, yPos, zPos + 1.0f ) );
            vertices.push_back( wmath::WPosition( xPos, miny, zPos + 1.0f ) );
            vertices.push_back( wmath::WPosition( xPos, maxy, zPos + 1.0f ) );
            break;
        }
    }

    for( size_t i = 0; i < vertices.size(); ++i )
    {
        crossVertices->push_back( vertices[i] );
    }

    colorArray->push_back( osg::Vec3( 1.0f, 0.0f, 0.0f ) );

    geometry->setColorArray( colorArray );
    geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    geometry->setVertexArray( crossVertices );

    unsigned int rawIndexData[] = { 0, 1, 2, 3 }; // NOLINT
    std::vector< unsigned int > indexData( rawIndexData, rawIndexData + sizeof( rawIndexData ) / sizeof( unsigned int ) );
    osg::DrawElementsUInt* lines = new osg::DrawElementsUInt( osg::PrimitiveSet::LINES, indexData.begin(), indexData.end() );

    geometry->addPrimitiveSet( lines );

    // disable light for this geode as lines can't be lit properly
    osg::StateSet* state = geometry->getOrCreateStateSet();
    state->setMode( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );
    state->setMode( GL_BLEND, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );

    osg::LineWidth* linewidth = new osg::LineWidth();
    linewidth->setWidth( 1.1f );
    state->setAttributeAndModes( linewidth, osg::StateAttribute::ON );

    return geometry;
}

void WMNavSlices::updateGeometry()
{
    {
        osg::ref_ptr< osg::Viewport > port;
        boost::shared_ptr< WGEViewer > viewer;
        int axialWidgetWidth = 0;
        int axialWidgetHeight = 0;
        int sagittalWidgetWidth = 0;
        int sagittalWidgetHeight = 0;
        int coronalWidgetWidth = 0;
        int coronalWidgetHeight = 0;
        // the view might not exists. Check for NULL here.
        viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Axial View" );
        if ( viewer )
        {
            port = viewer->getCamera()->getViewport();
            axialWidgetWidth = static_cast< int >( port->width() );
            axialWidgetHeight = static_cast< int >( port->height() );
        }

        viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Sagittal View" );
        if ( viewer )
        {
            port = viewer->getCamera()->getViewport();
            sagittalWidgetWidth = static_cast< int >( port->width() );
            sagittalWidgetHeight = static_cast< int >( port->height() );
        }

        viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Coronal View" );
        if ( viewer )
        {
            port = viewer->getCamera()->getViewport();
            coronalWidgetWidth = static_cast< int >( port->width() );
            coronalWidgetHeight = static_cast< int >( port->height() );
        }

        if ( axialWidgetWidth    != m_axialWidgetWidth    || axialWidgetHeight    != m_axialWidgetHeight    ||
             sagittalWidgetWidth != m_sagittalWidgetWidth || sagittalWidgetHeight != m_sagittalWidgetHeight ||
             coronalWidgetWidth  != m_coronalWidgetWidth  || coronalWidgetHeight  != m_coronalWidgetHeight )
        {
            m_axialWidgetWidth     = axialWidgetWidth;
            m_axialWidgetHeight    = axialWidgetHeight;
            m_sagittalWidgetWidth  = sagittalWidgetWidth;
            m_sagittalWidgetHeight = sagittalWidgetHeight;
            m_coronalWidgetWidth   = coronalWidgetWidth;
            m_coronalWidgetHeight  = coronalWidgetHeight;
            updateViewportMatrix();
        }
    }

    if ( m_textureChanged // Depends on call order of update routines in callback.
         || m_sagittalPos->changed()
         || m_coronalPos->changed()
         || m_axialPos->changed()
        )
    {
        // update the information property
        m_currentPosition->set( wmath::WPosition( m_axialPos->get(), m_coronalPos->get(), m_sagittalPos->get() ) );

        // if the texture got changed we want to rearange the scene Matrix for the SliceViewports to be centered
        updateViewportMatrix();

        osg::ref_ptr<osg::Geometry> xSliceGeometry = createGeometry( 0 );
        osg::ref_ptr<osg::Geometry> ySliceGeometry = createGeometry( 1 );
        osg::ref_ptr<osg::Geometry> zSliceGeometry = createGeometry( 2 );

        osg::ref_ptr<osg::Drawable> oldx = osg::ref_ptr<osg::Drawable>( m_xSliceNode->getDrawable( 0 ) );
        m_xSliceNode->replaceDrawable( oldx, xSliceGeometry );
        osg::ref_ptr<osg::Drawable> oldy = osg::ref_ptr<osg::Drawable>( m_ySliceNode->getDrawable( 0 ) );
        m_ySliceNode->replaceDrawable( oldy, ySliceGeometry );
        osg::ref_ptr<osg::Drawable> oldz = osg::ref_ptr<osg::Drawable>( m_zSliceNode->getDrawable( 0 ) );
        m_zSliceNode->replaceDrawable( oldz, zSliceGeometry );

        osg::ref_ptr<osg::Geometry> xCrossGeometry = createCrossGeometry( 0 );
        osg::ref_ptr<osg::Geometry> yCrossGeometry = createCrossGeometry( 1 );
        osg::ref_ptr<osg::Geometry> zCrossGeometry = createCrossGeometry( 2 );

        osg::ref_ptr<osg::Drawable> oldcx = osg::ref_ptr<osg::Drawable>( m_xCrossNode->getDrawable( 0 ) );
        m_xCrossNode->replaceDrawable( oldcx, xCrossGeometry );
        osg::ref_ptr<osg::Drawable> oldcy = osg::ref_ptr<osg::Drawable>( m_yCrossNode->getDrawable( 0 ) );
        m_yCrossNode->replaceDrawable( oldcy, yCrossGeometry );
        osg::ref_ptr<osg::Drawable> oldcz = osg::ref_ptr<osg::Drawable>( m_zCrossNode->getDrawable( 0 ) );
        m_zCrossNode->replaceDrawable( oldcz, zCrossGeometry );
    }

    std::vector< boost::shared_ptr< WDataTexture3D > > tex; // = WDataHandler::getDefaultSubject()->getDataTextures( true );
    bool noSlices = ( tex.size() == 0 ) || !m_active->get();

    m_slicesSwitchNode->setChildValue( m_zSliceNode, m_showAxial->get() && !noSlices );
    m_slicesSwitchNode->setChildValue( m_ySliceNode, m_showCoronal->get() && !noSlices );
    m_slicesSwitchNode->setChildValue( m_xSliceNode, m_showSagittal->get() && !noSlices );
}


void WMNavSlices::updateTextures()
{
    osg::StateSet* rootState = m_slicesNode->getOrCreateStateSet();
    if ( m_textureChanged )
    {
        m_textureChanged = false;

        // grab a list of data textures
        std::vector< boost::shared_ptr< WDataTexture3D > > tex; // = WDataHandler::getDefaultSubject()->getDataTextures( true );

        if ( tex.size() > 0 )
        {
            // reset all uniforms
            for ( size_t i = 0; i < wlimits::MAX_NUMBER_OF_TEXTURES; ++i )
            {
                m_typeUniforms[i]->set( 0 );
            }

            // for each texture -> apply
            size_t c = 0;

            //////////////////////////////////////////////////////////////////////////////////////////////////
            if ( WKernel::getRunningKernel()->getSelectionManager()->getUseTexture() )
            {
                osg::ref_ptr<osg::Texture3D> texture3D = WKernel::getRunningKernel()->getSelectionManager()->getTexture();


                m_typeUniforms[c]->set( W_DT_UNSIGNED_CHAR  );
                m_thresholdUniforms[c]->set( 0.0f );
                m_alphaUniforms[c]->set( WKernel::getRunningKernel()->getSelectionManager()->getTextureOpacity() );
                m_cmapUniforms[c]->set( WKernel::getRunningKernel()->getSelectionManager()->getShader() );

                texture3D->setFilter( osg::Texture::MIN_FILTER, osg::Texture::NEAREST );
                texture3D->setFilter( osg::Texture::MAG_FILTER, osg::Texture::NEAREST );

                rootState->setTextureAttributeAndModes( c, texture3D, osg::StateAttribute::ON );
                ++c;
            }
            //////////////////////////////////////////////////////////////////////////////////////////////////

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
                float minValue = ( *iter )->getMinValue();
                float maxValue = ( *iter )->getMaxValue();
                float t = ( ( *iter )->getThreshold() - minValue ) / ( maxValue - minValue ); // rescale to [0,1]
                float a = ( *iter )->getAlpha();
                int cmap = ( *iter )->getSelectedColormap();

                m_typeUniforms[c]->set( ( *iter )->getDataType() );
                m_thresholdUniforms[c]->set( t );
                m_alphaUniforms[c]->set( a );
                m_cmapUniforms[c]->set( cmap );

                ++c;
                if( c == wlimits::MAX_NUMBER_OF_TEXTURES )
                {
                    break;
                }
            }
        }
    }

    m_highlightUniformSagittal->set( m_isPickedSagittal );
    m_highlightUniformCoronal->set( m_isPickedCoronal );
    m_highlightUniformAxial->set( m_isPickedAxial );

    m_showCompleteUniform->set( m_showComplete->get() );

    m_xSliceNode->getOrCreateStateSet()->merge( *rootState );
    m_ySliceNode->getOrCreateStateSet()->merge( *rootState );
    m_zSliceNode->getOrCreateStateSet()->merge( *rootState );
}

void WMNavSlices::initUniforms( osg::StateSet* rootState )
{
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

    for ( size_t i = 0; i < wlimits::MAX_NUMBER_OF_TEXTURES; ++i )
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

    m_showCompleteUniform = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "showComplete", 0 ) );

    m_xSliceNode->getOrCreateStateSet()->addUniform( m_showCompleteUniform );
    m_ySliceNode->getOrCreateStateSet()->addUniform( m_showCompleteUniform );
    m_zSliceNode->getOrCreateStateSet()->addUniform( m_showCompleteUniform );
}

void WMNavSlices::updateViewportMatrix()
{
    setMaxMinFromBoundingBox();

    double aspectR;
    double top, left, width, height;
    double scale;
    boost::shared_ptr< WGEViewer > viewer;
    osg::ref_ptr< WGEGroupNode > currentScene;
    viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Axial View" );
    if( viewer )
    {
        currentScene = viewer->getScene();
        //aspectR = viewer->getCamera()->getViewport()->aspectRatio();

        left = m_bb.first[0];
        top = m_bb.first[1];
        width = m_bb.second[0] - m_bb.first[0];
        height = m_bb.second[1] - m_bb.first[1];
        aspectR = static_cast< double >( m_axialWidgetWidth ) / static_cast< double >( m_axialWidgetHeight );

        {
            double MStPX = 240.0 * aspectR / static_cast< double >( m_axialWidgetWidth );
            double MStPY = 240.0 / static_cast< double >( m_axialWidgetHeight );
            double scale1 = 240.0 * aspectR / width;
            double scale2 = 240.0 / height;
            double m1 = height * scale1 / MStPY;
            double m2 = width  * scale2 / MStPX;

            if( m1 <= static_cast< double >( m_axialWidgetHeight ) && m2 <= static_cast< double >( m_axialWidgetWidth ) )
                scale = std::max( scale1, scale2 );
            else
            if ( m2 > static_cast< double >( m_axialWidgetWidth ) )
                scale = scale1;
            else
                scale = scale2;
        }

        // 1. translate to center
        // 2. rotate around center
        // 3. scale to maximum
        osg::Matrix sm;
        osg::Matrix rm;
        osg::Matrix tm;

        tm.makeTranslate( osg::Vec3(
          -0.5 * ( m_bb.first[ 0 ] + m_bb.second[ 0 ] ),
          -0.5 * ( m_bb.first[ 1 ] + m_bb.second[ 1 ] ),
          -m_bb.second[ 2 ] + m_bb.first[ 2 ] - 0.5 ) );
        sm.makeScale( scale, scale, scale );

        tm *= sm;

        currentScene->setMatrix( tm );
    }

    viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Sagittal View" );
    if( viewer )
    {
        currentScene = viewer->getScene();
        aspectR = viewer->getCamera()->getViewport()->aspectRatio();

        left = m_bb.first[1];
        top = m_bb.first[2];
        width = m_bb.second[1] - m_bb.first[1];
        height = m_bb.second[2] - m_bb.first[2];
        aspectR = static_cast< double >( m_sagittalWidgetWidth ) / static_cast< double >( m_sagittalWidgetHeight );

        {
            double MStPX = 240.0 * aspectR / static_cast< double >( m_sagittalWidgetWidth );
            double MStPY = 240.0 / static_cast< double >( m_sagittalWidgetHeight );
            double scale1 = 240.0 * aspectR / width;
            double scale2 = 240.0 / height;
            double m1 = height * scale1 / MStPY;
            double m2 = width  * scale2 / MStPX;

            if( m1 <= static_cast< double >( m_sagittalWidgetHeight ) && m2 <= static_cast< double >( m_sagittalWidgetWidth ) )
                scale = std::max( scale1, scale2 );
            else
                if ( m2 > static_cast< double >( m_sagittalWidgetWidth ) )
                    scale = scale1;
                else
                    scale = scale2;
        }

        osg::Matrix rm;
        osg::Matrix rm2;
        osg::Matrix sm;
        osg::Matrix tm;

        tm.makeTranslate( osg::Vec3(
          m_bb.second[ 0 ] - m_bb.first[ 0 ] + 0.5,
          -0.5 * ( m_bb.first[ 1 ] + m_bb.second[ 1 ] ),
          -0.5 * ( m_bb.first[ 2 ] + m_bb.second[ 2 ] ) ) );
        rm.makeRotate( 90.0 * 3.141 / 180, 0.0, 1.0, 0.0 );
        rm2.makeRotate( 90.0 * 3.141 / 180, 0.0, 0.0, 1.0 );
        sm.makeScale( scale, scale, scale );

        tm *= rm;
        tm *= rm2;
        tm *= sm;

        currentScene->setMatrix( tm );
    }

    viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "Coronal View" );
    if( viewer )
    {
        currentScene = viewer->getScene();
        aspectR = viewer->getCamera()->getViewport()->aspectRatio();

        left = m_bb.first[0];
        top = m_bb.first[2];
        width = m_bb.second[0] - m_bb.first[0];
        height = m_bb.second[2] - m_bb.first[2];
        aspectR = static_cast< double >( m_coronalWidgetWidth ) / static_cast< double >( m_coronalWidgetHeight );

        {
            double MStPX = 240.0 * aspectR / static_cast< double >( m_coronalWidgetWidth );
            double MStPY = 240.0 / static_cast< double >( m_coronalWidgetHeight );
            double scale1 = 240.0 * aspectR / width;
            double scale2 = 240.0 / height;
            double m1 = height * scale1 / MStPY;
            double m2 = width  * scale2 / MStPX;

            if( m1 <= static_cast< double >( m_coronalWidgetHeight ) && m2 <= static_cast< double >( m_coronalWidgetWidth ) )
                scale = std::max( scale1, scale2 );
            else
                if ( m2 > static_cast< double >( m_coronalWidgetWidth ) )
                    scale = scale1;
                else
                    scale = scale2;
        }

        osg::Matrix sm;
        osg::Matrix rm;
        osg::Matrix tm;

        tm.makeTranslate( osg::Vec3(
          -0.5 * ( m_bb.first[ 0 ] + m_bb.second[ 0 ] ),
          m_bb.second[ 1 ] - m_bb.first[ 1 ] + 0.5,
          -0.5 * ( m_bb.first[ 2 ] + m_bb.second[ 2 ] ) ) );
        rm.makeRotate( -90.0 * 3.141 / 180, 1.0, 0.0, 0.0 );
        sm.makeScale( scale, scale, scale );

        tm *= rm;
        tm *= sm;

        currentScene->setMatrix( tm );
    }
}

void WMNavSlices::userData::updateGeometry()
{
    parent->updateGeometry();
}

void WMNavSlices::userData::updateTextures()
{
    parent->updateTextures();
}
