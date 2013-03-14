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

#include "../WQt4Gui.h"
#include "../WMainWindow.h"

#include "WQtDockTitleWidget.h"

#include "WQtDockWidget.h"
#include "WQtDockWidget.moc"

WQtDockWidget::WQtDockWidget( const QString& title, QWidget* parent, Qt::WindowFlags flags ):
    QDockWidget( title, parent, flags )
{
    // thats it. we now have the title bar
    m_titleBar = new WQtDockTitleWidget( this );
    setTitleBarWidget( m_titleBar );

    // some standard dock features
    setAllowedAreas( Qt::AllDockWidgetAreas );
    setFeatures( QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
}

WQtDockWidget::WQtDockWidget( QWidget* parent, Qt::WindowFlags flags ):
    QDockWidget( parent, flags )
{
    // thats it. we now have the title bar
    m_titleBar = new WQtDockTitleWidget( this );
    setTitleBarWidget( m_titleBar );

    // some standard dock features
    setAllowedAreas( Qt::AllDockWidgetAreas );
    setFeatures( QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
}

WQtDockWidget::~WQtDockWidget()
{
    // cleanup
}

void WQtDockWidget::addTitleAction( QAction* action, bool instantPopup )
{
    m_titleBar->addTitleAction( action, instantPopup );
}

void WQtDockWidget::removeTitleAction( QAction* action )
{
    m_titleBar->removeTitleAction( action );
}

void WQtDockWidget::addTitleSeperator()
{
    m_titleBar->addTitleSeperator();
}
