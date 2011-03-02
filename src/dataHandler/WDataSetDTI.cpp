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

#include "WDataSetDTI.h"

// prototype instance as singleton
boost::shared_ptr< WPrototyped > WDataSetDTI::m_prototype = boost::shared_ptr< WPrototyped >();


WDataSetDTI::WDataSetDTI( boost::shared_ptr< WValueSetBase > newValueSet, boost::shared_ptr< WGrid > newGrid )
    : WDataSetSingle( newValueSet, newGrid )
{
    WAssert( newValueSet, "No value set given." );
    WAssert( newGrid, "No grid given." );
    WAssert( newValueSet->size() == newGrid->size(), "Number of values unequal number of positions in grid." );
    WAssert( newValueSet->order() == 1, "The value set does not contain vectors." );
    WAssert( newValueSet->dimension() == 6, "The size of the vetors does not match symmetric matrices: must be equal to 6." );
}

WDataSetDTI::~WDataSetDTI()
{
}

WDataSetSingle::SPtr WDataSetDTI::clone( boost::shared_ptr< WValueSetBase > newValueSet ) const
{
    return WDataSetSingle::SPtr( new WDataSetDTI( newValueSet, getGrid() ) );
}

WDataSetSingle::SPtr WDataSetDTI::clone( boost::shared_ptr< WGrid > newGrid ) const
{
    return WDataSetSingle::SPtr( new WDataSetDTI( getValueSet(), newGrid ) );
}

WDataSetSingle::SPtr WDataSetDTI::clone() const
{
    return WDataSetSingle::SPtr( new WDataSetDTI( getValueSet(), getGrid() ) );
}

WTensorSym< 2, 3, float > WDataSetDTI::getTensor( size_t index ) const
{
    boost::shared_ptr< WValueSet< float > > values = boost::shared_dynamic_cast< WValueSet< float > >( m_valueSet );
    WAssert( values, "The value set of a WDataSetDTI must be a WValueSet< float >, nothing else!" );
    return WTensorSym< 2, 3, float >( values->getWValue( index ) );
}

