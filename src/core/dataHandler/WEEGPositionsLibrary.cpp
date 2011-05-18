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

#include <map>
#include <string>

#include "../common/WStringUtils.h"
#include "../common/exceptions/WOutOfBounds.h"
#include "../common/math/linearAlgebra/WLinearAlgebra.h"
#include "WEEGPositionsLibrary.h"


WEEGPositionsLibrary::WEEGPositionsLibrary( const std::map< std::string, WPosition >& positions )
{
    // put all the elements from positions in m_posititions, but convert the labels to uppercase
    for( std::map< std::string, WPosition >::const_iterator iter = positions.begin(); iter != positions.end(); ++iter )
    {
        m_positions[string_utils::toUpper( iter->first )] = iter->second;
    }
}

WPosition WEEGPositionsLibrary::getPosition( std::string label ) const throw( WOutOfBounds )
{
    std::map< std::string, WPosition >::const_iterator iter = m_positions.find( string_utils::toUpper( label ) );

    if( iter == m_positions.end() )
    {
        throw WOutOfBounds( std::string( "EEG Positions Library doesn't contain a position for electrode " + label ) );
    }

    return iter->second;
}
