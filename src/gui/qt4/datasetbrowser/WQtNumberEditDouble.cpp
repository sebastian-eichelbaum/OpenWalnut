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

#include "WQtNumberEditDouble.h"

WQtNumberEditDouble::WQtNumberEditDouble( QString name, QWidget* parent )
    : QLineEdit( parent ),
      m_name( name )
{
    connect( this, SIGNAL( returnPressed() ), this, SLOT( numberChanged() ) );
}

WQtNumberEditDouble::~WQtNumberEditDouble()
{
}

void WQtNumberEditDouble::setName( QString name )
{
    m_name = name;
}


void WQtNumberEditDouble::setDouble( double number )
{
    setText( QString::number( number ) );
    emit signalNumberWithName( m_name, number );
}

void WQtNumberEditDouble::numberChanged()
{
    bool ok;
    double number = text().toDouble( &ok );
    if ( ok )
    {
        emit signalNumberWithName( m_name, number );
    }
    else
    {
        setText( QString::number( 0 ) );
        emit signalNumberWithName( m_name, 0 );
    }
}
