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
