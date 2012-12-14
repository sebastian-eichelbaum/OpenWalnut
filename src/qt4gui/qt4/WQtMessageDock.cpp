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

#include <QtGui/QAction>
#include <QtGui/QDockWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QWidget>
#include <QtGui/QComboBox>

#include "WQt4Gui.h"
#include "WMainWindow.h"
#include "WQtMessagePopup.h"

#include "WQtMessageDock.h"
#include "WQtMessageDock.moc"

WQtMessageDock::WQtMessageDock( QString dockTitle, QWidget* parent ):
    QDockWidget( dockTitle, parent )
{
    setObjectName( "MessageDock:" + dockTitle );
    setAllowedAreas( Qt::AllDockWidgetAreas );
    setFeatures( QDockWidget::AllDockWidgetFeatures );

    QVBoxLayout* panelLayout = new QVBoxLayout();
    QWidget* panel = new QWidget( this );
    panel->setLayout( panelLayout );
    setWidget( panel );

    // fill the panel

    // log messages
    m_logList = new QListWidget( this );

    // filter list
    QLabel* filterLabel = new QLabel( "Filter Messages" );
    m_filterCombo = new QComboBox();
    m_filterCombo->addItem( "Debug" );
    m_filterCombo->addItem( "Info" );
    m_filterCombo->addItem( "Warning" );
    m_filterCombo->addItem( "Error" );
    m_filterCombo->setCurrentIndex(
        WMainWindow::getSettings().value( "MessageDockFilterIndex", 2 ).toInt()
    ); // warning is the default

    // the filter widgets reside in a common layout:
    QHBoxLayout* filterLayout = new QHBoxLayout();
    filterLayout->addWidget( filterLabel );
    filterLayout->addWidget( m_filterCombo );
    QWidget* filterWidget = new QWidget();
    filterWidget->setLayout( filterLayout );

    // compose them together into the panel
    panelLayout->addWidget( filterWidget );
    panelLayout->addWidget( m_logList );

    // connect filter combo
    connect( m_filterCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( handleFilterUpdate() ) );
}

WQtMessageDock::~WQtMessageDock()
{
    // cleanup
}

void WQtMessageDock::addLogMessage( const WLogEntry& entry )
{
    addLogMessage( QString::fromStdString( entry.getSource() ),
                   QString::fromStdString( entry.getMessage() ),
                   entry.getLogLevel() );
}

void WQtMessageDock::addLogMessage( QString sender, QString message, WQtMessagePopup::MessageType type )
{
    addMessage( "Log message from " + sender, message, type );
}

void WQtMessageDock::addMessage( QString title, QString message, WQtMessagePopup::MessageType type )
{
    WQtMessagePopup* w = new WQtMessagePopup( m_logList, title, message, type );
    w->setAutoClose( false );
    w->setShowCloseButton( false );
    w->setAutoPosition( false );

    QListWidgetItem* item = new QListWidgetItem( m_logList );
    item->setSizeHint( QSize( 0, w->sizeHint().height() ) );
    m_logList->addItem( item );
    m_logList->setItemWidget( item, w );
    m_logList->scrollToItem( item, QAbstractItemView::PositionAtBottom );

    // hide messages not matching the filter
    item->setHidden( type < m_filterCombo->currentIndex() );
}

void WQtMessageDock::saveSettings()
{
    WMainWindow::getSettings().setValue( "MessageDockFilterIndex", m_filterCombo->currentIndex() );
}

void WQtMessageDock::handleFilterUpdate()
{
    size_t i = 0;
    for( size_t i = 0; i < m_logList->count(); ++i )
    {
        QListWidgetItem* li = m_logList->item( i );
        QWidget* w = m_logList->itemWidget( li );
        WQtMessagePopup* popup = dynamic_cast< WQtMessagePopup* >( w );
        if( popup )
        {
            li->setHidden( popup->getType() < m_filterCombo->currentIndex() );
        }
    }
}
