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
#include <QScrollArea>
#include <QVBoxLayout>
#include <QListWidgetItem>
#include <QApplication>
#include <QWidgetAction>

#include "core/common/WAssert.h"
#include "core/dataHandler/WDataSet.h"
#include "core/dataHandler/WDataHandler.h"
#include "core/dataHandler/exceptions/WDHNoSuchSubject.h"
#include "core/graphicsEngine/WGEColormapping.h"
#include "core/graphicsEngine/WGETexture.h"
#include "WPropertyBoolWidget.h"
#include "WPropertyDoubleWidget.h"
#include "WQtPropertyGroupWidget.h"
#include "../events/WUpdateTextureSorterEvent.h"
#include "../events/WEventTypes.h"
#include "../guiElements/WScaleLabel.h"
#include "../WQtGui.h"
#include "../WMainWindow.h"

#include "WQtColormapper.h"

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
    m_textureListWidget->setDragDropMode( QAbstractItemView::InternalMove );

    // be notified when moving items around
    connect( m_textureListWidget->model(), SIGNAL( rowsMoved( const QModelIndex&, int, int, const QModelIndex&, int ) ),
             this, SLOT( rowsMoved( const QModelIndex&, int, int, const QModelIndex&, int ) ) );


    QWidget* panel = new QWidget( this );

    m_layout = new QVBoxLayout();
    m_layout->setContentsMargins( 0, 0, 0, 0 );

    // create the move-up/down buttons
    QAction* downAction = new QAction( WQtGui::getMainWindow()->getIconManager()->getIcon( "go-down" ), "Move selected colormap down.", this );
    connect( downAction, SIGNAL( triggered() ), this, SLOT( moveItemDown() ) );

    QAction* upAction = new QAction( WQtGui::getMainWindow()->getIconManager()->getIcon( "go-up" ), "Move selected colormap up.", this );
    connect( upAction, SIGNAL( triggered() ), this, SLOT( moveItemUp() ) );

    QAction* bottomAction = new QAction( WQtGui::getMainWindow()->getIconManager()->getIcon( "go-bottom" ), "Move selected colormap to the bottom.",
                                         this );
    connect( bottomAction, SIGNAL( triggered() ), this, SLOT( moveItemBottom() ) );

    QAction* topAction = new QAction( WQtGui::getMainWindow()->getIconManager()->getIcon( "go-top" ), "Move selected colormap to the top.", this );
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

