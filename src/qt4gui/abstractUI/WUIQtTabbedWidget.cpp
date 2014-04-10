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

#include "WUIQtTabbedWidget.h"

WUIQtTabbedWidget::WUIQtTabbedWidget(
            std::string title,
            WMainWindow* mainWindow,
            WUIQtWidgetBase::SPtr parent ):
    WUITabbedWidget( title ),
    WUIQtWidgetBase( mainWindow, parent )
{
    // initialize members
}

WUIQtTabbedWidget::~WUIQtTabbedWidget()
{
    // cleanup
}

QString WUIQtTabbedWidget::getTitleQString() const
{
    return QString::fromStdString( getTitle() );
}

void WUIQtTabbedWidget::show()
{
    WUIQtWidgetBase::show();
}

void WUIQtTabbedWidget::setVisible( bool visible )
{
    WUIQtWidgetBase::setVisible( visible );
}

bool WUIQtTabbedWidget::isVisible() const
{
    return WUIQtWidgetBase::isVisible();
}

void WUIQtTabbedWidget::cleanUpGT()
{
    // nothing to do
}

void WUIQtTabbedWidget::close()
{
    // use WUITabbedWidget to handle this
    WUITabbedWidget::close();
}

void WUIQtTabbedWidget::closeImpl()
{
    // notify child widgets
    WUIQtWidgetBase::closeImpl();
}

void WUIQtTabbedWidget::realizeImpl()
{
    // this is called from withing the GUI thread -> we can safely create QT widgets here

    // use QTabWidget
    m_tabWidget = new QTabWidget( getCompellingQParent() );
    embedContent( m_tabWidget );
}

int WUIQtTabbedWidget::addTabImpl( WUIWidgetBase::SPtr widget, std::string label )
{
    // get the real Qt widget
    WUIQtWidgetBase* widgetQt = dynamic_cast< WUIQtWidgetBase* >( widget.get() );
    QWidget* widgetQtwidget = NULL;
    if( widgetQt )
    {
        widgetQtwidget = widgetQt->getWidget();
    }

    // and forward call to GUI thread
    if( widgetQtwidget )
    {
        QString s = QString::fromStdString( label );
        WQt4Gui::execInGUIThread( boost::bind( &QTabWidget::addTab, m_tabWidget, widgetQtwidgetgvim , s ) );
    }

    return 0;
}

void WUIQtTabbedWidget::setTabText( int index, const std::string& label )
{
    if( m_tabWidget )
    {
        QString s = QString::fromStdString( label );
        WQt4Gui::execInGUIThread( boost::bind( &QTabWidget::setTabText, m_tabWidget, index, s ) );
    }
}

void WUIQtTabbedWidget::setActiveTab( int index )
{
    if( m_tabWidget )
    {
        WQt4Gui::execInGUIThread( boost::bind( &QTabWidget::setCurrentIndex, m_tabWidget, index ) );
    }
}

void WUIQtTabbedWidget::setTabToolTip( int index, const std::string& tip )
{
    if( m_tabWidget )
    {
        QString s = QString::fromStdString( tip );
        WQt4Gui::execInGUIThread( boost::bind( &QTabWidget::setTabToolTip, m_tabWidget, index, s ) );
    }
}

void WUIQtTabbedWidget::setTabEnabled( int index, bool enable )
{
    if( m_tabWidget )
    {
        WQt4Gui::execInGUIThread( boost::bind( &QTabWidget::setTabEnabled, m_tabWidget, index, enable ) );
    }
}

void WUIQtTabbedWidget::setTabPosition( TabPosition position )
{
    QTabWidget::TabPosition posInQt =  static_cast< QTabWidget::TabPosition >( position );
    if( m_tabWidget )
    {
        WQt4Gui::execInGUIThread( boost::bind( &QTabWidget::setTabPosition, m_tabWidget, posInQt ) );
    }
}
