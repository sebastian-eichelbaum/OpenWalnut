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

#include "core/dataHandler/WDataSet.h"
#include "core/dataHandler/WDataHandler.h"
#include "core/dataHandler/exceptions/WDHNoSuchSubject.h"
#include "core/graphicsEngine/WGEColormapping.h"
#include "core/graphicsEngine/WGETexture.h"
#include "../events/WUpdateTextureSorterEvent.h"
#include "../events/WEventTypes.h"
#include "../WQt4Gui.h"
#include "../WMainWindow.h"

#include "WQtColormapper.h"
#include "WQtColormapper.moc"

WQtColormapper::WQtColormapper( QWidget* parent )
    : WQtDockWidget( "Colormaps", parent )
{
    setObjectName( "Colormapper Dock" );

    m_textureListWidget = new QListWidget( this );
    m_textureListWidget->setToolTip( "List of available colormaps. Only the upper <b>"
                                     + QString().setNum( WGETexture3D::MAX_NUMBER_OF_TEXTURES )
                                     + "</b> textures will be applied." );
    this->setAllowedAreas( Qt::AllDockWidgetAreas );
    this->setFeatures( QDockWidget::AllDockWidgetFeatures );

    QWidget* panel = new QWidget( this );

    m_layout = new QVBoxLayout();
    m_layout->setContentsMargins( 0, 0, 0, 0 );

    // create the move-up/down buttons
    QAction* downAction = new QAction( WQt4Gui::getMainWindow()->getIconManager()->getIcon( "go-down" ), "Move selected colormap down.", this );
    connect( downAction, SIGNAL( triggered() ), this, SLOT( moveItemDown() ) );

    QAction* upAction = new QAction( WQt4Gui::getMainWindow()->getIconManager()->getIcon( "go-up" ), "Move selected colormap up.", this );
    connect( upAction, SIGNAL( triggered() ), this, SLOT( moveItemUp() ) );

    QAction* bottomAction = new QAction( WQt4Gui::getMainWindow()->getIconManager()->getIcon( "go-bottom" ), "Move selected colormap to the bottom.",
                                         this );
    connect( bottomAction, SIGNAL( triggered() ), this, SLOT( moveItemBottom() ) );

    QAction* topAction = new QAction( WQt4Gui::getMainWindow()->getIconManager()->getIcon( "go-top" ), "Move selected colormap to the top.", this );
    connect( topAction, SIGNAL( triggered() ), this, SLOT( moveItemTop() ) );

    addTitleAction( bottomAction );
    addTitleAction( downAction );
    addTitleAction( upAction );
    addTitleAction( topAction );

    m_layout->addWidget( m_textureListWidget );

    connect( m_textureListWidget, SIGNAL( itemClicked( QListWidgetItem* ) ), this, SLOT( handleTextureClicked() ) );

    panel->setLayout( m_layout );
    setWidget( panel );

    // get the proper subscriptions to the colormapper signals.
    boost::shared_ptr< WGEColormapping > p = WGEColormapping::instance();
    m_registerConnection = p->subscribeSignal( WGEColormapping::Registered,
            static_cast< WGEColormapping::TextureRegisterHandler >( boost::bind( &WQtColormapper::pushUpdateEvent, this ) ) );
    m_deregisterConnection = p->subscribeSignal( WGEColormapping::Deregistered,
            static_cast< WGEColormapping::TextureDeregisterHandler >( boost::bind( &WQtColormapper::pushUpdateEvent, this ) ) );
    m_replaceConnection = p->subscribeSignal( WGEColormapping::Replaced,
            static_cast< WGEColormapping::TextureReplaceHandler >( boost::bind( &WQtColormapper::pushUpdateEvent, this ) ) );
    m_sortConnection = p->subscribeSignal( WGEColormapping::Sorted,
            static_cast< WGEColormapping::TextureSortHandler>( boost::bind( &WQtColormapper::pushUpdateEvent, this ) ) );
}

WQtColormapper::~WQtColormapper()
{
    m_registerConnection.disconnect();
    m_deregisterConnection.disconnect();
    m_sortConnection.disconnect();
}

WQtColormapper::WQtTextureListItem::WQtTextureListItem( const osg::ref_ptr< WGETexture3D > texture, WQtColormapper* cmapper, QListWidget* parent ):
    QListWidgetItem( QString::fromStdString( texture->name()->get() ), parent ),
    m_texture( texture )
{
    // only show the filename. If the name is not a filename, the texture name is used directly
    std::vector< std::string > names = string_utils::tokenize( texture->name()->get().c_str(), "/" );
    if( names.size() )
    {
        setText( QString::fromStdString( names.back() ) );
    }

    // we need to know the name of the texture
    m_nameConnection = m_texture->name()->getUpdateCondition()->subscribeSignal(
        boost::bind( &WQtColormapper::pushUpdateEvent, cmapper )
    );
}

