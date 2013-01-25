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

#include "WScaleLabel.h"
#include "WScaleLabel.moc"

WScaleLabel::WScaleLabel( QWidget *parent /*= NULL */ ):
    QLabel( parent )
{
    setMinimumWidth( fontMetrics().width( m_orgText.left( WMIN_LABEL_LENGTH ) + tr( ".." ) ) );
    setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Expanding );
}

WScaleLabel::WScaleLabel( const QString &text, QWidget *parent /*= NULL */ ) :
    QLabel( text, parent ),
    m_orgText( text )
{
    setMinimumWidth( fontMetrics().width( m_orgText.left( WMIN_LABEL_LENGTH ) + tr( ".." ) ) );
    setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Expanding );
}

void WScaleLabel::resizeEvent( QResizeEvent* /*event*/ )
{
    fitTextToSize();
}

QSize WScaleLabel::sizeHint() const
{
    return QSize( fontMetrics().width( m_orgText ), QLabel::sizeHint().height() );
}

QSize WScaleLabel::minimumSizeHint() const
{
    return QSize( fontMetrics().width( m_orgText.left( WMIN_LABEL_LENGTH ) + tr( ".." ) ), QLabel::minimumSizeHint().height() );
}

void WScaleLabel::setText( const QString &text )
{
    m_orgText = text;
    setMinimumWidth( fontMetrics().width( m_orgText.left( WMIN_LABEL_LENGTH ) + tr( ".." ) ) );
    fitTextToSize();
}

void WScaleLabel::fitTextToSize()
{
    int newwidth = width();
    QFontMetrics fn = fontMetrics();
    if( newwidth < fn.width( m_orgText ) )
    {
        QString useText = m_orgText.left( m_orgText.length() - 1 );
        while( fn.width( useText + tr( ".." ) ) > newwidth || useText.length() == 0 )
        {
            useText = useText.left( useText.length() - 1 );
        }
        QLabel::setText( useText + tr( ".." ) );
    }
    else
    {
        QLabel::setText( m_orgText );
    }
}
