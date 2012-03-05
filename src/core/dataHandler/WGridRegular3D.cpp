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
#include <string>
#include <vector>

#include "../common/exceptions/WOutOfBounds.h"
#include "../common/exceptions/WPreconditionNotMet.h"
#include "../common/math/WLinearAlgebraFunctions.h"
#include "../common/WBoundingBox.h"
#include "../common/WProperties.h"
#include "../common/math/linearAlgebra/WLinearAlgebra.h"

#include "WGridRegular3D.h"

WGridRegular3D::WGridRegular3D( unsigned int nbPosX, unsigned int nbPosY, unsigned int nbPosZ,
                                WGridTransformOrtho const transform )
    : WGrid( nbPosX * nbPosY * nbPosZ ),
      m_nbPosX( nbPosX ),
      m_nbPosY( nbPosY ),
      m_nbPosZ( nbPosZ ),
      m_transform( transform )
{
    initInformationProperties();
}

WPosition WGridRegular3D::getPosition( unsigned int i ) const
{
    return getPosition( i % m_nbPosX, ( i / m_nbPosX ) % m_nbPosY, i / ( m_nbPosX * m_nbPosY ) );
}

WPosition WGridRegular3D::getPosition( unsigned int iX, unsigned int iY, unsigned int iZ ) const
{
    WVector3d i( iX, iY, iZ );
    return m_transform.positionToWorldSpace( i );
}

WVector3d WGridRegular3D::worldCoordToTexCoord( WPosition point )
{
    WVector3d r( m_transform.positionToGridSpace( point ) );

    // Scale to [0,1]
    r[0] = r[0] / m_nbPosX;
    r[1] = r[1] / m_nbPosY;
    r[2] = r[2] / m_nbPosZ;

    // Correct the coordinates to have the position at the center of the texture voxel.
    r[0] += 0.5 / m_nbPosX;
    r[1] += 0.5 / m_nbPosY;
    r[2] += 0.5 / m_nbPosZ;

    return r;
}

int WGridRegular3D::getVoxelNum( const WPosition& pos ) const
{
    // Note: the reason for the +1 is that the first and last Voxel in a x-axis
    // row are cut.
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
    if( x > m_nbPosX || y > m_nbPosY || z > m_nbPosZ )
    {
        return -1;
    }
    return x + y * ( m_nbPosX ) + z * ( m_nbPosX ) * ( m_nbPosY );
}


void WGridRegular3D::initInformationProperties()
{
    WPropInt xDim = m_infoProperties->addProperty( "X dimension: ", "The x dimension of the grid.", static_cast<int>( getNbCoordsX() ) );
    WPropInt yDim = m_infoProperties->addProperty( "Y dimension: ", "The y dimension of the grid.", static_cast<int>( getNbCoordsY() ) );
    WPropInt zDim = m_infoProperties->addProperty( "Z dimension: ", "The z dimension of the grid.", static_cast<int>( getNbCoordsZ() ) );
    WPropDouble xOffset = m_infoProperties->addProperty( "X offset: ", "The distance between samples in x direction", getOffsetX() );
    WPropDouble yOffset = m_infoProperties->addProperty( "Y offset: ", "The distance between samples in y direction", getOffsetY() );
    WPropDouble zOffset = m_infoProperties->addProperty( "Z offset: ", "The distance between samples in z direction", getOffsetZ() );
    WPropMatrix4X4 transformation = m_infoProperties->addProperty( "Transformation", "The transformation of this grid.",
        static_cast< WMatrix4d >( getTransform() ) );
}

int WGridRegular3D::getXVoxelCoord( const WPosition& pos ) const
{
    // the current get*Voxel stuff is too complicated anyway
    WPosition v = m_transform.positionToGridSpace( pos );

    // this part could be refactored into an inline function
    double d;
    v[ 2 ] = modf( v[ 0 ] + 0.5, &d );
    int i = static_cast< int >( v[ 0 ] >= 0.0 && v[ 0 ] < m_nbPosX - 1.0 );
    return -1 + i * static_cast< int >( 1.0 + d );
}

int WGridRegular3D::getYVoxelCoord( const WPosition& pos ) const
{
    WPosition v = m_transform.positionToGridSpace( pos );

    double d;
    v[ 0 ] = modf( v[ 1 ] + 0.5, &d );
    int i = static_cast< int >( v[ 1 ] >= 0.0 && v[ 1 ] < m_nbPosY - 1.0 );
    return -1 + i * static_cast< int >( 1.0 + d );
}

int WGridRegular3D::getZVoxelCoord( const WPosition& pos ) const
{
    WPosition v = m_transform.positionToGridSpace( pos );

    double d;
    v[ 0 ] = modf( v[ 2 ] + 0.5, &d );
    int i = static_cast< int >( v[ 2 ] >= 0.0 && v[ 2 ] < m_nbPosZ - 1.0 );
    return -1 + i * static_cast< int >( 1.0 + d );
}

WVector3i WGridRegular3D::getVoxelCoord( const WPosition& pos ) const
{
    WVector3i result;
    result[0] = getXVoxelCoord( pos );
    result[1] = getYVoxelCoord( pos );
    result[2] = getZVoxelCoord( pos );
    return result;
}

bool WGridRegular3D::isNotRotated() const
{
    return m_transform.isNotRotated();
}

