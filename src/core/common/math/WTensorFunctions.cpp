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

#include "WTensorFunctions.h"

std::ostream& operator<<( std::ostream& os, const RealEigenSystem& sys )
{
    os << sys[0].first << ", " << sys[0].second << std::endl;
    os << sys[1].first << ", " << sys[1].second << std::endl;
    os << sys[2].first << ", " << sys[2].second << std::endl;
    return os;
}

std::vector< double > getEigenvaluesCardano( WTensorSym< 2, 3 > const& m )
{
    // this is copied from the gpu glyph shader
    // src/graphicsEngine/shaders/tensorTools.fs
    // originally implemented by Mario Hlawitschka
    const double M_SQRT3 = 1.73205080756887729352744634151;
    double de = m( 1, 2 ) * m( 1, 0 );
    double dd = m( 1, 2 ) * m( 1, 2 );
    double ee = m( 1, 0 ) * m( 1, 0 );
    double ff = m( 2, 0 ) * m( 2, 0 );
    double m0 = m( 0, 0 ) + m( 1, 1 ) + m( 2, 2 );
    double c1 = m( 0, 0 ) * m( 1, 1 ) + m( 0, 0 ) * m( 2, 2 ) + m( 1, 1 ) * m( 2, 2 )
             - ( dd + ee + ff );
    double c0 = m( 2, 2 ) * dd + m( 0, 0 ) * ee + m( 1, 1 ) * ff - m( 0, 0 ) * m( 1, 1 ) * m( 2, 2 ) - 2. * m( 2, 0 ) * de;

    double p, sqrt_p, q, c, s, phi;
    p = m0 * m0 - 3. * c1;
    q = m0 * ( p - ( 3. / 2. ) * c1 ) - ( 27. / 2. ) * c0;
    sqrt_p = sqrt( fabs( p ) );

    phi = 27. * ( 0.25 * c1 * c1 * ( p - c1 ) + c0 * ( q + 27. / 4. * c0 ) );
    phi = ( 1. / 3. ) * atan2( sqrt( fabs( phi ) ), q );

    c = sqrt_p * cos( phi );
    s = ( 1. / M_SQRT3 ) * sqrt_p * sin( phi );

    std::vector< double > w( 3 );
    // fix: swapped w[ 2 ] and w[ 1 ]
    w[ 2 ] = ( 1. / 3. ) * ( m0 - c );
    w[ 1 ] = w[ 2 ] + s;
    w[ 0 ] = w[ 2 ] + c;
    w[ 2 ] -= s;
    return w;
}
