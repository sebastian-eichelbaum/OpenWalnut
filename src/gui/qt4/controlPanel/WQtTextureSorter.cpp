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
#include <map>
#include <set>
#include <string>
#include <vector>
#include <algorithm>

#include <boost/bind.hpp>

#include <QtCore/QList>
#include <QtGui/QScrollArea>
#include <QtGui/QVBoxLayout>
#include <QtGui/QListWidgetItem>
#include <QtGui/QApplication>

#include "../../../dataHandler/WDataSet.h"
#include "../../../dataHandler/WSubject.h"
#include "../../../dataHandler/WDataHandler.h"
#include "../../../dataHandler/exceptions/WDHNoSuchSubject.h"
#include "../../../kernel/WModuleFactory.h"
#include "../../../graphicsEngine/WGEColormapping.h"
#include "../../../graphicsEngine/WGETexture.h"
#include "../events/WUpdateTextureSorterEvent.h"
#include "../events/WEventTypes.h"

#include "WQtTextureSorter.h"

WQtTextureSorter::WQtTextureSorter( QWidget* parent )
    : QWidget( parent )
{
    m_textureListWidget = new QListWidget( this );
    m_textureListWidget->setToolTip( "List of available textures. Only the upper <b>eight</b> textures will be applied." );

    m_layout = new QVBoxLayout();

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_downButton = new QPushButton();
    m_downButton->setText( QString( "down" ) );
    m_upButton = new QPushButton();
    m_upButton->setText( QString( "up" ) );

    buttonLayout->addWidget( m_downButton );
    buttonLayout->addWidget( m_upButton );

    connect( m_upButton, SIGNAL( pressed() ), this, SLOT( moveItemUp() ) );
    connect( m_downButton, SIGNAL( pressed() ), this, SLOT( moveItemDown() ) );

    m_layout->addWidget( m_textureListWidget );
    m_layout->addLayout( buttonLayout );

    connect( m_textureListWidget, SIGNAL( itemClicked( QListWidgetItem* ) ), this, SLOT( handleTextureClicked() ) );

    setLayout( m_layout );

    // get the proper subscriptions to the colormapper signals.
    boost::shared_ptr< WGEColormapping > p = WGEColormapping::instance();
    m_registerConnection = p->subscribeSignal( WGEColormapping::Registered,
            static_cast< WGEColormapping::TextureRegisterHandler >( boost::bind( &WQtTextureSorter::pushUpdateEvent, this ) ) );
    m_deregisterConnection = p->subscribeSignal( WGEColormapping::Deregistered,
            static_cast< WGEColormapping::TextureDeregisterHandler >( boost::bind( &WQtTextureSorter::pushUpdateEvent, this ) ) );
    m_sortConnection = p->subscribeSignal( WGEColormapping::Sorted,
            static_cast< WGEColormapping::TextureSortHandler>( boost::bind( &WQtTextureSorter::pushUpdateEvent, this ) ) );
}

WQtTextureSorter::~WQtTextureSorter()
{
    m_registerConnection.disconnect();
    m_deregisterConnection.disconnect();
    m_sortConnection.disconnect();
}

void WQtTextureSorter::pushUpdateEvent()
{
    // create a new event for this and insert it into event queue
    QCoreApplication::postEvent( this,  new WUpdateTextureSorterEvent() );
}

bool WQtTextureSorter::event( QEvent* event )
{
    // a texture added/removed/sorted/moved
    if ( event->type() == WQT_UPDATE_TEXTURE_SORTER_EVENT )
    {
        update();
        return true;
    }

    return QWidget::event( event );
}

void WQtTextureSorter::update()
{
    boost::shared_ptr< WGEColormapping > cm = WGEColormapping::instance();

    WGEColormapping::TextureContainerType::ReadTicket r = cm->getReadTicket();

    m_textureListWidget->clear();
    for ( WGEColormapping::TextureConstIterator iter = r->get().begin(); iter != r->get().end(); ++iter )
    {
        m_textureListWidget->addItem( QString::fromStdString( ( *iter )->name()->get() ) );

    }
}

void WQtTextureSorter::handleTextureClicked()
{
    /*
    unsigned int index =  m_textureListWidget->currentIndex().row();

    DatasetSharedContainerType::ReadTicket r = m_textures.getReadTicket();

    QListWidgetItem* ci = m_textureListWidget->item( index );
    WAssert( ci, "Problem in source code." );
    boost::shared_ptr< WDataSet > dataSet = r->get()[index];

    emit textureSelectionChanged( dataSet );*/
}

void WQtTextureSorter::moveItemDown()
{
    boost::shared_ptr< WGEColormapping > cm = WGEColormapping::instance();

    WGEColormapping::TextureContainerType::ReadTicket r = cm->getReadTicket();
    //r->get().moveDown();
}

void WQtTextureSorter::moveItemUp()
{
    boost::shared_ptr< WGEColormapping > cm = WGEColormapping::instance();

    WGEColormapping::TextureContainerType::ReadTicket r = cm->getReadTicket();
    //r->get().moveUp();
}

void WQtTextureSorter::selectTexture( boost::shared_ptr< WDataSet > dataSet )
{
    /*DatasetSharedContainerType::ReadTicket readTex =  m_textures.getReadTicket();

    size_t i = 0;
    for( ; i < readTex->get().size(); ++i )
    {
        if( readTex->get()[i] == dataSet )
        {
            break;
        }
    }

    if( i < readTex->get().size() )
    {
        // select appropriate texture if the data set has a corresponding texture
        QListWidgetItem* ci = m_textureListWidget->item( i );
        m_textureListWidget->setCurrentItem( ci );
    }
    else
    {
        // unselect all if the dataset has no corresponding texture
        m_textureListWidget->clearSelection();
    }*/
}

