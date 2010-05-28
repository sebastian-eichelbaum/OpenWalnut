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

#include <cmath>
#include <string>

#include <QtGui/QPalette>
#include <QtGui/QListWidgetItem>
#include <QtGui/QPushButton>

#include <boost/lexical_cast.hpp>

#include "../../../common/WLogger.h"
#include "../../../common/WPropertyVariable.h"
#include "../../../common/WItemSelector.h"
#include "../../../common/WItemSelection.h"

#include "WPropertySelectionWidget.h"

/**
 * This function ensure a maximum icon site by scaling large pixmaps. Pixmaps smaller than the maximum size are not scaled.
 *
 * \param pix the pixmap to scale if needed.
 *
 * \return the maybe scaled pixmap.
 */
QPixmap ensureSize( QPixmap pix )
{
    // maximum size
    int maxW = 32;
    int maxH = 32;

    if ( ( pix.width() > maxW ) || ( pix.height() > maxH ) )
    {
        return pix.scaled( maxW, maxH, Qt::KeepAspectRatio );
    }

    // no scaling needed
    return pix;
}

WPropertySelectionWidget::WPropertySelectionWidget( WPropSelection property, QGridLayout* propertyGrid, QWidget* parent ):
    WPropertyWidget( property, propertyGrid, parent ),
    m_selectionProperty( property ),
    m_layout( &m_parameterWidgets ),
    m_update( false )
{
    // initialize members
    m_combo = NULL;
    m_list = NULL;

    // initialize the proper widget
    // Lists are used if the selection of multiple elements is allowed
    if ( m_selectionProperty->countConstraint( PC_SELECTONLYONE ) != 0 )
    {
        // TODO(all): how to show the icon inside a combobox?
        m_combo = new QComboBox( &m_parameterWidgets );

        // add all items from the selection set:
        WItemSelector s = m_selectionProperty->get();
        for ( size_t i = 0; i < s.sizeAll(); ++i )
        {
            m_combo->addItem( QString::fromStdString( s.atAll( i ).name ) );
            // if there is an icon -> show it
            if ( s.atAll( i ).icon )
            {
                // scale the pixmap to a maximum size if larger
                QPixmap pix = ensureSize( QPixmap( s.atAll( i ).icon ) );

                // set icon
                m_combo->setItemIcon( i, QIcon( pix ) );
                m_combo->setIconSize( QSize( pix.width(), pix.height() ) );
            }
        }

        // layout
        m_layout.addWidget( m_combo, 0, 0 );

        // connect
        connect( m_combo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( comboSelectionChanged( int ) ) );
    }
    else
    {
        m_list = new QListWidget( &m_parameterWidgets );
        m_list->setSelectionMode( QAbstractItemView::ExtendedSelection );

        // add all items from the selection set:
        WItemSelector s = m_selectionProperty->get();
        for ( size_t i = 0; i < s.sizeAll(); ++i )
        {
            // Create a custom widget which contains the name and description
            QWidget* widget = new QWidget( m_list );
            QGridLayout* layoutWidget = new QGridLayout();

            int column = 0;
            // if there is an icon -> show it
            if ( s.atAll( i ).icon )
            {
                QLabel* icon = new QLabel();
                QSizePolicy sizePolicy( QSizePolicy::Maximum, QSizePolicy::Preferred ); // <-- scale it down
                icon->setSizePolicy( sizePolicy );
                icon->setPixmap( ensureSize( QPixmap( s.atAll( i ).icon ) ) );
                layoutWidget->addWidget( icon, 0, 0, 2, 1 );

                ++column;
            }

            // Add Name and Description
            layoutWidget->addWidget( new QLabel( "<b>" + QString::fromStdString( s.atAll( i ).name )+ "</b>" ), 0, column );
            // if there is no description -> no widget added to save space
            if ( !s.atAll( i ).description.empty() )
            {
                layoutWidget->addWidget(  new QLabel( QString::fromStdString( s.atAll( i ).description ) ), 1, column );
            }

            layoutWidget->setSizeConstraint( QLayout::SetMaximumSize );
            widget->setLayout( layoutWidget );

            // add Item
            QListWidgetItem* item = new QListWidgetItem();
            item->setSizeHint( widget->sizeHint() );
            m_list->addItem( item );
            m_list->setItemWidget( item, widget );
            m_list->setMinimumHeight( 150 );
        }

        // layout
        m_layout.addWidget( m_list, 0, 0 );

        // add a select-all button
        QPushButton* selAllButton = new QPushButton( "Select All", this );
        connect( selAllButton, SIGNAL( released() ), m_list, SLOT( selectAll() ) );
        m_layout.addWidget( selAllButton, 1, 0 );

        // connect
        connect( m_list, SIGNAL( itemSelectionChanged() ), this, SLOT( listSelectionChanged() ) );
    }

    // Information Output ( Property Purpose = PV_PURPOSE_INFORMATION )
    // In this case -> use some disabled lists/combos here
    if ( m_property->getPurpose() == PV_PURPOSE_INFORMATION )
    {
        m_informationWidgets.setLayout( &m_layout );

        if ( m_list )
        {
            m_list->setEnabled( false );
        }
        else
        {
            m_combo->setEnabled( false );
        }
    }
    else
    {
        m_parameterWidgets.setLayout( &m_layout );
    }

    // set the initial values
    update();
}

WPropertySelectionWidget::~WPropertySelectionWidget()
{
    // cleanup
}

void WPropertySelectionWidget::update()
{
    m_update = true;

    WItemSelector s = m_selectionProperty->get();

    //apply selection
    if ( m_combo )
    {
        // mark the currently selected item. Just take care that there might be no item selected.
        if ( s.size() == 0 )
        {
            m_combo->setCurrentIndex( -1 );
        }
        else
        {
            // as there is the SELECTONLYONE constraint -> if something is selected, it always is the first one
            m_combo->setCurrentIndex( s.getItemIndexOfSelected( 0 ) );
        }
    }
    else
    {
        m_list->clearSelection();

        // select all items
        WItemSelector s = m_selectionProperty->get();
        for ( size_t i = 0; i < s.size(); ++i )
        {
            m_list->item( s.getItemIndexOfSelected( i ) )->setSelected( true );
        }
    }

    m_update = false;
}

void WPropertySelectionWidget::comboSelectionChanged( int index )
{
    // grab the list of selected indices, it is easy here as it always is one
    WItemSelector::IndexList l;
    l.push_back( index );

    // new selector
    WItemSelector ns = m_selectionProperty->get().newSelector( l );

    // set it
    // NOTE: set automatically checks the validity of the value
    invalidate( !m_selectionProperty->set( ns ) );
}

void WPropertySelectionWidget::listSelectionChanged()
{
    // unfortunately this slot is also called if the selection gets changed by the program. We are not interested in this case.
    if ( m_update )
    {
        return;
    }

    // grab the list of selected indices
    WItemSelector::IndexList l;

    // go through the item list
    for ( int i = 0; i < m_list->count(); ++i )
    {
        if ( m_list->item( i )->isSelected() )
        {
            l.push_back( i );
        }
    }

    // create new selector
    WItemSelector ns = m_selectionProperty->get().newSelector( l );

    // set it
    // NOTE: set automatically checks the validity of the value
    invalidate( !m_selectionProperty->set( ns ) );
}

