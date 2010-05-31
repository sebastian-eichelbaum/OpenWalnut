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

#include "../../../common/WLogger.h"
#include "../../../common/WPropertyVariable.h"

#include "WPropertyIntWidget.h"

WPropertyIntWidget::WPropertyIntWidget( WPropInt property, QGridLayout* propertyGrid, QWidget* parent ):
    WPropertyWidget( property, propertyGrid, parent ),
    m_intProperty( property ),
    m_slider( Qt::Horizontal, &m_parameterWidgets ),
    m_edit( &m_parameterWidgets ),
    m_layout( &m_parameterWidgets ),
    m_asText( &m_informationWidgets ),
    m_infoLayout( &m_informationWidgets )
{
    // initialize members
    m_edit.resize( m_edit.minimumSizeHint().width(), m_edit.size().height() );
    m_edit.setMaximumWidth( m_edit.minimumSizeHint().width() );

    // layout both against each other
    m_layout.addWidget( &m_slider );
    m_layout.addWidget( &m_edit );
    m_parameterWidgets.setLayout( &m_layout );

    // Information Output ( Property Purpose = PV_PURPOSE_INFORMATION )
    m_infoLayout.addWidget( &m_asText );
    m_informationWidgets.setLayout( &m_infoLayout );

    update();

    // connect the modification signal of the edit and slider with our callback
    connect( &m_slider, SIGNAL( valueChanged( int ) ), this, SLOT( sliderChanged( int ) ) );
    connect( &m_edit, SIGNAL( returnPressed() ), this, SLOT( editChanged() ) );
    connect( &m_edit, SIGNAL( textEdited( const QString& ) ), this, SLOT( textEdited( const QString& ) ) );
}

WPropertyIntWidget::~WPropertyIntWidget()
{
    // cleanup
}

void WPropertyIntWidget::update()
{
    // get the min constraint
    WPVInt::PropertyConstraintMin minC = m_intProperty->getMin();
    int min = 0;
    if ( minC.get() )
    {
        min = minC->getMin();
    }
    else
    {
        WLogger::getLogger()->addLogMessage(
                std::string( "The property has no minimum constraint. You should define it to avoid unexpected behaviour." ) +
                std::string( "Using default (" + boost::lexical_cast< std::string >( min ) + ")." ),
                "PropertyWidget( " + m_intProperty->getName() + " )", LL_WARNING );
    }

    // get the max constraint
    WPVInt::PropertyConstraintMax maxC = m_intProperty->getMax();
    int max = 100;
    if ( maxC.get() )
    {
        max = maxC->getMax();
    }
    else
    {
        WLogger::getLogger()->addLogMessage(
                std::string( "The property has no maximum constraint. You should define it to avoid unexpected behaviour." ) +
                std::string( "Using default (" + boost::lexical_cast< std::string >( max ) + ")." ),
                "PropertyWidget( " + m_intProperty->getName() + " )", LL_WARNING );
    }

    // setup the slider
    m_slider.setMinimum( min );
    m_slider.setMaximum( max );

    // calculate maximum size of the text widget.
    // XXX: this is not the optimal way but works for now
    // int length = min < 0 ? 3 : 2;   // reserve some extra space for the "-" in negative numbers
    // float fmax = static_cast<float>( std::max( std::abs( min ), std::abs( max ) ) );    // use the number with the most numbers
    // while ( ( fmax / 10 ) >= 1.0 )
    // {
    //     ++length;
    //     fmax /= 10.0;
    // }
    int length = 6; // use fixed length to have a uniform look among several widgets

    // resize the text widget
    // m_edit.setMaxLength( length );
    m_edit.setMaximumWidth( m_edit.minimumSizeHint().width() * length / 2 );
    // m_edit.setMinimumWidth( m_edit.minimumSizeHint().width() * length / 4 );
    // m_edit.resize( m_edit.minimumSizeHint().width() * length / 4, m_edit.size().height() );

    // set the initial values
    QString valStr = QString( boost::lexical_cast< std::string >( m_intProperty->get() ).c_str() );
    m_edit.setText( valStr );
    m_slider.setValue( m_intProperty->get() );

    // do not forget to update the label
    m_asText.setText( valStr );
}

void WPropertyIntWidget::sliderChanged( int value )
{
    // set the value in the line edit
    m_edit.setText( QString( boost::lexical_cast< std::string >( value ).c_str() ) );

    // set to the property
    invalidate( !m_intProperty->set( value ) );    // NOTE: set automatically checks the validity of the value
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

    // set to the property
    invalidate( !m_intProperty->set( value ) );    // NOTE: set automatically checks the validity of the value
}

void WPropertyIntWidget::textEdited( const QString& text )
{
    // this method does NOT set the property actually, but tries to validate it
    bool valid;
    int value = text.toInt( &valid );
    if ( !valid )
    {
        invalidate();
        return;
    }

    // simply check validity
    invalidate( !m_intProperty->accept( value ) );
}

