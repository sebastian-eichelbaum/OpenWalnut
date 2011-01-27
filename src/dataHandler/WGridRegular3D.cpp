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

#include <cmath>
#include <vector>
#include <string>
#include <utility>

#include "../common/WAssert.h"
#include "../common/WBoundingBox.h"
#include "../common/exceptions/WOutOfBounds.h"
#include "../common/math/WLinearAlgebraFunctions.h"

#include "WGridRegular3D.h"

using wmath::WVector3D;
using wmath::WPosition;
using wmath::WMatrix;
using wmath::WMatrix4x4;

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
      m_offsetXorig( offsetX ),
      m_offsetYorig( offsetY ),
      m_offsetZorig( offsetZ ),
      m_matrix(),
      m_matrixTexToWorld(),
      m_matrixWorldToTex(),
      m_matrixNoMatrix(),
      m_matrixQForm(),
      m_matrixSForm(),
      m_matrixActive( 0 ),
      m_translateMatrix(),
      m_rotMatrix(),
      m_stretchMatrix(),
      m_transformationUpdateCondition( new WCondition() )
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
    m_matrixNoMatrix = m_matrix;
    /**
     * qform and sform matrixes are initialized but will not be used
     */
    m_matrixQForm = m_matrix;
    m_matrixSForm = m_matrix;

    m_translateMatrix.makeIdentity();
    m_rotMatrix.makeIdentity();
    m_stretchMatrix.makeIdentity();

    recreateTextureTransformationMatrices();

    initInformationProperties();
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
      m_offsetXorig( offsetX ),
      m_offsetYorig( offsetY ),
      m_offsetZorig( offsetZ ),
      m_matrix( mat ),
      m_matrixTexToWorld(),
      m_matrixWorldToTex(),
      m_matrixNoMatrix(),
      m_matrixQForm(),
      m_matrixSForm(),
      m_matrixActive( 1 ),
      m_translateMatrix(),
      m_rotMatrix(),
      m_stretchMatrix(),
      m_transformationUpdateCondition( new WCondition() )
{
    WAssert( mat.getNbRows() == 4 && mat.getNbCols() == 4, "Transformation matrix has wrong dimensions." );
    // only affine transformations are allowed
    WAssert( mat( 3, 0 ) == 0.0 && mat( 3, 1 ) == 0.0 && mat( 3, 2 ) == 0.0, "Transf. matrix has to have no projection part." );

    m_origin = WPosition( mat( 0, 3 ) / mat( 3, 3 ), mat( 1, 3 ) / mat( 3, 3 ), mat( 2, 3 ) / mat( 3, 3 ) );

    m_directionX = WVector3D( mat( 0, 0 ) / mat( 3, 3 ), mat( 1, 0 ) / mat( 3, 3 ), mat( 2, 0 ) / mat( 3, 3 ) );
    m_directionY = WVector3D( mat( 0, 1 ) / mat( 3, 3 ), mat( 1, 1 ) / mat( 3, 3 ), mat( 2, 1 ) / mat( 3, 3 ) );
    m_directionZ = WVector3D( mat( 0, 2 ) / mat( 3, 3 ), mat( 1, 2 ) / mat( 3, 3 ), mat( 2, 2 ) / mat( 3, 3 ) );

    /**
     * both qform and sform are initialized with the given matrix, need to call setActiveMatrix to name the
     * correct matrix
     */
    m_matrixQForm = m_matrix;
    m_matrixSForm = m_matrix;

    recreateTextureTransformationMatrices();

    m_matrixNoMatrix.makeIdentity();
    m_matrixNoMatrix( 0, 0 ) = fabs( offsetX );
    m_matrixNoMatrix( 1, 1 ) = fabs( offsetY );
    m_matrixNoMatrix( 2, 2 ) = fabs( offsetZ );

    m_translateMatrix.makeIdentity();
    m_rotMatrix.makeIdentity();
    m_stretchMatrix.makeIdentity();

    initInformationProperties();
}

