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
#include <cmath>
#include <string>
#include <algorithm>

#include <boost/lexical_cast.hpp>

#include "../WGuiConsts.h"
#include "core/common/WLogger.h"
#include "core/common/WPropertyVariable.h"

#include "WPropertyIntWidget.h"
#include "WPropertyIntWidget.moc"

WPropertyIntWidget::WPropertyIntWidget( WPropInt property, QGridLayout* propertyGrid, QWidget* parent ):
    WPropertyWidget( property, propertyGrid, parent ),
    m_intProperty( property ),
    m_slider( Qt::Horizontal, &m_parameterWidgets ),
    m_edit( &m_parameterWidgets ),
    m_layout( &m_parameterWidgets ),
    m_asText( &m_informationWidgets ),
    m_infoLayout( &m_informationWidgets )
{
    // layout both against each other
    m_layout.addWidget( &m_slider );
    m_layout.addWidget( &m_edit );
    m_layout.setMargin( WGLOBAL_MARGIN );
    m_layout.setSpacing( WGLOBAL_SPACING );
    m_parameterWidgets.setLayout( &m_layout );

    // Information Output ( Property Purpose = PV_PURPOSE_INFORMATION )
    m_infoLayout.addWidget( &m_asText );
    m_infoLayout.setMargin( WGLOBAL_MARGIN );
    m_infoLayout.setSpacing( WGLOBAL_SPACING );
    m_informationWidgets.setLayout( &m_infoLayout );

    m_slider.setMinimumWidth( WMIN_SLIDER_WIDTH );

    update();

    // connect the modification signal of the edit and slider with our callback
    connect( &m_slider, SIGNAL( valueChanged( int ) ), this, SLOT( sliderChanged( int ) ) );
    connect( &m_edit, SIGNAL( editingFinished() ), this, SLOT( editChanged() ) );
    connect( &m_edit, SIGNAL( textEdited( const QString& ) ), this, SLOT( textEdited( const QString& ) ) );
}

WPropertyIntWidget::~WPropertyIntWidget()
{
    // cleanup
}

void WPropertyIntWidget::update()
{
    // // calculate maximum size of the text widget.
    // // XXX: this is not the optimal way but works for now
    // Same as in WPropertyDouble.cpp: This does not work as expected on Mac OS X => reset to default

    // set the initial values
    QString valStr = QString::number( m_intProperty->get() );
    m_edit.setText( valStr );

    // get the min constraint
    WPVInt::PropertyConstraintMin minC = m_intProperty->getMin();
    WPVInt::PropertyConstraintMax maxC = m_intProperty->getMax();
    bool minMaxConstrained = minC && maxC;
    if( minMaxConstrained )
    {
        m_slider.setHidden( false );
        m_slider.setMinimum( minC->getMin() );
        m_slider.setMaximum( maxC->getMax() );
        m_slider.setValue( m_intProperty->get() );

        m_edit.setMaximumWidth( fontMetrics().width( QString::number( - ( maxC->getMax() - minC->getMin() ) ) ) + 5 );
    }
    else
    {
        m_slider.setHidden( true );
    }

    // do not forget to update the label
    m_asText.setText( valStr );
}

void WPropertyIntWidget::sliderChanged( int value )
{
    if( !m_slider.isHidden() )
    {
        // set the value in the line edit
        m_edit.setText( QString::number( value ) );

        // set to the property
        invalidate( !m_intProperty->set( value ) );    // NOTE: set automatically checks the validity of the value
    }
}

void WPropertyIntWidget::editChanged()
{
    // set the value in the line edit
    bool valid;
    int value = m_edit.text().toInt( &valid );
    if( !valid )
    {
        invalidate();
        return;
    }

    // update slider
    m_slider.setValue( value );

    // set to the property
    invalidate( !m_intProperty->set( value ) );    // NOTE: set automatically checks the validity of the value
}

void WPropertyIntWidget::textEdited( const QString& text )
{
    // this method does NOT set the property actually, but tries to validate it
    bool valid;
    int value = text.toInt( &valid );
    if( !valid )
    {
        invalidate();
        return;
    }

    // simply check validity
    invalidate( !m_intProperty->accept( value ) );
}

