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

#include "WTalairachConverter.h"
#include "core/common/math/WLinearAlgebraFunctions.h"

WTalairachConverter::WTalairachConverter( WVector3d ac, WVector3d pc, WVector3d ihp ) :
    m_rotMat( 3, 3 ),
    m_rotMatInvert( 3, 3 ),
    m_ac( ac ),
    m_pc( pc ),
    m_ihp( ihp )
{
    // initalize the bounding box of the head with some arbitrary values, these have to be set to real values
    // before any coordinate conversion
    // the point of origin in the canonical system is the right inferior posterior corner

    m_ap = WVector3d( 255, 0, 0 );
    m_pp = WVector3d( 0, 0, 0 );

    m_lp = WVector3d( 0, 255, 0 );
    m_rp = WVector3d( 0, 0, 0 );

    m_ip = WVector3d( 0, 0, 0 );
    m_sp = WVector3d( 0, 0, 255 );

    defineRotationMatrix();
}

WTalairachConverter::~WTalairachConverter()
{
}

WVector3d WTalairachConverter::Canonical2ACPC( const WVector3d point )
{
    WVector3d rpoint = point - m_ac;
    return multMatrixWithVector3D( m_rotMat, rpoint );
}

WVector3d WTalairachConverter::ACPC2Canonical( const WVector3d point )
{
    WVector3d rpoint = multMatrixWithVector3D( m_rotMatInvert, point );
    return rpoint + m_ac;
}

WVector3d WTalairachConverter::Canonical2Talairach( const WVector3d point )
{
    return ACPC2Talairach( Canonical2ACPC( point ) );
}

WVector3d WTalairachConverter::Talairach2Canonical( const WVector3d point )
{
    return ACPC2Canonical( Talairach2ACPC( point ) );
}

WVector3d WTalairachConverter::ACPC2Talairach( const WVector3d point )
{
    // declare some variables for readability
    double x = point[0];
    double y = point[1];
    double z = point[2];

//    double X1 = length( m_pp - m_pc );
//    double X2 = length( m_ac - m_pc );
//    double X3 = length( m_ap - m_ac );
//    double Y1 = length( m_ac - m_rp );
//    double Y2 = length( m_lp - m_ac );
//    double Z1 = length( m_ac - m_ip );
//    double Z2 = length( m_sp - m_ac );

    double X1 = fabs( m_pp[0] - m_pc[0] );
    double X2 = fabs( m_ac[0] - m_pc[0] );
    double X3 = fabs( m_ap[0] - m_ac[0] );
    double Y1 = fabs( m_ac[1] - m_rp[1] );
    double Y2 = fabs( m_lp[1] - m_ac[1] );
    double Z1 = fabs( m_ac[2] - m_ip[2] );
    double Z2 = fabs( m_sp[2] - m_ac[2] );

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

    if( x < -X2 ) // posterior to PC
    {
        xt = ( X1T / X1 ) * ( x + X2 ) - X2T;
    }
    else if( x >= 0 ) // anterior to AC
    {
        xt = ( X3T / X3 ) * x;
    }
    else // between AC and PC
    {
        xt = ( X2T / X2 ) * x;
    }

    if( y < 0 ) // right hemisphere
    {
        yt = ( Y1T / Y1 ) * y;
    }
    else // left hemisphere
    {
        yt = ( Y2T / Y2 ) * y;
    }

    if( z < 0 ) // inferior to AC-PC
    {
        zt = ( Z1T / Z1 ) * z;
    }
    else // superior to AC-PC
    {
        zt = ( Z2T / Z2 ) * z;
    }

    return WVector3d( xt, yt, zt );
}

