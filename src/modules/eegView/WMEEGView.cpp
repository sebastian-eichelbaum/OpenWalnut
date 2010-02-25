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

#include "../../graphicsEngine/WGEGeodeUtils.h"
#include "../../graphicsEngine/WGEGeometryUtils.h"
#include "../../graphicsEngine/WGEUtils.h"
#include "../../kernel/WKernel.h"
#include "WMEEGView.h"


WMEEGView::WMEEGView()
    : WModule(),
      m_dataChanged( new WCondition, true ),
      m_wasActive( false ),
      m_event( -1.0 ),
      m_eventPositionFlag( NULL )
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

void WMEEGView::connectors()
{
    // initialize connectors
    m_input = boost::shared_ptr< WModuleInputData< WEEG > >( new WModuleInputData< WEEG >(
        shared_from_this(), "in", "Loaded EEG-dataset." ) );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_input );

    // call WModules initialization
    WModule::connectors();
}

void WMEEGView::properties()
{
    // m_active gets initialized in WModule and is available for all modules. Overwrite activate() to have a special callback for m_active
    // changes or add a callback manually.
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

        // new event position marked?
        if( m_eventPositionFlag )
        {
            double eventPosition = m_eventPositionFlag->get();
            if( eventPosition != m_event.getTime() )
            {
                debugLog() << "New event position: " << eventPosition;
                updateEvent( &m_event, eventPosition );
            }
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
                    m_FinishRequested = true;
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
    m_widget = WKernel::getRunningKernel()->getGui()->openCustomWidget(
        getName(), WGECamera::TWO_D, m_shutdownFlag.getCondition() );
    bool success = m_widget.get();
    if( success )
    {
        debugLog() << "Succesfully opened EEG View widget.";
        m_eventPositionFlag = m_widget->getViewer()->getMarkHandler()->getPositionFlag();
        m_moduleState.add( m_eventPositionFlag->getCondition() );
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
    m_moduleState.remove( m_eventPositionFlag->getCondition() );
    m_eventPositionFlag = NULL;
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
    }

    if( m_eeg.get() )
    {
        const osg::Vec3 text2dOffset( -8.0, 0.0, 0.0 );
        const double text2dSize = 32.0;
        const osg::Vec4 text2dColor( 0.0, 0.0, 0.0, 1.0 );
        const osg::Vec4 linesColor( 0.0, 0.0, 0.0, 1.0 );
        const osg::Matrix scaleMatrix = osg::Matrix::scale( 1.0, 4.0, 1.0 );
        const double xOffset = 0.0;
        const unsigned int spacing = 16;

        const float sphereSize = 4.0f;
        const osg::Vec3 text3dOffset( 0.0, 0.0, sphereSize );
        const double text3dSize = 32.0;
        const osg::Vec4 text3dColor( 0.0, 0.0, 0.0, 1.0 );

        m_rootNode2d = new WGEGroupNode;
        osg::StateSet* stateset = m_rootNode2d->getOrCreateStateSet();
        stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

        m_rootNode3d = new WGEGroupNode;

        debugLog() << "Displaying EEG " << m_eeg->getFileName();
        debugLog() << "  Number of segments: " << m_eeg->getNumberOfSegments();
        size_t nbChannels = m_eeg->getNumberOfChannels();
        debugLog() << "  Number of channels: " << nbChannels;

        // draw 2D plot
        for( size_t segment = 0; segment < m_eeg->getNumberOfSegments(); ++segment )
        {
            debugLog() << "  Segment " << segment;
            size_t nbSamples = m_eeg->getNumberOfSamples( segment );
            debugLog() << "    Number of Samples: " << nbSamples;

            for( size_t channel = 0; channel < nbChannels; ++channel )
            {
                debugLog() << "    Channel " << channel;
                debugLog() << "      Channel label: " << m_eeg->getChannelLabel( channel );

                // create text geode for the channel label
                osgText::Text* text = new osgText::Text;
                text->setText( m_eeg->getChannelLabel( channel ) );
                text->setPosition( text2dOffset );
                text->setAlignment( osgText::Text::RIGHT_CENTER );
                text->setAxisAlignment( osgText::Text::SCREEN );
                text->setCharacterSize( text2dSize );
                text->setCharacterSizeMode( osgText::Text::SCREEN_COORDS );
                text->setColor( text2dColor );

                osg::Geode* textGeode = new osg::Geode;
                textGeode->addDrawable( text );

                // create geode to draw the actual data as line strip
                osg::Geometry* geometry = new osg::Geometry;

                osg::Vec3Array* vertices = new osg::Vec3Array( nbSamples );
                for( size_t sample = 0; sample < nbSamples; ++sample )
                {
                    (*vertices)[sample] = osg::Vec3( sample, (*m_eeg)( segment, channel, sample ), 0.0 );
                }
                geometry->setVertexArray( vertices );

                osg::Vec4Array* colors = new osg::Vec4Array;
                colors->push_back( linesColor );
                geometry->setColorArray( colors );
                geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

                geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, 0, nbSamples ) );

                osg::Geode* linesGeode = new osg::Geode;
                linesGeode->addDrawable( geometry );

                // create the matrix transform nodes
                osg::MatrixTransform* scale = new osg::MatrixTransform;
                scale->setMatrix( scaleMatrix );

                osg::MatrixTransform* translate = new osg::MatrixTransform;
                translate->setMatrix( osg::Matrix::translate( xOffset, 2 * spacing * ( nbChannels - channel ) - spacing, 0.0 ) );

                // connect all creates nodes
                scale->addChild( linesGeode );
                translate->addChild( textGeode );
                translate->addChild( scale );
                m_rootNode2d->addChild( translate );
            }
        }

        // draw event marker
        if( m_event.getNode().valid() && 0.0 < m_event.getTime() && m_event.getTime() <= m_eeg->getNumberOfSamples( 0 ) - 1 )
        {
            m_rootNode2d->addChild( m_event.getNode() );
        }
        else
        {
            m_event.setTime( -1.0 );
            m_event.setNode( NULL );
        }

        // draw 3d positions
        for( size_t channel = 0; channel < nbChannels; ++channel )
        {
            osg::Vec3 pos = wge::osgVec3( m_eeg->getChannelPosition( channel ) );

            // create sphere geode on electrode position
            osg::ShapeDrawable* shape = new osg::ShapeDrawable( new osg::Sphere( pos, sphereSize ) );
            shape->setUpdateCallback( new UpdateColorOfShapeDrawableCallback( channel, &m_event, m_eeg, m_colorMap ) );

            osg::Geode* sphereGeode = new osg::Geode;
            sphereGeode->addDrawable( shape );
            m_rootNode3d->addChild( sphereGeode );

            // create text geode for the channel label
            osgText::Text* text = new osgText::Text;
            text->setText( m_eeg->getChannelLabel( channel ) );
            text->setPosition( pos + text3dOffset );
            text->setAlignment( osgText::Text::CENTER_BOTTOM );
            text->setAxisAlignment( osgText::Text::SCREEN );
            text->setCharacterSize( text3dSize );
            text->setCharacterSizeMode( osgText::Text::SCREEN_COORDS );
            text->setColor( text3dColor );

            osg::Geode* textGeode = new osg::Geode;
            textGeode->addDrawable( text );
            m_rootNode3d->addChild( textGeode );
        }

        // draw head surface
        std::vector< wmath::WPosition > positions;
        positions.reserve( nbChannels );
        for( size_t channel = 0; channel < nbChannels; ++channel )
        {
            positions.push_back( m_eeg->getChannelPosition( channel ) );
        }

        WTriangleMesh mesh = wge::triangulate( positions );
        osg::ref_ptr< osg::Geometry > geometry = wge::convertToOsgGeometry( &mesh, true );

        osg::Vec4Array* colors = new osg::Vec4Array;
        colors->push_back( osg::Vec4( 1.0, 1.0, 1.0, 1.0 ) );
        geometry->setColorArray( colors );
        geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

        osg::LightModel* lightModel = new osg::LightModel;
        lightModel->setTwoSided( true );
        osg::StateSet* state = geometry->getOrCreateStateSet();
        state->setAttributeAndModes( lightModel );

        state->setTextureAttributeAndModes( 0, m_colorMapTexture );
        osg::FloatArray* texCoords = new osg::FloatArray;
        texCoords->reserve( nbChannels );
        for( size_t channelID = 0; channelID < nbChannels; ++channelID )
        {
            texCoords->push_back( 0.5f );
        }
        geometry->setTexCoordArray( 0, texCoords );
        geometry->setUpdateCallback( new UpdateColorOfGeometryCallback( &m_event, m_eeg ) );

        osg::Geode* geode = new osg::Geode;
        geode->addDrawable( geometry );
        m_rootNode3d->addChild( geode );

        // add rootNode to scene
        if( m_wasActive )
        {
            debugLog() << "Adding rootNode to scene after redraw";
            m_widget->getScene()->insert( m_rootNode2d );

            WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode3d );
        }
    }
    else
    {
        m_rootNode2d = NULL;
        m_rootNode3d = NULL;
        m_event.setTime( -1.0 );
        m_event.setNode( NULL );
    }
}

