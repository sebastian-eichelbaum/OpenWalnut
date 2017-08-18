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

#include "../../common/math/linearAlgebra/WPosition.h"
#include "../../common/WTransferable.h"
#include "WSinglePosition.h"

// The prototype as singleton. Created during first getPrototype() call
boost::shared_ptr< WPrototyped > WSinglePosition::m_prototype = boost::shared_ptr< WPrototyped >();

WSinglePosition::WSinglePosition()
    : WTransferable(),
      WPosition()
{
}

WSinglePosition::WSinglePosition( const WPosition& position )
    : WTransferable(),
      WPosition( position )
{
}

WSinglePosition::~WSinglePosition()
{
}

boost::shared_ptr< WPrototyped > WSinglePosition::getPrototype()
{
    if( !m_prototype )
    {
        m_prototype = boost::shared_ptr< WPrototyped >( new WSinglePosition() );
    }
    return m_prototype;
}
