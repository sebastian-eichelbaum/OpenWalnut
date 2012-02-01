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

#include "WGEGraphicsWindow.h"

#include "exceptions/WGEInitFailed.h"

WGEGraphicsWindow::WGEGraphicsWindow( osg::ref_ptr<osg::Referenced>
     #ifdef WGEMODE_MULTITHREADED
        wdata  // this parameter is only needed on non-mac
     #endif
        ,
                                            int x,
                                            int y,
                                            int width,
                                            int height )
{
#ifdef WGEMODE_MULTITHREADED
    // initialize context
    m_WindowData = wdata;
    try
    {
        createContext( x, y, width, height );
    }
    catch( ... )
    {
        // use our own exceptions
        throw WGEInitFailed( "Initialization of OpenGL graphics context failed." );
    }
#else
    m_GraphicsWindow = osg::ref_ptr<osgViewer::GraphicsWindow>(
            static_cast<osgViewer::GraphicsWindow*>( new osgViewer::GraphicsWindowEmbedded( x, y, width, height ) ) );
#endif
}

WGEGraphicsWindow::~WGEGraphicsWindow()
{
    // cleanup
}

osg::ref_ptr<osgViewer::GraphicsWindow> WGEGraphicsWindow::getGraphicsWindow()
{
    return m_GraphicsWindow;
}

#ifdef WGEMODE_MULTITHREADED
void WGEGraphicsWindow::createContext( int x, int y, int width, int height )
{
    // Create traits for graphics contest request
    osg::ref_ptr<osg::DisplaySettings> ds = osg::DisplaySettings::instance();
    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;

    // ensure correct $DISPLAY variable
    traits->readDISPLAY();
    if( traits->displayNum < 0 )
    {
        traits->displayNum = 0;
    }

    // set a lot of values
    traits->windowName = "OpenWalnut";
    traits->screenNum = 0;      // XXX is this a good idea?
    traits->x = x;
    traits->y = y;
    traits->width = width;
    traits->height = height;
    traits->alpha = ds->getMinimumNumAlphaBits();
    traits->stencil = ds->getMinimumNumStencilBits();
    // traits->windowDecoration = false;
    traits->doubleBuffer = true;
    traits->sharedContext = 0;
    traits->sampleBuffers = ds->getMultiSamples();
    traits->samples = ds->getNumMultiSamples();
    traits->inheritedWindowData = m_WindowData;

    // finally create graphics context and window
    m_GraphicsContext = osg::GraphicsContext::createGraphicsContext( traits.get() );

    m_GraphicsWindow = osg::ref_ptr<osgViewer::GraphicsWindow>(
            static_cast<osgViewer::GraphicsWindow*>( m_GraphicsContext.get() ) );

    // get around dearranged traits on X11 (MTCompositeViewer only)
    traits->x = x;
    traits->y = x;
    traits->width = width;
    traits->height = height;
}
#endif

void WGEGraphicsWindow::resize( int width, int height )
{
    m_GraphicsWindow->getEventQueue()->windowResize( 0, 0, width, height );
    m_GraphicsWindow->resized( 0, 0, width, height );
}

void WGEGraphicsWindow::close()
{
    m_GraphicsWindow->getEventQueue()->closeWindow();
}

void WGEGraphicsWindow::keyEvent( KeyEvents eventType, int key )
{
    switch( eventType )
    {
        case KEYPRESS:
            m_GraphicsWindow->getEventQueue()->keyPress( static_cast<osgGA::GUIEventAdapter::KeySymbol>( key ) );
            break;
        case KEYRELEASE:
            m_GraphicsWindow->getEventQueue()->keyRelease( static_cast<osgGA::GUIEventAdapter::KeySymbol>( key ) );
            break;
    }
}

void WGEGraphicsWindow::mouseEvent( MouseEvents eventType, int x, int y, int button )
{
    switch( eventType )
    {
        case MOUSEPRESS:
            m_GraphicsWindow->getEventQueue()->mouseButtonPress( x, y, button );
            break;
        case MOUSERELEASE:
            m_GraphicsWindow->getEventQueue()->mouseButtonRelease( x, y, button );
            break;
        case MOUSEDOUBLECLICK:
            m_GraphicsWindow->getEventQueue()->mouseDoubleButtonPress( x, y, button );
            break;
        case MOUSEMOVE:
            m_GraphicsWindow->getEventQueue()->mouseMotion( x, y );
            break;
        case MOUSESCROLL:
            m_GraphicsWindow->getEventQueue()->mouseScroll2D( x, y );
            break;
    }
}

