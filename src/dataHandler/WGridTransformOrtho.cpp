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
    m_directionX = WVector3D( mat( 0, 0 ), mat( 1, 0 ), mat( 2, 0 ) );
    m_directionY = WVector3D( mat( 0, 1 ), mat( 1, 1 ), mat( 2, 1 ) );
    m_directionZ = WVector3D( mat( 0, 2 ), mat( 1, 2 ), mat( 2, 2 ) );

    m_scaling = WVector3D( m_directionX.norm(), m_directionY.norm(), m_directionZ.norm() );

    WPrecond( m_scaling[ 0 ] != 0.0 && m_scaling[ 1 ] != 0.0 && m_scaling[ 2 ] != 0.0, "" );
    m_directionX /= m_scaling[ 0 ];
    m_directionY /= m_scaling[ 1 ];
    m_directionZ /= m_scaling[ 2 ];

    WPrecondLess( fabs( m_directionX.dotProduct( m_directionY ) ), 0.0001 );
    WPrecondLess( fabs( m_directionX.dotProduct( m_directionZ ) ), 0.0001 );
    WPrecondLess( fabs( m_directionY.dotProduct( m_directionZ ) ), 0.0001 );
    m_origin = WVector3D( mat( 0, 3 ), mat( 1, 3 ), mat( 2, 3 ) );
}

WGridTransformOrtho::~WGridTransformOrtho()
{
}

WVector3D WGridTransformOrtho::positionToWorldSpace( WVector3D const& position ) const
{
    return WVector3D( m_scaling[ 0 ] * position[ 0 ] * m_directionX[ 0 ] + m_scaling[ 1 ] * position[ 1 ] * m_directionY[ 0 ]
                    + m_scaling[ 2 ] * position[ 2 ] * m_directionZ[ 0 ] + m_origin[ 0 ],
                      m_scaling[ 0 ] * position[ 0 ] * m_directionX[ 1 ] + m_scaling[ 1 ] * position[ 1 ] * m_directionY[ 1 ]
                    + m_scaling[ 2 ] * position[ 2 ] * m_directionZ[ 1 ] + m_origin[ 1 ],
                      m_scaling[ 0 ] * position[ 0 ] * m_directionX[ 2 ] + m_scaling[ 1 ] * position[ 1 ] * m_directionY[ 2 ]
                    + m_scaling[ 2 ] * position[ 2 ] * m_directionZ[ 2 ] + m_origin[ 2 ] );
}

WVector3D WGridTransformOrtho::positionToGridSpace( WVector3D const& position ) const
{
    WVector3D p = position - m_origin;
    p = WVector3D( p.dotProduct( m_directionX ), p.dotProduct( m_directionY ), p.dotProduct( m_directionZ ) );
    p[ 0 ] /= m_scaling[ 0 ];
    p[ 1 ] /= m_scaling[ 1 ];
    p[ 2 ] /= m_scaling[ 2 ];
    return p;
}

WVector3D WGridTransformOrtho::directionToWorldSpace( WVector3D const& direction ) const
{
    return WVector3D( m_scaling[ 0 ] * direction[ 0 ] * m_directionX[ 0 ] + m_scaling[ 1 ] * direction[ 1 ] * m_directionY[ 0 ]
                    + m_scaling[ 2 ] * direction[ 2 ] * m_directionZ[ 0 ],
                      m_scaling[ 0 ] * direction[ 0 ] * m_directionX[ 1 ] + m_scaling[ 1 ] * direction[ 1 ] * m_directionY[ 1 ]
                    + m_scaling[ 2 ] * direction[ 2 ] * m_directionZ[ 1 ],
                      m_scaling[ 0 ] * direction[ 0 ] * m_directionX[ 2 ] + m_scaling[ 1 ] * direction[ 1 ] * m_directionY[ 2 ]
                    + m_scaling[ 2 ] * direction[ 2 ] * m_directionZ[ 2 ] );
}

WVector3D WGridTransformOrtho::directionToGridSpace( WVector3D const& direction ) const
{
    WVector3D p( direction.dotProduct( m_directionX ), direction.dotProduct( m_directionY ), direction.dotProduct( m_directionZ ) );
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

WVector3D WGridTransformOrtho::getUnitDirectionX() const
{
    return m_directionX;
}

WVector3D WGridTransformOrtho::getUnitDirectionY() const
{
    return m_directionY;
}

WVector3D WGridTransformOrtho::getUnitDirectionZ() const
{
    return m_directionZ;
}

WVector3D WGridTransformOrtho::getDirectionX() const
{
    return m_directionX * m_scaling[ 0 ];
}

WVector3D WGridTransformOrtho::getDirectionY() const
{
    return m_directionY * m_scaling[ 1 ];
}

WVector3D WGridTransformOrtho::getDirectionZ() const
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
    return m_directionX == WVector3D( 1.0, 0.0, 0.0 )
        && m_directionY == WVector3D( 0.0, 1.0, 0.0 )
        && m_directionZ == WVector3D( 0.0, 0.0, 1.0 );
}

WGridTransformOrtho::operator WMatrix4x4() const
{
    // NOTE: OSG matrices are row-major!
    WMatrix4x4 mat;
    mat.makeIdentity();
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

