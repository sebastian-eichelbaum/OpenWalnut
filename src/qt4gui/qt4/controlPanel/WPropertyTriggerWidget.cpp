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

#include "WPropertyTriggerWidget.h"
#include "WPropertyTriggerWidget.moc"

WPropertyTriggerWidget::WPropertyTriggerWidget( WPropTrigger property, QGridLayout* propertyGrid, QWidget* parent ):
    WPropertyWidget( property, propertyGrid, parent ),
    m_triggerProperty( property ),
    m_button( &m_parameterWidgets ),
    m_layout( &m_parameterWidgets ),
    m_asText( &m_informationWidgets ),
    m_infoLayout( &m_informationWidgets )
{
    // initialize members
    m_button.setCheckable( true );
    update();

    // layout both against each other
    m_layout.addWidget( &m_button );
    m_layout.setMargin( WGLOBAL_MARGIN );
    m_layout.setSpacing( WGLOBAL_SPACING );
    m_button.setText( QString::fromStdString( property->getDescription() ) );

    m_parameterWidgets.setLayout( &m_layout );

    // Information Output ( Property Purpose = PV_PURPOSE_INFORMATION )
    m_infoLayout.addWidget( &m_asText );
    m_infoLayout.setMargin( WGLOBAL_MARGIN );
    m_infoLayout.setSpacing( WGLOBAL_SPACING );
    m_informationWidgets.setLayout( &m_infoLayout );

    // connect the modification signal of m_checkbox with our callback
    connect( &m_button, SIGNAL( toggled( bool ) ), this, SLOT( changed() ) );
}

WPropertyTriggerWidget::~WPropertyTriggerWidget()
{
    // cleanup
}

void WPropertyTriggerWidget::update()
{
    // simply set the new state
    m_button.setChecked( m_triggerProperty->get() == WPVBaseTypes::PV_TRIGGER_TRIGGERED );
    m_button.setEnabled( m_triggerProperty->get() == WPVBaseTypes::PV_TRIGGER_READY );

    // do not forget to update the label
    m_asText.setText( m_triggerProperty->get() == WPVBaseTypes::PV_TRIGGER_TRIGGERED ? QString( "Operation triggered" ) : QString( "Ready" ) );
}

QPushButton* WPropertyTriggerWidget::getButton()
{
    return &m_button;
}

void WPropertyTriggerWidget::changed()
{
    // set the value
    invalidate( !m_triggerProperty->set( m_button.isChecked() ? WPVBaseTypes::PV_TRIGGER_TRIGGERED : WPVBaseTypes::PV_TRIGGER_READY ) );
}