void WMEEGView::updateEvent( WEvent* event, double time )
{
    if( m_eeg.get() && 0.0 <= time && time <= m_eeg->getNumberOfSamples( 0 ) - 1 )
    {
        const osg::Vec4 color( 0.75, 0.0, 0.0, 1.0 );
        const unsigned int spacing = 16;

        // create geode to draw the event as line
        osg::Geometry* geometry = new osg::Geometry;

        osg::Vec3Array* vertices = new osg::Vec3Array( 2 );
        (*vertices)[0] = osg::Vec3( time, 0.0, 0.0 );
        (*vertices)[1] = osg::Vec3( time, 2 * spacing * m_eeg->getNumberOfChannels(), 0.0 );
        geometry->setVertexArray( vertices );

        osg::Vec4Array* colors = new osg::Vec4Array;
        colors->push_back( color );
        geometry->setColorArray( colors );
        geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

        geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINES, 0, 2 ) );

        osg::Geode* geode = new osg::Geode;
        geode->addDrawable( geometry );

        if( m_rootNode2d.valid() )
        {
            if( event->getNode().valid() )
            {
                m_rootNode2d->remove( event->getNode() );
            }
            m_rootNode2d->insert( geode );
        }

        event->setTime( time );
        event->setNode( geode );
    }
}

