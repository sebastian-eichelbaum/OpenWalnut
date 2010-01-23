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

#include "../../common/WConditionOneShot.h"
#include "../../graphicsEngine/WGEViewer.h"
#include "../../kernel/WKernel.h"
#include "WMEEGView.h"


WMEEGView::WMEEGView()
    : WModule(),
      m_dataChanged( new WCondition, true ),
      m_isActive( new WCondition, true ),
      m_wasActive( false )
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
    return "Test module to open an EEG View.";
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
    // properties
    m_properties->addBool( "active", true, true )->connect( boost::bind( &WMEEGView::slotPropertyChanged, this, _1 ) );
}

void WMEEGView::slotPropertyChanged( std::string propertyName )
{
    if( propertyName == "active" )
    {
        m_isActive.set( m_properties->getValue< bool >( propertyName ) );
    }
    else
    {
        // instead of WLogger we must use std::cerr since WLogger needs to much time!
        std::cerr << propertyName << std::endl;
        assert( 0 && "This property name is not supported by this function yet." );
    }
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
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_dataChanged.getCondition() );
    m_moduleState.add( m_isActive.getCondition() );
    m_updateChildNodeCallback = new UpdateChildNodeCallback;

    // signal ready
    ready();

    while( !m_shutdownFlag() ) // loop until the module container requests the module to quit
    {
        // data changed?
        if( m_dataChanged() )
        {
            debugLog() << "Data changed";
            m_dataChanged.set( false );
            m_eeg = m_input->getData();
            redraw();
        }

        // "active" property changed?
        bool isActive = m_isActive();
        if( isActive != m_wasActive )
        {
            if( isActive )
            {
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
            }
            m_wasActive = isActive;
        }

        m_moduleState.wait(); // waits for firing of m_moduleState ( dataChanged, shutdown, etc. )
    }

    // clean up
    if( m_wasActive )
    {
        closeCustomWidget();
    }
}

bool WMEEGView::openCustomWidget()
{
    m_widget = WKernel::getRunningKernel()->getGui()->openCustomWidget(
        getName(), WGECamera::TWO_D, m_shutdownFlag.getCondition() );
    bool success = m_widget.get();
    if( success )
    {
        debugLog() << "Succesfully opened EEG View widget.";
        m_updateChildNodeCallback->setActive( true );
        m_widget->getScene()->addUpdateCallback( m_updateChildNodeCallback );
    }
    else
    {
        warnLog() << "Could not create EEG View widget.";
    }
    return success;
}

void WMEEGView::closeCustomWidget()
{
    m_updateChildNodeCallback->setActive( false );
    WKernel::getRunningKernel()->getGui()->closeCustomWidget( getName() );
}

void WMEEGView::redraw()
{
    osg::ref_ptr< osg::Group > rootNode;

    if( m_eeg.get() )
    {
        const osg::Vec3 textOffset( -8.0, 0.0, 0.0 );
        const double textSize = 32.0;
        const osg::Vec4 textColor( 0.0, 0.0, 0.0, 1.0 );
        const osg::Vec4 linesColor( 0.0, 0.0, 0.0, 1.0 );
        const osg::Matrix scaleMatrix = osg::Matrix::scale( 1.0, 0.01, 1.0 );
        const double xOffset = 64.0;
        const unsigned int spacing = 16;

        rootNode = new osg::Group;
        osg::StateSet* stateset = rootNode->getOrCreateStateSet();
        stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

        debugLog() << "Displaying EEG " << m_eeg->getFileName();
        debugLog() << "  Number of segments: " << m_eeg->getNumberOfSegments();
        size_t nbChannels = m_eeg->getNumberOfChannels();
        debugLog() << "  Number of channels: " << nbChannels;

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
                text->setPosition( textOffset );
                text->setAlignment( osgText::Text::RIGHT_CENTER );
                text->setAxisAlignment( osgText::Text::SCREEN );
                text->setCharacterSize( textSize );
                text->setCharacterSizeMode( osgText::Text::SCREEN_COORDS );
                text->setColor( textColor );

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
                rootNode->addChild( translate );
            }
        }
    }

    m_updateChildNodeCallback->setTargetChild( rootNode );
}

void WMEEGView::UpdateChildNodeCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    osg::ref_ptr <osg::Node> targetChild;

    if( m_active)
    {
        targetChild = m_targetChild;
    }
    else
    {
        targetChild = NULL;
        node->removeUpdateCallback( this );
    }

    if( targetChild != m_currentChild )
    {
        osg::Group* group = static_cast< osg::Group* >( node );
        if( m_currentChild.valid() )
        {
            group->removeChild( m_currentChild );
        }
        if( targetChild.valid() )
        {
            group->addChild( targetChild );
        }
        m_currentChild = targetChild;
    }

    traverse( node, nv );
}

void WMEEGView::UpdateChildNodeCallback::setTargetChild( osg::ref_ptr< osg::Node > targetChild )
{
    m_targetChild = targetChild;
}

void WMEEGView::UpdateChildNodeCallback::setActive( bool active )
{
    m_active = active;
}
