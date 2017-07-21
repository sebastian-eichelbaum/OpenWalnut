//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2017 OpenWalnut Community, Hochschule Worms
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

#include "WMouseLocationHandler.h"
#include "../common/WAssert.h"


WMouseLocationHandler::WMouseLocationHandler()
    : m_mouseLocation( WMouseLocationInfo() ),
      m_viewerName( "" )
{
}

WMouseLocationHandler::WMouseLocationHandler( std::string viewerName )
    : m_mouseLocation( WMouseLocationInfo() ),
      m_viewerName( viewerName )
{
}

WMouseLocationHandler::~WMouseLocationHandler()
{
}

boost::signals2::signal< void( WMouseLocationHandler::WMouseLocationInfo ) >* WMouseLocationHandler::getLocationSignal()
{
    return &m_locationSignal;
}

bool WMouseLocationHandler::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& /* aa */ )
{
    float x = ea.getX();
    float y = ea.getY();

    WMouseLocationInfo locationInfo;

    locationInfo = WMouseLocationInfo( m_viewerName, std::make_pair( x, y ) );
    m_mouseLocation = locationInfo;
    m_locationSignal( getLocationInfo() );
    return false;
}

WMouseLocationHandler::WMouseLocationInfo WMouseLocationHandler::getLocationInfo()
{
    return m_mouseLocation;
}
