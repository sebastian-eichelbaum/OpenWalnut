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

#include "../../../common/WPathHelper.h"

#include "WGELabel.h"

WGELabel::WGELabel():
    osgText::Text(),
    WGELayoutableItem(),
    m_anchor( 0.0, 0.0, 0.0 )
{
    // initialize members
    setCharacterSize( 21 );
    setFont( WPathHelper::getAllFonts().Default.string() );
    setAxisAlignment( osgText::Text::SCREEN );
    setAlignment( osgText::Text::LEFT_BOTTOM );
    setBackdropType( osgText::Text::OUTLINE );
    setColor( osg::Vec4( 0.9, 0.9, 0.9, 1 ) );
}

WGELabel::~WGELabel()
{
    // cleanup
}

osg::Vec3 WGELabel::getAnchor() const
{
    return m_anchor;
}

void WGELabel::setAnchor( const osg::Vec3& anchor )
{
    m_anchor = osg::Vec3( anchor );
}

