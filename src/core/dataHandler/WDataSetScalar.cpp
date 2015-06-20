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

#include <string>
#include <vector>

#include "../common/WAssert.h"
#include "../common/WLimits.h"
#include "datastructures/WValueSetHistogram.h"
#include "WDataSetSingle.h"

#include "WDataSetScalar.h"

// prototype instance as singleton
boost::shared_ptr< WPrototyped > WDataSetScalar::m_prototype = boost::shared_ptr< WPrototyped >();

WDataSetScalar::WDataSetScalar( boost::shared_ptr< WValueSetBase > newValueSet,
                                boost::shared_ptr< WGrid > newGrid )
    : WDataSetSingle( newValueSet, newGrid )
{
    WAssert( newValueSet, "No value set given." );
    WAssert( newGrid, "No grid given." );
    WAssert( newValueSet->size() == newGrid->size(), "Number of values unequal number of positions in grid." );
    WAssert( newValueSet->order() == 0, "The value set does not contain scalars." );
}

WDataSetScalar::WDataSetScalar()
    : WDataSetSingle()
{
    // default constructor used by the prototype mechanism
}

WDataSetScalar::~WDataSetScalar()
{
}

WDataSetSingle::SPtr WDataSetScalar::clone( boost::shared_ptr< WValueSetBase > newValueSet, boost::shared_ptr< WGrid > newGrid ) const
{
    return WDataSetSingle::SPtr( new WDataSetScalar( newValueSet, newGrid ) );
}

WDataSetSingle::SPtr WDataSetScalar::clone( boost::shared_ptr< WValueSetBase > newValueSet ) const
{
    return WDataSetSingle::SPtr( new WDataSetScalar( newValueSet, getGrid() ) );
}

WDataSetSingle::SPtr WDataSetScalar::clone( boost::shared_ptr< WGrid > newGrid ) const
{
    return WDataSetSingle::SPtr( new WDataSetScalar( getValueSet(), newGrid ) );
}

WDataSetSingle::SPtr WDataSetScalar::clone() const
{
    return WDataSetSingle::SPtr( new WDataSetScalar( getValueSet(), getGrid() ) );
}

std::string const WDataSetScalar::getName() const
{
    return "WDataSetScalar";
}

std::string const WDataSetScalar::getDescription() const
{
    return "A scalar dataset, i.e. one scalar value per voxel.";
}

double WDataSetScalar::getMax() const
{
    return m_valueSet->getMaximumValue();
}

double WDataSetScalar::getMin() const
{
    return m_valueSet->getMinimumValue();
}

boost::shared_ptr< WPrototyped > WDataSetScalar::getPrototype()
{
    if( !m_prototype )
    {
        m_prototype = boost::shared_ptr< WPrototyped >( new WDataSetScalar() );
    }

    return m_prototype;
}

double WDataSetScalar::interpolate( const WPosition& pos, bool* success ) const
{
    boost::shared_ptr< WGridRegular3D > grid = boost::dynamic_pointer_cast< WGridRegular3D >( m_grid );

    WAssert( grid, "This data set has a grid whose type is not yet supported for interpolation." );
    WAssert( ( m_valueSet->order() == 0 &&  m_valueSet->dimension() == 1 ),
             "Only implemented for scalar values so far." );

    bool isInside = true;
    size_t cellId = grid->getCellId( pos, &isInside );

    if( !isInside )
    {
        *success = false;
        return 0.0;
    }

    WGridRegular3D::CellVertexArray vertexIds = grid->getCellVertexIds( cellId );

    WPosition localPos = grid->getTransform().directionToGridSpace( pos - grid->getPosition( vertexIds[0] ) );

    double lambdaX = localPos[0];
    double lambdaY = localPos[1];
    double lambdaZ = localPos[2];
    std::vector< double > h( 8 );
//         lZ     lY
//         |      /
//         | 6___/_7
//         |/:    /|
//         4_:___5 |
//         | :...|.|
//         |.2   | 3
//         |_____|/ ____lX
//        0      1
    h[0] = ( 1 - lambdaX ) * ( 1 - lambdaY ) * ( 1 - lambdaZ );
    h[1] = (     lambdaX ) * ( 1 - lambdaY ) * ( 1 - lambdaZ );
    h[2] = ( 1 - lambdaX ) * (     lambdaY ) * ( 1 - lambdaZ );
    h[3] = (     lambdaX ) * (     lambdaY ) * ( 1 - lambdaZ );
    h[4] = ( 1 - lambdaX ) * ( 1 - lambdaY ) * (     lambdaZ );
    h[5] = (     lambdaX ) * ( 1 - lambdaY ) * (     lambdaZ );
    h[6] = ( 1 - lambdaX ) * (     lambdaY ) * (     lambdaZ );
    h[7] = (     lambdaX ) * (     lambdaY ) * (     lambdaZ );

    double result = 0;
    for( size_t i = 0; i < 8; ++i )
    {
        result += h[i] * WDataSetSingle::getSingleRawValue( vertexIds[i] );
    }

    *success = true;
    return result;
}

double WDataSetScalar::getValueAt( int x, int y, int z ) const
{
    boost::shared_ptr< WGridRegular3D > grid = boost::dynamic_pointer_cast< WGridRegular3D >( m_grid );
    size_t id = x + y * grid->getNbCoordsX() + z * grid->getNbCoordsX() * grid->getNbCoordsY();

    return WDataSetSingle::getSingleRawValue( id );
}

double WDataSetScalar::getValueAt( size_t id ) const
{
    return WDataSetSingle::getSingleRawValue( id );
}

boost::shared_ptr< const WValueSetHistogram > WDataSetScalar::getHistogram( size_t buckets )
{
    boost::lock_guard<boost::mutex> lock( m_histogramLock );

    if( m_histograms.count( buckets ) != 0 )
    {
        return m_histograms[ buckets ];
    }

    // create if not yet existing
    m_histograms[ buckets ] = boost::shared_ptr< WValueSetHistogram >( new WValueSetHistogram( m_valueSet, buckets ) );

    return m_histograms[ buckets ];
}
