//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2013 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

#include "WNewVertex.h"


namespace butterfly
{
WNewVertex::WNewVertex( size_t toID )
{
    this->m_toID = toID;
    this->m_newVertexID = 0;
    this->m_isValid = true;
    this->m_coordinate = osg::Vec3( 0, 0, 0 );
}

WNewVertex::~WNewVertex()
{
    //No arrays or vectors to decompose
}

size_t WNewVertex::getNewVertexID() // NOLINT
{
    return this->m_newVertexID;
}
void WNewVertex::setNewVertexID( size_t newVertexID )
{
    this->m_newVertexID = newVertexID;
}

size_t WNewVertex::getToID()
{
    return this->m_toID;
}
void WNewVertex::setValid( bool isValid )
{
    this->m_isValid = isValid;
}
bool WNewVertex::isValid()
{
    return this->m_isValid;
}
osg::Vec3 WNewVertex::getCoordinate()
{
    return this->m_coordinate;
}
void WNewVertex::setCoordinate( osg::Vec3 coordinate )
{
    this->m_coordinate = coordinate;
}

} /* namespace butterfly */
