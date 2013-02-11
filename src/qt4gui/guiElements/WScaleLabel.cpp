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

WScaleLabel::WScaleLabel( size_t length, QWidget* parent ):
    QLabel( parent ),
    m_additionalWidth( 0 ),
    m_minLength( length )
{
    construct();
}

WScaleLabel::WScaleLabel( const QString &text, size_t length, QWidget* parent ) :
    QLabel( text, parent ),
    m_orgText( text ),
    m_additionalWidth( 0 ),
    m_minLength( length )
{
    construct();
}

WScaleLabel::WScaleLabel( QWidget* parent ):
    QLabel( parent ),
    m_additionalWidth( 0 ),
    m_minLength( WPREFERRED_LABEL_LENGTH )
{
    construct();
}

WScaleLabel::WScaleLabel( const QString &text, QWidget* parent ):
    QLabel( text, parent ),
    m_orgText( text ),
    m_additionalWidth( 0 ),
    m_minLength( WPREFERRED_LABEL_LENGTH )
{
    construct();
}

void WScaleLabel::construct()
{
    setMinimumWidth( fontMetrics().width( m_orgText.left( m_minLength ) + tr( ".." ) ) + m_additionalWidth );
    setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Maximum );
    setMargin( WGLOBAL_MARGIN );
    setTextInteractionFlags( Qt::TextSelectableByMouse );
}

void WScaleLabel::resizeEvent( QResizeEvent* /*event*/ )
{
    fitTextToSize();
}

QSize WScaleLabel::sizeHint() const
{
    return QSize( calculateSize( m_orgText.length() ), QLabel::sizeHint().height() );
}

QSize WScaleLabel::minimumSizeHint() const
{
    return QSize( calculateSize( m_minLength ),QLabel::minimumSizeHint().height() );
}

size_t WScaleLabel::calculateSize( size_t chars ) const
{
    return fontMetrics().width( m_orgText.left( chars ) + tr( ".." ) ) + 2 * margin() + m_additionalWidth;
}

void WScaleLabel::setText( const QString &text )
{
    m_orgText = text;
    setMinimumWidth( fontMetrics().width( m_orgText.left( m_minLength ) + tr( ".." ) ) + 2 * margin() + m_additionalWidth );
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

void WScaleLabel::addAdditionalWidth( int w )
{
    m_additionalWidth = w;
}

void WScaleLabel::setMinimalLength( size_t chars )
{
    setText( m_orgText );
    m_minLength = chars;
}

size_t WScaleLabel::getMinimalLength() const
{
    return m_minLength;
}
