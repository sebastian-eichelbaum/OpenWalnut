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

#include <cassert>

#include "WTalairachConverter.h"

WTalairachConverter::WTalairachConverter( wmath::WVector3D ac, wmath::WVector3D pc, wmath::WVector3D ihp ) :
    m_rotMat( 3, 3 ),
    m_rotMatInvert( 3, 3 ),
    m_ac( ac ),
    m_pc( pc ),
    m_ihp( ihp )
{
    // initalize the bounding box of the head with some arbitrary values, these have to set real values
    // befor any coordinate conversion
    // the point of origin in the canonical system is the right inferior posterior corner

    m_ap = wmath::WVector3D( 255, 0, 0 );
    m_pp = wmath::WVector3D( 0, 0, 0 );

    m_lp = wmath::WVector3D( 0, 255, 0 );
    m_rp = wmath::WVector3D( 0, 0, 0 );

    m_ip = wmath::WVector3D( 0, 0, 0 );
    m_sp = wmath::WVector3D( 0, 0, 255 );

    defineRotationMatrix();
}

WTalairachConverter::~WTalairachConverter()
{
}

wmath::WVector3D WTalairachConverter::Canonical2ACPC( const wmath::WVector3D point )
{
    // TODO(schurade): add rotation
    wmath::WVector3D rpoint = point - m_ac;
    return multMatrixWithVector3D( m_rotMat, rpoint );
}

wmath::WVector3D WTalairachConverter::ACPC2Canonical( const wmath::WVector3D point )
{
    // TODO(schurade): add rotation
    return point + m_ac;
}

wmath::WVector3D WTalairachConverter::Canonical2Talairach( const wmath::WVector3D point )
{
    return ACPC2Talairach( Canonical2ACPC( point ) );
}

wmath::WVector3D WTalairachConverter::Talairach2Canonical( const wmath::WVector3D point )
{
    return ACPC2Canonical( Talairach2ACPC( point ) );
}

wmath::WVector3D WTalairachConverter::ACPC2Talairach( const wmath::WVector3D point )
{
    // declare some variables for readability
    double x = point[0];
    double y = point[1];
    double z = point[2];

    double X1 = ( m_pp - m_pc ).norm();
    double X2 = ( m_ac - m_pc ).norm();
    double X3 = ( m_ap - m_ac ).norm();
    double Y1 = ( m_ac - m_rp ).norm();
    double Y2 = ( m_lp - m_ac ).norm();
    double Z1 = ( m_ac - m_ip ).norm();
    double Z2 = ( m_sp - m_ac ).norm();

    double X1T = 79.0;
    double X2T = 23.0;
    double X3T = 70.0;
    double Y1T = 68.0;
    double Y2T = 68.0;
    double Z1T = 42.0;
    double Z2T = 74.0;

    double xt = 0;
    double yt = 0;
    double zt = 0;

    if ( x < -X2 ) // posterior to PC
    {
        xt = ( X1T / X1 ) * ( x + X2 ) - X2T;
    }
    else if ( x >= 0 ) // anterior to AC
    {
        xt = ( X3T / X3 ) * x;
    }
    else // between AC and PC
    {
        xt = ( X2T / X2 ) * x;
    }

    if ( y < 0 ) // right hemisphere
    {
        yt = ( Y1T / Y1 ) * y;
    }
    else // left hemisphere
    {
        yt = ( Y2T / Y2 ) * y;
    }

    if ( z < 0 ) // inferior to AC-PC
    {
        zt = ( Z1T / Z1 ) * z;
    }
    else // superior to AC-PC
    {
        zt = ( Z2T / Z2 ) * z;
    }

    return wmath::WVector3D( xt, yt, zt );
}

