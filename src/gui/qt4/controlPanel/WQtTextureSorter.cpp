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
#include "../../../dataHandler/WDataHandler.h"
#include "../../../dataHandler/exceptions/WDHNoSuchSubject.h"
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

WQtTextureSorter::WQtTextureListItem::WQtTextureListItem( const osg::ref_ptr< WGETexture3D > texture, QListWidget* parent ):
    QListWidgetItem( QString::fromStdString( texture->name()->get() ), parent ),
    m_texture( texture )
{
    // only show the filename. If the name is not a filename, the texture name is used directly
    std::vector< std::string > names = string_utils::tokenize( texture->name()->get().c_str(), "/" );
    if( names.size() )
    {
        setText( QString::fromStdString( names.back() ) );
    }
}

WQtTextureSorter::WQtTextureListItem::~WQtTextureListItem()
{
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

    // we need to store the last selected texture if there was any
    osg::ref_ptr< WGETexture3D > lastSelected;
    WQtTextureListItem* item = dynamic_cast< WQtTextureListItem* >( m_textureListWidget->item( m_textureListWidget->currentIndex().row() ) );
    if ( item )
    {
        lastSelected = item->m_texture;
    }

    // remove all items and rebuild list.
    m_textureListWidget->clear();
    for ( WGEColormapping::TextureConstIterator iter = r->get().begin(); iter != r->get().end(); ++iter )
    {
        WQtTextureListItem* item = new WQtTextureListItem( *iter );
        m_textureListWidget->addItem( item );    // the list widget removes the item (and frees the reference to the texture pointer).

        // is the item the texture that has been selected previously?
        if ( item->m_texture == lastSelected )
        {
            m_textureListWidget->setCurrentItem( item );
        }
    }
}

void WQtTextureSorter::handleTextureClicked()
{
    // maybe someone is interested in this signal:
    emit textureSelectionChanged(
        static_cast< WQtTextureListItem* >( m_textureListWidget->item( m_textureListWidget->currentIndex().row() ) )->m_texture
    );
}

void WQtTextureSorter::moveItemDown()
{
    boost::shared_ptr< WGEColormapping > cm = WGEColormapping::instance();
    WQtTextureListItem* item = dynamic_cast< WQtTextureListItem* >( m_textureListWidget->item( m_textureListWidget->currentIndex().row() ) );
    if ( item )
    {
        cm->moveDown( item->m_texture );
    }
}

void WQtTextureSorter::moveItemUp()
{
    boost::shared_ptr< WGEColormapping > cm = WGEColormapping::instance();
    WQtTextureListItem* item = dynamic_cast< WQtTextureListItem* >( m_textureListWidget->item( m_textureListWidget->currentIndex().row() ) );
    if ( item )
    {
        cm->moveUp( item->m_texture );
    }
}

void WQtTextureSorter::selectTexture( boost::shared_ptr< WDataSet > dataSet )
{
    // simply check each item against the texture in the specified dataset
    for ( int i = 0; i < m_textureListWidget->count(); ++i )
    {
        WQtTextureListItem* item = dynamic_cast< WQtTextureListItem* >( m_textureListWidget->item(i ) );
        if (item &&  ( item->m_texture == dataSet->getTexture2() ) )
        {
            m_textureListWidget->setCurrentItem( item );
        }
    }
}

