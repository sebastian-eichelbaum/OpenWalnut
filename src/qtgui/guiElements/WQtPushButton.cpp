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

#include <string>

#include "WQtPushButton.h"

WQtPushButton::WQtPushButton( QIcon icon, QString name, QWidget* parent, QString label )
    : QToolButton( parent )
{
    setText( label );
    setIcon( icon );
    setAutoRaise( false );

    m_name = name;
    // we need to use released signal here, as the pushed signal also gets emitted on newly created buttons which are under the mouse pointer with
    // pressed left button.
    connect( this, SIGNAL( released() ), this, SLOT( emitPressed() ) );
}

WQtPushButton::~WQtPushButton()
{
}

void WQtPushButton::setName( QString name )
{
    m_name = name;
}

QString WQtPushButton::getName()
{
    return m_name;
}

void WQtPushButton::emitPressed()
{
    emit pushButtonPressed( m_name );
}

