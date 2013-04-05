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
#include "WPropertyDoubleWidget.h"
#include "WPropertyColorWidget.h"
#include "WQtPropertyGroupWidget.h"

#include "WTreeItemTypes.h"
#include "WQtRoiTreeItem.h"
#include "WQtBranchTreeItem.h"

WQtBranchTreeItem::WQtBranchTreeItem( QTreeWidgetItem* parent, boost::shared_ptr< WRMBranch > branch ) :
    QTreeWidgetItem( parent, ROIBRANCH ),
    m_branch( branch )
{
    setFlags( Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDropEnabled );

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

    WPropertyStringWidget* name = new WPropertyStringWidget( branch->nameProperty(), NULL, m_itemWidget );
    name->forceInformationMode();
    name->disableTextInteraction();
    name->setToolTip( "The name of this branch." );
    labelContainerLayout->addWidget( name );

    // color
    WPropertyColorWidget* color = new WPropertyColorWidget( branch->colorProperty(), NULL, m_itemWidget );
    color->setToolTip( QString::fromStdString( branch->colorProperty()->getDescription() ) );
    color->setColorPickerButtonHidden();

    // inverse
    WPropertyBoolWidget* isnot = new WPropertyBoolWidget( branch->invertProperty(), NULL, m_itemWidget );
    isnot->setToolTip( QString::fromStdString( branch->invertProperty()->getDescription() ) );

    QLabel* grabWidget = new QLabel( m_itemWidget );
    grabWidget->setPixmap( WQt4Gui::getMainWindow()->getIconManager()->getIcon( "touchpoint_small" ).pixmap( 24, 32 ) );
    grabWidget->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
    grabWidget->setFixedWidth( 24 );
    grabWidget->setToolTip( "Drag and drop these textures to change their composition ordering." );

    // style
    QPalette palette;
    QColor defaultCol = palette.window().color();

    // label color
    QColor labelCol = defaultCol.darker( 150 );
    // property color
    QColor propertyCol = defaultCol;

    name->setStyleSheet( "background-color:" + labelCol.name() + ";" );
    labelContainer->setStyleSheet( "background-color:" + labelCol.name() + ";" );
    propertyContainer->setStyleSheet( "QWidget#propertyContainer{ background-color:" + propertyCol.name() + ";}" );

    propertyContainerLayout->addWidget( isnot );
    propertyContainerLayout->addWidget( color );

    // fill layout
    containerLayout->addWidget( grabWidget );
    containerLayout->addWidget( labelContainer );
    containerLayout->addWidget( propertyContainer );

    // compact layout
    containerLayout->setContentsMargins( 0, 2, 0, 2 );
    containerLayout->setSpacing( 0 );
    labelContainerLayout->setContentsMargins( 5, 2, 0, 2 );
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

WQtBranchTreeItem::~WQtBranchTreeItem()
{
}

WQtRoiTreeItem* WQtBranchTreeItem::addRoiItem( osg::ref_ptr< WROI > roi )
{
    WQtRoiTreeItem* rti = new WQtRoiTreeItem( this, roi, ROI );
    return rti;
}

boost::shared_ptr< WRMBranch > WQtBranchTreeItem::getBranch()
{
    return m_branch;
}

QWidget* WQtBranchTreeItem::getWidget() const
{
    return m_itemWidget;
}

class RoiSort
{
public:
    RoiSort( std::map< WROI::RefPtr, int > indexMap ):
        m_indexMap( indexMap )
    {
    }

    /**
     * Operator to compare the order of two ROIs
     *
     * \param a first ROI
     * \param b second ROI
     *
     * \return true if first is in front of second
     */
    bool operator()( WROI::RefPtr a, WROI::RefPtr b )
    {
        return ( m_indexMap[ a ] < m_indexMap[ b ] );
    }
private:
    /**
     * Map needed to know the index in the GUI representation
     */
    std::map< WROI::RefPtr, int > m_indexMap;
};

void WQtBranchTreeItem::updateRoiManagerSorting()
{
    std::map< WROI::RefPtr, int > indexMap;
    // build an index map for each roi to its item
    for( int i = 0; i < childCount(); ++i )
    {
        WQtRoiTreeItem* rti = dynamic_cast< WQtRoiTreeItem* >( child( i ) );
        WAssert( rti, "All children of a branch item need to be ROI items." );
        indexMap[ rti->getRoi() ] = i;
    }

    // this map now allows to start a comparator
    m_branch->sort( RoiSort( indexMap ) );
}
