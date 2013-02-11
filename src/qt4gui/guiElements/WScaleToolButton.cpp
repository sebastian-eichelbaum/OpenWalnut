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

#include "WScaleToolButton.h"
#include "WScaleToolButton.moc"

WScaleToolButton::WScaleToolButton( size_t length, QWidget *parent /*= NULL */ ):
    QToolButton( parent ),
    m_additionalWidth( 0 ),
    m_minLength( length )
{
    setMinimumWidth( fontMetrics().width( m_orgText.left( m_minLength ) + tr( ".." ) ) + m_additionalWidth );
    setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Maximum );
}

WScaleToolButton::WScaleToolButton( const QString &text, size_t length, QWidget *parent /*= NULL */ ) :
    QToolButton( parent ),
    m_orgText( text ),
    m_additionalWidth( 0 ),
    m_minLength( length )
{
    setText( text );
    setMinimumWidth( fontMetrics().width( m_orgText.left( m_minLength ) + tr( ".." ) ) + m_additionalWidth );
    setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Maximum );
}

void WScaleToolButton::resizeEvent( QResizeEvent* /*event*/ )
{
    fitTextToSize();
}

QSize WScaleToolButton::sizeHint() const
{
    return QSize( fontMetrics().width( m_orgText ) + m_additionalWidth,
                  QToolButton::sizeHint().height() );
}

QSize WScaleToolButton::minimumSizeHint() const
{
    return QSize( fontMetrics().width( m_orgText.left( m_minLength ) + tr( ".." ) ) + m_additionalWidth,
                  QToolButton::minimumSizeHint().height() );
}

void WScaleToolButton::setText( const QString &text )
{
    m_orgText = text;
    setMinimumWidth( fontMetrics().width( m_orgText.left( m_minLength ) + tr( ".." ) ) + m_additionalWidth );
    fitTextToSize();
}

void WScaleToolButton::fitTextToSize()
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
        QToolButton::setText( useText + tr( ".." ) );
    }
    else
    {
        QToolButton::setText( m_orgText );
    }
}

void WScaleToolButton::addAdditionalWidth( int w )
{
    m_additionalWidth = w;
}

void WScaleToolButton::setMinimalLength( size_t chars )
{
    setText( m_orgText );
    m_minLength = chars;
}

size_t WScaleToolButton::getMinimalLength() const
{
    return m_minLength;
}
