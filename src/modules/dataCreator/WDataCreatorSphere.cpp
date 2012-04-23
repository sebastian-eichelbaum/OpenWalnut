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

#include <vector>

#include "WDataCreatorSphere.h"

WDataCreatorSphere::WDataCreatorSphere():
    WObjectNDIP< WMDataCreatorScalar::DataCreatorInterface >( "Spherical", "Creates a spherical volume." )
{
}

WDataCreatorSphere::~WDataCreatorSphere()
{
}

WValueSetBase::SPtr WDataCreatorSphere::operator()( WGridRegular3D::ConstSPtr grid, dataType /* type */ )
{
    // currently, the type is fixed. This will come soon.
    typedef double ValueType;
    typedef WValueSet< ValueType > ValueSetType;

    // create some memory for the data
    boost::shared_ptr< std::vector< ValueType > > data( new std::vector< ValueType > );
    // for scalar data we need only as much space as we have voxels
    data->resize( grid->size() );

    // iterate the data and fill in some values
    for( size_t x = 0; x < grid->getNbCoordsX(); ++x )
    {
        for( size_t y = 0; y < grid->getNbCoordsY(); ++y )
        {
            for( size_t z = 0; z < grid->getNbCoordsZ(); ++z )
            {
                data->operator[]( grid->getVoxelNum( x, y, z ) ) = x*x + y*y + z*z;
            }
        }
    }

    // finally, create the value set and return it
    // We have scalar data (order = 0 ) in 3d
    return ValueSetType::SPtr( new ValueSetType( 0, 1, data ) );
}
