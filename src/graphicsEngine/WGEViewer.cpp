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

#include <osg/ShapeDrawable>
#include <osg/Geode>

#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/UFOManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator>
#include <osgViewer/ViewerEventHandlers>

#include <osgDB/ReadFile>

#include "exceptions/WGEInitFailed.h"

#include "WPickHandler.h"

#include "WGEViewer.h"

WGEViewer::WGEViewer( osg::ref_ptr<WindowData> wdata, int x, int y, int width, int height, WGECamera::ProjectionMode projectionMode ):
    WGEGraphicsWindow( wdata, x, y, width, height )
{
    try
    {
        // initialize OSG render window
        m_Viewer = osg::ref_ptr<osgViewer::CompositeViewer>( new osgViewer::CompositeViewer() );
        m_Viewer->setThreadingModel( osgViewer::Viewer::DrawThreadPerContext );
        // m_Viewer->setThreadingModel( osgViewer::Viewer::SingleThreaded );

        m_View = osg::ref_ptr<osgViewer::Viewer>( new osgViewer::Viewer() );
        m_View->getCamera()->setGraphicsContext( m_GraphicsContext );

        switch( projectionMode )
        {
            case( WGECamera::ORTHOGRAPHIC ):
                m_View->getCamera()->setProjectionMatrixAsOrtho(
                    -120.0 * width / height, 120.0 * width / height, -120.0, 120.0, -1000.0, +1000.0 );
                break;
            case( WGECamera::PERSPECTIVE ):
                m_View->getCamera()->setProjectionMatrixAsPerspective(
                    30.0, static_cast< double >( width ) / static_cast< double >( height ), 1.0, 1000.0 );
                break;
            default:
                throw WGEInitFailed( "Unknown projection mode" );
                break;
        }

        m_View->getCamera()->setViewport( 0, 0, width, height );
        m_View->getCamera()->setProjectionResizePolicy( osg::Camera::HORIZONTAL );
        m_View->getCamera()->setClearColor( osg::Vec4( .9, .9, .9, 1. ) );

        // add the stats handler
        m_View->addEventHandler( new osgViewer::StatsHandler );

        // camera manipulator
        m_View->setCameraManipulator( new WGEZoomTrackballManipulator() );

        m_View->setLightingMode( osg::View::HEADLIGHT ); // this is the default anyway

        osg::ref_ptr<osgText::Text> updateText = new osgText::Text;
        m_View->addEventHandler( new WPickHandler( updateText.get() ) );

        // finally add view
        // there is the possibility to use ONE single composite viewer instance for every view, but
        // currently this possibility is not used.
        m_Viewer->addView( m_View.get() );
    }
    catch( ... )
    {
        throw WGEInitFailed( "Initialization of WGEViewer failed" );
    }
}

WGEViewer::~WGEViewer()
{
    // cleanup
    close();
}

osg::ref_ptr<osgViewer::CompositeViewer> WGEViewer::getViewer()
{
    return m_Viewer;
}

void WGEViewer::setCameraManipulator( osg::ref_ptr<osgGA::MatrixManipulator> manipulator )
{
    m_View->setCameraManipulator( manipulator );
    // redraw request?? no since it redraws permanently and uses the new settings
}

osg::ref_ptr<osgGA::MatrixManipulator> WGEViewer::getCameraManipulator()
{
    return m_View->getCameraManipulator();
}

void WGEViewer::setCamera( osg::ref_ptr<osg::Camera> camera )
{
    m_View->setCamera( camera );
    // redraw request?? no since it redraws permanently and uses the new settings
}

osg::ref_ptr<osg::Camera> WGEViewer::getCamera()
{
    return m_View->getCamera();
}

void WGEViewer::setScene( osg::ref_ptr<osg::Node> node )
{
    m_View->setSceneData( node );
}

osg::ref_ptr<osg::Node> WGEViewer::getNode()
{
    return m_View->getSceneData();
}

void WGEViewer::paint()
{
    m_Viewer->frame();
}

void WGEViewer::resize( int width, int height )
{
    WGEGraphicsWindow::resize( width, height );

    // also update the camera
    m_View->getCamera()->setViewport( 0, 0, width, height );
}

void WGEViewer::close()
{
    // wait for thread to finish
    wait( true );

    // forward close event
    WGEGraphicsWindow::close();
}

void WGEViewer::threadMain()
{
    // TODO(ebaum): Ticket #125 (synchronize GUI and GraphicsEngine)
    sleep( 1 );

    while( !m_FinishRequested )
    {
        paint();
    }
}
