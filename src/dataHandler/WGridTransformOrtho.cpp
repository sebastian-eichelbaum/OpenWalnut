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

#include "../common/exceptions/WPreconditionNotMet.h"

#include "WGridTransformOrtho.h"

WGridTransformOrtho::WGridTransformOrtho()
    : m_directionX( 1.0, 0.0, 0.0 ),
      m_directionY( 0.0, 1.0, 0.0 ),
      m_directionZ( 0.0, 0.0, 1.0 ),
      m_scaling( 1.0, 1.0, 1.0 ),
      m_origin( 0.0, 0.0, 0.0 )
{
}

WGridTransformOrtho::WGridTransformOrtho( double scaleX, double scaleY, double scaleZ )
    : m_directionX( ( scaleX > 0.0 ) - ( scaleX < 0.0 ), 0.0, 0.0 ),
      m_directionY( 0.0, ( scaleY > 0.0 ) - ( scaleY < 0.0 ), 0.0 ),
      m_directionZ( 0.0, 0.0, ( scaleZ > 0.0 ) - ( scaleZ < 0.0 ) ),
      m_scaling( fabs( scaleX ), fabs( scaleY ), fabs( scaleZ ) ),
      m_origin( 0.0, 0.0, 0.0 )
{
    WPrecond( m_scaling[ 0 ] != 0.0 && m_scaling[ 1 ] != 0.0 && m_scaling[ 2 ] != 0.0, "" );
}

WGridTransformOrtho::WGridTransformOrtho( WMatrix< double > const& mat )
{
    WPrecond( mat.getNbRows() == 4 && mat.getNbCols() == 4, "" );
    m_directionX = WVector3d( mat( 0, 0 ), mat( 1, 0 ), mat( 2, 0 ) );
    m_directionY = WVector3d( mat( 0, 1 ), mat( 1, 1 ), mat( 2, 1 ) );
    m_directionZ = WVector3d( mat( 0, 2 ), mat( 1, 2 ), mat( 2, 2 ) );

    m_scaling = WVector3d( length( m_directionX ), length( m_directionY ), length( m_directionZ ) );

    WPrecond( m_scaling[ 0 ] != 0.0 && m_scaling[ 1 ] != 0.0 && m_scaling[ 2 ] != 0.0, "" );
    m_directionX /= m_scaling[ 0 ];
    m_directionY /= m_scaling[ 1 ];
    m_directionZ /= m_scaling[ 2 ];

    WPrecondLess( fabs( dot( m_directionX, m_directionY ) ), 0.0001 );
    WPrecondLess( fabs( dot( m_directionX, m_directionZ ) ), 0.0001 );
    WPrecondLess( fabs( dot( m_directionY, m_directionZ ) ), 0.0001 );
    m_origin = WVector3d( mat( 0, 3 ), mat( 1, 3 ), mat( 2, 3 ) );
}

WGridTransformOrtho::~WGridTransformOrtho()
{
}

WVector3d WGridTransformOrtho::positionToWorldSpace( WVector3d const& position ) const
{
    return WVector3d( m_scaling[ 0 ] * position[ 0 ] * m_directionX[ 0 ] + m_scaling[ 1 ] * position[ 1 ] * m_directionY[ 0 ]
                    + m_scaling[ 2 ] * position[ 2 ] * m_directionZ[ 0 ] + m_origin[ 0 ],
                      m_scaling[ 0 ] * position[ 0 ] * m_directionX[ 1 ] + m_scaling[ 1 ] * position[ 1 ] * m_directionY[ 1 ]
                    + m_scaling[ 2 ] * position[ 2 ] * m_directionZ[ 1 ] + m_origin[ 1 ],
                      m_scaling[ 0 ] * position[ 0 ] * m_directionX[ 2 ] + m_scaling[ 1 ] * position[ 1 ] * m_directionY[ 2 ]
                    + m_scaling[ 2 ] * position[ 2 ] * m_directionZ[ 2 ] + m_origin[ 2 ] );
}

WVector3d WGridTransformOrtho::positionToGridSpace( WVector3d const& position ) const
{
    WVector3d p = position - m_origin;
    p = WVector3d( dot( p, m_directionX ), dot( p, m_directionY ), dot( p, m_directionZ ) );
    p[ 0 ] /= m_scaling[ 0 ];
    p[ 1 ] /= m_scaling[ 1 ];
    p[ 2 ] /= m_scaling[ 2 ];
    return p;
}

