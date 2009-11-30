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

#include "WGridRegular3D.h"

using wmath::WVector3D;
using wmath::WPosition;
using wmath::WMatrix;

WGridRegular3D::WGridRegular3D( unsigned int nbPosX, unsigned int nbPosY, unsigned int nbPosZ,
                                double originX, double originY, double originZ,
                                const WVector3D& directionX,
                                const WVector3D& directionY,
                                const WVector3D& directionZ,
                                double offsetX, double offsetY, double offsetZ )
    : WGrid( nbPosX * nbPosY * nbPosZ ),
      m_origin( WPosition( originX, originY, originZ ) ),
      m_nbPosX( nbPosX ),  m_nbPosY( nbPosY ),  m_nbPosZ( nbPosZ ),
      m_directionX( directionX ),
      m_directionY( directionY ),
      m_directionZ( directionZ ),
      m_offsetX( offsetX ),
      m_offsetY( offsetY ),
      m_offsetZ( offsetZ ),
      m_matrix( 4, 4 ),
      m_matrixInverse( 3, 3 )
{
    m_matrix( 0, 0 ) = directionX[0];
    m_matrix( 0, 1 ) = directionY[0];
    m_matrix( 0, 2 ) = directionZ[0];
    m_matrix( 1, 0 ) = directionX[1];
    m_matrix( 1, 1 ) = directionY[1];
    m_matrix( 1, 2 ) = directionZ[1];
    m_matrix( 2, 0 ) = directionX[2];
    m_matrix( 2, 1 ) = directionY[2];
    m_matrix( 2, 2 ) = directionZ[2];
    m_matrix( 0, 3 ) = originX;
    m_matrix( 1, 3 ) = originY;
    m_matrix( 2, 3 ) = originZ;

    m_matrix( 3, 3 ) = 1.;

    m_matrixInverse = invertMatrix3x3( m_matrix );
}

WGridRegular3D::WGridRegular3D( unsigned int nbPosX, unsigned int nbPosY, unsigned int nbPosZ,
                                const WMatrix< double >& mat,
                                double offsetX, double offsetY, double offsetZ )
    : WGrid( nbPosX * nbPosY * nbPosZ ),
      m_nbPosX( nbPosX ),
      m_nbPosY( nbPosY ),
      m_nbPosZ( nbPosZ ),
      m_offsetX( offsetX ),
      m_offsetY( offsetY ),
      m_offsetZ( offsetZ ),
      m_matrix( wmath::WMatrix<double>( mat ) ),
      m_matrixInverse( 3, 3 )
{
    assert( mat.getNbRows() == 4 && mat.getNbCols() == 4 );
    // only affine transformations are allowed
    assert( mat( 3, 0 ) == 0.0 && mat( 3, 1 ) == 0.0 && mat( 3, 2 ) == 0.0 );

    m_origin = WPosition( mat( 0, 3 ) / mat( 3, 3 ), mat( 1, 3 ) / mat( 3, 3 ), mat( 2, 3 ) / mat( 3, 3 ) );

    m_directionX = WVector3D( mat( 0, 0 ) / mat( 3, 3 ), mat( 1, 0 ) / mat( 3, 3 ), mat( 2, 0 ) / mat( 3, 3 ) );
    m_directionY = WVector3D( mat( 0, 1 ) / mat( 3, 3 ), mat( 1, 1 ) / mat( 3, 3 ), mat( 2, 1 ) / mat( 3, 3 ) );
    m_directionZ = WVector3D( mat( 0, 2 ) / mat( 3, 3 ), mat( 1, 2 ) / mat( 3, 3 ), mat( 2, 2 ) / mat( 3, 3 ) );

    m_matrix = mat;

    m_matrixInverse = invertMatrix3x3( m_matrix );
}


