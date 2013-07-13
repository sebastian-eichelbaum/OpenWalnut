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

/*
 * MidPoint.cpp
 *
 *  Created on: 16.06.2013
 *      Author: renegade
 */

#include "MidPoint.h"

namespace butterfly
{
MidPoint::MidPoint( size_t toID, size_t midID )
{
    this->toID = toID;
    this->midID = midID;
}

MidPoint::~MidPoint()
{
    // TODO(schwarzkopf): Auto-generated destructor stub
}

size_t MidPoint::getMidID()
{
    return this->midID;
}

size_t MidPoint::getToID()
{
    return this->toID;
}

} /* namespace butterfly */