WQtColormapper::WQtTextureListItem::~WQtTextureListItem()
{
    m_nameConnection.disconnect();
}

const osg::ref_ptr< WGETexture3D > WQtColormapper::WQtTextureListItem::getTexture() const
{
    return m_texture;
}

void WQtColormapper::pushUpdateEvent()
{
    // create a new event for this and insert it into event queue
    QCoreApplication::postEvent( this,  new WUpdateTextureSorterEvent() );
}

bool WQtColormapper::event( QEvent* event )
{
    // a texture added/removed/sorted/moved
    if( event->type() == WQT_UPDATE_TEXTURE_SORTER_EVENT )
    {
        update();
        return true;
    }

    return WQtDockWidget::event( event );
}

void WQtColormapper::update()
{
    boost::shared_ptr< WGEColormapping > cm = WGEColormapping::instance();

    WGEColormapping::TextureContainerType::ReadTicket r = cm->getReadTicket();

    // we need to store the last selected texture if there was any
    osg::ref_ptr< WGETexture3D > lastSelected;
    WQtTextureListItem* item = dynamic_cast< WQtTextureListItem* >( m_textureListWidget->item( m_textureListWidget->currentIndex().row() ) );
    if( item )
    {
        lastSelected = item->getTexture();
    }

    // remove all items and rebuild list.
    m_textureListWidget->clear();
    for( WGEColormapping::TextureConstIterator iter = r->get().begin(); iter != r->get().end(); ++iter )
    {
        WQtTextureListItem* item = new WQtTextureListItem( *iter, this, m_textureListWidget );
        m_textureListWidget->addItem( item );    // the list widget removes the item (and frees the reference to the texture pointer).

        // is the item the texture that has been selected previously?
        if( item->getTexture() == lastSelected )
        {
            m_textureListWidget->setCurrentItem( item );
        }
    }
}

void WQtColormapper::handleTextureClicked()
{
    // maybe someone is interested in this signal:
    emit textureSelectionChanged(
        static_cast< WQtTextureListItem* >( m_textureListWidget->item( m_textureListWidget->currentIndex().row() ) )->getTexture()
    );
}

void WQtColormapper::moveItemDown()
{
    boost::shared_ptr< WGEColormapping > cm = WGEColormapping::instance();
    WQtTextureListItem* item = dynamic_cast< WQtTextureListItem* >( m_textureListWidget->item( m_textureListWidget->currentIndex().row() ) );
    if( item )
    {
        cm->moveDown( item->getTexture() );
    }
}

void WQtColormapper::moveItemUp()
{
    boost::shared_ptr< WGEColormapping > cm = WGEColormapping::instance();
    WQtTextureListItem* item = dynamic_cast< WQtTextureListItem* >( m_textureListWidget->item( m_textureListWidget->currentIndex().row() ) );
    if( item )
    {
        cm->moveUp( item->getTexture() );
    }
}

void WQtColormapper::moveItemBottom()
{
    boost::shared_ptr< WGEColormapping > cm = WGEColormapping::instance();
    WQtTextureListItem* item = dynamic_cast< WQtTextureListItem* >( m_textureListWidget->item( m_textureListWidget->currentIndex().row() ) );
    if( item )
    {
        cm->moveToBottom( item->getTexture() );
    }
}

void WQtColormapper::moveItemTop()
{
    boost::shared_ptr< WGEColormapping > cm = WGEColormapping::instance();
    WQtTextureListItem* item = dynamic_cast< WQtTextureListItem* >( m_textureListWidget->item( m_textureListWidget->currentIndex().row() ) );
    if( item )
    {
        cm->moveToTop( item->getTexture() );
    }
}

void WQtColormapper::selectTexture( boost::shared_ptr< WDataSet > dataSet )
{
    // simply check each item against the texture in the specified dataset
    for( int i = 0; i < m_textureListWidget->count(); ++i )
    {
        WQtTextureListItem* item = dynamic_cast< WQtTextureListItem* >( m_textureListWidget->item( i ) );
        if(item && dataSet->isTexture() && ( item->getTexture() == dataSet->getTexture() ) )
        {
            m_textureListWidget->setCurrentItem( item );
        }
    }
}

