//---------------------------------------------------------------------------
//
// Project: OpenWalnut
//
// Copyright 2009 SomeCopyrightowner
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

#include <osgGA/TrackballManipulator>
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
#include "WGEViewer.h"

WGEViewer::WGEViewer( boost::shared_ptr<WindowData> wdata, int x, int y, int width, int height ):
    WGEGraphicsWindow( wdata, x, y, width, height )
{
    try
    {
        // initialize OSG render window
        m_Viewer = boost::shared_ptr<osgViewer::CompositeViewer>( new osgViewer::CompositeViewer() );
        m_Viewer->setThreadingModel( osgViewer::Viewer::DrawThreadPerContext );
        // m_Viewer->setThreadingModel( osgViewer::Viewer::SingleThreaded );

        m_View = boost::shared_ptr<osgViewer::Viewer>( new osgViewer::Viewer() );
        m_View->getCamera()->setGraphicsContext( m_GraphicsContext );
        m_View->getCamera()->setProjectionMatrixAsPerspective( 30.0f, 1.333, 1.0, 1000.0 );
        m_View->getCamera()->setViewport( new osg::Viewport( 0, 0, 10, 10 ) );

        // add the stats handler
        m_View->addEventHandler( new osgViewer::StatsHandler );

        // camera manipulator
        m_View->setCameraManipulator( new osgGA::TrackballManipulator() );

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
}

boost::shared_ptr<osgViewer::CompositeViewer> WGEViewer::getViewer()
{
    return m_Viewer;
}

void WGEViewer::setCameraManipulator( osgGA::MatrixManipulator* manipulator )
{
    m_View->setCameraManipulator( manipulator );
    // redraw request?? no since it redraws permanently and uses the new settings
}

osgGA::MatrixManipulator* WGEViewer::getCameraManipulator()
{
    return m_View->getCameraManipulator();
}

void WGEViewer::setCamera( osg::Camera* camera )
{
    m_View->setCamera( camera );
    // redraw request?? no since it redraws permanently and uses the new settings
}

osg::Camera* WGEViewer::getCamera()
{
    return m_View->getCamera();
}

void WGEViewer::setScene( osg::Node* node )
{
    m_View->setSceneData( node );
}

osg::Node* WGEViewer::getNode()
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
    m_View->getCamera()->setProjectionMatrixAsPerspective( 30.0f, 1.333, 1.0, 1000.0 );
    m_View->getCamera()->setViewport( new osg::Viewport( 0, 0, width, height ) );
}

