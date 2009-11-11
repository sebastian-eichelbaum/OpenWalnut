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
    return point - m_ac;
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

wmath::WVector3D WTalairachConverter::ACPC2Talairach( const wmath::WVector3D /*point*/ )
{
    return wmath::WVector3D();
}

wmath::WVector3D WTalairachConverter::Talairach2ACPC( const wmath::WVector3D /*point*/ )
{
    return wmath::WVector3D();
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
