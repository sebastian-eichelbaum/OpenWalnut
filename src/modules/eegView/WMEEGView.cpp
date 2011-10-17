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

#include <string>
#include <vector>

#include <osg/LightModel>
#include <osgSim/ColorRange>

#include "core/dataHandler/WEEG2.h"
#include "core/dataHandler/WEEG2Segment.h"
#include "core/dataHandler/WEEGChannelInfo.h"
#include "core/dataHandler/WEEGValueMatrix.h"
#include "core/graphicsEngine/WGEGeodeUtils.h"
#include "core/graphicsEngine/WGEGeometryUtils.h"
#include "core/graphicsEngine/WGEGroupNode.h"
#include "core/graphicsEngine/WGEUtils.h"
#include "core/graphicsEngine/WROIBox.h"
#include "core/gui/WCustomWidget.h"
#include "core/gui/WGUI.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WROIManager.h"
#include "WEEGEvent.h"
#include "WEEGSourceCalculator.h"
#include "WEEGViewHandler.h"
#include "WElectrodePositionCallback.h"
#include "WHeadSurfaceCallback.h"
#include "WLabelsTransformCallback.h"
#include "WLineStripCallback.h"
#include "WMEEGView.h"
#include "WMEEGView.xpm"
#include "WPanTransformCallback.h"
#include "WScaleTransformCallback.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMEEGView )

WMEEGView::WMEEGView()
    : WModule(),
      m_dataChanged( new WCondition, true ),
      m_wasActive( false ),
      m_currentEventTime( -1.0 )
{
}

WMEEGView::~WMEEGView()
{
}

boost::shared_ptr< WModule > WMEEGView::factory() const
{
    return boost::shared_ptr< WModule >( new WMEEGView() );
}

const std::string WMEEGView::getName() const
{
    return "EEG View";
}

const std::string WMEEGView::getDescription() const
{
    return "Displays EEG data.";
}

const char** WMEEGView::getXPMIcon() const
{
    return eeg_xpm;
}

void WMEEGView::connectors()
{
    // initialize connectors
    m_input = boost::shared_ptr< WModuleInputData< WEEG2 > >( new WModuleInputData< WEEG2 >(
        shared_from_this(), "in", "Loaded EEG-dataset." ) );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_input );

    // call WModules initialization
    WModule::connectors();
}

void WMEEGView::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );
    m_drawElectrodes   = m_properties->addProperty( "Draw Electrodes",
                                                    "Draw the 3D positions of the electrodes.",
                                                    true,
                                                    m_propCondition );
    m_drawHeadSurface  = m_properties->addProperty( "Draw Head Surface",
                                                    "Draw the head surface between the electrodes.",
                                                    true,
                                                    m_propCondition );
    m_drawLabels       = m_properties->addProperty( "Draw Labels",
                                                    "Draw the labels of the electrodes at their 3D positions.",
                                                    true,
                                                    m_propCondition );
    m_labelsWidth      = m_properties->addProperty( "Labels Width",
                                                    "The width of the label display in pixel.",
                                                    24 );
    m_timePos          = m_properties->addProperty( "Time Position",
                                                    "The time position in seconds where to start the graph at the left edge.",
                                                    0.0 );
    m_timeRange        = m_properties->addProperty( "Time Range",
                                                    "The width of the graph in seconds.",
                                                    4.0 );
    m_graphWidth       = m_properties->addProperty( "Graph Width",
                                                    "The width of the graph in pixel.",
                                                    992 );
    m_yPos             = m_properties->addProperty( "Y Position",
                                                    "The y position in pixel at the lower edge.",
                                                    -724.5 );
    m_ySpacing         = m_properties->addProperty( "Spacing",
                                                    "The distance between two curves of the graph in pixel.",
                                                    23.0 );
    m_ySensitivity     = m_properties->addProperty( "Sensitivity",
                                                    "The sensitivity of the graph in microvolt per pixel.",
                                                    2.0 );
    m_colorSensitivity = m_properties->addProperty( "Color Sensitivity",
            "The sensitivity of the color map. It ranges from -Color Sensitivity to +Color Sensitivity in microvolt.",
                                                    23.0 );

    m_labelsWidth->setMin( 0 );
    m_labelsWidth->setMax( 64 );
    m_timePos->setMin( 0.0 );
    m_timePos->setMax( 86400.0 );
    m_timeRange->setMin( 0.01 );
    m_timeRange->setMax( 60.0 );
    m_graphWidth->setMin( 1 );
    m_graphWidth->setMax( 4096 );
    m_yPos->setMin( -1048576.0 );
    m_yPos->setMax( 0.0 );
    m_ySpacing->setMin( 0.0 );
    m_ySpacing->setMax( 1024.0 );
    m_ySensitivity->setMin( 0.001 );
    m_ySensitivity->setMax( 100.0 );
    m_colorSensitivity->setMin( 0.01 );
    m_colorSensitivity->setMax( 10000.0 );

    WModule::properties();
}

