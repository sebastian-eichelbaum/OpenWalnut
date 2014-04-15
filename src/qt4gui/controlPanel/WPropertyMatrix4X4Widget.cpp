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

#include <sstream>
#include <cmath>
#include <string>


#include "core/common/WStringUtils.h"
#include "core/common/WLogger.h"
#include "core/common/WPropertyVariable.h"
#include "../WGuiConsts.h"
#include "../guiElements/WScaleLabel.h"

#include "WPropertyMatrix4X4Widget.h"
#include "WPropertyMatrix4X4Widget.moc"

WPropertyMatrix4X4Widget::WPropertyMatrix4X4Widget( WPropMatrix4X4 property, QGridLayout* propertyGrid, QWidget* parent ):
    WPropertyWidget( property, propertyGrid, parent ),
    m_matrixProperty( property ),
    m_layout( &m_parameterWidgets ),
    m_asText( &m_informationWidgets ),
    m_infoLayout( &m_informationWidgets )
{
    // initialize members
    for( size_t row = 0; row < 4; ++row )
    {
        QHBoxLayout* h = new QHBoxLayout();

        for( size_t col = 0; col < 4; ++col )
        {
            size_t i = row * 4 + col;
            m_edits[ i ].setParent( &m_parameterWidgets );
            m_edits[ i ].setMinimumHeight( WMIN_WIDGET_HEIGHT );
            m_edits[ i ].resize( m_edits[ i ].minimumSizeHint().width() * 2.0, m_edits[ i ].size().height() );

            connect( &m_edits[ i ], SIGNAL( returnPressed() ), this, SLOT( editChanged() ) );
            connect( &m_edits[ i ], SIGNAL( textEdited( const QString& ) ), this, SLOT( textEdited( const QString& ) ) );

            h->addWidget( &m_edits[ i ] );
        }

        m_layout.addLayout( h );
    }

    m_parameterWidgets.setLayout( &m_layout );
    m_layout.setMargin( WGLOBAL_MARGIN );
    m_layout.setSpacing( WGLOBAL_SPACING );

    // Information Output ( Property Purpose = PV_PURPOSE_INFORMATION )
    m_infoLayout.addWidget( &m_asText );
    m_infoLayout.setMargin( WGLOBAL_MARGIN );
    m_infoLayout.setSpacing( WGLOBAL_SPACING );
    m_informationWidgets.setLayout( &m_infoLayout );

    update();
}

WPropertyMatrix4X4Widget::~WPropertyMatrix4X4Widget()
{
    // cleanup
}

void WPropertyMatrix4X4Widget::update()
{
    // set the values
    WPVBaseTypes::PV_MATRIX4X4 m = m_matrixProperty->get();

    for( size_t row = 0; row < 4; ++row )
    {
        for( size_t col = 0; col < 4; ++col )
        {
            size_t i = row * 4 + col;
            m_edits[ i ].setText( QString::fromStdString( string_utils::toString( m( row, col ) ) ) );
        }
    }

    // do not forget to update the label
    m_asText.setText( QString::fromStdString( string_utils::toString( m_matrixProperty->get() ) ) );
}

void WPropertyMatrix4X4Widget::editChanged()
{
    setPropertyFromWidgets();
}

void WPropertyMatrix4X4Widget::textEdited( const QString& /*text*/ )
{
    setPropertyFromWidgets( true );
}

void WPropertyMatrix4X4Widget::setPropertyFromWidgets( bool validateOnly )
{
    // grab all the values
    bool valid = true;
    // create a new matrix
    WPVBaseTypes::PV_MATRIX4X4 m;

    for( size_t row = 0; row < 4; ++row )
    {
        for( size_t col = 0; col < 4; ++col )
        {
            size_t i = row * 4 + col;

            // check validity
            bool tmp;
            double value = m_edits[ i ].text().toDouble( &tmp );
            valid = valid && tmp;
            m( row, col ) = value;
        }
    }

    if( !valid )
    {
        invalidate();
        return;
    }

    // set/validate to the property
    if( validateOnly )
    {
        invalidate( !m_matrixProperty->accept( m ) );
    }
    else
    {
        invalidate( !m_matrixProperty->set( m ) );    // NOTE: set automatically checks the validity of the value
    }
}