size_t WGridRegular3D::getCellId( const WPosition& pos, bool* success ) const
{
    WAssert( isNotRotated(), "Only feasible for grids that are only translated or scaled so far." );

    WPosition posRelativeToOrigin = pos - getOrigin();

    double xCellId = floor( posRelativeToOrigin[0] / getOffsetX() );
    double yCellId = floor( posRelativeToOrigin[1] / getOffsetY() );
    double zCellId = floor( posRelativeToOrigin[2] / getOffsetZ() );

    *success = xCellId >= 0 && yCellId >=0 && zCellId >= 0 && xCellId < m_nbPosX - 1 && yCellId < m_nbPosY -1 && zCellId < m_nbPosZ -1;

    return xCellId + yCellId * ( m_nbPosX - 1 ) + zCellId * ( m_nbPosX - 1 ) * ( m_nbPosY - 1 );
}

WGridRegular3D::CellVertexArray WGridRegular3D::getCellVertexIds( size_t cellId ) const
{
    WGridRegular3D::CellVertexArray vertices;
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

boost::shared_ptr< std::vector< WPosition > > WGridRegular3D::getVoxelVertices( const WPosition& point, const double margin ) const
{
    typedef boost::shared_ptr< std::vector< WPosition > > ReturnType;
    ReturnType result = ReturnType( new std::vector< WPosition > );
    result->reserve( 8 );
    double halfMarginX = getOffsetX() / 2.0 - std::abs( margin );
    double halfMarginY = getOffsetY() / 2.0 - std::abs( margin );
    double halfMarginZ = getOffsetZ() / 2.0 - std::abs( margin );
    result->push_back( WPosition( point[0] - halfMarginX, point[1] - halfMarginY, point[2] - halfMarginZ ) ); // a
    result->push_back( WPosition( point[0] + halfMarginX, point[1] - halfMarginY, point[2] - halfMarginZ ) ); // b
    result->push_back( WPosition( point[0] + halfMarginX, point[1] - halfMarginY, point[2] + halfMarginZ ) ); // c
    result->push_back( WPosition( point[0] - halfMarginX, point[1] - halfMarginY, point[2] + halfMarginZ ) ); // d
    result->push_back( WPosition( point[0] - halfMarginX, point[1] + halfMarginY, point[2] - halfMarginZ ) ); // e
    result->push_back( WPosition( point[0] + halfMarginX, point[1] + halfMarginY, point[2] - halfMarginZ ) ); // f
    result->push_back( WPosition( point[0] + halfMarginX, point[1] + halfMarginY, point[2] + halfMarginZ ) ); // g
    result->push_back( WPosition( point[0] - halfMarginX, point[1] + halfMarginY, point[2] + halfMarginZ ) ); // h
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

    for( size_t k = 0; k < tempResult.size(); ++k )
    {
        if( tempResult[k] != -1 )
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
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x - 1, y - 1, z );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x, y - 1, z );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x + 1, y - 1, z );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x + 1, y, z );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x + 1, y + 1, z );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x, y + 1, z );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x - 1, y + 1, z );
    if( vNum != -1 )
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
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x, y - 1, z - 1 );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x, y - 1, z );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x, y - 1, z + 1 );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x, y, z + 1 );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x, y + 1, z + 1 );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x, y + 1, z );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x, y + 1, z - 1 );
    if( vNum != -1 )
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
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x - 1, y, z - 1 );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x - 1, y, z );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x - 1, y, z + 1 );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x, y, z + 1 );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x + 1, y, z + 1 );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x + 1, y, z );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }
    vNum = getVoxelNum( x + 1, y, z - 1 );
    if( vNum != -1 )
    {
        neighbours.push_back( vNum );
    }

    return neighbours;
}

bool WGridRegular3D::encloses( WPosition const& pos ) const
{
    WVector3d v = m_transform.positionToGridSpace( pos );

    if( v[ 0 ] < 0.0 || v[ 0 ] >= m_nbPosX - 1 )
    {
        return false;
    }
    if( v[ 1 ] < 0.0 || v[ 1 ] >= m_nbPosY - 1 )
    {
        return false;
    }
    if( v[ 2 ] < 0.0 || v[ 2 ] >= m_nbPosZ - 1 )
    {
        return false;
    }
    return true;
}

WBoundingBox WGridRegular3D::getBoundingBox() const
{
    WBoundingBox result;
    result.expandBy( m_transform.positionToWorldSpace( WPosition( 0.0,                0.0,                0.0            ) ) );
    result.expandBy( m_transform.positionToWorldSpace( WPosition( getNbCoordsX() - 1, 0.0,                0.0            ) ) );
    result.expandBy( m_transform.positionToWorldSpace( WPosition( 0.0,                getNbCoordsY() - 1, 0.0            ) ) );
    result.expandBy( m_transform.positionToWorldSpace( WPosition( getNbCoordsX() - 1, getNbCoordsY() - 1, 0.0            ) ) );
    result.expandBy( m_transform.positionToWorldSpace( WPosition( 0.0,                0.0,                getNbCoordsZ() - 1 ) ) );
    result.expandBy( m_transform.positionToWorldSpace( WPosition( getNbCoordsX() - 1, 0.0,                getNbCoordsZ() - 1 ) ) );
    result.expandBy( m_transform.positionToWorldSpace( WPosition( 0.0,                getNbCoordsY() - 1, getNbCoordsZ() - 1 ) ) );
    result.expandBy( m_transform.positionToWorldSpace( WPosition( getNbCoordsX() - 1, getNbCoordsY() - 1, getNbCoordsZ() - 1 ) ) );
    return result;
}
