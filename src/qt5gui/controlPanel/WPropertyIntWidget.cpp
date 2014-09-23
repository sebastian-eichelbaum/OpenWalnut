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

#include <algorithm>
#include <cmath>
#include <limits>
#include <sstream>
#include <iostream>
#include <string>

#include <QInputDialog>

#include "../WGuiConsts.h"
#include "../WQt4Gui.h"
#include "../guiElements/WQtIntervalEdit.h"
#include "core/common/WLogger.h"
#include "core/common/WPropertyVariable.h"

#include "WPropertyIntWidget.h"

int WPropertyIntWidget::SliderResolution = 10000;

WPropertyIntWidget::WPropertyIntWidget( WPropInt property, QGridLayout* propertyGrid, QWidget* parent ):
    WPropertyWidget( property, propertyGrid, parent ),
    m_integralProperty( property ),
    m_slider( Qt::Horizontal, &m_parameterWidgets ),
    m_edit( &m_parameterWidgets ),
    m_layout(),
    m_vLayout( &m_parameterWidgets ),
    m_asText( &m_informationWidgets ),
    m_infoLayout( &m_informationWidgets ),
    m_intervalEdit( &m_parameterWidgets )
{
    m_layout.addWidget( &m_slider );

    m_layout.addWidget( &m_edit );
    m_layout.setMargin( WGLOBAL_MARGIN );
    m_layout.setSpacing( WGLOBAL_SPACING );
    m_vLayout.setMargin( WGLOBAL_MARGIN );
    m_vLayout.setSpacing( WGLOBAL_SPACING );

    m_edit.setMinimumHeight( WMIN_WIDGET_HEIGHT );
    m_slider.setMinimumHeight( WMIN_WIDGET_HEIGHT );

    // add the m_layout to the vlayout
    QWidget* layoutContainer = new QWidget();
    layoutContainer->setLayout( &m_layout );
    m_vLayout.addWidget( layoutContainer );

    // configure the interval edit
    m_vLayout.addWidget( &m_intervalEdit );
    if( !WQt4Gui::getSettings().value( "qt4gui/sliderMinMaxEdit", false ).toBool() )
    {
        m_intervalEdit.hide();
    }

    m_parameterWidgets.setLayout( &m_vLayout );

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
    connect( &m_intervalEdit, SIGNAL( minimumChanged() ), this, SLOT( minMaxUpdated() ) );
    connect( &m_intervalEdit, SIGNAL( maximumChanged() ), this, SLOT( minMaxUpdated() ) );
}

WPropertyIntWidget::~WPropertyIntWidget()
{
    // cleanup
}

void WPropertyIntWidget::update()
{
    // // calculate maximum size of the text widget.
    // // XXX: this is not the optimal way but works for now
    // NO, it doesn't work on Mac OS X: You won't be able to any digits in it!, So I reset it to default which should work on other platforms too
    QString valStr = QString::number( m_integralProperty->get() );
    m_edit.setText( valStr );

       // get the min constraint
    WPVInt::PropertyConstraintMin minC = m_integralProperty->getMin();
    WPVInt::PropertyConstraintMax maxC = m_integralProperty->getMax();
    bool minMaxConstrained = minC && maxC;
    if( minMaxConstrained )
    {
        // setup the slider
        m_slider.setMinimum( 0 );
        m_slider.setMaximum( SliderResolution );

        // update the interval edit too
        m_intervalEdit.setAllowedMin( minC->getMin() );
        m_intervalEdit.setAllowedMax( maxC->getMax() );
        m_min = m_intervalEdit.getMin();
        m_max = m_intervalEdit.getMax();

        // updating the interval edit causes the proper values to be set in m_min and m_max
        m_slider.setHidden( false );
        m_intervalEdit.setHidden( !WQt4Gui::getSettings().value( "qt4gui/sliderMinMaxEdit", false ).toBool() );
        m_slider.setValue( toSliderValue( m_integralProperty->get() ) );
    }
    else
    {
        m_slider.setHidden( true );
        m_intervalEdit.setHidden( true );
    }

    // do not forget to update the label
    m_asText.setText( valStr );
}

int WPropertyIntWidget::toSliderValue( double value )
{
    int perc = static_cast< double >( SliderResolution ) * ( ( value - m_min ) / ( m_max - m_min ) );
    return std::min( std::max( perc, 0 ), SliderResolution );
}

double WPropertyIntWidget::fromSliderValue( int perc )
{
    return ( static_cast< double >( perc ) / static_cast< double >( SliderResolution ) ) * ( m_max - m_min ) + m_min;
}

void WPropertyIntWidget::sliderChanged( int value )
{
    if( !m_slider.isHidden() && toSliderValue( m_integralProperty->get() ) != value )
    {
        // set to the property
        invalidate( !m_integralProperty->set( fromSliderValue( value ) ) );    // NOTE: set automatically checks the validity of the value

        // set the value in the line edit
        m_edit.setText( QString::number( m_integralProperty->get() ) );
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
    // set to the property
    invalidate( !m_integralProperty->set( value ) );    // NOTE: set automatically checks the validity of the value

    // update slider
    m_slider.setValue( toSliderValue( value ) );
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
    invalidate( !m_integralProperty->accept( value ) );
}

void WPropertyIntWidget::minMaxUpdated()
{
    m_min = m_intervalEdit.getMin();
    m_max = m_intervalEdit.getMax();

    if( m_min > m_integralProperty->get() )
    {
        m_integralProperty->set( m_min );
    }
    if( m_max < m_integralProperty->get() )
    {
        m_integralProperty->set( m_max );
    }

    m_slider.setValue( toSliderValue( m_integralProperty->get() ) );
}

