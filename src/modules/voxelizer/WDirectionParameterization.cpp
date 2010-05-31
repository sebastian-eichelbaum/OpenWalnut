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

#include "WDirectionParameterization.h"

WDirectionParameterization::WDirectionParameterization( boost::shared_ptr< WGridRegular3D > grid ):
    WRasterParameterization( grid ),
    m_dirValues( 3 * grid->size(), 0.0 )
{
    // initialize members
}

WDirectionParameterization::~WDirectionParameterization()
{
    // cleanup
}

boost::shared_ptr< WDataSetSingle > WDirectionParameterization::getDataSet()
{
    boost::shared_ptr< WValueSet< double > > valueSet( new WValueSet< double >( 1, 3, m_dirValues, W_DT_DOUBLE ) );
    return boost::shared_ptr< WDataSetSingle >( new WDataSetSingle( valueSet, m_grid ) );
}

void WDirectionParameterization::parameterizeVoxel( const wmath::WValue< int >& /*voxel*/, size_t voxelId, const int /*axis*/,
                                                    const double /*value*/,
                                                    const wmath::WPosition& start,
                                                    const wmath::WPosition& end )
{
    // segment end and start point define its direction
    wmath::WPosition vec = end - start;

    m_dirValues[ ( 3 * voxelId )     ] = vec[0];
    m_dirValues[ ( 3 * voxelId ) + 1 ] = vec[1];
    m_dirValues[ ( 3 * voxelId ) + 2 ] = vec[2];
}