WQtColormapper::WQtTextureListItem::WQtTextureListItem( const osg::ref_ptr< WGETexture3D > texture, WQtColormapper* /*cmapper*/,
                                                        QListWidget* parent ):
    QListWidgetItem( parent ),
    m_texture( texture ),
    m_parent( parent )
{
    // only show the filename. If the name is not a filename, the texture name is used directly
    std::vector< std::string > names = string_utils::tokenize( texture->name()->get().c_str(), "/" );
    QString name = QString::fromStdString( texture->name()->get() );
    if( names.size() )
    {
        name = QString::fromStdString( names.back() );
    }

    // create nice widget
    m_itemWidget = new QWidget( m_parent );
    QHBoxLayout* containerLayout = new QHBoxLayout();
    m_itemWidget->setLayout( containerLayout );

    // active property
    WPropertyBoolWidget* active = new WPropertyBoolWidget( m_texture->active(), NULL, m_itemWidget );
    active->setToolTip( "Turn this texture on or off. A turned off texture is completely invisible." );

    // create a slider for the  for the texture
    QWidget* labelContainer = new QWidget( m_itemWidget );
    QHBoxLayout* labelContainerLayout = new QHBoxLayout();
    labelContainer->setLayout( labelContainerLayout );

    WPropertyStringWidget* nameLabel = new WPropertyStringWidget( m_texture->name(), NULL, m_itemWidget );
    nameLabel->setToolTip( "The name of this texture. This usually is the name of the file it was loaded from." );
    nameLabel->forceInformationMode();
    nameLabel->disableTextInteraction();

    labelContainerLayout->addWidget( nameLabel );

    // alpha property
    WPropertyDoubleWidget* alpha = new WPropertyDoubleWidget( m_texture->alpha(), NULL, m_itemWidget );
    alpha->setToolTip( "Change transparency of a texture. The higher this value, the more opaque is the texture. "
                       "This is very useful for mixing several datasets." );

    // create a button for opening the texture props

    // first, create a property group widget and an according widget action
    QScrollArea* sa = new QScrollArea( m_itemWidget );
    QWidget* props = WQtPropertyGroupWidget::createPropertyGroupBox(
                         m_texture->getProperties(), QString::fromStdString( "Configure Colormap" ), 0, sa
                     );
    sa->setWidget( props );

    // create widget action
    QWidgetAction* propAction = new QWidgetAction( m_itemWidget );
    propAction->setDefaultWidget( sa );

    // add it into an menu
    QMenu* propActionMenu = new QMenu();
    propActionMenu->addAction( propAction );

    // tool button
    QToolButton* propActionBtn = new QToolButton( m_itemWidget );
    propActionBtn->setPopupMode( QToolButton::InstantPopup );
    propActionBtn->setMenu( propActionMenu );
    propActionBtn->setIcon( WQtGui::getMainWindow()->getIconManager()->getIcon( "configure" ) );
    propActionBtn->setToolButtonStyle( Qt::ToolButtonIconOnly );
    propActionBtn->setContentsMargins( 0, 0, 0, 0 );
    propActionBtn->setAutoRaise( true );
    propActionBtn->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
    propActionBtn->setToolTip( "Show all the configuration options for this texture and its colormap." );

    QLabel* grabWidget = new QLabel( m_itemWidget );
    grabWidget->setPixmap( WQtGui::getMainWindow()->getIconManager()->getIcon( "touchpoint_small" ).pixmap( 24, 32 ) );
    grabWidget->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
    grabWidget->setFixedWidth( 24 );
    grabWidget->setToolTip( "Drag and drop these textures to change their composition ordering." );

    // style
    QPalette palette;
    QColor defaultCol = palette.window().color();

    // label color
    QColor labelCol = defaultCol.darker( 115 );
    // property color
    QColor propertyCol = defaultCol;

    propActionBtn->setStyleSheet( "background-color:" + propertyCol.name() + ";" );
    active->setStyleSheet( "background-color:" + labelCol.name() + ";" );
    alpha->setStyleSheet( "#ControlPanelPropertyWidget{ background-color:" + propertyCol.name() + ";}" );
    nameLabel->setStyleSheet( "background-color:" + labelCol.name() + ";border:none;" );
    labelContainer->setStyleSheet( "background-color:" + labelCol.name() + ";" );

    // fill layout
    containerLayout->addWidget( grabWidget );
    containerLayout->addWidget( active );
    containerLayout->addWidget( labelContainer );
    containerLayout->addWidget( alpha );
    containerLayout->addWidget( propActionBtn );

    // compact layout
    containerLayout->setContentsMargins( 0, 2, 0, 2 );
    containerLayout->setSpacing( 0 );
    labelContainerLayout->setContentsMargins( 2, 2, 0, 2 );
    labelContainerLayout->setSpacing( 0 );

    // prefer stretching the label
    containerLayout->setStretchFactor( active, 0 );
    containerLayout->setStretchFactor( grabWidget, 0 );
    containerLayout->setStretchFactor( labelContainer, 100 );
    containerLayout->setStretchFactor( alpha, 75 );
    containerLayout->setStretchFactor( propActionBtn, 0 );

    // widget size constraints and policies
    m_itemWidget->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
}

WQtColormapper::WQtTextureListItem::~WQtTextureListItem()
{
    // cleanup
}

const osg::ref_ptr< WGETexture3D > WQtColormapper::WQtTextureListItem::getTexture() const
{
    return m_texture;
}

QWidget* WQtColormapper::WQtTextureListItem::getWidget() const
{
    return m_itemWidget;
}

void WQtColormapper::pushUpdateEvent()
{
    // create a new event for this and insert it into event queue
    QCoreApplication::postEvent( this, new WUpdateTextureSorterEvent() );
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
        QWidget* widget = item->getWidget();

        m_textureListWidget->addItem( item );    // the list widget removes the item (and frees the reference to the texture pointer).
        item->setSizeHint( QSize( 0, widget->sizeHint().height() ) );
        m_textureListWidget->setItemWidget( item, widget );

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

void WQtColormapper::rowsMoved( const QModelIndex& sourceParent, int sourceStart, int sourceEnd,
                                const QModelIndex& destinationParent, int destinationRow )
{
    WAssert( sourceStart == sourceEnd, "Multiple texture items selected. This should not be the case." );
    WAssert( sourceParent == destinationParent, "Source and target parent are not the same. This should not be the case." );

    // just utilize WGEColormapper for this:
    boost::shared_ptr< WGEColormapping > cm = WGEColormapping::instance();
    WQtTextureListItem* item = dynamic_cast< WQtTextureListItem* >( m_textureListWidget->item( m_textureListWidget->currentIndex().row() ) );
    if( item )
    {
        cm->moveTo( item->getTexture(), destinationRow );
    }
}

