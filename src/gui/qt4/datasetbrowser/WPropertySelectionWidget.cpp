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

#include <boost/lexical_cast.hpp>

#include "../../../common/WLogger.h"
#include "../../../common/WPropertyVariable.h"
#include "../../../common/WItemSelector.h"
#include "../../../common/WItemSelection.h"

#include "WPropertySelectionWidget.h"

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
        m_combo = new QComboBox( &m_parameterWidgets );

        // add all items from the selection set:
        WItemSelector s = m_selectionProperty->get();
        for ( size_t i = 0; i < s.sizeAll(); ++i )
        {
            m_combo->addItem( QString::fromStdString( s.atAll( i ).first ) );
        }

        // layout
        m_layout.addWidget( m_combo );

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

            // Add Name and Description
            layoutWidget->addWidget( new QLabel( "<b>" + QString::fromStdString( s.atAll( i ).first )+ "</b>" ), 0, 0 );
            layoutWidget->addWidget(  new QLabel( QString::fromStdString( s.atAll( i ).second ) ), 1, 0 );
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
        m_layout.addWidget( m_list );

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

