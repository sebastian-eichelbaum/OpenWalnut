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

#include "WQtSliderWithEdit.h"

WQtSliderWithEdit::WQtSliderWithEdit( QWidget* parent )
    : QWidget( parent ),
    m_slider( Qt::Horizontal ),
    m_edit(),
    m_layout()
{
    m_layout.addWidget( &m_slider );
    m_layout.addWidget( &m_edit );

    m_edit.resize( 10, m_edit.size().height() );
    m_edit.setMaximumWidth( 25 );

    setLayout( &m_layout );

    connect( &m_slider, SIGNAL( valueChanged( int ) ), &m_edit, SLOT( setInt( int ) ) );
    connect( &m_edit, SIGNAL( signalNumber( int ) ), &m_slider, SLOT( setValue( int ) ) );
}

WQtSliderWithEdit::~WQtSliderWithEdit()
{
}

void WQtSliderWithEdit::setMin( int min )
{
    m_slider.setMinimum( min );
}

void WQtSliderWithEdit::setMax( int max )
{
    m_slider.setMaximum( max );

    int length = 1;
    float fmax = static_cast<float>( max );
    while ( ( fmax / 10 ) >= 1.0 )
    {
        ++length;
        fmax /= 10.0;
    }
    m_edit.setMaxLength( length );
}

void WQtSliderWithEdit::setValue( int value )
{
    m_slider.setValue( value );
}

boost::signal1< void, int >*WQtSliderWithEdit::getboostSignalObject()
{
    return m_edit.getboostSignalObject();
}