void WMEEGView::notifyConnectionEstablished(
    boost::shared_ptr< WModuleConnector > /*here*/, boost::shared_ptr< WModuleConnector > /*there*/ )
{
    m_dataChanged.set( true );
}

void WMEEGView::notifyDataChange(
    boost::shared_ptr< WModuleConnector > /*input*/, boost::shared_ptr< WModuleConnector > /*output*/ )
{
    m_dataChanged.set( true );
}

void WMEEGView::moduleMain()
{
    // do initialization
    // add conditions to m_moduleState
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_dataChanged.getCondition() );
    m_moduleState.add( m_active->getCondition() );
    m_moduleState.add( m_propCondition );

    // create WFlag for the event
    m_event = boost::shared_ptr< WFlag< boost::shared_ptr< WEEGEvent > > >( new WFlag< boost::shared_ptr< WEEGEvent > >(
            m_propCondition, boost::shared_ptr< WEEGEvent >() ) );

    {
        // create color map
        std::vector< osg::Vec4 > colors;
        colors.reserve( 3 );
        colors.push_back( osg::Vec4( 0.0, 0.0, 1.0, 1.0 ) ); // blue
        colors.push_back( osg::Vec4( 1.0, 1.0, 1.0, 1.0 ) ); // white
        colors.push_back( osg::Vec4( 1.0, 0.0, 0.0, 1.0 ) ); // red
        m_colorMap = new osgSim::ColorRange( -1.0, 1.0, colors );

        // create texture containing color map
        const int size = 256;
        osg::Image* image = new osg::Image;
        // allocate the image data, size x 1 x 1 with 4 rgba floats - equivalent to a Vec4!
        image->allocateImage( size, 1, 1, GL_RGBA, GL_FLOAT );
        image->setInternalTextureFormat( GL_RGBA );

        osg::Vec4* data = reinterpret_cast< osg::Vec4* >( image->data() );
        for( int i = 0; i < size; ++i)
        {
            data[i] = m_colorMap->getColor( ( 2 * i + 1 - size ) / static_cast< float >( size - 1 ) );
        }

        m_colorMapTexture = new osg::Texture1D( image );
        m_colorMapTexture->setWrap( osg::Texture1D::WRAP_S, osg::Texture1D::CLAMP_TO_EDGE );
        m_colorMapTexture->setFilter( osg::Texture1D::MIN_FILTER, osg::Texture1D::LINEAR );
    }

    // signal ready
    ready();

    while( !m_shutdownFlag() ) // loop until the module container requests the module to quit
    {
        // data changed?
        if( m_dataChanged() )
        {
            debugLog() << "Data changed";
            m_dataChanged.set( false );
            if( m_input.get() )
            {
                m_eeg = m_input->getData();
            }
            redraw();
        }

        // draw electrodes property changed?
        if( m_drawElectrodes->changed() )
        {
            if( m_electrodesNode.valid() )
            {
                m_rootNode3d->remove( m_electrodesNode );
            }

            if( m_drawElectrodes->get( true ) && m_eeg.get() )
            {
                m_electrodesNode = drawElectrodes();
                m_rootNode3d->insert( m_electrodesNode );
            }
            else
            {
                m_electrodesNode = NULL;
            }
        }

        // draw head surface property changed?
        if( m_drawHeadSurface->changed() )
        {
            if( m_headSurfaceNode.valid() )
            {
                m_rootNode3d->remove( m_headSurfaceNode );
            }

            if( m_drawHeadSurface->get( true ) && m_eeg.get() )
            {
                m_headSurfaceNode = drawHeadSurface();
                m_rootNode3d->insert( m_headSurfaceNode );
            }
            else
            {
                m_headSurfaceNode = NULL;
            }
        }

        // draw labels property changed?
        if( m_drawLabels->changed() )
        {
            if( m_labelsNode.valid() )
            {
                m_rootNode3d->remove( m_labelsNode );
            }

            if( m_drawLabels->get( true ) && m_eeg.get() )
            {
                m_labelsNode = drawLabels();
                m_rootNode3d->insert( m_labelsNode );
            }
            else
            {
                m_labelsNode = NULL;
            }
        }

        // event position changed?
        boost::shared_ptr< WEEGEvent > event = m_event->get();
        if( event && event->getTime() != m_currentEventTime )
        {
            debugLog() << "New event position: " << event->getTime();

            if( m_roi )
            {
                WKernel::getRunningKernel()->getRoiManager()->removeRoi( m_roi );
            }

            if(  m_sourceCalculator )
            {
                WPosition position = m_sourceCalculator->calculate( event );
                m_roi = new WROIBox( position - WVector3d( 5.0, 5.0, 5.0 ),
                                     position + WVector3d( 5.0, 5.0, 5.0 ) );
                WKernel::getRunningKernel()->getRoiManager()->addRoi( m_roi );
            }
            else
            {
                m_roi.release();
            }

            m_currentEventTime = event->getTime();
        }

        // "active" property changed?
        bool isActive = m_active->get();
        if( isActive != m_wasActive )
        {
            if( isActive )
            {
                if( m_rootNode3d.valid() )
                {
                    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode3d );
                }

                if( !openCustomWidget() )
                {
                    // Shut down module if widget could not be opened.
                    m_shutdownFlag.set( true );
                }
            }
            else
            {
                closeCustomWidget();
                if( m_rootNode3d.valid() )
                {
                    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode3d );
                }
            }
            m_wasActive = isActive;
        }

        m_moduleState.wait(); // waits for firing of m_moduleState ( dataChanged, shutdown, etc. )
    }

    // clean up
    if( m_wasActive )
    {
        closeCustomWidget();
        if( m_rootNode3d.valid() )
        {
            WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode3d );
        }
    }
}

