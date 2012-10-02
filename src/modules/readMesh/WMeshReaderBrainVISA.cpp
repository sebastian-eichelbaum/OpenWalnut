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

#include "WMeshReaderBrainVISA.h"

WMeshReaderBrainVISA::WMeshReaderBrainVISA():
    WObjectNDIP< WMeshReaderInterface >( "BrainVISA", "Load BrainVISA Meshes." )
{
    // add properties
    m_propDatasetSizeX = m_properties->addProperty( "Dataset size X", "Size of the dataset in x direction", 160 );
    m_propDatasetSizeY = m_properties->addProperty( "Dataset size Y", "Size of the dataset in y direction", 200 );
    m_propDatasetSizeZ = m_properties->addProperty( "Dataset size Z", "Size of the dataset in z direction", 160 );
}

WMeshReaderBrainVISA::~WMeshReaderBrainVISA()
{
    // cleanup
}

WTriangleMesh::SPtr WMeshReaderBrainVISA::operator()( WProgressCombiner::SPtr parentProgress,
                                                boost::filesystem::path file )
{
    namespace su = string_utils;

    std::string fileName = file.string();
    WAssert( !fileName.empty(), "No filename specified." );

    boost::shared_ptr< WProgress > progress = boost::shared_ptr< WProgress >( new WProgress( "Read Mesh", 3 ) );
    parentProgress->addSubProgress( progress );

    std::ifstream ifs;
    ifs.open( fileName.c_str(), std::ifstream::in );
    if( !ifs || ifs.bad() )
    {
        WLogger::getLogger()->addLogMessage( "Trying to load a broken file '" + fileName + "'", "Read Mesh", LL_ERROR );
        throw std::runtime_error( "Problem during reading file. Probably file not found." );
    }
    std::string line;

    // ------ HEADER -------
    char * buffer = new char[10];
    ifs.read( buffer, 10 );
    buffer[9] = 0;

    std::string fileType( buffer );

    if( fileType != "binarDCBA" )
    {
        WLogger::getLogger()->addLogMessage( "Unsupported file type", "Read Mesh", LL_ERROR );
        progress->finish();
        return boost::shared_ptr< WTriangleMesh >();
    }
    size_t numVertices = 0;
    size_t numNormals = 0;
    size_t numTriangles = 0;

    ifs.seekg( 29 );
    int* count = new int[1];
    ifs.read( reinterpret_cast< char* >( count ), 4 );
    numVertices = count[0];

    ifs.seekg( 33 + count[0] * 12 );
    ifs.read( reinterpret_cast< char* >( count ), 4 );
    numNormals = count[0];

    ifs.seekg( 41 + numVertices * 12 + numNormals * 12 );
    ifs.read( reinterpret_cast< char* >( count ), 4 );
    numTriangles = count[0];

    boost::shared_ptr< WTriangleMesh > triMesh( new WTriangleMesh( numVertices, numTriangles ) );

    ifs.seekg( 33 );
    float *pointData = new float[ 3 * numVertices ];
    ifs.read( reinterpret_cast< char* >( pointData ), 3 * sizeof( float ) * numVertices );

    // skipping normals, as they are calculated by the triangle mesh class

    ifs.seekg( 45 + numVertices * 12 + numNormals * 12 );
    int *triData = new int[ 3 * numTriangles ];
    ifs.read( reinterpret_cast< char* >( triData ), 3 * sizeof( int ) * numTriangles );

    for( size_t i = 0; i < numVertices; ++i )
    {
        triMesh->addVertex( pointData[i * 3], m_propDatasetSizeY->get( true )-pointData[i*3+1], m_propDatasetSizeZ->get( true )-pointData[i*3+2] );
    }
    for( size_t i = 0; i < numTriangles; ++i )
    {
        triMesh->addTriangle( triData[i * 3], triData[i * 3 + 2], triData[i * 3 + 1] );
    }

//     skipping normals, as they are calculated by the triangle mesh class
//     the following code would need an adjustment to the triangle mesh class, to avoid overriding
//    the loaded normals
//
//    if( numVertices == numNormals )
//    {
//        ifs.seekg( 37 + numVertices * 12 );
//        float *normalData = new float[ 3 * numNormals ];
//        ifs.read( reinterpret_cast< char* >( normalData ), 3 * sizeof( float ) * numVertices );
//
//        for( size_t i = 0; i < numNormals; ++i )
//        {
//            triMesh->setVertexNormal( i, WPosition( normalData[i * 3], normalData[i * 3 + 1], normalData[i * 3 + 2] ) );
//        }
//        triMesh->setUseExternalVertexNormals( true );
//    }

    ifs.close();
    progress->finish();

    return triMesh;
}