wmath::WVector3D WTalairachConverter::Talairach2ACPC( const wmath::WVector3D point )
{
    // declare some variables for readability
    double xt = point[0];
    double yt = point[1];
    double zt = point[2];

    double X1 = ( m_pp - m_pc ).norm();
    double X2 = ( m_ac - m_pc ).norm();
    double X3 = ( m_ap - m_ac ).norm();
    double Y1 = ( m_ac - m_rp ).norm();
    double Y2 = ( m_lp - m_ac ).norm();
    double Z1 = ( m_ac - m_ip ).norm();
    double Z2 = ( m_sp - m_ac ).norm();

    double X1T = 79.0;
    double X2T = 23.0;
    double X3T = 70.0;
    double Y1T = 68.0;
    double Y2T = 68.0;
    double Z1T = 42.0;
    double Z2T = 74.0;

    double x = 0;
    double y = 0;
    double z = 0;

    if ( xt < -X2T ) // posterior to PC
    {
        x = ( X1 / X1T ) * ( xt + X2T ) - X2;
    }
    else if ( xt >= 0 ) // anterior to AC
    {
        x = ( X3 / X3T ) * xt;
    }
    else // between AC and PC
    {
        x = ( X2 / X2T ) * xt;
    }

    if ( yt < 0 ) // right hemisphere
    {
        y = ( Y1 / Y1T ) * yt;
    }
    else // left hemisphere
    {
        y = ( Y2 / Y2T ) * yt;
    }

    if ( zt < 0 ) // inferior to AC-PC
    {
        z = ( Z1 / Z1T ) * z;
    }
    else // superior to AC-PC
    {
        z = ( Z2 / Z2T ) * z;
    }

    return wmath::WVector3D( x, y, z );
}


void WTalairachConverter::defineRotationMatrix()
{
    wmath::WVector3D ihp_proj( ( ( ( m_ac - m_ihp ) * ( m_pc - m_ac ) ) * ( m_pc - m_ac ) / m_pc.distanceSquare( m_ac ) ) + m_ihp );
    wmath::WVector3D ex( m_ac - m_pc );
    ex.normalize();
    wmath::WVector3D ez( ihp_proj - m_ac );
    ez.normalize();
    wmath::WVector3D ey = ez.crossProduct( ex );

    m_rotMat( 0, 0 ) = ex[0];
    m_rotMat( 0, 1 ) = ex[1];
    m_rotMat( 0, 2 ) = ex[2];
    m_rotMat( 1, 0 ) = ey[0];
    m_rotMat( 1, 1 ) = ey[1];
    m_rotMat( 1, 2 ) = ey[2];
    m_rotMat( 2, 0 ) = ez[0];
    m_rotMat( 2, 1 ) = ez[1];
    m_rotMat( 2, 2 ) = ez[2];

    m_rotMatInvert = invertMatrix3x3( m_rotMat );
}

wmath::WVector3D WTalairachConverter::getAc() const
{
    return m_ac;
}

void WTalairachConverter::setAc( wmath::WVector3D ac )
{
    assert( ac[0] > m_pc[0] );
    m_ac = ac;
    defineRotationMatrix();
}

wmath::WVector3D WTalairachConverter::getPc() const
{
    return m_pc;
}

void WTalairachConverter::setPc( wmath::WVector3D pc )
{
    assert( pc[0] < m_ac[0] );
    m_pc = pc;
    defineRotationMatrix();
}

wmath::WVector3D WTalairachConverter::getIhp() const
{
    return m_ihp;
}

void WTalairachConverter::setIhp( wmath::WVector3D ihp )
{
    assert( ihp[2] != m_ac[2] );
    m_ihp = ihp;
    defineRotationMatrix();
}

wmath::WVector3D WTalairachConverter::getAp() const
{
    return m_ap;
}

void WTalairachConverter::setAp( wmath::WVector3D ap )
{
    assert( ap[0] > m_ac[0] );
    m_ap = ap;
}

wmath::WVector3D WTalairachConverter::getPp() const
{
    return m_pp;
}

void WTalairachConverter::setPp( wmath::WVector3D pp )
{
    assert( pp[0] < m_pc[0] );
    m_pp = pp;
}

