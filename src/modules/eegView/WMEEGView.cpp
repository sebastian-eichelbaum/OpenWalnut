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

#include <osgUtil/Optimizer>
#include <osgDB/ReadFile>

#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>

#include <osgGA/TrackballManipulator>

#include <osg/Material>
#include <osg/Geode>
#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/PolygonOffset>
#include <osg/MatrixTransform>
#include <osg/Camera>
#include <osg/RenderInfo>

#include <osgDB/WriteFile>

#include <osgText/Text>

#include "../../kernel/WKernel.h"
#include "../../graphicsEngine/WGEViewer.h"
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

void WMEEGView::notifyDataChange( boost::shared_ptr< WModuleConnector > /*input*/, boost::shared_ptr< WModuleConnector > /*output*/ )
{
    m_dataChanged.set( true );
}

void WMEEGView::moduleMain()
{
    // do initialization
    m_moduleState.add( m_dataChanged.getCondition() );
    m_moduleState.add( m_isActive.getCondition() );
    m_rootNode = new osg::Group;

    // signal ready
    ready();

    while( !m_shutdownFlag() ) // loop until the module container requests the module to quit
    {
        // data changed?
        if( m_dataChanged() )
        {
            m_dataChanged.set( false );
            m_eeg = m_input->getData();
            if( m_eeg.get() )
            {
                m_rootNode->addChild( createText() );
            }
        }

        // "active" property changed?
        bool isActive = m_isActive();
        if( isActive != m_wasActive )
        {
            if ( isActive )
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
        m_widget->getScene()->addChild( m_rootNode );
    }
    else
    {
        warnLog() << "Could not create EEG View widget.";
    }
    return success;
}

void WMEEGView::closeCustomWidget()
{
    m_widget->getScene()->removeChild( m_rootNode );
    WKernel::getRunningKernel()->getGui()->closeCustomWidget( getName() );
}

osg::Node* WMEEGView::createText()
{
    osg::Geode* geode = new osg::Geode();

    std::string timesFont( "fonts/arial.ttf" );

    // turn lighting off for the text and disable depth test to ensure its always ontop.
    osg::StateSet* stateset = geode->getOrCreateStateSet();
    stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

    osg::Vec3 position( 150.0f, 800.0f, 0.0f );
    osg::Vec3 delta( 0.0f, -120.0f, 0.0f );

    {
        osgText::Text* text = new  osgText::Text;
        geode->addDrawable( text );

        text->setFont( timesFont );
        text->setPosition( position );
        text->setText( "Head Up Displays are simple :-)" );

        position += delta;
    }

    {
        osgText::Text* text = new  osgText::Text;
        geode->addDrawable( text );

        text->setFont( timesFont );
        text->setPosition( position );
        text->setText( "All you need to do is create your text in a subgraph." );

        position += delta;
    }

    {
        osgText::Text* text = new  osgText::Text;
        geode->addDrawable( text );

        text->setFont( timesFont );
        text->setPosition( position );
        text->setText( "Then place an osg::Camera above the subgraph\n"
                        "to create an orthographic projection.\n" );

        position += delta;
    }

    {
        osgText::Text* text = new  osgText::Text;
        geode->addDrawable( text );

        text->setFont( timesFont );
        text->setPosition( position );
        text->setText( "Set the Camera's ReferenceFrame to ABSOLUTE_RF to ensure\n"
                        "it remains independent from any external model view matrices." );

        position += delta;
    }

    {
        osgText::Text* text = new  osgText::Text;
        geode->addDrawable( text );

        text->setFont( timesFont );
        text->setPosition( position );
        text->setText( "And set the Camera's clear mask to just clear the depth buffer." );

        position += delta;
    }

    {
        osgText::Text* text = new  osgText::Text;
        geode->addDrawable( text );

        text->setFont( timesFont );
        text->setPosition( position );
        text->setText( "And finally set the Camera's RenderOrder to POST_RENDER\n"
                        "to make sure its drawn last." );

        position += delta;
    }

    {
        osg::BoundingBox bb;
        for( unsigned int i = 0; i < geode->getNumDrawables(); ++i)
        {
            bb.expandBy( geode->getDrawable( i )->getBound() );
        }

        osg::Geometry* geom = new osg::Geometry;

        osg::Vec3Array* vertices = new osg::Vec3Array;
        float depth = bb.zMin()-0.1;
        vertices->push_back( osg::Vec3( bb.xMin(), bb.yMax(), depth ) );
        vertices->push_back( osg::Vec3( bb.xMin(), bb.yMin(), depth ) );
        vertices->push_back( osg::Vec3( bb.xMax(), bb.yMin(), depth ) );
        vertices->push_back( osg::Vec3( bb.xMax(), bb.yMax(), depth ) );
        geom->setVertexArray( vertices );

        osg::Vec3Array* normals = new osg::Vec3Array;
        normals->push_back( osg::Vec3( 0.0f, 0.0f, 1.0f ) );
        geom->setNormalArray( normals );
        geom->setNormalBinding( osg::Geometry::BIND_OVERALL );

        osg::Vec4Array* colors = new osg::Vec4Array;
        colors->push_back( osg::Vec4( 1.0f, 1.0, 0.8f, 0.2f ) );
        geom->setColorArray( colors );
        geom->setColorBinding( osg::Geometry::BIND_OVERALL );

        geom->addPrimitiveSet( new osg::DrawArrays( GL_QUADS, 0, 4 ) );

        osg::StateSet* stateset = geom->getOrCreateStateSet();
        stateset->setMode( GL_BLEND, osg::StateAttribute::ON );
        //stateset->setAttribute(new osg::PolygonOffset(1.0f,1.0f),osg::StateAttribute::ON);
        stateset->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

        geode->addDrawable( geom );
    }

    return geode;
}