WVector3d WTalairachConverter::Talairach2ACPC( const WVector3d point )
{
    // declare some variables for readability
    double xt = point[0];
    double yt = point[1];
    double zt = point[2];

//    double X1 = length( m_pp - m_pc );
//    double X2 = length( m_ac - m_pc );
//    double X3 = length( m_ap - m_ac );
//    double Y1 = length( m_ac - m_rp );
//    double Y2 = length( m_lp - m_ac );
//    double Z1 = length( m_ac - m_ip );
//    double Z2 = length( m_sp - m_ac );

    double X1 = fabs( m_pp[0] - m_pc[0] );
    double X2 = fabs( m_ac[0] - m_pc[0] );
    double X3 = fabs( m_ap[0] - m_ac[0] );
    double Y1 = fabs( m_ac[1] - m_rp[1] );
    double Y2 = fabs( m_lp[1] - m_ac[1] );
    double Z1 = fabs( m_ac[2] - m_ip[2] );
    double Z2 = fabs( m_sp[2] - m_ac[2] );


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

    if( xt < -X2T ) // posterior to PC
    {
        x = ( X1 / X1T ) * ( xt + X2T ) - X2;
    }
    else if( xt >= 0 ) // anterior to AC
    {
        x = ( X3 / X3T ) * xt;
    }
    else // between AC and PC
    {
        x = ( X2 / X2T ) * xt;
    }

    if( yt < 0 ) // right hemisphere
    {
        y = ( Y1 / Y1T ) * yt;
    }
    else // left hemisphere
    {
        y = ( Y2 / Y2T ) * yt;
    }

    if( zt < 0 ) // inferior to AC-PC
    {
        z = ( Z1 / Z1T ) * zt;
    }
    else // superior to AC-PC
    {
        z = ( Z2 / Z2T ) * zt;
    }

    return WVector3d( x, y, z );
}


void WTalairachConverter::defineRotationMatrix()
{
    //WVector3d ihp_proj( ( ( ( m_ac - m_ihp ) * ( m_pc - m_ac ) ) * ( m_pc - m_ac ) / length2( m_pc - m_as ) ) + m_ihp );
    WVector3d v1 = m_pc - m_ac;
    float apnorm = v1[0] * v1[0] + v1[1] * v1[1] + v1[2] * v1[2];

    WVector3d v2 = m_ac - m_ihp;
    float dist = v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];


    WVector3d ihp_proj( dist * ( m_pc - m_ac ) / apnorm  + m_ihp );

    m_ihp_proj = ihp_proj;
    WVector3d ex( m_ac - m_pc );
    ex = normalize( ex );
    WVector3d ez( ihp_proj - m_ac );
    ez = normalize( ez );
    WVector3d ey = cross( ez, ex );

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

WVector3d WTalairachConverter::getAc() const
{
    return m_ac;
}

void WTalairachConverter::setAc( WVector3d ac )
{
    m_ac = ac;
    defineRotationMatrix();
}

WVector3d WTalairachConverter::getPc() const
{
    return m_pc;
}

void WTalairachConverter::setPc( WVector3d pc )
{
    m_pc = pc;
    defineRotationMatrix();
}

WVector3d WTalairachConverter::getIhp() const
{
    return m_ihp;
}

void WTalairachConverter::setIhp( WVector3d ihp )
{
    m_ihp = ihp;
    defineRotationMatrix();
}

WVector3d WTalairachConverter::getAp() const
{
    return m_ap;
}

void WTalairachConverter::setAp( WVector3d ap )
{
    m_ap = ap;
}

WVector3d WTalairachConverter::getPp() const
{
    return m_pp;
}

void WTalairachConverter::setPp( WVector3d pp )
{
    m_pp = pp;
}

WVector3d WTalairachConverter::getSp() const
{
    return m_sp;
}

void WTalairachConverter::setSp( WVector3d sp )
{
    m_sp = sp;
}

WVector3d WTalairachConverter::getIp() const
{
    return m_ip;
}

void WTalairachConverter::setIp( WVector3d ip )
{
    m_ip = ip;
}

WVector3d WTalairachConverter::getRp() const
{
    return m_rp;
}

void WTalairachConverter::setRp( WVector3d rp )
{
    m_rp = rp;
}

WVector3d WTalairachConverter::getLp() const
{
    return m_lp;
}

void WTalairachConverter::setLp( WVector3d lp )
{
    m_lp = lp;
}

WMatrix<double> WTalairachConverter::getRotMat()
{
    return m_rotMat;
}

WMatrix<double> WTalairachConverter::getInvRotMat()
{
    return m_rotMatInvert;
}
