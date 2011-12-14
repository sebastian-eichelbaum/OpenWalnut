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

#include "WIntegrationParameterization.h"

WIntegrationParameterization::WIntegrationParameterization( boost::shared_ptr< WGridRegular3D > grid ):
    WRasterParameterization( grid ),
    m_lengthValues( grid->size(), 0.0 ),
    m_curLength( 0.0 )
{
    // initialize members
}

WIntegrationParameterization::~WIntegrationParameterization()
{
    // cleanup
}

boost::shared_ptr< WDataSetScalar > WIntegrationParameterization::getDataSet()
{
    boost::shared_ptr< WValueSet< double > > valueSet( new WValueSet< double >( 0,
                                                                                1,
                                                                                boost::shared_ptr< std::vector< double > >(
                                                                                    new std::vector< double >( m_lengthValues ) ),
                                                                                W_DT_DOUBLE ) );
    return boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( valueSet, m_grid ) );
}

namespace wip //WIntegrationParameterization
{
    size_t index( int x, int y, int z, boost::shared_ptr< WGridRegular3D > grid )
    {
        // check validity of voxel
        x = x < 0 ? 0 : x;
        y = y < 0 ? 0 : y;
        z = z < 0 ? 0 : z;
        x = x >= static_cast< int >( grid->getNbCoordsX() ) ? static_cast< int >( grid->getNbCoordsX() ) - 1 : x;
        y = y >= static_cast< int >( grid->getNbCoordsY() ) ? static_cast< int >( grid->getNbCoordsY() ) - 1 : y;
        z = z >= static_cast< int >( grid->getNbCoordsZ() ) ? static_cast< int >( grid->getNbCoordsZ() ) - 1 : z;

        // calculate the index inside the grid
        size_t nbX  = grid->getNbCoordsX();
        size_t nbXY = grid->getNbCoordsX() * grid->getNbCoordsY();
        return x + y * nbX + z * nbXY;
    }
}

void WIntegrationParameterization::parameterizeVoxel( const WValue< int >& voxel, size_t /*voxelIdx*/, const int /*axis*/,
                                                      const double /*value*/,
                                                      const WPosition& /*start*/,
                                                      const WPosition& /*end*/ )
{
    // ok, this looks ugly but setting the whole 27-neighborhood produces better results
    m_lengthValues[ wip::index( voxel[0],   voxel[1]+1, voxel[2]+1, m_grid ) ] = m_curLength;
    m_lengthValues[ wip::index( voxel[0],   voxel[1]+1, voxel[2]-1, m_grid ) ] = m_curLength;
    m_lengthValues[ wip::index( voxel[0],   voxel[1]+1, voxel[2],   m_grid ) ] = m_curLength;
    m_lengthValues[ wip::index( voxel[0],   voxel[1]-1, voxel[2]+1, m_grid ) ] = m_curLength;
    m_lengthValues[ wip::index( voxel[0],   voxel[1]-1, voxel[2]-1, m_grid ) ] = m_curLength;
    m_lengthValues[ wip::index( voxel[0],   voxel[1]-1, voxel[2],   m_grid ) ] = m_curLength;
    m_lengthValues[ wip::index( voxel[0],   voxel[1],   voxel[2]+1, m_grid ) ] = m_curLength;
    m_lengthValues[ wip::index( voxel[0],   voxel[1],   voxel[2]-1, m_grid ) ] = m_curLength;
    m_lengthValues[ wip::index( voxel[0],   voxel[1],   voxel[2],   m_grid ) ] = m_curLength;

    m_lengthValues[ wip::index( voxel[0]+1, voxel[1]+1, voxel[2]+1, m_grid ) ] = m_curLength;
    m_lengthValues[ wip::index( voxel[0]+1, voxel[1]+1, voxel[2]-1, m_grid ) ] = m_curLength;
    m_lengthValues[ wip::index( voxel[0]+1, voxel[1]+1, voxel[2],   m_grid ) ] = m_curLength;
    m_lengthValues[ wip::index( voxel[0]+1, voxel[1]-1, voxel[2]+1, m_grid ) ] = m_curLength;
    m_lengthValues[ wip::index( voxel[0]+1, voxel[1]-1, voxel[2]-1, m_grid ) ] = m_curLength;
    m_lengthValues[ wip::index( voxel[0]+1, voxel[1]-1, voxel[2],   m_grid ) ] = m_curLength;
    m_lengthValues[ wip::index( voxel[0]+1, voxel[1],   voxel[2]+1, m_grid ) ] = m_curLength;
    m_lengthValues[ wip::index( voxel[0]+1, voxel[1],   voxel[2]-1, m_grid ) ] = m_curLength;
    m_lengthValues[ wip::index( voxel[0]+1, voxel[1],   voxel[2],   m_grid ) ] = m_curLength;

    m_lengthValues[ wip::index( voxel[0]-1, voxel[1]+1, voxel[2]+1, m_grid ) ] = m_curLength;
    m_lengthValues[ wip::index( voxel[0]-1, voxel[1]+1, voxel[2]-1, m_grid ) ] = m_curLength;
    m_lengthValues[ wip::index( voxel[0]-1, voxel[1]+1, voxel[2],   m_grid ) ] = m_curLength;
    m_lengthValues[ wip::index( voxel[0]-1, voxel[1]-1, voxel[2]+1, m_grid ) ] = m_curLength;
    m_lengthValues[ wip::index( voxel[0]-1, voxel[1]-1, voxel[2]-1, m_grid ) ] = m_curLength;
    m_lengthValues[ wip::index( voxel[0]-1, voxel[1]-1, voxel[2],   m_grid ) ] = m_curLength;
    m_lengthValues[ wip::index( voxel[0]-1, voxel[1],   voxel[2]+1, m_grid ) ] = m_curLength;
    m_lengthValues[ wip::index( voxel[0]-1, voxel[1],   voxel[2]-1, m_grid ) ] = m_curLength;
    m_lengthValues[ wip::index( voxel[0]-1, voxel[1],   voxel[2],   m_grid ) ] = m_curLength;
}

void WIntegrationParameterization::newLine( const WLine& /*line*/ )
{
    // new line -> reset length
    m_curLength = 0.0;
}

void WIntegrationParameterization::newSegment( const WPosition& start, const WPosition& end )
{
    // add this segments length
    m_curLength += length2( start - end );
}

