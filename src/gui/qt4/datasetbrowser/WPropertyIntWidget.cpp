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
#include <sstream>

#include "WPropertyIntWidget.h"

#include "../../../common/WPropertyVariable.h"

WPropertyIntWidget::WPropertyIntWidget( WPropInt property, QGridLayout* propertyGrid, QWidget* parent ):
    WPropertyWidget( property, propertyGrid, parent ),
    m_intProperty( property ),
    m_slider( Qt::Horizontal, this ),
    m_edit( this ),
    m_layout()
{
    // initialize members
    m_edit.resize( m_edit.minimumSizeHint().width() , m_edit.size().height() );
    m_edit.setMaximumWidth( m_edit.minimumSizeHint().width() );
    setLayout( &m_layout );

    // layout both against each other
    m_layout.addWidget( &m_slider );
    m_layout.addWidget( &m_edit );

    // connect the modification signal of the edit and slider with our callback
    connect( &m_slider, SIGNAL( valueChanged( int ) ), this, SLOT( sliderChanged( int ) ) );
    connect( &m_edit, SIGNAL( returnPressed() ), this, SLOT( editChanged() ) );
}

WPropertyIntWidget::~WPropertyIntWidget()
{
    // cleanup
}

void WPropertyIntWidget::sliderChanged( int value )
{
    // set the value in the line edit
    std::ostringstream s;
    s << value;
    m_edit.setText( QString( s.str().c_str() ) );

    // set to the property
    if ( !m_intProperty->accept( value ) )
    {
        // this is not a valid value!
        invalidate();
    }
    else
    {
        invalidate( false );
        m_intProperty->set( value );
    }
}

void WPropertyIntWidget::editChanged()
{
    // set the value in the line edit
    bool valid;
    int value = m_edit.text().toInt( &valid );
    if ( !valid )
    {
        invalidate();
        return;
    }

    // update slider
    m_slider.setValue( value );

    // now: is the value acceptable by the property?
    if ( !m_intProperty->accept( value ) )
    {
        invalidate();
    }
    else
    {
        invalidate( false );
        m_intProperty->set( value );
    }
}

