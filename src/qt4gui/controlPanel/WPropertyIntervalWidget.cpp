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
#include <QtGui/QAction>

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
    m_minEdit( &m_parameterWidgets ),
    m_maxEdit( &m_parameterWidgets ),
    m_resetBtn( &m_parameterWidgets )
{
    // layout all the elements
    m_layout.addWidget( new QLabel( "[" ) );
    m_layout.addWidget( &m_minEdit );
    m_layout.addWidget( new QLabel( "," ) );
    m_layout.addWidget( &m_maxEdit );
    m_layout.addWidget( new QLabel( "]" ) );

    m_layout.addWidget( &m_resetBtn );
    // WPropertyVartiable does not yet provide a default value mechanism.
    m_resetBtn.setHidden( true );

    m_minEdit.setMinimumHeight( WMIN_WIDGET_HEIGHT );
    m_maxEdit.setMinimumHeight( WMIN_WIDGET_HEIGHT );

    // create resetButton action
    QAction* reset = new QAction( WQt4Gui::getIconManager()->getIcon( "undo" ), "Reset to defaults", &m_resetBtn );
    m_resetBtn.setDefaultAction( reset );

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
    connect( &m_minEdit, SIGNAL( textEdited( const QString& ) ), this, SLOT( minMaxUpdated() ) );
    connect( &m_maxEdit, SIGNAL( textEdited( const QString& ) ), this, SLOT( minMaxUpdated() ) );
    connect( reset, SIGNAL( triggered( bool ) ), this, SLOT( reset() ) );
}

WPropertyIntervalWidget::~WPropertyIntervalWidget()
{
    // cleanup
}

void WPropertyIntervalWidget::update()
{
    QString lowValStr = QString::number( m_intervalProperty->get().getLower() );
    QString upValStr = QString::number( m_intervalProperty->get().getUpper() );
    m_minEdit.setText( lowValStr );
    m_maxEdit.setText( upValStr );

    // do not forget to update the label
    m_asText.setText( "[" + lowValStr + ", " + upValStr + "]" );
}

void WPropertyIntervalWidget::minMaxUpdated()
{
    bool validMin;
    double minValue = m_minEdit.text().toDouble( &validMin );
    bool validMax;
    double maxValue = m_maxEdit.text().toDouble( &validMax );

    // mark the prop invalid if the user specifies some strange values
    if( !( validMin && validMax ) || ( minValue > maxValue ) )
    {
        invalidate();
        return;
    }

    // set to the property
    invalidate( !m_intervalProperty->set( WIntervalDouble( minValue, maxValue ) ) );    // NOTE: set automatically checks the validity of the value
}

void WPropertyIntervalWidget::reset()
{
    // WProperties does not really provide a default value mechanism yet.
}

