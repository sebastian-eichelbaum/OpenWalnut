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

#include "WPropertyBoolWidget.h"

#include "../../../common/WPropertyVariable.h"

WPropertyBoolWidget::WPropertyBoolWidget( WPropBool property, QGridLayout* propertyGrid, QWidget* parent, bool asButton ):
    WPropertyWidget( property, propertyGrid, parent ),
    m_boolProperty( property ),
    m_checkbox( this ),
    m_button( this ),
    m_layout(),
    m_asButton( asButton )
{
    // initialize members
    m_button.setCheckable( true );
    update();

    // layout both against each other
    m_button.setVisible( m_asButton );
    m_checkbox.setVisible( !m_asButton );
    m_layout.addWidget( m_asButton ? static_cast< QWidget* >( &m_button ) : static_cast< QWidget* >( &m_checkbox ) );

    if ( m_asButton )
    {
        m_layout.setContentsMargins( 1, 1, 1, 1 );
    }
    setLayout( &m_layout );

    // connect the modification signal of m_checkbox with our callback
    connect( &m_checkbox, SIGNAL( toggled( bool ) ), this, SLOT( changed() ) );
    connect( &m_button, SIGNAL( toggled( bool ) ), this, SLOT( changed() ) );
}

WPropertyBoolWidget::~WPropertyBoolWidget()
{
    // cleanup
}

void WPropertyBoolWidget::update()
{
    // simply set the new state
    m_checkbox.setChecked( m_boolProperty->get() );
    m_button.setChecked( m_boolProperty->get() );
}

QPushButton* WPropertyBoolWidget::getButton()
{
    return &m_button;
}

void WPropertyBoolWidget::changed()
{
    // set the value
    if ( m_asButton )
    {
        invalidate( !m_boolProperty->set( m_button.isChecked() ) );
    }
    else
    {
        invalidate( !m_boolProperty->set( m_checkbox.isChecked() ) );
    }
}

