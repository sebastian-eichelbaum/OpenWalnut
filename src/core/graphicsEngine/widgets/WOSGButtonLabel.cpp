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

#include "WOSGButtonLabel.h"

WOSGButtonLabel::WOSGButtonLabel( bool pushable ) :
    osgWidget::Label( "", "" ),
    m_clicked( false ),
    m_pushable( pushable ),
    m_pushed( false )
{
    setFontSize( 14 );
    setFontColor( 1.0f, 1.0f, 1.0f, 1.0f );
    setColor( 0.3f, 0.3f, 0.3f, 1.0f );
    addHeight( 18.0f );
    setCanFill( true );
    setEventMask( osgWidget::EVENT_MOUSE_PUSH  );
}

WOSGButtonLabel::~WOSGButtonLabel()
{
}

bool WOSGButtonLabel::mousePush( double, double, osgWidget::WindowManager* )
{
    m_clicked = true;
    if( m_pushable )
    {
        m_pushed = !m_pushed;

        if( m_pushed )
        {
            setFontColor( 0.0f, 0.0f, 0.0f, 1.0f );
        }
        else
        {
            setFontColor( 1.0f, 1.0f, 1.0f, 1.0f );
        }
    }
    return true;
}
