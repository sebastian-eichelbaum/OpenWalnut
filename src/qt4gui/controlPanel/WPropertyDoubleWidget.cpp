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
#include <sstream>
#include <string>


#include "core/common/WLogger.h"
#include "core/common/WPropertyVariable.h"
#include "../WGuiConsts.h"

#include "WPropertyDoubleWidget.h"
#include "WPropertyDoubleWidget.moc"

WPropertyDoubleWidget::WPropertyDoubleWidget( WPropDouble property, QGridLayout* propertyGrid, QWidget* parent ):
    WPropertyWidget( property, propertyGrid, parent ),
    m_doubleProperty( property ),
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

WPropertyDoubleWidget::~WPropertyDoubleWidget()
{
    // cleanup
}

void WPropertyDoubleWidget::update()
{
    // // calculate maximum size of the text widget.
    // // XXX: this is not the optimal way but works for now
    // NO, it doesn't work on Mac OS X: You won't be able to any digits in it!, So I reset it to default which should work on other platforms too
    QString valStr = QString::number( m_doubleProperty->get() );
    m_edit.setText( valStr );

       // get the min constraint
    WPVDouble::PropertyConstraintMin minC = m_doubleProperty->getMin();
    WPVDouble::PropertyConstraintMax maxC = m_doubleProperty->getMax();
    bool minMaxConstrained = minC && maxC;
    if( minMaxConstrained )
    {
        // setup the slider
        m_slider.setMinimum( 0 );
        m_slider.setMaximum( 100 );
        m_min = minC->getMin();
        m_max = maxC->getMax();

        m_slider.setHidden( false );
        m_slider.setValue( toPercent( m_doubleProperty->get() ) );
    }
    else
    {
        m_slider.setHidden( true );
    }

    // do not forget to update the label
    m_asText.setText( valStr );
}

int WPropertyDoubleWidget::toPercent( double value )
{
    return 100.0 * ( ( value - m_min ) / ( m_max - m_min ) );
}

double WPropertyDoubleWidget::fromPercent( int perc )
{
    return ( static_cast< double >( perc ) / 100.0 ) * ( m_max - m_min ) + m_min;
}

void WPropertyDoubleWidget::sliderChanged( int value )
{
    if( !m_slider.isHidden() && toPercent( m_doubleProperty->get() ) != value )
    {
        // set to the property
        invalidate( !m_doubleProperty->set( fromPercent( value ) ) );    // NOTE: set automatically checks the validity of the value

        // set the value in the line edit
        m_edit.setText( QString::number( m_doubleProperty->get() ) );
    }
}

void WPropertyDoubleWidget::editChanged()
{
    // set the value in the line edit
    bool valid;
    double value = m_edit.text().toDouble( &valid );

    if( !valid )
    {
        invalidate();
        return;
    }
    // set to the property
    invalidate( !m_doubleProperty->set( value ) );    // NOTE: set automatically checks the validity of the value

    // update slider
    m_slider.setValue( toPercent( value ) );
}

void WPropertyDoubleWidget::textEdited( const QString& text )
{
    // this method does NOT set the property actually, but tries to validate it
    bool valid;

    double value = text.toDouble( &valid );
    if( !valid )
    {
        invalidate();
        return;
    }

    // simply check validity
    invalidate( !m_doubleProperty->accept( value ) );
}

