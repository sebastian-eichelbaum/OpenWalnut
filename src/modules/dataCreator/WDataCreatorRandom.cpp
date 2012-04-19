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

#include <ctime>

#include <core/common/WLogger.h>

#include <core/dataHandler/WDataHandlerEnums.h>
#include <core/dataHandler/WValueSet.h>

#include "WDataCreatorRandom.h"

WDataCreatorRandom::WDataCreatorRandom():
    WObjectNDIP< WMDataCreatorScalar::DataCreatorInterface >( "Random", "Creates a noise volume." ),
    m_rand( std::time( 0 ) ),
    m_values01( 0.0, 1.0 )
{
}

WDataCreatorRandom::~WDataCreatorRandom()
{
}

WValueSetBase::SPtr WDataCreatorRandom::operator()( WGridRegular3D::ConstSPtr grid, dataType type )
{
    typedef double ValueType;
    typedef WValueSet< ValueType > ValueSetType;

    // create some memory for the data
    boost::shared_ptr< std::vector< ValueType > > data( new std::vector< ValueType > );
    // for scalar data we need only as much space as we have voxels
    data->reserve( grid->size() );

    // iterate the data and fill in some random values
    for( size_t i = 0; i < grid->size(); ++i )
    {
        data->push_back( m_values01( m_rand ) );
    }

    // finally, create the value set and return it
    // We have scalar data (order = 0 ) in 3d
    return ValueSetType::SPtr( new ValueSetType( 0, 1, data ) );
}

