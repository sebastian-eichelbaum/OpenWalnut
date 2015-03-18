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

#include <cmath>
#include <string>
#include <vector>

#include "../common/WAssert.h"
#include "../common/math/linearAlgebra/WPosition.h"
#include "../common/math/WSymmetricSphericalHarmonic.h"
#include "WDataSetSingle.h"
#include "WDataSetSphericalHarmonics.h"

// prototype instance as singleton
boost::shared_ptr< WPrototyped > WDataSetSphericalHarmonics::m_prototype = boost::shared_ptr< WPrototyped >();

WDataSetSphericalHarmonics::WDataSetSphericalHarmonics( boost::shared_ptr< WValueSetBase > newValueSet,
                                                        boost::shared_ptr< WGrid > newGrid ) :
    WDataSetSingle( newValueSet, newGrid ), m_valueSet( newValueSet )
{
    m_gridRegular3D = boost::dynamic_pointer_cast< WGridRegular3D >( newGrid );
    WAssert( newValueSet, "No value set given." );
    WAssert( newGrid, "No grid given." );
}

WDataSetSphericalHarmonics::WDataSetSphericalHarmonics()
    : WDataSetSingle()
{
}

WDataSetSphericalHarmonics::~WDataSetSphericalHarmonics()
{
}

WDataSetSingle::SPtr WDataSetSphericalHarmonics::clone( boost::shared_ptr< WValueSetBase > newValueSet ) const
{
    return WDataSetSingle::SPtr( new WDataSetSphericalHarmonics( newValueSet, getGrid() ) );
}

WDataSetSingle::SPtr WDataSetSphericalHarmonics::clone( boost::shared_ptr< WGrid > newGrid ) const
{
    return WDataSetSingle::SPtr( new WDataSetSphericalHarmonics( getValueSet(), newGrid ) );
}

WDataSetSingle::SPtr WDataSetSphericalHarmonics::clone() const
{
    return WDataSetSingle::SPtr( new WDataSetSphericalHarmonics( getValueSet(), getGrid() ) );
}

boost::shared_ptr< WPrototyped > WDataSetSphericalHarmonics::getPrototype()
{
    if( !m_prototype )
    {
        m_prototype = boost::shared_ptr< WPrototyped >( new WDataSetSphericalHarmonics() );
    }

    return m_prototype;
}

WSymmetricSphericalHarmonic< double > WDataSetSphericalHarmonics::interpolate( const WPosition& pos, bool* success ) const
{
    *success = m_gridRegular3D->encloses( pos );

    bool isInside = true;
    size_t cellId = m_gridRegular3D->getCellId( pos, &isInside );

    if( !isInside )
    {
        *success = false;
        return WSymmetricSphericalHarmonic< double >();
    }

    // ids of vertices for interpolation
    WGridRegular3D::CellVertexArray vertexIds = m_gridRegular3D->getCellVertexIds( cellId );

    WPosition localPos = m_gridRegular3D->getTransform().directionToGridSpace( pos - m_gridRegular3D->getPosition( vertexIds[0] ) );

    double lambdaX = localPos[0];
    double lambdaY = localPos[1];
    double lambdaZ = localPos[2];

    WValue< double > h( 8 );
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

    // take
    WValue<double> interpolatedCoefficients( m_valueSet->dimension() );
    for( size_t i = 0; i < 8; ++i )
    {
        interpolatedCoefficients += h[i] * m_valueSet->getWValueDouble( vertexIds[i] );
    }

    *success = true;

    return WSymmetricSphericalHarmonic< double >( interpolatedCoefficients );
}

WSymmetricSphericalHarmonic< double > WDataSetSphericalHarmonics::getSphericalHarmonicAt( size_t index ) const
{
    if( index < m_valueSet->size() ) return WSymmetricSphericalHarmonic< double >( m_valueSet->getWValueDouble( index ) );
    return WSymmetricSphericalHarmonic< double >();
}

const std::string WDataSetSphericalHarmonics::getName() const
{
    return "WDataSetSphericalHarmonics";
}

const std::string WDataSetSphericalHarmonics::getDescription() const
{
    return "Contains factors for spherical harmonics.";
}

bool WDataSetSphericalHarmonics::isTexture() const
{
    return false;
}

