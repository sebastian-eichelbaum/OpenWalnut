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

#include <QtCore/QList>
#include <QtGui/QScrollArea>
#include <QtGui/QVBoxLayout>
#include <QtGui/QListWidgetItem>
#include <QtGui/QApplication>
#include <QtGui/QWidgetAction>

#include "../guiElements/WScaleLabel.h"
#include "../WQt4Gui.h"
#include "../WMainWindow.h"
#include "WPropertyBoolWidget.h"
#include "WPropertyStringWidget.h"

#include "WTreeItemTypes.h"
#include "WQtRoiTreeItem.h"

WQtRoiTreeItem::WQtRoiTreeItem( QTreeWidgetItem * parent, osg::ref_ptr< WROI > roi, WTreeItemType type ) :
    QTreeWidgetItem( parent, type ),
    m_roi( roi )
{
    setFlags( Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled );

    // create nice widget
    m_itemWidget = new QWidget( );
    QHBoxLayout* containerLayout = new QHBoxLayout();
    m_itemWidget->setLayout( containerLayout );

    // create a slider for the  for the texture
    QWidget* labelContainer = new QWidget( m_itemWidget );
    QHBoxLayout* labelContainerLayout = new QHBoxLayout();
    labelContainer->setLayout( labelContainerLayout );

    QWidget* propertyContainer = new QWidget( m_itemWidget );
    QHBoxLayout* propertyContainerLayout = new QHBoxLayout();
    propertyContainer->setLayout( propertyContainerLayout );
    propertyContainer->setObjectName( "propertyContainer" );

    // name
    WPropertyStringWidget* name = new WPropertyStringWidget( roi->nameProperty(), NULL, m_itemWidget );
    name->setToolTip( "The name of this ROI." );
    name->forceInformationMode();
    name->disableTextInteraction();

    // inverse
    WPropertyBoolWidget* isnot = new WPropertyBoolWidget( roi->invertProperty(), NULL, m_itemWidget );
    isnot->setToolTip( QString::fromStdString( roi->invertProperty()->getDescription() ) );

    // active
    WPropertyBoolWidget* active = new WPropertyBoolWidget( roi->activeProperty(), NULL, m_itemWidget );
    active->setToolTip( QString::fromStdString( roi->activeProperty()->getDescription() ) );

    // show
    WPropertyBoolWidget* show = new WPropertyBoolWidget( roi->showProperty(), NULL, m_itemWidget );
    show->setToolTip( QString::fromStdString( roi->showProperty()->getDescription() ) );

    QLabel* grabWidget = new QLabel( m_itemWidget );
    grabWidget->setPixmap( WQt4Gui::getMainWindow()->getIconManager()->getIcon( "touchpoint_small" ).pixmap( 24, 32 ) );
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

    //name->setStyleSheet( "background-color:" + labelCol.name() + ";border:none;" );
    labelContainer->setStyleSheet( "background-color:" + labelCol.name() + ";" );
    propertyContainer->setStyleSheet( "QWidget#propertyContainer{ background-color:" + propertyCol.name() + ";}" );

    // fill all layouts
    propertyContainerLayout->addWidget( isnot );
    propertyContainerLayout->addWidget( show );

    labelContainerLayout->addWidget( active );
    labelContainerLayout->addWidget( name );
    labelContainerLayout->setStretchFactor( name, 100 );
    labelContainerLayout->setStretchFactor( active, 0 );

    // fill layout
    containerLayout->addWidget( grabWidget );
    containerLayout->addWidget( labelContainer );
    containerLayout->addWidget( propertyContainer );

    // compact layout
    containerLayout->setContentsMargins( 0, 2, 0, 2 );
    containerLayout->setSpacing( 0 );
    labelContainerLayout->setContentsMargins( 2, 2, 0, 2 );
    labelContainerLayout->setSpacing( 0 );
    propertyContainerLayout->setContentsMargins( 2, 2, 0, 2 );
    propertyContainerLayout->setSpacing( 0 );

    // prefer stretching the label
    containerLayout->setStretchFactor( grabWidget, 0 );
    containerLayout->setStretchFactor( labelContainer, 100 );
    containerLayout->setStretchFactor( propertyContainer, 0 );

    // widget size constraints and policies
    m_itemWidget->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
}

WQtRoiTreeItem::~WQtRoiTreeItem()
{
}

osg::ref_ptr< WROI > WQtRoiTreeItem::getRoi()
{
    return m_roi;
}

QWidget* WQtRoiTreeItem::getWidget() const
{
    return m_itemWidget;
}

