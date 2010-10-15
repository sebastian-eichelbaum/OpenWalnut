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

WGEGraphicsWindow::WGEGraphicsWindow( int x,
                                      int y,
                                      int width,
                                      int height )
    : m_GraphicsWindow( new osgViewer::GraphicsWindowEmbedded( x, y, width, height ) )
{
}

WGEGraphicsWindow::~WGEGraphicsWindow()
{
    // cleanup
}

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

