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

#include <QApplication>

#include "../WMainWindow.h"
#include "WUIQtViewWidget.h"
#include "WUIQtGridWidget.h"
#include "WUIQtTabbedWidget.h"
#include "WUIQtPropertyGroupWidget.h"

#include "WUIQtWidgetFactory.h"

WUIQtWidgetFactory::WUIQtWidgetFactory( WMainWindow* mainWindow ):
    WUIWidgetFactory(),
    m_mainWindow( mainWindow )
{
    // initialize members
}

WUIQtWidgetFactory::~WUIQtWidgetFactory()
{
    // cleanup
}

bool WUIQtWidgetFactory::implementsUI() const
{
    return true;
}

WUIQtWidgetBase::SPtr WUIQtWidgetFactory::getAsQtWidgetBase( WUIWidgetBase::SPtr widget )
{
    WUIQtWidgetBase::SPtr w = boost::dynamic_pointer_cast< WUIQtWidgetBase >( widget );
    return w;
}

WUIGridWidget::SPtr WUIQtWidgetFactory::createGridWidgetImpl( const std::string& title, WUIWidgetBase::SPtr parent ) const
{
    WUIQtGridWidget::SPtr widget( new WUIQtGridWidget( title, m_mainWindow, getAsQtWidgetBase( parent ) ) );
    WUIWidgetFactory::setParent( widget, parent );    // NOTE: this is the parent on the WUI side
    widget->realize();
    return widget;
}

WUITabbedWidget::SPtr WUIQtWidgetFactory::createTabbedWidgetImpl( const std::string& title, WUIWidgetBase::SPtr parent ) const
{
    WUIQtTabbedWidget::SPtr widget( new WUIQtTabbedWidget( title, m_mainWindow, getAsQtWidgetBase( parent ) ) );
    WUIWidgetFactory::setParent( widget, parent );    // NOTE: this is the parent on the WUI side
    widget->realize();
    return widget;
}

WUIPropertyGroupWidget::SPtr WUIQtWidgetFactory::createPropertyGroupWidgetImpl( const std::string& title, WPropGroup properties,
                                                                              WUIWidgetBase::SPtr parent ) const
{
    WUIQtPropertyGroupWidget::SPtr widget( new WUIQtPropertyGroupWidget( title, properties, m_mainWindow, getAsQtWidgetBase( parent ) ) );
    WUIWidgetFactory::setParent( widget, parent );    // NOTE: this is the parent on the WUI side
    widget->realize();
    return widget;
}

WUIViewWidget::SPtr WUIQtWidgetFactory::createViewWidgetImpl(
    std::string title,
    WGECamera::ProjectionMode projectionMode,
    boost::shared_ptr< WCondition > abortCondition,
    WUIWidgetBase::SPtr parent ) const
{
    WUIQtViewWidget::SPtr widget( new WUIQtViewWidget( title, projectionMode, m_mainWindow, getAsQtWidgetBase( parent ) ) );
    WUIWidgetFactory::setParent( widget, parent );    // NOTE: this is the parent on the WUI side
    widget->realize( abortCondition );
    return widget;
}
