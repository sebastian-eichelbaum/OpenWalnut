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

#include <QtGui/QCloseEvent>

#include "WQtCustomDockWidget.h"
#include "core/graphicsEngine/WGEViewer.h"

WQtCustomDockWidget::WQtCustomDockWidget( std::string title, QWidget* parent, WGECamera::ProjectionMode projectionMode )
    : WQtGLDockWidget( QString::fromStdString( title ), QString::fromStdString( title ), parent, projectionMode ),
      WUIViewWidget( title ),
      m_useCount( 1 )
{
    setObjectName( QString( "Custom Dock Window " ) + QString::fromStdString( title ) );

    m_scene = new WGEGroupNode();
    m_scene->setDataVariance( osg::Object::DYNAMIC );
    getGLWidget()->getViewer()->setScene( m_scene );

    // send mouse move events even if no button is pressed
    getGLWidget()->setMouseTracking( true );
}

WQtCustomDockWidget::~WQtCustomDockWidget()
{
    // nothing
}

osg::ref_ptr< WGEGroupNode > WQtCustomDockWidget::getScene() const
{
    return m_scene;
}

boost::shared_ptr< WGEViewer > WQtCustomDockWidget::getViewer() const
{
    return getGLWidget()->getViewer();
}

void WQtCustomDockWidget::increaseUseCount()
{
    ++m_useCount;
}

bool WQtCustomDockWidget::decreaseUseCount()
{
    --m_useCount;
    bool shouldClose = ( m_useCount == 0 );
    if( shouldClose )
    {
        close();
    }
    return shouldClose;
}

size_t WQtCustomDockWidget::height() const
{
    return static_cast< size_t >( getViewer()->getCamera()->getViewport()->height() );
}

size_t WQtCustomDockWidget::width() const
{
    return static_cast< size_t >( getViewer()->getCamera()->getViewport()->width() );
}

void WQtCustomDockWidget::addEventHandler( osgGA::GUIEventHandler* handler )
{
    getViewer()->getView()->addEventHandler( handler );
}

void WQtCustomDockWidget::show()
{
}

void WQtCustomDockWidget::close()
{
}

void WQtCustomDockWidget::setVisible( bool visible )
{
}

bool WQtCustomDockWidget::isVisible() const
{
    return true;
}