bool WMEEGView::openCustomWidget()
{
    debugLog() << "Try to open EEG View widget...";
    m_widget = WKernel::getRunningKernel()->getGui()->openCustomWidget( getName(), WGECamera::TWO_D, m_shutdownFlag.getCondition() );
    bool success = m_widget.get();
    if( success )
    {
        debugLog() << "Succesfully opened EEG View widget.";

        if( m_handler )
        {
            m_widget->getViewer()->getView()->addEventHandler( m_handler );
        }

        if( m_rootNode2d.valid() )
        {
            debugLog() << "Adding rootNode to scene after opened EEG View widget";
            m_widget->getScene()->insert( m_rootNode2d );
        }
    }
    else
    {
        warnLog() << "Could not create EEG View widget.";
    }
    return success;
}

void WMEEGView::closeCustomWidget()
{
    if( m_rootNode2d.valid() )
    {
        m_widget->getScene()->remove( m_rootNode2d );
    }

    if( m_handler )
    {
        m_widget->getViewer()->getView()->getEventHandlers().remove( m_handler );
    }

    WKernel::getRunningKernel()->getGui()->closeCustomWidget( getName() );
}

void WMEEGView::redraw()
{
    if( m_wasActive )
    {
        if( m_rootNode2d.valid() )
        {
            m_widget->getScene()->remove( m_rootNode2d );
        }
        if( m_rootNode3d.valid() )
        {
            WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode3d );
        }
        if( m_handler )
        {
            m_widget->getViewer()->getView()->getEventHandlers().remove( m_handler );
        }
    }

    // reset event position
    m_event->set( boost::shared_ptr< WEEGEvent >( new WEEGEvent ) );

    if( m_eeg.get() && 0 < m_eeg->getNumberOfSegments() )
    {
        const float text2dOffset = 2.0f;
        const float text2dSize = 32.0f;
        const osg::Vec4 text2dColor( 0.0, 0.0, 0.0, 1.0 );
        const osg::Vec4 linesColor( 0.0, 0.0, 0.0, 1.0 );

        m_rootNode2d = new WGEGroupNode;
        osg::StateSet* stateset = m_rootNode2d->getOrCreateStateSet();
        stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

        m_rootNode3d = new WGEGroupNode;

        debugLog() << "Displaying EEG " << m_eeg->getFileName();
        debugLog() << "  Number of segments: " << m_eeg->getNumberOfSegments();
        size_t nbChannels = m_eeg->getNumberOfChannels();
        debugLog() << "  Number of channels: " << nbChannels;
        double rate = m_eeg->getSamplingRate();
        debugLog() << "  Sampling Rate: " << rate;
        debugLog() << "  Segment " << 0;
        boost::shared_ptr< WEEG2Segment > segment = m_eeg->getSegment( 0 );
        size_t nbSamples = segment->getNumberOfSamples();
        debugLog() << "    Number of Samples: " << nbSamples;

        // reset and adjust properties to given dataset
        m_ySpacing->set( 736.0 / nbChannels );
        m_yPos->set( 368.0 / nbChannels - 736.0 );
        m_timePos->set( 0.0 );
        m_timePos->setMax( nbSamples / rate );

        // draw 2D plot
        // create text geode for the channel labels
        osg::Geode* textGeode = new osg::Geode;

        // create pan matrix transfom node
        osg::MatrixTransform* panTransform = new osg::MatrixTransform;
        panTransform->setDataVariance( osg::Object::DYNAMIC );
        panTransform->setUpdateCallback( new WPanTransformCallback( m_labelsWidth, m_timePos, m_timeRange, m_graphWidth, m_yPos ) );

        for( size_t channelID = 0; channelID < nbChannels; ++channelID )
        {
            debugLog() << "    Channel " << channelID;
            boost::shared_ptr< WEEGChannelInfo > channelInfo = m_eeg->getChannelInfo( channelID );
            debugLog() << "      Channel unit: " << channelInfo->getUnit();
            debugLog() << "      Channel label: " << channelInfo->getLabel();

            // create text for the channel label
            osgText::Text* text = new osgText::Text;
            text->setText( channelInfo->getLabel() );
            text->setPosition( osg::Vec3( text2dOffset, -static_cast< float >( channelID ), 0.0f ) );
            text->setAlignment( osgText::Text::LEFT_CENTER );
            text->setAxisAlignment( osgText::Text::SCREEN );
            text->setCharacterSize( text2dSize );
            text->setCharacterSizeMode( osgText::Text::SCREEN_COORDS );
            text->setColor( text2dColor );

            textGeode->addDrawable( text );

            // create geode to draw the actual data as line strip
            osg::Geometry* geometry = new osg::Geometry;

            osg::Vec4Array* colors = new osg::Vec4Array;
            colors->push_back( linesColor );
            geometry->setColorArray( colors );
            geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

            geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, 0, 0 ) );

            geometry->setDataVariance( osg::Object::DYNAMIC );
            geometry->setUpdateCallback( new WLineStripCallback( channelID, m_timePos, m_timeRange, segment, rate ) );

            osg::Geode* linesGeode = new osg::Geode;
            linesGeode->addDrawable( geometry );

            // create scaling matrix transform node
            osg::MatrixTransform* scaleTransform = new osg::MatrixTransform;
            scaleTransform->setDataVariance( osg::Object::DYNAMIC );
            scaleTransform->setUpdateCallback( new WScaleTransformCallback( channelID, m_ySpacing, m_ySensitivity ) );

            // connect all creates nodes
            scaleTransform->addChild( linesGeode );
            panTransform->addChild( scaleTransform );
        }

        // create matrix transform node for the labels
        osg::MatrixTransform* labelsTransform = new osg::MatrixTransform;
        labelsTransform->setDataVariance( osg::Object::DYNAMIC );
        labelsTransform->setUpdateCallback( new WLabelsTransformCallback( m_yPos, m_ySpacing ) );
        labelsTransform->addChild( textGeode );

        // create group node as parent for the events
        WGEGroupNode* eventParentNode = new WGEGroupNode;
        panTransform->addChild( eventParentNode );

        // add labels and graph to the root node
        m_rootNode2d->addChild( labelsTransform );
        m_rootNode2d->addChild( panTransform );

        // create GUI event handler
        m_handler = new WEEGViewHandler( m_labelsWidth,
                                         m_timePos,
                                         m_timeRange,
                                         m_graphWidth,
                                         m_yPos,
                                         m_ySpacing,
                                         m_ySensitivity,
                                         m_colorSensitivity,
                                         m_event,
                                         eventParentNode,
                                         m_eeg,
                                         0 );

        // draw the electrode positions in 3D
        if( m_drawElectrodes->get( true ) )
        {
            m_electrodesNode = drawElectrodes();
            m_rootNode3d->addChild( m_electrodesNode );
        }
        else
        {
            m_electrodesNode = NULL;
        }

        // draw the head surface in 3D
        if( m_drawHeadSurface->get( true ) )
        {
            m_headSurfaceNode = drawHeadSurface();
            m_rootNode3d->addChild( m_headSurfaceNode );
        }
        else
        {
            m_headSurfaceNode = NULL;
        }

        // draw the electrode labels in 3D
        if( m_drawLabels->get( true ) )
        {
            m_labelsNode = drawLabels();
            m_rootNode3d->addChild( m_labelsNode );
        }
        else
        {
            m_labelsNode = NULL;
        }

        // add rootNode to scene
        if( m_wasActive )
        {
            debugLog() << "Adding rootNode to scene after redraw";
            m_widget->getScene()->insert( m_rootNode2d );

            WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode3d );

            m_widget->getViewer()->getView()->addEventHandler( m_handler );
        }

        // create new source calculator
        m_sourceCalculator = boost::shared_ptr< WEEGSourceCalculator >( new WEEGSourceCalculator( m_eeg ) );
    }
    else
    {
        m_rootNode2d = NULL;
        m_rootNode3d = NULL;
        m_handler = NULL;

        m_electrodesNode = NULL;
        m_headSurfaceNode = NULL;
        m_labelsNode = NULL;

        m_sourceCalculator.reset();
    }
}

