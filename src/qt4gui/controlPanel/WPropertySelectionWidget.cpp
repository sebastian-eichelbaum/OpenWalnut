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
#include <QtGui/QScrollBar>


#include "core/common/WLogger.h"
#include "core/common/WPropertyVariable.h"
#include "core/common/WItemSelector.h"
#include "core/common/WItemSelection.h"

#include "../WGuiConsts.h"

#include "WPropertySelectionWidget.h"
#include "WPropertySelectionWidget.moc"

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

    if( ( pix.width() > maxW ) || ( pix.height() > maxH ) )
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
    m_layout.setMargin( WGLOBAL_MARGIN );
    m_layout.setSpacing( WGLOBAL_SPACING );

    // initialize the proper widget
    // Lists are used if the selection of multiple elements is allowed
    if( m_selectionProperty->countConstraint( PC_SELECTONLYONE ) != 0 )
    {
        m_combo = new QComboBox( &m_parameterWidgets );

        // layout
        m_layout.addWidget( m_combo, 0, 0 );

        // connect
        connect( m_combo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( comboSelectionChanged( int ) ) );
    }
    else
    {
        m_list = new QListWidget( &m_parameterWidgets );
        m_list->setSelectionMode( QAbstractItemView::ExtendedSelection );

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
    if( m_property->getPurpose() == PV_PURPOSE_INFORMATION )
    {
        m_informationWidgets.setLayout( &m_layout );

        if( m_list )
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

    // we need to read-lock the whole selection to ensure that the underlying selection can't be changed while creating the items
    s.lock();
    // we also need to check whether the selector is valid. Only valid selectors are allowed to use. You can check validity by calling
    // WItemSelector::isValid. The simpler way is to get a new selector directly as selectors in properties are always valid (due to the
    // WPropertyConstraintIsValid - constraint).
    // NOTE: do NOT overwrite the current selector in "s" as it keeps the lock.
    WItemSelector sValid = m_selectionProperty->get();

    //apply selection
    if( m_combo )
    {
        disconnect( m_combo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( comboSelectionChanged( int ) ) );

        m_combo->clear();

        // add all items from the selection set:
        for( size_t i = 0; i < sValid.sizeAll(); ++i )
        {
            m_combo->addItem( QString::fromStdString( sValid.atAll( i )->getName() ) );
            // if there is an icon -> show it
            if( sValid.atAll( i )->getIcon() )
            {
                // scale the pixmap to a maximum size if larger
                QPixmap pix = ensureSize( QPixmap( sValid.atAll( i )->getIcon() ) );

                // set icon
                m_combo->setItemIcon( i, QIcon( pix ) );
                m_combo->setIconSize( QSize( pix.width(), pix.height() ) );
            }
        }

        // mark the currently selected item. Just take care that there might be no item selected.
        if( sValid.size() == 0 )
        {
            m_combo->setCurrentIndex( -1 );
        }
        else
        {
            // as there is the SELECTONLYONE constraint -> if something is selected, it always is the first one
            m_combo->setCurrentIndex( sValid.getItemIndexOfSelected( 0 ) );
        }

        connect( m_combo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( comboSelectionChanged( int ) ) );
    }
    else
    {
        disconnect( m_list, SIGNAL( itemSelectionChanged() ), this, SLOT( listSelectionChanged() ) );

        int scrollPosition = m_list->verticalScrollBar()->sliderPosition();

        m_list->clearSelection();
        m_list->clear();

        // add all items from the selection set:
        for( size_t i = 0; i < sValid.sizeAll(); ++i )
        {
            // Create a custom widget which contains the name and description
            QWidget* widget = new QWidget( m_list );
            QGridLayout* layoutWidget = new QGridLayout();

            int column = 0;
            // if there is an icon -> show it
            if( sValid.atAll( i )->getIcon() )
            {
                QLabel* icon = new QLabel();
                QSizePolicy sizePolicy( QSizePolicy::Maximum, QSizePolicy::Preferred ); // <-- scale it down
                icon->setSizePolicy( sizePolicy );
                icon->setPixmap( ensureSize( QPixmap( sValid.atAll( i )->getIcon() ) ) );
                layoutWidget->addWidget( icon, 0, 0, 2, 1 );

                ++column;
            }

            // Add Name and Description
            layoutWidget->addWidget( new QLabel( "<b>" + QString::fromStdString( sValid.atAll( i )->getName() )+ "</b>" ), 0, column );
            // if there is no description -> no widget added to save space
            if( !sValid.atAll( i )->getDescription().empty() )
            {
                layoutWidget->addWidget(  new QLabel( QString::fromStdString( sValid.atAll( i )->getDescription() ) ), 1, column );
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

        // select all items
        for( size_t i = 0; i < sValid.size(); ++i )
        {
            m_list->item( sValid.getItemIndexOfSelected( i ) )->setSelected( true );
        }

        m_list->verticalScrollBar()->setSliderPosition( scrollPosition );

        connect( m_list, SIGNAL( itemSelectionChanged() ), this, SLOT( listSelectionChanged() ) );
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
    if( m_update )
    {
        return;
    }

    // grab the list of selected indices
    WItemSelector::IndexList l;

    // go through the item list
    for( int i = 0; i < m_list->count(); ++i )
    {
        if( m_list->item( i )->isSelected() )
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

