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

#include <vector>
#include <map>

#include "WGeometryFunctions.h"

void tesselateIcosahedron( std::vector< WVector3d >* vertices, std::vector< unsigned int >* triangles, unsigned int level )
{
    WAssert( vertices, "Missing input vector." );
    WAssert( triangles, "Missing input vector." );
    vertices->clear();
    triangles->clear();

    unsigned int nv = 12;
    unsigned int nt = 20;
    for( unsigned int t = 1; t <= level; ++t )
    {
        nv += 3 * nt / 2;
        nt *= 4;
    }
    vertices->reserve( nv );
    triangles->reserve( nt );

    // add icosahedron vertices
    double phi = 0.5 * ( 1.0 + sqrt( 5.0 ) );

    std::vector< WVector3d > g;

    vertices->push_back( WVector3d( phi, 1, 0 ) ); // 0
    vertices->push_back( WVector3d( -phi, 1, 0 ) ); // 1
    vertices->push_back( WVector3d( phi, -1, 0 ) ); // 2
    vertices->push_back( WVector3d( -phi, -1, 0 ) ); // 3

    vertices->push_back( WVector3d( 1, 0, phi ) ); // 4
    vertices->push_back( WVector3d( -1, 0, phi ) ); // 5
    vertices->push_back( WVector3d( 1, 0, -phi ) ); // 6
    vertices->push_back( WVector3d( -1, 0, -phi ) ); // 7

    vertices->push_back( WVector3d( 0, phi, 1 ) ); // 8
    vertices->push_back( WVector3d( 0, -phi, 1 ) ); // 9
    vertices->push_back( WVector3d( 0, phi, -1 ) ); // 10
    vertices->push_back( WVector3d( 0, -phi, -1 ) ); // 11

    for( std::vector< WVector3d >::iterator it = vertices->begin(); it != vertices->end(); ++it )
    {
        *it = normalize( *it );
    }

    // add triangle indices
    unsigned int inc[ 60 ] =
    {
        8, 5, 4,
        8, 4, 0,
        8, 0, 10,
        8, 10, 1,
        8, 1, 5,
        5, 9, 4,
        4, 2, 0,
        0, 6, 10,
        10, 7, 1,
        1, 3, 5,
        4, 9, 2,
        0, 2, 6,
        10, 6, 7,
        1, 7, 3,
        5, 3, 9,
        9, 11, 2,
        2, 11, 6,
        6, 11, 7,
        7, 11, 3,
        3, 11, 9
    };
    triangles->assign( inc, inc + 60 );

    std::map< utility::Edge, unsigned int > edgeVertices;

    for( unsigned int t = 0; t < level; ++t )
    {
        // for every triangle
        std::size_t numTriangles = triangles->size() / 3;
        for( std::size_t k = 0; k < numTriangles; ++k )
        {
            unsigned int idx[ 3 ];
            // generate a new vertex for every edge (if there is no vertex yet)

            for( int i = 0; i < 3; ++i )
            {
                utility::Edge e( ( *triangles )[ 3 * k + i ], ( *triangles )[ 3 * k + ( i + 1 ) % 3 ] );
                if( edgeVertices.find( e ) == edgeVertices.end() )
                {
                    WVector3d v0 = vertices->at( e.first );
                    WVector3d v1 = vertices->at( e.second );
                    WVector3d v = v0 + v1;
                    v = normalize( v );
                    vertices->push_back( v );
                    edgeVertices[ e ] = vertices->size() - 1;
                }
                idx[ i ] = edgeVertices[ e ];
            }

            // make 4 triangles from the current one
            // add 1st triangle
            triangles->push_back( ( *triangles )[ 3 * k + 0 ] );
            triangles->push_back( idx[ 0 ] );
            triangles->push_back( idx[ 2 ] );

            // add 2nd triangle
            triangles->push_back( ( *triangles )[ 3 * k + 1 ] );
            triangles->push_back( idx[ 1 ] );
            triangles->push_back( idx[ 0 ] );

            // add 3rd triangle
            triangles->push_back( ( *triangles )[ 3 * k + 2 ] );
            triangles->push_back( idx[ 2 ] );
            triangles->push_back( idx[ 1 ] );

            // changed indices of this triangle to the indices of the 4th triangle
            ( *triangles )[ 3 * k + 0 ] = idx[ 0 ];
            ( *triangles )[ 3 * k + 1 ] = idx[ 1 ];
            ( *triangles )[ 3 * k + 2 ] = idx[ 2 ];
        }
        edgeVertices.clear();
    }
}
