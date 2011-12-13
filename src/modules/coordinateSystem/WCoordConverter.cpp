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

#include <utility>

#include "core/common/WBoundingBox.h"
#include "core/common/math/WLinearAlgebraFunctions.h"

#include "WCoordConverter.h"


WCoordConverter::WCoordConverter( WMatrix<double> rotMat, WVector3d origin, WVector3d scale ) :
    m_rotMat( rotMat ),
    m_origin( origin ),
    m_scale( scale ),
    m_coordinateSystemMode( CS_WORLD )
{
}

WCoordConverter::~WCoordConverter()
{
}

WVector3d WCoordConverter::operator()( WVector3d in )
{
    WVector3d out( in );
    return out;
}

WBoundingBox WCoordConverter::getBoundingBox()
{
    return m_boundingBox;
}

void WCoordConverter::setBoundingBox( WBoundingBox boundingBox )
{
    m_boundingBox = boundingBox;
}

WVector3d WCoordConverter::worldCoordTransformed( WPosition point )
{
    WVector3d r( transformPosition3DWithMatrix4D( m_rotMat, point ) );
    return r;
}

void WCoordConverter::setCoordinateSystemMode( coordinateSystemMode mode )
{
    m_coordinateSystemMode = mode;
}

coordinateSystemMode WCoordConverter::getCoordinateSystemMode()
{
    return m_coordinateSystemMode;
}

int WCoordConverter::numberToCsX( int number )
{
    switch( m_coordinateSystemMode )
    {
        case CS_WORLD:
            return number;
            break;
        case CS_CANONICAL:
            return static_cast<int>( m_boundingBox.xMax() - number );
            break;
        case CS_TALAIRACH:
        {
            WVector3d tmp( number, 0.0, 0.0 );
            return static_cast<int>( w2t( tmp )[1] + 0.5 );
        }
            break;
        default:
            return number;
            break;
    }
}

int WCoordConverter::numberToCsY( int number )
{
    switch( m_coordinateSystemMode )
    {
        case CS_WORLD:
            return number;
            break;
        case CS_CANONICAL:
            return static_cast<int>( number - m_boundingBox.yMin() );
            break;
        case CS_TALAIRACH:
        {
            WVector3d tmp( 0.0, number, 0.0 );
            return static_cast<int>( w2t( tmp )[0] + 0.5 );
        }
            break;
        default:
            return number;
            break;
    }
}

int WCoordConverter::numberToCsZ( int number )
{
    switch( m_coordinateSystemMode )
    {
        case CS_WORLD:
            return number;
            break;
        case CS_CANONICAL:
            return static_cast<int>( number - m_boundingBox.zMin() );
            break;
        case CS_TALAIRACH:
        {
            WVector3d tmp( 0.0, 0.0, number );
            return static_cast<int>( w2t( tmp )[2] + 0.5 );
        }
            break;
        default:
            return number;
            break;
    }
}

void WCoordConverter::setTalairachConverter( boost::shared_ptr<WTalairachConverter> tc )
{
    m_talairachConverter = tc;
}

boost::shared_ptr<WTalairachConverter> WCoordConverter::getTalairachConverter()
{
    return m_talairachConverter;
}

WVector3d WCoordConverter::w2c( WVector3d in )
{
    return WVector3d( in[1], m_boundingBox.xMax() - in[0], in[2] );
}

WVector3d WCoordConverter::c2w( WVector3d in )
{
    return WVector3d( m_boundingBox.xMax() - in[1], in[0] , in[2] );
}


WVector3d WCoordConverter::w2t( WVector3d in )
{
    return m_talairachConverter->Canonical2Talairach( w2c( in ) );
}

WVector3d WCoordConverter::t2w( WVector3d in )
{
    return c2w( m_talairachConverter->ACPC2Canonical( m_talairachConverter->Talairach2ACPC( in ) ) );
}