WGridRegular3D::WGridRegular3D( unsigned int nbPosX, unsigned int nbPosY, unsigned int nbPosZ,
                                double originX, double originY, double originZ,
                                double offsetX, double offsetY, double offsetZ )
    : WGrid( nbPosX * nbPosY * nbPosZ ),
      m_origin( WPosition( originX, originY, originZ ) ),
      m_nbPosX( nbPosX ),
      m_nbPosY( nbPosY ),
      m_nbPosZ( nbPosZ ),
      m_directionX( WVector3D( offsetX, 0., 0. ) ),
      m_directionY( WVector3D( 0., offsetY, 0. ) ),
      m_directionZ( WVector3D( 0., 0., offsetZ ) ),
      m_offsetX( offsetX ),
      m_offsetY( offsetY ),
      m_offsetZ( offsetZ ),
      m_matrix( 4, 4 ),
      m_matrixInverse( 3, 3 )
{
    m_matrix( 0, 0 ) = offsetX;
    m_matrix( 1, 1 ) = offsetY;
    m_matrix( 2, 2 ) = offsetZ;
    m_matrix( 0, 3 ) = originX;
    m_matrix( 1, 3 ) = originY;
    m_matrix( 2, 3 ) = originZ;

    m_matrix( 3, 3 ) = 1.;
}

WGridRegular3D::WGridRegular3D( unsigned int nbPosX, unsigned int nbPosY, unsigned int nbPosZ,
                                double offsetX, double offsetY, double offsetZ )
    : WGrid( nbPosX * nbPosY * nbPosZ ),
      m_origin( WPosition( 0., 0., 0. ) ),
      m_nbPosX( nbPosX ),
      m_nbPosY( nbPosY ),
      m_nbPosZ( nbPosZ ),
      m_directionX( WVector3D( offsetX, 0., 0. ) ),
      m_directionY( WVector3D( 0., offsetY, 0. ) ),
      m_directionZ( WVector3D( 0., 0., offsetZ ) ),
      m_offsetX( offsetX ),
      m_offsetY( offsetY ),
      m_offsetZ( offsetZ ),
      m_matrix( 4, 4 ),
      m_matrixInverse( 3, 3 )
{
    m_matrix( 0, 0 ) = offsetX;
    m_matrix( 1, 1 ) = offsetY;
    m_matrix( 2, 2 ) = offsetZ;

    m_matrix( 3, 3 ) = 1.;
}

WPosition WGridRegular3D::getPosition( unsigned int i ) const
{
    return getPosition(i % m_nbPosX, ( i / m_nbPosX ) % m_nbPosY, i / ( m_nbPosX * m_nbPosY ) );
}

WPosition WGridRegular3D::getPosition( unsigned int iX, unsigned int iY, unsigned int iZ ) const
{
    return m_origin + iX * m_directionX + iY * m_directionY + iZ * m_directionZ;
}
wmath::WMatrix<double> WGridRegular3D::getTransformationMatrix() const
{
    return m_matrix;
}

wmath::WVector3D WGridRegular3D::multMatrixWithVector3D( wmath::WMatrix<double> mat, wmath::WVector3D vec )
{
    wmath::WVector3D result;
    result[0] = mat( 0, 0 ) * vec[0] + mat( 0, 1 ) * vec[1] + mat( 0, 2 ) * vec[2];
    result[1] = mat( 1, 0 ) * vec[0] + mat( 1, 1 ) * vec[1] + mat( 1, 2 ) * vec[2];
    result[2] = mat( 2, 0 ) * vec[0] + mat( 2, 1 ) * vec[1] + mat( 2, 2 ) * vec[2];
    return result;
}

wmath::WMatrix<double> WGridRegular3D::invertMatrix3x3( wmath::WMatrix<double> mat )
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

osg::Vec3 WGridRegular3D::transformTexCoord( osg::Vec3 point )
{
    wmath::WVector3D p( point.x() - 0.5 , point.y() - 0.5, point.z() - 0.5 );

    wmath::WVector3D r( multMatrixWithVector3D( m_matrixInverse , p ) );

    r[0] = r[0] * m_offsetX + 0.5;
    r[1] = r[1] * m_offsetY + 0.5;
    r[2] = r[2] * m_offsetZ + 0.5;

    return osg::Vec3( r[0], r[1], r[2] );
}