wmath::WVector3D WTalairachConverter::getSp() const
{
    return m_sp;
}

void WTalairachConverter::setSp( wmath::WVector3D sp )
{
    assert( sp[2] > m_ac[2] );
    m_sp = sp;
}

wmath::WVector3D WTalairachConverter::getIp() const
{
    return m_ip;
}

void WTalairachConverter::setIp( wmath::WVector3D ip )
{
    assert( ip[2] < m_ac[2] );
    m_ip = ip;
}

wmath::WVector3D WTalairachConverter::getRp() const
{
    return m_rp;
}

void WTalairachConverter::setRp( wmath::WVector3D rp )
{
    assert( rp[1] < m_ac[1] );
    m_rp = rp;
}

wmath::WVector3D WTalairachConverter::getLp() const
{
    return m_lp;
}

void WTalairachConverter::setLp( wmath::WVector3D lp )
{
    assert( lp[1] > m_ac[1] );
    m_lp = lp;
}

wmath::WVector3D WTalairachConverter::multMatrixWithVector3D( wmath::WMatrix<double> mat, wmath::WVector3D vec )
{
    wmath::WVector3D result;
    result[0] = mat( 0, 0 ) * vec[0] + mat( 0, 1 ) * vec[1] + mat( 0, 2 ) * vec[2];
    result[1] = mat( 1, 0 ) * vec[0] + mat( 1, 1 ) * vec[1] + mat( 1, 2 ) * vec[2];
    result[2] = mat( 2, 0 ) * vec[0] + mat( 2, 1 ) * vec[1] + mat( 2, 2 ) * vec[2];
    return result;
}

wmath::WMatrix<double> WTalairachConverter::invertMatrix3x3( wmath::WMatrix<double> mat )
{
    double det = mat( 0, 0 ) * mat( 1, 1 ) * mat( 2, 2 ) +
                mat( 0, 1 ) * mat( 1, 2 ) * mat( 2, 0 ) +
                mat( 0, 2 ) * mat( 1, 0 ) * mat( 2, 1 ) -
                mat( 0, 2 ) * mat( 1, 1 ) * mat( 2, 0 ) -
                mat( 0, 1 ) * mat( 1, 0 ) * mat( 2, 2 ) -
                mat( 0, 0 ) * mat( 1, 2 ) * mat( 2, 1 );

    wmath::WMatrix<double> r( 3, 3 );

    r( 0, 0 ) = ( mat( 1, 1 ) * mat( 2, 2 ) - mat(  1, 2 ) * mat( 2, 1 ) ) / det;
    r( 1, 0 ) = ( mat( 1, 2 ) * mat( 2, 0 ) - mat(  1, 0 ) * mat( 2, 2 ) ) / det;
    r( 2, 0 ) = ( mat( 1, 0 ) * mat( 2, 1 ) - mat(  1, 1 ) * mat( 2, 0 ) ) / det;
    r( 0, 1 ) = ( mat( 0, 2 ) * mat( 2, 1 ) - mat(  0, 1 ) * mat( 2, 2 ) ) / det;
    r( 1, 1 ) = ( mat( 0, 0 ) * mat( 2, 2 ) - mat(  0, 2 ) * mat( 2, 0 ) ) / det;
    r( 2, 1 ) = ( mat( 0, 1 ) * mat( 2, 0 ) - mat(  0, 0 ) * mat( 2, 1 ) ) / det;
    r( 0, 2 ) = ( mat( 0, 1 ) * mat( 1, 2 ) - mat(  0, 2 ) * mat( 1, 1 ) ) / det;
    r( 1, 2 ) = ( mat( 0, 2 ) * mat( 1, 0 ) - mat(  0, 0 ) * mat( 1, 2 ) ) / det;
    r( 2, 2 ) = ( mat( 0, 0 ) * mat( 1, 1 ) - mat(  0, 1 ) * mat( 1, 0 ) ) / det;

    return r;
}