osg::ref_ptr< osg::Node > WMEEGView::drawElectrodes()
{
    // draw 3d positions of electrodes
    const float sphereSize = 4.0f;

    osg::ref_ptr< osg::Group > electrodes( new osg::Group );

    for( size_t channelID = 0; channelID < m_eeg->getNumberOfChannels(); ++channelID )
    {
        boost::shared_ptr< WEEGChannelInfo > channelInfo = m_eeg->getChannelInfo( channelID );
        try
        {
            osg::Vec3 pos = channelInfo->getPosition();

            // create sphere geode on electrode position
            osg::ShapeDrawable* shape = new osg::ShapeDrawable( new osg::Sphere( pos, sphereSize ) );
            shape->setDataVariance( osg::Object::DYNAMIC );
            shape->setUpdateCallback( new WElectrodePositionCallback( channelID, m_colorSensitivity, m_event, m_colorMap ) );

            osg::Geode* sphereGeode = new osg::Geode;
            sphereGeode->addDrawable( shape );
            electrodes->addChild( sphereGeode );
        }
        catch( const WDHException& )
        {
            warnLog() << "The position of the electrode " << channelInfo->getLabel() << " is unknown.";
        }
    }

    return electrodes;
}

osg::ref_ptr< osg::Node > WMEEGView::drawHeadSurface()
{
    // draw head surface
    const size_t nbChannels = m_eeg->getNumberOfChannels();

    std::vector< WPosition > positions;
    positions.reserve( nbChannels );
    std::vector< std::size_t > channelIDs;
    channelIDs.reserve( nbChannels );
    for( size_t channelID = 0; channelID < nbChannels; ++channelID )
    {
        boost::shared_ptr< WEEGChannelInfo > channelInfo = m_eeg->getChannelInfo( channelID );
        try
        {
            WPosition position = channelInfo->getPosition();
            positions.push_back( position );
            channelIDs.push_back( channelID );
        }
        catch( const WDHException& )
        {
            warnLog() << "The position of the electrode " << channelInfo->getLabel() << " is unknown.";
        }
    }

    const std::size_t nbPositions = positions.size();

    osg::ref_ptr< osg::Geometry > geometry = wge::convertToOsgGeometry( wge::triangulate( positions, -0.005 ), WColor( 1.0, 1.0, 1.0, 1.0 ), true );

    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back( osg::Vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    osg::LightModel* lightModel = new osg::LightModel;
    lightModel->setTwoSided( true );
    osg::StateSet* state = geometry->getOrCreateStateSet();
    state->setAttributeAndModes( lightModel );

    state->setTextureAttributeAndModes( 0, m_colorMapTexture );
    osg::FloatArray* texCoords = new osg::FloatArray;
    texCoords->assign( nbPositions, 0.5f );
    geometry->setTexCoordArray( 0, texCoords );

    geometry->setDataVariance( osg::Object::DYNAMIC );
    geometry->setUpdateCallback( new WHeadSurfaceCallback( channelIDs, m_colorSensitivity, m_event ) );

    osg::ref_ptr< osg::Geode > surface( new osg::Geode );
    surface->addDrawable( geometry );
    return surface;
}

osg::ref_ptr< osg::Node > WMEEGView::drawLabels()
{
    // draw electrode labels in 3d
    const float sphereSize = 4.0f;
    const osg::Vec3 text3dOffset( 0.0, 0.0, sphereSize );
    const double text3dSize = 32.0;
    const osg::Vec4 text3dColor( 0.0, 0.0, 0.0, 1.0 );

    osg::ref_ptr< osg::Group > labels( new osg::Group );

    for( size_t channelID = 0; channelID < m_eeg->getNumberOfChannels(); ++channelID )
    {
        boost::shared_ptr< WEEGChannelInfo > channelInfo = m_eeg->getChannelInfo( channelID );
        try
        {
            osg::Vec3 pos = channelInfo->getPosition();

            // create text geode for the channel label
            osgText::Text* text = new osgText::Text;
            text->setText( channelInfo->getLabel() );
            text->setPosition( pos + text3dOffset );
            text->setAlignment( osgText::Text::CENTER_BOTTOM );
            text->setAxisAlignment( osgText::Text::SCREEN );
            text->setCharacterSize( text3dSize );
            text->setCharacterSizeMode( osgText::Text::SCREEN_COORDS );
            text->setColor( text3dColor );

            osg::Geode* textGeode = new osg::Geode;
            textGeode->addDrawable( text );
            labels->addChild( textGeode );
        }
        catch( const WDHException& )
        {
            warnLog() << "The position of the electrode " << channelInfo->getLabel() << " is unknown.";
        }
    }

    return labels;
}

