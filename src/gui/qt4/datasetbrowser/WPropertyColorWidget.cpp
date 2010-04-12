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
#include <string>

#include <boost/lexical_cast.hpp>

#include <QtGui/QColorDialog>

#include "../../../common/WLogger.h"
#include "../../../common/WPropertyVariable.h"

#include "WPropertyColorWidget.h"

WPropertyColorWidget::WPropertyColorWidget( WPropColor property, QGridLayout* propertyGrid, QWidget* parent ):
    WPropertyWidget( property, propertyGrid, parent ),
    m_colorProperty( property ),
    m_button( this ),
    m_layout()
{
    // initialize members
    setLayout( &m_layout );

    // layout both against each other
    m_layout.addWidget( &m_button );

    // set the initial values
    update();

    // connect the modification signal of the edit and slider with our callback
    connect( &m_button, SIGNAL( released() ), this, SLOT( buttonReleased() ) );
}

WPropertyColorWidget::~WPropertyColorWidget()
{
    // cleanup
}

void WPropertyColorWidget::update()
{
    m_button.setPalette( QPalette( toQColor( m_colorProperty->get() ) ) );
}

QColor WPropertyColorWidget::toQColor( WColor color )
{
    QColor tmp;
    tmp.setRgbF( color.getRed(),
                 color.getGreen(),
                 color.getBlue(),
                 color.getAlpha() );

    return tmp;
}

WColor WPropertyColorWidget::toWColor( QColor color )
{
    return WColor( color.redF(), color.greenF(), color.blueF(), color.alphaF() );
}

void WPropertyColorWidget::buttonReleased()
{
    QColor current = toQColor( m_colorProperty->get() );
    current = QColorDialog::getColor( current, this );

    // convert it back to a WColor
    invalidate( !m_colorProperty->set( toWColor( current ) ) ); // NOTE: set automatically checks the validity of the value

    // set the button background to the appropriate color
    m_button.setPalette( QPalette( current ) );
}

