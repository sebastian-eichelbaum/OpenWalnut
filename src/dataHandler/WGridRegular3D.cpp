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
#include "../common/exceptions/WOutOfBounds.h"
#include "../common/math/WLinearAlgebraFunctions.h"
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

    m_matrixInverse = wmath::invertMatrix4x4( m_matrix );
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
    WAssert( mat.getNbRows() == 4 && mat.getNbCols() == 4, "Transformation matrix has wrong dimensions." );
    // only affine transformations are allowed
    WAssert( mat( 3, 0 ) == 0.0 && mat( 3, 1 ) == 0.0 && mat( 3, 2 ) == 0.0, "Transf. matrix has to have no projection part." );

    m_origin = WPosition( mat( 0, 3 ) / mat( 3, 3 ), mat( 1, 3 ) / mat( 3, 3 ), mat( 2, 3 ) / mat( 3, 3 ) );

    m_directionX = WVector3D( mat( 0, 0 ) / mat( 3, 3 ), mat( 1, 0 ) / mat( 3, 3 ), mat( 2, 0 ) / mat( 3, 3 ) );
    m_directionY = WVector3D( mat( 0, 1 ) / mat( 3, 3 ), mat( 1, 1 ) / mat( 3, 3 ), mat( 2, 1 ) / mat( 3, 3 ) );
    m_directionZ = WVector3D( mat( 0, 2 ) / mat( 3, 3 ), mat( 1, 2 ) / mat( 3, 3 ), mat( 2, 2 ) / mat( 3, 3 ) );

    m_matrix = mat;

    m_matrixInverse = wmath::invertMatrix4x4( m_matrix );
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

    m_matrixInverse = wmath::invertMatrix4x4( m_matrix );
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
      m_matrix( 4, 4 ),
      m_matrixInverse( 3, 3 )
{
    m_matrix( 0, 0 ) = offsetX;
    m_matrix( 1, 1 ) = offsetY;
    m_matrix( 2, 2 ) = offsetZ;
    m_matrix( 0, 3 ) = origin[0];
    m_matrix( 1, 3 ) = origin[1];
    m_matrix( 2, 3 ) = origin[2];

    m_matrix( 3, 3 ) = 1.;

    m_matrixInverse = wmath::invertMatrix4x4( m_matrix );
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

    m_matrixInverse = wmath::invertMatrix4x4( m_matrix );
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
    return m_matrix;
}

wmath::WVector3D WGridRegular3D::worldCoordToTexCoord( wmath::WPosition point )
{
    wmath::WVector3D r( wmath::transformPosition3DWithMatrix4D( m_matrixInverse, point ) );

    r[0] = r[0] / m_nbPosX;
    r[1] = r[1] / m_nbPosY;
    r[2] = r[2] / m_nbPosZ;

    return r;
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
    if( result < 0 || result > offsetAxis * ( nbAxisPos - 1 ) )
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

size_t WGridRegular3D::getCellId( const wmath::WPosition& pos ) const
{
    WAssert( isNotRotatedOrSheared(), "Only feasible for grids that are only translated or scaled so far." );

    wmath::WPosition posRelativeToOrigin = pos - m_origin;

    size_t xCellId = floor( posRelativeToOrigin[0] / m_offsetX );
    size_t yCellId = floor( posRelativeToOrigin[1] / m_offsetY );
    size_t zCellId = floor( posRelativeToOrigin[2] / m_offsetZ );

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

bool WGridRegular3D::encloses( const wmath::WPosition& pos ) const
{
    return getVoxelNum( pos ) != -1; // note this is an integer comparision, hence it should be numerical stable!
}

std::pair< wmath::WPosition, wmath::WPosition > WGridRegular3D::getBoundingBox() const
{
    // Get the transformed corner points of the regular grid
    std::vector< wmath::WPosition > cornerPs;
    cornerPs.push_back( transformPosition3DWithMatrix4D( m_matrix, wmath::WPosition( 0.0,                0.0,                0.0                ) ) );
    cornerPs.push_back( transformPosition3DWithMatrix4D( m_matrix, wmath::WPosition( getNbCoordsX() - 1, 0.0,                0.0                ) ) );
    cornerPs.push_back( transformPosition3DWithMatrix4D( m_matrix, wmath::WPosition( 0.0,                getNbCoordsY() - 1, 0.0                ) ) );
    cornerPs.push_back( transformPosition3DWithMatrix4D( m_matrix, wmath::WPosition( getNbCoordsX() - 1, getNbCoordsY() - 1, 0.0                ) ) );
    cornerPs.push_back( transformPosition3DWithMatrix4D( m_matrix, wmath::WPosition( 0.0,                0.0,                getNbCoordsZ() - 1 ) ) );
    cornerPs.push_back( transformPosition3DWithMatrix4D( m_matrix, wmath::WPosition( getNbCoordsX() - 1, 0.0,                getNbCoordsZ() - 1 ) ) );
    cornerPs.push_back( transformPosition3DWithMatrix4D( m_matrix, wmath::WPosition( 0.0,                getNbCoordsY() - 1, getNbCoordsZ() - 1 ) ) );
    cornerPs.push_back( transformPosition3DWithMatrix4D( m_matrix, wmath::WPosition( getNbCoordsX() - 1, getNbCoordsY() - 1, getNbCoordsZ() - 1 ) ) );

    wmath::WPosition minBB( wlimits::MAX_DOUBLE, wlimits::MAX_DOUBLE, wlimits::MAX_DOUBLE );
    wmath::WPosition maxBB( wlimits::MIN_DOUBLE, wlimits::MIN_DOUBLE, wlimits::MIN_DOUBLE );

    // Check the components of the corner points separately against the components of the current maxBB and minBB
    for( size_t posId = 0; posId < cornerPs.size(); ++posId)
    {
        for( size_t compId = 0; compId < 3; ++compId )
        {
            minBB[compId] = cornerPs[posId][compId] < minBB[compId] ? cornerPs[posId][compId] : minBB[compId];
            maxBB[compId] = cornerPs[posId][compId] > maxBB[compId] ? cornerPs[posId][compId] : maxBB[compId];
        }
    }

    return std::make_pair( minBB, maxBB );
}
