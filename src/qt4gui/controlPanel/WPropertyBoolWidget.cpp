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

#include "../WGuiConsts.h"
#include "core/common/WPropertyVariable.h"

#include "WPropertyBoolWidget.h"
#include "WPropertyBoolWidget.moc"

WPropertyBoolWidget::WPropertyBoolWidget( WPropBool property, QGridLayout* propertyGrid, QWidget* parent ):
    WPropertyWidget( property, propertyGrid, parent ),
    m_boolProperty( property ),
    m_checkbox( &m_parameterWidgets ),
    m_layout( &m_parameterWidgets ),
    m_asText( &m_informationWidgets ),
    m_infoLayout( &m_informationWidgets )
{
    // initialize members
    update();

    m_checkbox.setMinimumHeight( WMIN_WIDGET_HEIGHT );

    // layout both against each other
    m_layout.addWidget( static_cast< QWidget* >( &m_checkbox ) );
    m_layout.setMargin( WGLOBAL_MARGIN );
    m_layout.setSpacing( WGLOBAL_SPACING );
    m_parameterWidgets.setLayout( &m_layout );

    // Information Output ( Property Purpose = PV_PURPOSE_INFORMATION )
    m_infoLayout.addWidget( &m_asText );
    m_infoLayout.setMargin( WGLOBAL_MARGIN );
    m_infoLayout.setSpacing( WGLOBAL_SPACING );
    m_informationWidgets.setLayout( &m_infoLayout );

    // connect the modification signal of m_checkbox with our callback
    connect( &m_checkbox, SIGNAL( toggled( bool ) ), this, SLOT( checkboxChanged() ) );
}

WPropertyBoolWidget::~WPropertyBoolWidget()
{
    // cleanup
}

void WPropertyBoolWidget::update()
{
    // simply set the new state
    m_checkbox.setChecked( m_boolProperty->get() );
    m_asText.setText( m_boolProperty->get() ? QString( "Yes" ) : QString( "No" ) );
}

void WPropertyBoolWidget::checkboxChanged()
{
    invalidate( !m_boolProperty->set( m_checkbox.isChecked() ) );
    m_checkbox.setChecked( m_boolProperty->get() );
}