WGridRegular3D::WGridRegular3D( unsigned int nbPosX, unsigned int nbPosY, unsigned int nbPosZ,
                                const WMatrix< double >& qFormMat,
                                const WMatrix< double >& sFormMat,
                                double offsetX, double offsetY, double offsetZ )
    : WGrid( nbPosX * nbPosY * nbPosZ ),
      m_nbPosX( nbPosX ),
      m_nbPosY( nbPosY ),
      m_nbPosZ( nbPosZ ),
      m_offsetX( offsetX ),
      m_offsetY( offsetY ),
      m_offsetZ( offsetZ ),
      m_offsetXorig( offsetX ),
      m_offsetYorig( offsetY ),
      m_offsetZorig( offsetZ ),
      m_matrix(),
      m_matrixTexToWorld(),
      m_matrixWorldToTex(),
      m_matrixNoMatrix(),
      m_matrixQForm(),
      m_matrixSForm(),
      m_matrixActive( 1 ),
      m_translateMatrix(),
      m_rotMatrix(),
      m_stretchMatrix(),
      m_transformationUpdateCondition( new WCondition() )
{
    m_matrixQForm = qFormMat;
    m_matrixSForm = sFormMat;

    // Check if matrix is completely zero and replace it by identity if so
    if ( fabs( m_matrixQForm( 0, 0 ) ) + fabs( m_matrixQForm( 0, 1 ) ) + fabs( m_matrixQForm( 0, 2 ) ) + fabs( m_matrixQForm( 0, 3 ) ) +
         fabs( m_matrixQForm( 1, 0 ) ) + fabs( m_matrixQForm( 1, 1 ) ) + fabs( m_matrixQForm( 1, 2 ) ) + fabs( m_matrixQForm( 1, 3 ) ) +
         fabs( m_matrixQForm( 2, 0 ) ) + fabs( m_matrixQForm( 2, 1 ) ) + fabs( m_matrixQForm( 2, 2 ) ) + fabs( m_matrixQForm( 2, 3 ) ) +
         fabs( m_matrixQForm( 3, 0 ) ) + fabs( m_matrixQForm( 3, 1 ) ) + fabs( m_matrixQForm( 3, 2 ) ) + fabs( m_matrixQForm( 3, 3 ) ) == 0 )
    {
        m_matrixQForm.makeIdentity();
    }

    // Check if matrix is completely zero and replace it by identity if so
    if ( fabs( m_matrixSForm( 0, 0 ) ) + fabs( m_matrixSForm( 0, 1 ) ) + fabs( m_matrixSForm( 0, 2 ) ) + fabs( m_matrixSForm( 0, 3 ) ) +
         fabs( m_matrixSForm( 1, 0 ) ) + fabs( m_matrixSForm( 1, 1 ) ) + fabs( m_matrixSForm( 1, 2 ) ) + fabs( m_matrixSForm( 1, 3 ) ) +
         fabs( m_matrixSForm( 2, 0 ) ) + fabs( m_matrixSForm( 2, 1 ) ) + fabs( m_matrixSForm( 2, 2 ) ) + fabs( m_matrixSForm( 2, 3 ) ) +
         fabs( m_matrixSForm( 3, 0 ) ) + fabs( m_matrixSForm( 3, 1 ) ) + fabs( m_matrixSForm( 3, 2 ) ) + fabs( m_matrixSForm( 3, 3 ) ) == 0 )
    {
        m_matrixSForm.makeIdentity();
    }

    m_matrixNoMatrix.makeIdentity();
    m_matrixNoMatrix( 0, 0 ) = fabs( m_offsetX );
    m_matrixNoMatrix( 1, 1 ) = fabs( m_offsetY );
    m_matrixNoMatrix( 2, 2 ) = fabs( m_offsetZ );

    m_matrix = m_matrixNoMatrix;
    recreateTextureTransformationMatrices();
    m_matrixActive = 0;

    // TODO(all): this seems quite wrong
    m_directionX = WVector3D( 1, 0, 0 );
    m_directionY = WVector3D( 0, 1, 0 );
    m_directionZ = WVector3D( 0, 0, 1 );
    m_origin = WPosition( 0. , 0. , 0. );

    m_translateMatrix.makeIdentity();
    m_rotMatrix.makeIdentity();
    m_stretchMatrix.makeIdentity();

    initInformationProperties();
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
      m_offsetXorig( offsetX ),
      m_offsetYorig( offsetY ),
      m_offsetZorig( offsetZ ),
      m_matrix(),
      m_matrixTexToWorld(),
      m_matrixWorldToTex(),
      m_matrixNoMatrix(),
      m_matrixQForm(),
      m_matrixSForm(),
      m_matrixActive( 0 ),
      m_translateMatrix(),
      m_rotMatrix(),
      m_stretchMatrix(),
      m_transformationUpdateCondition( new WCondition() )
{
    m_matrix( 0, 0 ) = offsetX;
    m_matrix( 1, 1 ) = offsetY;
    m_matrix( 2, 2 ) = offsetZ;
    m_matrix( 0, 3 ) = originX;
    m_matrix( 1, 3 ) = originY;
    m_matrix( 2, 3 ) = originZ;

    m_matrix( 3, 3 ) = 1.;
    m_matrixNoMatrix = m_matrix;
    m_matrixQForm = m_matrix;
    m_matrixSForm = m_matrix;

    recreateTextureTransformationMatrices();

    m_translateMatrix.makeIdentity();
    m_rotMatrix.makeIdentity();
    m_stretchMatrix.makeIdentity();

    initInformationProperties();
}

WGridRegular3D::WGridRegular3D( unsigned int nbPosX, unsigned int nbPosY, unsigned int nbPosZ,
                                wmath::WPosition origin,
                                double offsetX, double offsetY, double offsetZ )
    : WGrid( nbPosX * nbPosY * nbPosZ ),
      m_origin( origin ),
      m_nbPosX( nbPosX ),
      m_nbPosY( nbPosY ),
      m_nbPosZ( nbPosZ ),
      m_directionX( WVector3D( offsetX, 0., 0. ) ),
      m_directionY( WVector3D( 0., offsetY, 0. ) ),
      m_directionZ( WVector3D( 0., 0., offsetZ ) ),
      m_offsetX( offsetX ),
      m_offsetY( offsetY ),
      m_offsetZ( offsetZ ),
      m_offsetXorig( offsetX ),
      m_offsetYorig( offsetY ),
      m_offsetZorig( offsetZ ),
      m_matrix(),
      m_matrixTexToWorld(),
      m_matrixWorldToTex(),
      m_matrixNoMatrix(),
      m_matrixQForm(),
      m_matrixSForm(),
      m_matrixActive( 0 ),
      m_translateMatrix(),
      m_rotMatrix(),
      m_stretchMatrix(),
      m_transformationUpdateCondition( new WCondition() )
{
    m_matrix( 0, 0 ) = offsetX;
    m_matrix( 1, 1 ) = offsetY;
    m_matrix( 2, 2 ) = offsetZ;
    m_matrix( 0, 3 ) = origin[0];
    m_matrix( 1, 3 ) = origin[1];
    m_matrix( 2, 3 ) = origin[2];

    m_matrix( 3, 3 ) = 1.;
    m_matrixNoMatrix = m_matrix;
    m_matrixQForm = m_matrix;
    m_matrixSForm = m_matrix;

    recreateTextureTransformationMatrices();

    m_translateMatrix.makeIdentity();
    m_rotMatrix.makeIdentity();
    m_stretchMatrix.makeIdentity();

    initInformationProperties();
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
      m_offsetXorig( offsetX ),
      m_offsetYorig( offsetY ),
      m_offsetZorig( offsetZ ),
      m_matrix(),
      m_matrixTexToWorld(),
      m_matrixWorldToTex(),
      m_matrixNoMatrix(),
      m_matrixQForm(),
      m_matrixSForm(),
      m_matrixActive( 0 ),
      m_translateMatrix(),
      m_rotMatrix(),
      m_stretchMatrix(),
      m_transformationUpdateCondition( new WCondition() )
{
    m_matrix( 0, 0 ) = offsetX;
    m_matrix( 1, 1 ) = offsetY;
    m_matrix( 2, 2 ) = offsetZ;

    m_matrix( 3, 3 ) = 1.;
    m_matrixNoMatrix = m_matrix;
    m_matrixQForm = m_matrix;
    m_matrixSForm = m_matrix;

    recreateTextureTransformationMatrices();

    m_translateMatrix.makeIdentity();
    m_rotMatrix.makeIdentity();
    m_stretchMatrix.makeIdentity();

    initInformationProperties();
}

WPosition WGridRegular3D::getPosition( unsigned int i ) const
{
    return getPosition(i % m_nbPosX, ( i / m_nbPosX ) % m_nbPosY, i / ( m_nbPosX * m_nbPosY ) );
}

WPosition WGridRegular3D::getPosition( unsigned int iX, unsigned int iY, unsigned int iZ ) const
{
    return m_origin + iX * m_directionX + iY * m_directionY + iZ * m_directionZ;
}

wmath::WMatrix< double > WGridRegular3D::getTransformationMatrix() const
{
    // TODO(ebaum): remove this or replace this with a function returning WMatrix4x4
    return m_matrix; // this automatically casts
}

void WGridRegular3D::recreateTextureTransformationMatrices()
{
    // World-to-Tex:

    // Scale according to bbox
    WMatrix4x4 scale;
    scale( 0, 0 ) = 1.0 / m_nbPosX;
    scale( 1, 1 ) = 1.0 / m_nbPosY;
    scale( 2, 2 ) = 1.0 / m_nbPosZ;
    scale( 3, 3 ) = 1.0;

    // Move to voxel center
    WMatrix4x4 offset = WMatrix4x4::identity();
    offset( 0, 3 ) = 0.5 / m_nbPosX;
    offset( 1, 3 ) = 0.5 / m_nbPosY;
    offset( 2, 3 ) = 0.5 / m_nbPosZ;

    m_matrixWorldToTex = WMatrix4x4::inverse( m_matrix ) * scale * offset;

    // Tex-To-World:
    // Scale according to bbox
    // actually, this is the inverse of scale
    scale( 0, 0 ) = m_nbPosX;
    scale( 1, 1 ) = m_nbPosY;
    scale( 2, 2 ) = m_nbPosZ;

    // Move to voxel center
    // actually, this is the inverse of offset
    offset( 3, 0 ) = -0.5 / m_nbPosX;
    offset( 3, 1 ) = -0.5 / m_nbPosY;
    offset( 3, 2 ) = -0.5 / m_nbPosZ;

    // actually, this is the inverse of m_matrixWorldToTex
    m_matrixTexToWorld = offset * scale * m_matrix;

    m_transformationUpdateCondition->notify();
}

wmath::WVector3D WGridRegular3D::worldCoordToTexCoord( wmath::WPosition point )
{
    return wmath::transformPosition3DWithMatrix4D( m_matrixWorldToTex, point );
}

wmath::WPosition WGridRegular3D::texCoordToWorldCoord( wmath::WVector3D coords )
{
    return wmath::transformPosition3DWithMatrix4D( m_matrixTexToWorld, coords );
}

const wmath::WMatrix4x4& WGridRegular3D::getWorldToTexMatrix() const
{
    return m_matrixWorldToTex;
}

const wmath::WMatrix4x4& WGridRegular3D::getTexToWorldMatrix() const
{
    return m_matrixTexToWorld;
}

int WGridRegular3D::getVoxelNum( const wmath::WPosition& pos ) const
{
    // Note: the reason for the +1 is that the first and last Voxel in a x-axis
    // row are cutted.
    //
    //  y-axis
    //  _|_______     ___ this is the 3rd Voxel
    // 1 |   |   |   v
    //   |...............
    //  _|_:_|_:_|_:_|_:____ x-axis
    //   | : | : | : | :
    //   |.:...:...:...:.
    //   0   1   2
    int xVoxelCoord = getXVoxelCoord( pos );
    int yVoxelCoord = getYVoxelCoord( pos );
    int zVoxelCoord = getZVoxelCoord( pos );
    if( xVoxelCoord == -1 || yVoxelCoord == -1 || zVoxelCoord == -1 )
    {
        return -1;
    }
    return xVoxelCoord
         + yVoxelCoord * ( m_nbPosX )
         + zVoxelCoord * ( m_nbPosX ) * ( m_nbPosY );
}

int WGridRegular3D::getVoxelNum( const size_t x, const size_t y, const size_t z ) const
{
    // since we use size_t here only a check for the upper bounds is needed
    if ( x > m_nbPosX || y > m_nbPosY || z > m_nbPosZ )
    {
        return -1;
    }
    return x + y * ( m_nbPosX ) + z * ( m_nbPosX ) * ( m_nbPosY );
}

int WGridRegular3D::getNVoxelCoord( const wmath::WPosition& pos, size_t axis ) const
{
    double result = pos[ axis ] - m_origin[ axis ];
    unsigned int nbAxisPos = 0;
    double offsetAxis = 0;
    switch( axis )
    {
        case 0 : nbAxisPos = m_nbPosX;
                 offsetAxis = m_offsetX;
                 break;
        case 1 : nbAxisPos = m_nbPosY;
                 offsetAxis = m_offsetY;
                 break;
        case 2 : nbAxisPos = m_nbPosZ;
                 offsetAxis = m_offsetZ;
                 break;
        default : WAssert( false, "Invalid axis selected, must be between 0 and 2, including 0 and 2." );
    }
    if( result < 0 || result >= offsetAxis * ( nbAxisPos - 1 ) )
    {
        return -1;
    }
    WAssert( offsetAxis != 0.0, "The offset in axis direction has to be non-zero for all grids." );
    int integerFactor = std::floor( result / offsetAxis );
    double remainder = result - integerFactor * offsetAxis;
    double x = integerFactor + std::floor( remainder / ( offsetAxis * 0.5 ) );
    // std::cout << "Axis: factor, remainder, Voxelpos: " << axis << " " << integerFactor << " " << remainder << " " << x << std::endl;
    return x;
}


void WGridRegular3D::initInformationProperties()
{
    WPropInt xDim = m_infoProperties->addProperty( "X dimension: ", "The x dimension of the grid.", static_cast<int>( getNbCoordsX() ) );
    WPropInt yDim = m_infoProperties->addProperty( "Y dimension: ", "The y dimension of the grid.", static_cast<int>( getNbCoordsY() ) );
    WPropInt zDim = m_infoProperties->addProperty( "Z dimension: ", "The z dimension of the grid.", static_cast<int>( getNbCoordsZ() ) );
    WPropDouble xOffset = m_infoProperties->addProperty( "X offset: ", "The distance between samples in x direction", getOffsetX() );
    WPropDouble yOffset = m_infoProperties->addProperty( "Y offset: ", "The distance between samples in y direction", getOffsetY() );
    WPropDouble zOffset = m_infoProperties->addProperty( "Z offset: ", "The distance between samples in z direction", getOffsetZ() );
    std::stringstream ss1;
    ss1 << m_matrixQForm;
    WPropString qForm = m_infoProperties->addProperty( "qForm-Matrix: ", "The qForm matrix in row major", ss1.str() );
    std::stringstream ss2;
    ss2 << m_matrixQForm;
    WPropString sForm = m_infoProperties->addProperty( "sForm-Matrix: ", "The sForm matrix in row major", ss2.str() );
}

int WGridRegular3D::getXVoxelCoord( const wmath::WPosition& pos ) const
{
    return getNVoxelCoord( pos, 0 );
}

int WGridRegular3D::getYVoxelCoord( const wmath::WPosition& pos ) const
{
    return getNVoxelCoord( pos, 1 );
}

int WGridRegular3D::getZVoxelCoord( const wmath::WPosition& pos ) const
{
    return getNVoxelCoord( pos, 2 );
}

int WGridRegular3D::getXVoxelCoordRotated( const wmath::WPosition& pos ) const
{
    wmath::WVector3D v = pos - m_origin;
    v[ 2 ] = v.dotProduct( m_directionX ) / getOffsetX() / getOffsetX();
    double tmp;
    v[ 0 ] = modf( v[ 2 ] + 0.5, &tmp );
    v[1] = tmp;
    int i = static_cast< int >( v[ 2 ] >= 0.0 && v[ 2 ] < m_nbPosX - 1.0 );
    return -1 + i * static_cast< int >( 1.0 + v[ 1 ] );
}

int WGridRegular3D::getYVoxelCoordRotated( const wmath::WPosition& pos ) const
{
    wmath::WVector3D v = pos - m_origin;
    v[ 2 ] = v.dotProduct( m_directionY ) / getOffsetY() / getOffsetY();
    double tmp;
    v[ 0 ] = modf( v[ 2 ] + 0.5, &tmp );
    v[1] = tmp;
    int i = static_cast< int >( v[ 2 ] >= 0.0 && v[ 2 ] < m_nbPosY - 1.0 );
    return -1 + i * static_cast< int >( 1.0 + v[ 1 ] );
}

int WGridRegular3D::getZVoxelCoordRotated( const wmath::WPosition& pos ) const
{
    wmath::WVector3D v = pos - m_origin;
    v[ 2 ] = v.dotProduct( m_directionZ ) / getOffsetZ() / getOffsetZ();
    double tmp;
    v[ 0 ] = modf( v[ 2 ] + 0.5, &tmp );
    v[1] = tmp;
    int i = static_cast< int >( v[ 2 ] >= 0.0 && v[ 2 ] < m_nbPosZ - 1.0 );
    return -1 + i * static_cast< int >( 1.0 + v[ 1 ] );
}

wmath::WValue< int > WGridRegular3D::getVoxelCoord( const wmath::WPosition& pos ) const
{
    wmath::WValue< int > result( 3 );
    result[0] = getXVoxelCoord( pos );
    result[1] = getYVoxelCoord( pos );
    result[2] = getZVoxelCoord( pos );
    return result;
}

bool WGridRegular3D::isNotRotatedOrSheared() const
{
    bool onlyTranslatedOrScaled = true;
    for( size_t r = 0; r < 4; ++r )
    {
        for( size_t c = 0; c < 3; ++c )
        {
            if( c != r && m_matrix( r, c )!=0 )
            {
                onlyTranslatedOrScaled &= false;
            }
        }
    }
    return onlyTranslatedOrScaled;
}

size_t WGridRegular3D::getCellId( const wmath::WPosition& pos, bool* success ) const
{
    WAssert( isNotRotatedOrSheared(), "Only feasible for grids that are only translated or scaled so far." );

    wmath::WPosition posRelativeToOrigin = pos - m_origin;

    double xCellId = floor( posRelativeToOrigin[0] / m_offsetX );
    double yCellId = floor( posRelativeToOrigin[1] / m_offsetY );
    double zCellId = floor( posRelativeToOrigin[2] / m_offsetZ );

    *success = xCellId >= 0 && yCellId >=0 && zCellId >= 0 && xCellId < m_nbPosX - 1 && yCellId < m_nbPosY -1 && zCellId < m_nbPosZ -1;

    return xCellId + yCellId * ( m_nbPosX - 1 ) + zCellId * ( m_nbPosX - 1 ) * ( m_nbPosY - 1 );
}

std::vector< size_t > WGridRegular3D::getCellVertexIds( size_t cellId ) const
{
    std::vector< size_t > vertices( 8 );
    size_t minVertexIdZ =  cellId / ( ( m_nbPosX - 1 ) * ( m_nbPosY - 1 ) );
    size_t remainderXY = cellId - minVertexIdZ * ( ( m_nbPosX - 1 ) * ( m_nbPosY - 1 ) );
    size_t minVertexIdY = remainderXY  / ( m_nbPosX - 1 );
    size_t minVertexIdX = remainderXY % ( m_nbPosX - 1 );

    size_t minVertexId = minVertexIdX + minVertexIdY * m_nbPosX + minVertexIdZ * m_nbPosX * m_nbPosY;

    vertices[0] = minVertexId;
    vertices[1] = minVertexId + 1;
    vertices[2] = minVertexId + m_nbPosX;
    vertices[3] = minVertexId + m_nbPosX +1;
    vertices[4] = minVertexId + m_nbPosX * m_nbPosY;
    vertices[5] = minVertexId + m_nbPosX * m_nbPosY + 1;
    vertices[6] = minVertexId + m_nbPosX * m_nbPosY + m_nbPosX;
    vertices[7] = minVertexId + m_nbPosX * m_nbPosY + m_nbPosX +1;
    return vertices;
}

boost::shared_ptr< std::vector< wmath::WPosition > > WGridRegular3D::getVoxelVertices( const wmath::WPosition& point, const double margin ) const
{
    typedef boost::shared_ptr< std::vector< wmath::WPosition > > ReturnType;
    ReturnType result = ReturnType( new std::vector< wmath::WPosition > );
    result->reserve( 8 );
    double halfMarginX = m_offsetX / 2.0 - std::abs( margin );
    double halfMarginY = m_offsetY / 2.0 - std::abs( margin );
    double halfMarginZ = m_offsetZ / 2.0 - std::abs( margin );
    result->push_back( wmath::WPosition( point[0] - halfMarginX, point[1] - halfMarginY, point[2] - halfMarginZ ) ); // a
    result->push_back( wmath::WPosition( point[0] + halfMarginX, point[1] - halfMarginY, point[2] - halfMarginZ ) ); // b
    result->push_back( wmath::WPosition( point[0] + halfMarginX, point[1] - halfMarginY, point[2] + halfMarginZ ) ); // c
    result->push_back( wmath::WPosition( point[0] - halfMarginX, point[1] - halfMarginY, point[2] + halfMarginZ ) ); // d
    result->push_back( wmath::WPosition( point[0] - halfMarginX, point[1] + halfMarginY, point[2] - halfMarginZ ) ); // e
    result->push_back( wmath::WPosition( point[0] + halfMarginX, point[1] + halfMarginY, point[2] - halfMarginZ ) ); // f
    result->push_back( wmath::WPosition( point[0] + halfMarginX, point[1] + halfMarginY, point[2] + halfMarginZ ) ); // g
    result->push_back( wmath::WPosition( point[0] - halfMarginX, point[1] + halfMarginY, point[2] + halfMarginZ ) ); // h
    return result;
}

std::vector< size_t > WGridRegular3D::getNeighbours( size_t id ) const
{
    std::vector< size_t > neighbours;
    size_t x = id % m_nbPosX;
    size_t y = ( id / m_nbPosX ) % m_nbPosY;
    size_t z = id / ( m_nbPosX * m_nbPosY );

    if( x >= m_nbPosX || y >= m_nbPosY || z >= m_nbPosZ )
    {
        std::stringstream ss;
        ss << "This point: " << id << " is not part of this grid: ";
        ss << " nbPosX: " << m_nbPosX;
        ss << " nbPosY: " << m_nbPosY;
        ss << " nbPosZ: " << m_nbPosZ;
        throw WOutOfBounds( ss.str() );
    }
    // for every neighbour we must check if its not on the boundary, it will be skipped otherwise
    if( x > 0 )
    {
        neighbours.push_back( id - 1 );
    }
    if( x < m_nbPosX - 1 )
    {
        neighbours.push_back( id + 1 );
    }
    if( y > 0 )
    {
        neighbours.push_back( id - m_nbPosX );
    }
    if( y < m_nbPosY - 1 )
    {
        neighbours.push_back( id + m_nbPosX );
    }
    if( z > 0 )
    {
        neighbours.push_back( id - ( m_nbPosX * m_nbPosY ) );
    }
    if( z < m_nbPosZ - 1 )
    {
         neighbours.push_back( id + ( m_nbPosX * m_nbPosY ) );
    }
    return neighbours;
}

std::vector< size_t > WGridRegular3D::getNeighbours27( size_t id ) const
{
    std::vector< size_t > neighbours;
    size_t x = id % m_nbPosX;
    size_t y = ( id / m_nbPosX ) % m_nbPosY;
    size_t z = id / ( m_nbPosX * m_nbPosY );

    if( x >= m_nbPosX || y >= m_nbPosY || z >= m_nbPosZ )
    {
        std::stringstream ss;
        ss << "This point: " << id << " is not part of this grid: ";
        ss << " nbPosX: " << m_nbPosX;
        ss << " nbPosY: " << m_nbPosY;
        ss << " nbPosZ: " << m_nbPosZ;
        throw WOutOfBounds( ss.str() );
    }
    // for every neighbour we must check if its not on the boundary, it will be skipped otherwise
    std::vector< int >tempResult;

    tempResult.push_back( getVoxelNum( x    , y    , z ) );
    tempResult.push_back( getVoxelNum( x    , y - 1, z ) );
    tempResult.push_back( getVoxelNum( x    , y + 1, z ) );
    tempResult.push_back( getVoxelNum( x - 1, y    , z ) );
    tempResult.push_back( getVoxelNum( x - 1, y - 1, z ) );
    tempResult.push_back( getVoxelNum( x - 1, y + 1, z ) );
    tempResult.push_back( getVoxelNum( x + 1, y    , z ) );
    tempResult.push_back( getVoxelNum( x + 1, y - 1, z ) );
    tempResult.push_back( getVoxelNum( x + 1, y + 1, z ) );

    tempResult.push_back( getVoxelNum( x    , y    , z - 1 ) );
    tempResult.push_back( getVoxelNum( x    , y - 1, z - 1 ) );
    tempResult.push_back( getVoxelNum( x    , y + 1, z - 1 ) );
    tempResult.push_back( getVoxelNum( x - 1, y    , z - 1 ) );
    tempResult.push_back( getVoxelNum( x - 1, y - 1, z - 1 ) );
    tempResult.push_back( getVoxelNum( x - 1, y + 1, z - 1 ) );
    tempResult.push_back( getVoxelNum( x + 1, y    , z - 1 ) );
    tempResult.push_back( getVoxelNum( x + 1, y - 1, z - 1 ) );
    tempResult.push_back( getVoxelNum( x + 1, y + 1, z - 1 ) );

    tempResult.push_back( getVoxelNum( x    , y    , z + 1 ) );
    tempResult.push_back( getVoxelNum( x    , y - 1, z + 1 ) );
    tempResult.push_back( getVoxelNum( x    , y + 1, z + 1 ) );
    tempResult.push_back( getVoxelNum( x - 1, y    , z + 1 ) );
    tempResult.push_back( getVoxelNum( x - 1, y - 1, z + 1 ) );
    tempResult.push_back( getVoxelNum( x - 1, y + 1, z + 1 ) );
    tempResult.push_back( getVoxelNum( x + 1, y    , z + 1 ) );
    tempResult.push_back( getVoxelNum( x + 1, y - 1, z + 1 ) );
    tempResult.push_back( getVoxelNum( x + 1, y + 1, z + 1 ) );

    for ( size_t k = 0; k < tempResult.size(); ++k )
    {
        if ( tempResult[k] != -1 )
        {
            neighbours.push_back( tempResult[k] );
        }
    }
    return neighbours;
}

std::vector< size_t > WGridRegular3D::getNeighbours9XY( size_t id ) const
{
    std::vector< size_t > neighbours;
    size_t x = id % m_nbPosX;
    size_t y = ( id / m_nbPosX ) % m_nbPosY;
    size_t z = id / ( m_nbPosX * m_nbPosY );

    if( x >= m_nbPosX || y >= m_nbPosY || z >= m_nbPosZ )
    {
        std::stringstream ss;
        ss << "This point: " << id << " is not part of this grid: ";
        ss << " nbPosX: " << m_nbPosX;
        ss << " nbPosY: " << m_nbPosY;
        ss << " nbPosZ: " << m_nbPosZ;
        throw WOutOfBounds( ss.str() );
    }
    // boundary check now happens in the getVoxelNum function
    int vNum;

    vNum = getVoxelNum( x - 1, y, z );
    if ( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x - 1, y - 1, z );
    if ( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x, y - 1, z );
    if ( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x + 1, y - 1, z );
    if ( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x + 1, y, z );
    if ( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x + 1, y + 1, z );
    if ( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x, y + 1, z );
    if ( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x - 1, y + 1, z );
    if ( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    return neighbours;
}

std::vector< size_t > WGridRegular3D::getNeighbours9YZ( size_t id ) const
{
    std::vector< size_t > neighbours;
    size_t x = id % m_nbPosX;
    size_t y = ( id / m_nbPosX ) % m_nbPosY;
    size_t z = id / ( m_nbPosX * m_nbPosY );

    if( x >= m_nbPosX || y >= m_nbPosY || z >= m_nbPosZ )
    {
        std::stringstream ss;
        ss << "This point: " << id << " is not part of this grid: ";
        ss << " nbPosX: " << m_nbPosX;
        ss << " nbPosY: " << m_nbPosY;
        ss << " nbPosZ: " << m_nbPosZ;
        throw WOutOfBounds( ss.str() );
    }
    // boundary check now happens in the getVoxelNum function
    int vNum;

    vNum = getVoxelNum( x, y, z - 1 );
    if ( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x, y - 1, z - 1 );
    if ( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x, y - 1, z );
    if ( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x, y - 1, z + 1 );
    if ( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x, y, z + 1 );
    if ( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x, y + 1, z + 1 );
    if ( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x, y + 1, z );
    if ( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x, y + 1, z - 1 );
    if ( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }

    return neighbours;
}

std::vector< size_t > WGridRegular3D::getNeighbours9XZ( size_t id ) const
{
    std::vector< size_t > neighbours;
    size_t x = id % m_nbPosX;
    size_t y = ( id / m_nbPosX ) % m_nbPosY;
    size_t z = id / ( m_nbPosX * m_nbPosY );

    if( x >= m_nbPosX || y >= m_nbPosY || z >= m_nbPosZ )
    {
        std::stringstream ss;
        ss << "This point: " << id << " is not part of this grid: ";
        ss << " nbPosX: " << m_nbPosX;
        ss << " nbPosY: " << m_nbPosY;
        ss << " nbPosZ: " << m_nbPosZ;
        throw WOutOfBounds( ss.str() );
    }
    // boundary check now happens in the getVoxelNum function
    int vNum;

    vNum = getVoxelNum( x, y, z - 1 );
    if ( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x - 1, y, z - 1 );
    if ( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x - 1, y, z );
    if ( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x - 1, y, z + 1 );
    if ( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x, y, z + 1 );
    if ( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x + 1, y, z + 1 );
    if ( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x + 1, y, z );
    if ( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x + 1, y, z - 1 );
    if ( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }

    return neighbours;
}

bool WGridRegular3D::encloses( const wmath::WPosition& pos ) const
{
    return getVoxelNum( pos ) != -1; // note this is an integer comparision, hence it should be numerical stable!
}

bool WGridRegular3D::enclosesRotated( wmath::WPosition const& pos ) const
{
    wmath::WVector3D v = pos - m_origin;
    double d = v.dotProduct( m_directionX ) / getOffsetX() / getOffsetX();
    if( d < 0.0 || d > m_nbPosX - 1 )
    {
        return false;
    }
    d = v.dotProduct( m_directionY ) / getOffsetY() / getOffsetY();
    if( d < 0.0 || d > m_nbPosY - 1 )
    {
        return false;
    }
    d = v.dotProduct( m_directionZ ) / getOffsetZ() / getOffsetZ();
    if( d < 0.0 || d > m_nbPosZ - 1 )
    {
        return false;
    }
    return true;
}

WBoundingBox WGridRegular3D::getBoundingBox() const
{
    WBoundingBox result;
    result.expandBy( transformPosition3DWithMatrix4D( m_matrix, wmath::WPosition( 0.0,                0.0,                0.0            ) ) );
    result.expandBy( transformPosition3DWithMatrix4D( m_matrix, wmath::WPosition( getNbCoordsX() - 1, 0.0,                0.0            ) ) );
    result.expandBy( transformPosition3DWithMatrix4D( m_matrix, wmath::WPosition( 0.0,                getNbCoordsY() - 1, 0.0            ) ) );
    result.expandBy( transformPosition3DWithMatrix4D( m_matrix, wmath::WPosition( getNbCoordsX() - 1, getNbCoordsY() - 1, 0.0            ) ) );
    result.expandBy( transformPosition3DWithMatrix4D( m_matrix, wmath::WPosition( 0.0,                0.0,                getNbCoordsZ() - 1 ) ) );
    result.expandBy( transformPosition3DWithMatrix4D( m_matrix, wmath::WPosition( getNbCoordsX() - 1, 0.0,                getNbCoordsZ() - 1 ) ) );
    result.expandBy( transformPosition3DWithMatrix4D( m_matrix, wmath::WPosition( 0.0,                getNbCoordsY() - 1, getNbCoordsZ() - 1 ) ) );
    result.expandBy( transformPosition3DWithMatrix4D( m_matrix, wmath::WPosition( getNbCoordsX() - 1, getNbCoordsY() - 1, getNbCoordsZ() - 1 ) ) );

    return result;
}

void WGridRegular3D::translate( wmath::WPosition translate )
{
    switch ( m_matrixActive )
    {
        case 0:
            m_matrix = m_matrixNoMatrix;
            break;
        case 1:
            m_matrix = m_matrixQForm;
            //std::cout << "qform" << std::endl;
            break;
        case 2:
            m_matrix = m_matrixSForm;
            //std::cout << "sform" << std::endl;
            break;
        default:
            m_matrix = m_matrixNoMatrix;
    }
    m_translateMatrix( 0, 3 ) = translate[0];
    m_translateMatrix( 1, 3 ) = translate[1];
    m_translateMatrix( 2, 3 ) = translate[2];

    m_matrix = m_matrix * m_translateMatrix;
    m_matrix = m_matrix * m_stretchMatrix;
    m_matrix = m_matrix * m_rotMatrix;

    recreateTextureTransformationMatrices();
}


void WGridRegular3D::stretch( wmath::WPosition str )
{
    m_offsetX = m_offsetXorig * str[0];
    m_offsetY = m_offsetYorig * str[1];
    m_offsetZ = m_offsetZorig * str[2];

    m_stretchMatrix( 0, 0 ) = str[0];
    m_stretchMatrix( 1, 1 ) = str[1];
    m_stretchMatrix( 2, 2 ) = str[2];

    switch ( m_matrixActive )
    {
        case 0:
            m_matrix = m_matrixNoMatrix;
            break;
        case 1:
            m_matrix = m_matrixQForm;
            //std::cout << "qform" << std::endl;
            break;
        case 2:
            m_matrix = m_matrixSForm;
            //std::cout << "sform" << std::endl;
            break;
        default:
            m_matrix = m_matrixNoMatrix;
    }

    m_matrix = m_matrix * m_translateMatrix;
    m_matrix = m_matrix * m_stretchMatrix;
    m_matrix = m_matrix * m_rotMatrix;

    recreateTextureTransformationMatrices();
}

void WGridRegular3D::rotate( osg::Matrixf osgrot, wmath::WPosition center )
{
    switch ( m_matrixActive )
    {
        case 0:
            m_matrix = m_matrixNoMatrix;
            break;
        case 1:
            m_matrix = m_matrixQForm;
            //std::cout << "qform" << std::endl;
            break;
        case 2:
            m_matrix = m_matrixSForm;
            //std::cout << "sform" << std::endl;
            break;
        default:
            m_matrix = m_matrixNoMatrix;
    }

    WMatrix4x4 rotmat;
    rotmat.makeIdentity();

    WMatrix4x4 transTo;
    transTo.makeIdentity();

    WMatrix4x4 transFrom;
    transFrom.makeIdentity();

    transTo( 0, 3 ) = -center.x();
    transTo( 1, 3 ) = -center.y();
    transTo( 2, 3 ) = -center.z();

    transFrom( 0, 3 ) = center.x();
    transFrom( 1, 3 ) = center.y();
    transFrom( 2, 3 ) = center.z();

    rotmat( 0, 0 ) = osgrot( 0, 0 );
    rotmat( 1, 0 ) = osgrot( 1, 0 );
    rotmat( 2, 0 ) = osgrot( 2, 0 );

    rotmat( 0, 1 ) = osgrot( 0, 1 );
    rotmat( 1, 1 ) = osgrot( 1, 1 );
    rotmat( 2, 1 ) = osgrot( 2, 1 );

    rotmat( 0, 2 ) = osgrot( 0, 2 );
    rotmat( 1, 2 ) = osgrot( 1, 2 );
    rotmat( 2, 2 ) = osgrot( 2, 2 );

    m_rotMatrix = m_rotMatrix * transFrom;
    m_rotMatrix = m_rotMatrix * rotmat;
    m_rotMatrix = m_rotMatrix * transTo;

    m_matrix = m_matrix * m_translateMatrix;
    m_matrix = m_matrix * m_stretchMatrix;
    m_matrix = m_matrix * m_rotMatrix;

    recreateTextureTransformationMatrices();
}

void WGridRegular3D::setActiveMatrix( int matrix )
{
    m_matrixActive = matrix;

    switch ( m_matrixActive )
    {
        case 0:
            m_matrix = m_matrixNoMatrix;
            break;
        case 1:
            m_matrix = m_matrixQForm;
            //std::cout << "qform" << std::endl;
            break;
        case 2:
            m_matrix = m_matrixSForm;
            //std::cout << "sform" << std::endl;
            break;
        default:
            m_matrix = m_matrixNoMatrix;
    }
    recreateTextureTransformationMatrices();

    m_directionX = WVector3D( m_matrix( 0, 0 ) / m_matrix( 3, 3 ), m_matrix( 1, 0 ) / m_matrix( 3, 3 ), m_matrix( 2, 0 ) / m_matrix( 3, 3 ) );
    m_directionY = WVector3D( m_matrix( 0, 1 ) / m_matrix( 3, 3 ), m_matrix( 1, 1 ) / m_matrix( 3, 3 ), m_matrix( 2, 1 ) / m_matrix( 3, 3 ) );
    m_directionZ = WVector3D( m_matrix( 0, 2 ) / m_matrix( 3, 3 ), m_matrix( 1, 2 ) / m_matrix( 3, 3 ), m_matrix( 2, 2 ) / m_matrix( 3, 3 ) );

    m_origin = WPosition( m_matrix( 0, 3 ) / m_matrix( 3, 3 ),
                          m_matrix( 1, 3 ) / m_matrix( 3, 3 ),
                          m_matrix( 2, 3 ) / m_matrix( 3, 3 ) );
}

int WGridRegular3D::getActiveMatrix()
{
    return m_matrixActive;
}

wmath::WPosition WGridRegular3D::getTranslate()
{
    wmath::WPosition translate( m_translateMatrix( 0, 3 ), m_translateMatrix( 1, 3 ), m_translateMatrix( 2, 3 ) );
    return translate;
}

WCondition::SPtr WGridRegular3D::getTransformationUpdateCondition() const
{
    return m_transformationUpdateCondition;
}