WVector3d WGridTransformOrtho::directionToWorldSpace( WVector3d const& direction ) const
{
    return WVector3d( m_scaling[ 0 ] * direction[ 0 ] * m_directionX[ 0 ] + m_scaling[ 1 ] * direction[ 1 ] * m_directionY[ 0 ]
                    + m_scaling[ 2 ] * direction[ 2 ] * m_directionZ[ 0 ],
                      m_scaling[ 0 ] * direction[ 0 ] * m_directionX[ 1 ] + m_scaling[ 1 ] * direction[ 1 ] * m_directionY[ 1 ]
                    + m_scaling[ 2 ] * direction[ 2 ] * m_directionZ[ 1 ],
                      m_scaling[ 0 ] * direction[ 0 ] * m_directionX[ 2 ] + m_scaling[ 1 ] * direction[ 1 ] * m_directionY[ 2 ]
                    + m_scaling[ 2 ] * direction[ 2 ] * m_directionZ[ 2 ] );
}

WVector3d WGridTransformOrtho::directionToGridSpace( WVector3d const& direction ) const
{
    WVector3d p( dot( direction, m_directionX ), dot( direction, m_directionY ), dot( direction, m_directionZ ) );
    p[ 0 ] /= m_scaling[ 0 ];
    p[ 1 ] /= m_scaling[ 1 ];
    p[ 2 ] /= m_scaling[ 2 ];
    return p;
}

double WGridTransformOrtho::getOffsetX() const
{
    return m_scaling[ 0 ];
}

double WGridTransformOrtho::getOffsetY() const
{
    return m_scaling[ 1 ];
}

double WGridTransformOrtho::getOffsetZ() const
{
    return m_scaling[ 2 ];
}

WVector3d WGridTransformOrtho::getUnitDirectionX() const
{
    return m_directionX;
}

WVector3d WGridTransformOrtho::getUnitDirectionY() const
{
    return m_directionY;
}

WVector3d WGridTransformOrtho::getUnitDirectionZ() const
{
    return m_directionZ;
}

WVector3d WGridTransformOrtho::getDirectionX() const
{
    return m_directionX * m_scaling[ 0 ];
}

WVector3d WGridTransformOrtho::getDirectionY() const
{
    return m_directionY * m_scaling[ 1 ];
}

WVector3d WGridTransformOrtho::getDirectionZ() const
{
    return m_directionZ * m_scaling[ 2 ];
}

WPosition WGridTransformOrtho::getOrigin() const
{
    return m_origin;
}

WMatrix< double > WGridTransformOrtho::getTransformationMatrix() const
{
    WMatrix< double > mat( 4, 4 );
    mat.makeIdentity();
    mat( 0, 0 ) = m_scaling[ 0 ] * m_directionX[ 0 ];
    mat( 1, 0 ) = m_scaling[ 0 ] * m_directionX[ 1 ];
    mat( 2, 0 ) = m_scaling[ 0 ] * m_directionX[ 2 ];
    mat( 0, 1 ) = m_scaling[ 1 ] * m_directionY[ 0 ];
    mat( 1, 1 ) = m_scaling[ 1 ] * m_directionY[ 1 ];
    mat( 2, 1 ) = m_scaling[ 1 ] * m_directionY[ 2 ];
    mat( 0, 2 ) = m_scaling[ 2 ] * m_directionZ[ 0 ];
    mat( 1, 2 ) = m_scaling[ 2 ] * m_directionZ[ 1 ];
    mat( 2, 2 ) = m_scaling[ 2 ] * m_directionZ[ 2 ];
    mat( 0, 3 ) = m_origin[ 0 ];
    mat( 1, 3 ) = m_origin[ 1 ];
    mat( 2, 3 ) = m_origin[ 2 ];
    return mat;
}

bool WGridTransformOrtho::isNotRotated() const
{
    return m_directionX == WVector3d( 1.0, 0.0, 0.0 )
        && m_directionY == WVector3d( 0.0, 1.0, 0.0 )
        && m_directionZ == WVector3d( 0.0, 0.0, 1.0 );
}

WGridTransformOrtho::operator WMatrix4d() const
{
    WMatrix4d mat = WMatrix4d::identity();
    mat( 0, 0 ) = m_scaling[ 0 ] * m_directionX[ 0 ];
    mat( 0, 1 ) = m_scaling[ 0 ] * m_directionX[ 1 ];
    mat( 0, 2 ) = m_scaling[ 0 ] * m_directionX[ 2 ];
    mat( 1, 0 ) = m_scaling[ 1 ] * m_directionY[ 0 ];
    mat( 1, 1 ) = m_scaling[ 1 ] * m_directionY[ 1 ];
    mat( 1, 2 ) = m_scaling[ 1 ] * m_directionY[ 2 ];
    mat( 2, 0 ) = m_scaling[ 2 ] * m_directionZ[ 0 ];
    mat( 2, 1 ) = m_scaling[ 2 ] * m_directionZ[ 1 ];
    mat( 2, 2 ) = m_scaling[ 2 ] * m_directionZ[ 2 ];
    mat( 3, 0 ) = m_origin[ 0 ];
    mat( 3, 1 ) = m_origin[ 1 ];
    mat( 3, 2 ) = m_origin[ 2 ];
    return mat;
}

