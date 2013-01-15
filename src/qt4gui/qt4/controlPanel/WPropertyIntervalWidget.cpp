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

#include <QtGui/QInputDialog>

#include "../WGuiConsts.h"
#include "../WQt4Gui.h"
#include "../guiElements/WQtIntervalEdit.h"
#include "core/common/WLogger.h"
#include "core/common/WPropertyVariable.h"

#include "WPropertyIntervalWidget.h"
#include "WPropertyIntervalWidget.moc"

WPropertyIntervalWidget::WPropertyIntervalWidget( WPropInterval property, QGridLayout* propertyGrid, QWidget* parent ):
    WPropertyWidget( property, propertyGrid, parent ),
    m_intervalProperty( property ),
    m_layout(),
    m_vLayout( &m_parameterWidgets ),
    m_asText( &m_informationWidgets ),
    m_infoLayout( &m_informationWidgets ),
    m_intervalEdit( &m_parameterWidgets )
{
    // layout all the elements
    m_layout.addWidget( &m_intervalEdit );

    m_layout.setMargin( WGLOBAL_MARGIN );
    m_layout.setSpacing( WGLOBAL_SPACING );

    m_vLayout.setMargin( WGLOBAL_MARGIN );
    m_vLayout.setSpacing( WGLOBAL_SPACING );

    // add the m_layout to the vlayout
    QWidget* layoutContainer = new QWidget();
    layoutContainer->setLayout( &m_layout );
    m_vLayout.addWidget( layoutContainer );

    m_parameterWidgets.setLayout( &m_vLayout );

    // Information Output ( Property Purpose = PV_PURPOSE_INFORMATION )
    m_infoLayout.addWidget( &m_asText );
    m_infoLayout.setMargin( WGLOBAL_MARGIN );
    m_infoLayout.setSpacing( WGLOBAL_SPACING );
    m_informationWidgets.setLayout( &m_infoLayout );

    update();

    // connect the modification signal of the edit and slider with our callback
    connect( &m_intervalEdit, SIGNAL( minimumChanged() ), this, SLOT( minMaxUpdated() ) );
    connect( &m_intervalEdit, SIGNAL( maximumChanged() ), this, SLOT( minMaxUpdated() ) );
}

WPropertyIntervalWidget::~WPropertyIntervalWidget()
{
    // cleanup
}

void WPropertyIntervalWidget::update()
{
 /*   // // calculate maximum size of the text widget.
    // // XXX: this is not the optimal way but works for now
    // NO, it doesn't work on Mac OS X: You won't be able to any digits in it!, So I reset it to default which should work on other platforms too
    QString valStr = QString::number( m_integralProperty->get() );
    m_edit.setText( valStr );

       // get the min constraint
    WPVDouble::PropertyConstraintMin minC = m_integralProperty->getMin();
    WPVDouble::PropertyConstraintMax maxC = m_integralProperty->getMax();
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
    m_asText.setText( valStr );*/
}

void WPropertyIntervalWidget::minMaxUpdated()
{
    /*m_min = m_intervalEdit.getMin();
    m_max = m_intervalEdit.getMax();

    if( m_min > m_integralProperty->get() )
    {
        m_integralProperty->set( m_min );
    }
    if( m_max < m_integralProperty->get() )
    {
        m_integralProperty->set( m_max );
    }

    m_slider.setValue( toSliderValue( m_integralProperty->get() ) );*/
}

