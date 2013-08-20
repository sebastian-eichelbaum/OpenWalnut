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

#include "WDataCreatorConstant.h"

WDataCreatorConstant::WDataCreatorConstant():
    WObjectNDIP< WDataSetSingleCreatorInterface >( "Constant", "Creates a volume containing only one value." )
{
    m_value = m_properties->addProperty( "Value", "The value in the data.", 0.0 );
}

WDataCreatorConstant::~WDataCreatorConstant()
{
}

WValueSetBase::SPtr WDataCreatorConstant::operator()( WProgress::SPtr progress,
                                                    WGridRegular3D::ConstSPtr grid, unsigned char order, unsigned char dimension,
                                                    dataType /*type*/ )
{
    std::srand( time( 0 ) );

    // currently, the type is fixed. This will come soon.
    typedef double ValueType;
    typedef WValueSet< ValueType > ValueSetType;

    ValueType val = m_value->get( true );

    // create some memory for the data
    boost::shared_ptr< std::vector< ValueType > > data( new std::vector< ValueType > );
    // for scalar data we need only as much space as we have voxels
    size_t valuesPerVoxel = ValueSetType::getRequiredRawSizePerVoxel( order, dimension );
    data->resize( valuesPerVoxel * grid->size() );

    // iterate the data and fill in some conatnt values
    for( size_t x = 0; x < grid->getNbCoordsX(); ++x )
    {
        for( size_t y = 0; y < grid->getNbCoordsY(); ++y )
        {
            for( size_t z = 0; z < grid->getNbCoordsZ(); ++z )
            {
                // each voxels might need multiple values
                for( size_t v = 0; v < valuesPerVoxel; ++v )
                {
                    data->operator[]( ( valuesPerVoxel * grid->getVoxelNum( x, y, z ) ) + v ) =
                        static_cast< ValueType >( val );
                }
            }

            // updating progress for each voxel is not needed. It is enough to update each slice
            progress->increment( grid->getNbCoordsZ() );
        }
    }

    // finally, create the value set and return it
    // We have scalar data (order = 0 ) in 3D
    return ValueSetType::SPtr( new ValueSetType( order, dimension , data ) );
}

