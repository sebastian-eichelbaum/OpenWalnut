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

WPropertyBoolWidget::WPropertyBoolWidget( WPropBool property, QGridLayout* propertyGrid, QWidget* parent ):
    WPropertyWidget( property, propertyGrid, parent ),
    m_boolProperty( property ),
    m_checkbox( this ),
    m_layout()
{
    // initialize members
    m_checkbox.setChecked( m_boolProperty->get() );

    // layout both against each other
    m_layout.addWidget( &m_checkbox );
    setLayout( &m_layout );

    // connect the modification signal of m_checkbox with our callback
    connect( &m_checkbox, SIGNAL( toggled( bool ) ), this, SLOT( changed() ) );
}

WPropertyBoolWidget::~WPropertyBoolWidget()
{
    // cleanup
}

void WPropertyBoolWidget::changed()
{
    // set the value
    m_boolProperty->set( m_checkbox.isChecked() );
}

