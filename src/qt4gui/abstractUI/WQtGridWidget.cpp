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

#include <QtGui/QGridLayout>

#include "core/common/WConditionOneShot.h"
#include "core/common/WFlag.h"

#include "WQtGridWidget.h"

WQtGridWidget::WQtGridWidget(
            std::string title,
            WMainWindow* mainWindow,
            WQtWidgetBase::SPtr parent ):
    WUIGridWidget( title ),
    WQtWidgetBase( mainWindow, parent ),
    m_gridLayout( NULL )
{
    // initialize members
}

WQtGridWidget::~WQtGridWidget()
{
    // cleanup
}

QString WQtGridWidget::getTitleQString() const
{
    return QString::fromStdString( getTitle() );
}

void WQtGridWidget::show()
{
    WQtWidgetBase::show();
}

void WQtGridWidget::setVisible( bool visible )
{
    WQtWidgetBase::setVisible( visible );
}

bool WQtGridWidget::isVisible() const
{
    return WQtWidgetBase::isVisible();
}

void WQtGridWidget::cleanUpGT()
{
    delete m_widget;
    m_widget = NULL;
}

void WQtGridWidget::close()
{
    // use WUIGridWidget to handle this
    WUIGridWidget::close();
}

void WQtGridWidget::closeImpl()
{
    // notify child widgets
    WQtWidgetBase::closeImpl();
}

void WQtGridWidget::realizeImpl()
{
    // this is called from withing the GUI thread -> we can safely create QT widgets here

    // parent info:
    QWidget* parent = getParentAsQtWidget();
    bool hasParent = parent;
    if( !parent )
    {
        parent = m_mainWindow;
    }

    // is the widget embedded somewhere?
    if( hasParent )
    {
        // then we use a plain QWidget
        // create the actual grid widget (using a QGridLayout)
        QWidget* gridWidget = new QWidget( parent );
        m_gridLayout = new QGridLayout();
        gridWidget->setLayout( m_gridLayout );

        // store them. Allow WQtWidgetBase to work on our widget instance
        m_widget = gridWidget;

        m_widget->setVisible( true );
    }
    else
    {
        // it is a top-level window -> use a dock
        // create as dock widget
        m_widgetDock = new WQtDockWidget( QString::fromStdString( getTitle() ), parent );
        m_widgetDock->setObjectName( QString( "Custom Dock Window " ) + QString::fromStdString( getTitle() ) );

        // create the actual grid widget (using a QGridLayout)
        QWidget* gridWidget = new QWidget( m_widgetDock );
        m_gridLayout = new QGridLayout();
        gridWidget->setLayout( m_gridLayout );
        m_widgetDock->setWidget( gridWidget );

        // store them. Allow WQtWidgetBase to work on our widget instance
        m_widget = m_widgetDock;

        // hide by default if we do not have a parent
        m_widgetDock->setVisible( false );
        m_mainWindow->addDockWidget( Qt::BottomDockWidgetArea, m_widgetDock );
    }
}

void WQtGridWidget::placeWidgetImpl( WUIWidgetBase::SPtr widget, int x, int y )
{
    // get the real Qt widget
    WQtWidgetBase* widgetQt = dynamic_cast< WQtWidgetBase* >( widget.get() );
    QWidget* widgetQtwidget = NULL;
    if( widgetQt )
    {
        widgetQtwidget = widgetQt->getWidget();
    }

    // and forward call to GUI thread
    if( widgetQtwidget )
    {
        WQt4Gui::execInGUIThread( boost::bind( &WQtGridWidget::placeWidgetImplGT, this, widgetQtwidget, x, y ) );
    }
}

void WQtGridWidget::placeWidgetImplGT( QWidget* widget, int x, int y )
{
    if( m_gridLayout )
    {
        m_gridLayout->addWidget( widget, x, y );
    }
}