WMEEGView::UpdateColorOfShapeDrawableCallback::UpdateColorOfShapeDrawableCallback(
        size_t channel,
        const WEvent* event,
        boost::shared_ptr< const WEEG > eeg,
        osg::ref_ptr< const osgSim::ScalarsToColors > colorMap )
    : m_channel( channel ),
      m_event( event ),
      m_eeg( eeg ),
      m_colorMap( colorMap )
{
    assert( channel < eeg->getNumberOfChannels() );
    m_currentTime = -2.0;
}

void WMEEGView::UpdateColorOfShapeDrawableCallback::update( osg::NodeVisitor* /*nv*/, osg::Drawable* drawable )
{
    const double newTime = m_event->getTime();
    if( newTime != m_currentTime )
    {
        osg::ShapeDrawable* shape = static_cast< osg::ShapeDrawable* >( drawable );
        if( shape )
        {
            // calculate color value between -1 and 1
            float color;
            if( 0.0 <= newTime && newTime <= m_eeg->getNumberOfSamples( 0 ) - 1 )
            {
                const float scale = 0.5;
                color = (*m_eeg)( 0, m_channel, newTime + 0.5 ) * scale;
            }
            else
            {
                color = 0.0f;
            }

            shape->setColor( m_colorMap->getColor( color ) );
        }

        m_currentTime = newTime;
    }
}

WMEEGView::UpdateColorOfGeometryCallback::UpdateColorOfGeometryCallback( const WEvent* event,
                                                                         boost::shared_ptr< const WEEG > eeg )
    : m_event( event ),
      m_eeg( eeg )
{
    m_currentTime = -1.0;
}

void WMEEGView::UpdateColorOfGeometryCallback::update( osg::NodeVisitor* /*nv*/, osg::Drawable* drawable )
{
    const double newTime = m_event->getTime();
    if( newTime != m_currentTime )
    {
        osg::Geometry* geometry = static_cast< osg::Geometry* >( drawable );
        if( geometry )
        {
            osg::FloatArray* texCoords = static_cast< osg::FloatArray* >( geometry->getTexCoordArray( 0 ) );
            if( 0.0 <= newTime && newTime <= m_eeg->getNumberOfSamples( 0 ) - 1 )
            {
                for( size_t channelID = 0; channelID < m_eeg->getNumberOfChannels(); ++channelID )
                {
                    const int size = 256;
                    const float scale = 0.25f;
                    const float factor = scale * ( 1.0f - 1.0f / size );
                    (*texCoords)[channelID] = (*m_eeg)( 0, channelID, newTime + 0.5 ) * factor + 0.5f;
                }
            }
            else
            {
                for( size_t channelID = 0; channelID < m_eeg->getNumberOfChannels(); ++channelID )
                {
                    (*texCoords)[channelID] = 0.5f;
                }
            }
            geometry->setTexCoordArray( 0, texCoords );
        }

        m_currentTime = newTime;
    }
}
