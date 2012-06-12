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
#include <cstdlib>
#include <vector>

#include "core/common/WLogger.h"
#include "core/dataHandler/WDataHandlerEnums.h"
#include "core/dataHandler/WValueSet.h"

#include "WDataCreatorSingleDirection.h"

WDataCreatorSingleDirection::WDataCreatorSingleDirection():
    WObjectNDIP< WDataSetSingleCreatorInterface >( "Single direction.", "Creates a volume with a single direction for all vectors." )
{
    m_vector = m_properties->addProperty( "Direction", "The direction for all vectors in the dataset.", WPosition( 0.0, 0.0, 0.0 ) );
}

WDataCreatorSingleDirection::~WDataCreatorSingleDirection()
{
}

WValueSetBase::SPtr WDataCreatorSingleDirection::operator()( WProgress::SPtr progress,
                                                    WGridRegular3D::ConstSPtr grid, unsigned char order, unsigned char dimension,
                                                    dataType /*type*/ )
{
    typedef double ValueType;
    typedef WValueSet< ValueType > ValueSetType;

    if( order != 1 || dimension != 3 )
    {
        return ValueSetType::SPtr();
    }

    boost::shared_ptr< std::vector< ValueType > > data( new std::vector< ValueType > );

    size_t valuesPerVoxel = ValueSetType::getRequiredRawSizePerVoxel( order, dimension );
    data->resize( valuesPerVoxel * grid->size() );

    WPosition dir = m_vector->get( true );

    for( size_t x = 0; x < grid->getNbCoordsX(); ++x )
    {
        for( size_t y = 0; y < grid->getNbCoordsY(); ++y )
        {
            for( size_t z = 0; z < grid->getNbCoordsZ(); ++z )
            {
                data->operator[]( ( valuesPerVoxel * grid->getVoxelNum( x, y, z ) ) + 0 ) = dir[ 0 ];
                data->operator[]( ( valuesPerVoxel * grid->getVoxelNum( x, y, z ) ) + 1 ) = dir[ 1 ];
                data->operator[]( ( valuesPerVoxel * grid->getVoxelNum( x, y, z ) ) + 2 ) = dir[ 2 ];
            }

            progress->increment( grid->getNbCoordsZ() );
        }
    }

    return ValueSetType::SPtr( new ValueSetType( order, dimension, data ) );
}
