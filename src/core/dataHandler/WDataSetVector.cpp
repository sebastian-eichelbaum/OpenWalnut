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

#include <stdint.h>

#include <string>
#include <vector>

#include <boost/array.hpp>

#include "../common/WAssert.h"
#include "WDataSetSingle.h"
#include "WDataSetVector.h"

// prototype instance as singleton
boost::shared_ptr< WPrototyped > WDataSetVector::m_prototype = boost::shared_ptr< WPrototyped >();

WDataSetVector::WDataSetVector( boost::shared_ptr< WValueSetBase > newValueSet,
                                boost::shared_ptr< WGrid > newGrid )
    : WDataSetSingle( newValueSet, newGrid )
{
    WAssert( newValueSet, "No value set given." );
    WAssert( newGrid, "No grid given." );
    WAssert( newValueSet->size() == newGrid->size(), "Number of values unequal number of positions in grid." );
    WAssert( newValueSet->order() == 1, "The value set does not contain vectors." );
}

WDataSetVector::WDataSetVector()
    : WDataSetSingle()
{
}

WDataSetVector::~WDataSetVector()
{
}

WDataSetSingle::SPtr WDataSetVector::clone( boost::shared_ptr< WValueSetBase > newValueSet ) const
{
    return WDataSetSingle::SPtr( new WDataSetVector( newValueSet, getGrid() ) );
}

WDataSetSingle::SPtr WDataSetVector::clone( boost::shared_ptr< WGrid > newGrid ) const
{
    return WDataSetSingle::SPtr( new WDataSetVector( getValueSet(), newGrid ) );
}

WDataSetSingle::SPtr WDataSetVector::clone() const
{
    return WDataSetSingle::SPtr( new WDataSetVector( getValueSet(), getGrid() ) );
}

boost::shared_ptr< WPrototyped > WDataSetVector::getPrototype()
{
    if( !m_prototype )
    {
        m_prototype = boost::shared_ptr< WPrototyped >( new WDataSetVector() );
    }

    return m_prototype;
}

namespace
{
    boost::array< double, 8 > computePrefactors( const WPosition& pos, boost::shared_ptr< const WGrid > i_grid,
            boost::shared_ptr< const WValueSetBase > i_valueSet, bool *success, boost::shared_ptr< std::vector< size_t > > vertexIds )
    {
        boost::shared_ptr< const WGridRegular3D > grid = boost::shared_dynamic_cast< const WGridRegular3D >( i_grid );

        WAssert( grid,  "This data set has a grid whose type is not yet supported for interpolation." );
        WAssert( grid->isNotRotated(), "Only feasible for grids that are only translated or scaled so far." );
        WAssert( ( i_valueSet->order() == 1 &&  i_valueSet->dimension() == 3 ),
                "Only implemented for 3D Vectors so far." );
        boost::array< double, 8 > h;

        bool isInside = true;
        size_t cellId = grid->getCellId( pos, &isInside );

        if( !isInside )
        {
            *success = false;
            return h;
        }
        *success = true; // set it here, before the real work is done, because this cannot fail anymore.

        *vertexIds = grid->getCellVertexIds( cellId );

        WPosition localPos = pos - grid->getPosition( ( *vertexIds )[0] );

        double lambdaX = localPos[0] / grid->getOffsetX();
        double lambdaY = localPos[1] / grid->getOffsetY();
        double lambdaZ = localPos[2] / grid->getOffsetZ();

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

        return h;
    }
}

WVector3d WDataSetVector::interpolate( const WPosition& pos, bool *success ) const
{
    boost::shared_ptr< std::vector< size_t > > vertexIds( new std::vector< size_t > );
    boost::array< double, 8 > h = computePrefactors( pos, m_grid, m_valueSet, success, vertexIds );
    WVector3d result( 0.0, 0.0, 0.0 );

    if( *success ) // only if pos was iniside the grid, we proivde a result different to 0.0, 0.0, 0.0
    {
        for( size_t i = 0; i < 8; ++i )
        {
            result += h[i] * getVectorAt( ( *vertexIds )[i] );
        }
    }

    return result;
}

WVector3d WDataSetVector::eigenVectorInterpolate( const WPosition& pos, bool *success ) const
{
    boost::shared_ptr< std::vector< size_t > > vertexIds( new std::vector< size_t > );
    boost::array< double, 8 > h = computePrefactors( pos, m_grid, m_valueSet, success, vertexIds );
    WVector3d result( 0.0, 0.0, 0.0 );

    if( *success ) // only if pos was iniside the grid, we proivde a result different to 0.0, 0.0, 0.0
    {
        for( size_t i = 0; i < 8; ++i )
        {
            double sign = 1.0;
            if( dot( getVectorAt( ( *vertexIds )[0] ), getVectorAt( ( *vertexIds )[i] ) ) < 0.0 )
            {
                sign = -1.0;
            }
            result += h[i] * sign * getVectorAt( ( *vertexIds )[i] );
        }
    }

    return result;
}

WVector3d WDataSetVector::getVectorAt( size_t index ) const
{
    switch( getValueSet()->getDataType() )
    {
        case W_DT_UNSIGNED_CHAR:
        {
            return boost::shared_dynamic_cast< WValueSet< uint8_t > >( getValueSet() )->getVector3D( index );
        }
        case W_DT_INT16:
        {
            return boost::shared_dynamic_cast< WValueSet< int16_t > >( getValueSet() )->getVector3D( index );
        }
        case W_DT_SIGNED_INT:
        {
            return boost::shared_dynamic_cast< WValueSet< int32_t > >( getValueSet() )->getVector3D( index );
        }
        case W_DT_FLOAT:
        {
            return boost::shared_dynamic_cast< WValueSet< float > >( getValueSet() )->getVector3D( index );
        }
        case W_DT_DOUBLE:
        {
            return boost::shared_dynamic_cast< WValueSet< double > >( getValueSet() )->getVector3D( index );
        }
        default:
            WAssert( false, "Unknow data type in dataset." );
    }

    return WVector3d( 0, 0, 0 );
}

bool WDataSetVector::isTexture() const
{
    return true;
}

