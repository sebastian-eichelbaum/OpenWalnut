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

#include <string>
#include <vector>

#include "WMeshReaderFreesurfer.h"

WMeshReaderFreesurfer::WMeshReaderFreesurfer():
    WObjectNDIP< WMeshReaderInterface >( "Freesurfer", "Load Freesurfer Meshes." )
{
    // add properties
    m_propDatasetSizeX = m_properties->addProperty( "Dataset size X", "Size of the dataset in x direction", 160 );
    m_propDatasetSizeY = m_properties->addProperty( "Dataset size Y", "Size of the dataset in y direction", 200 );
    m_propDatasetSizeZ = m_properties->addProperty( "Dataset size Z", "Size of the dataset in z direction", 160 );
}

WMeshReaderFreesurfer::~WMeshReaderFreesurfer()
{
    // cleanup
}

WTriangleMesh::SPtr WMeshReaderFreesurfer::operator()( WProgressCombiner::SPtr /*parentProgress*/,
                                                boost::filesystem::path file )
{
    namespace su = string_utils;

    std::string fileName = file.string();
    WAssert( !fileName.empty(), "No filename specified." );

    boost::shared_ptr< std::ifstream > ifs = boost::shared_ptr< std::ifstream >( new std::ifstream() );
    ifs->open( fileName.c_str(), std::ifstream::in | std::ifstream::binary );
    if( !ifs || ifs->bad() )
    {
        throw WDHIOFailure( std::string( "internal error while opening" ) );
    }
    getLine( ifs, "" );
    getLine( ifs, "" );

    int *pointData = new int[ 2 ];
    ifs->read( reinterpret_cast< char* >( pointData ), 8 );
    switchByteOrderOfArray( pointData, 2 );
    size_t numVertices = pointData[0];
    size_t numTriangles = pointData[1];
    //std::cout << numVertices << " " << numTriangles << std::endl;

    float* verts = new float[ numVertices * 3 ];
    ifs->read( reinterpret_cast< char* >( verts ), 3 * sizeof( float ) * numVertices );
    switchByteOrderOfArray( verts, numVertices * 3 );

    int* tris = new int[ numTriangles * 3 ];
    ifs->read( reinterpret_cast< char* >( tris ), 3 * sizeof( int ) * numTriangles );
    switchByteOrderOfArray( tris, numTriangles * 3 );

    boost::shared_ptr< WTriangleMesh > triMesh( new WTriangleMesh( numVertices, numTriangles ) );

    for( size_t i = 0; i < numVertices; ++i )
    {
        triMesh->addVertex( verts[i*3]+  ( ( m_propDatasetSizeX->get( true )+1 )/2.0 ),
                            verts[i*3+1]+( ( m_propDatasetSizeY->get( true )+1 )/2.0 ),
                            verts[i*3+2]+( ( m_propDatasetSizeZ->get( true )+1 )/2.0 ) );
    }
    for( size_t i = 0; i < numTriangles; ++i )
    {
        triMesh->addTriangle( tris[i * 3], tris[i * 3 + 2], tris[i * 3 + 1] );
    }
    return triMesh;
}
