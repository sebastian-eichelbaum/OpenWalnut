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

#include <boost/regex.hpp>

#include "WMeshReaderOBJ.h"

WMeshReaderOBJ::WMeshReaderOBJ():
    WObjectNDIP< WMeshReaderInterface >( "OBJ", "Load OBJ Meshes." )
{
    // add properties
}

WMeshReaderOBJ::~WMeshReaderOBJ()
{
    // cleanup
}

WTriangleMesh::SPtr WMeshReaderOBJ::operator()( WProgressCombiner::SPtr parentProgress,
                                                boost::filesystem::path file )
{
    // open the file
    std::string fileName = file.string();
    WAssert( !fileName.empty(), "No filename specified." );

    boost::shared_ptr< WProgress > progress( new WProgress( "Read Mesh" ) );
    parentProgress->addSubProgress( progress );

    std::ifstream ifs;
    ifs.open( fileName.c_str(), std::ifstream::in );
    if( !ifs || ifs.bad() )
    {
        throw  WDHIOFailure( "Could not open \"" + fileName + "\" for reading." );
    }

    // regex for the different lines possible in OBJ
    // mateches vertex only
    static const boost::regex faceVRegex(   "^ *[f,F] *([0-9]+) +([0-9]+) +([0-9]+).*$" );
    // matches vertex-tex coord
    static const boost::regex faceVTRegex(  "^ *[f,F] *([0-9]+)/([0-9]+) +([0-9]+)/([0-9]+) +([0-9]+)/([0-9]+).*$" );
    // matches vertex-tex coord-normal
    static const boost::regex faceVTNRegex( "^ *[f,F] *([0-9]+)/([0-9]+)/([0-9]+) +([0-9]+)/([0-9]+)/([0-9]+) +([0-9]+)/([0-9]+)/([0-9]+).*$" );
    // matches vertex-normal
    static const boost::regex faceVNRegex(  "^ *[f,F] *([0-9]+)//([0-9]+) +([0-9]+)//([0-9]+) +([0-9]+)//([0-9]+).*$" );

    static const boost::regex vertexRegex(  "^ *[v,V][^n] *(-?[0-9]*\\.?[0-9]*) +(-?[0-9]*\\.?[0-9]*) +(-?[0-9]*\\.?[0-9]*).*$" );
    static const boost::regex vertexColorRegex(  "^ *[v,V][^n] *(-?[0-9]*\\.?[0-9]*) +(-?[0-9]*\\.?[0-9]*) +(-?[0-9]*\\.?[0-9]*) (-?[0-9]*\\.?[0-9]*) +(-?[0-9]*\\.?[0-9]*) +(-?[0-9]*\\.?[0-9]*).*$" ); // NOLINT
    static const boost::regex normalRegex(  "^ *[v,V][n,N] *(-?[0-9]*\\.?[0-9]*) +(-?[0-9]*\\.?[0-9]*) +(-?[0-9]*\\.?[0-9]*).*$" );
    static const boost::regex commentRegex( "^ *#.*$" );
    // please note that there are several more possible definitions ... Please see http://en.wikipedia.org/wiki/Wavefront_.obj_file

    // read contents
    size_t numUnsupported = 0;
    std::string line = "";

    std::vector< float > vertices;
    std::vector< float > colors;
    std::vector< size_t > faces;
    std::vector< size_t > normals;

    vertices.reserve( 3000 );
    faces.reserve( 3000 );

    // read line by line
    while( !ifs.eof() )
    {
        // get the line
        std::getline( ifs, line, '\n' );

        // ignore empty lines
        if( !line.size() )
        {
            // empty line
            continue;
        }

        // for interpreting the lines, we utilize boost::regex here
        boost::smatch matches;  // the list of matches

        // check whether this is a vertex definition
        if( boost::regex_match( line, matches, vertexColorRegex ) )
        {
            vertices.push_back( string_utils::fromString< float >( matches[1] ) );
            vertices.push_back( string_utils::fromString< float >( matches[2] ) );
            vertices.push_back( string_utils::fromString< float >( matches[3] ) );
            colors.push_back( string_utils::fromString< float >( matches[4] ) );
            colors.push_back( string_utils::fromString< float >( matches[5] ) );
            colors.push_back( string_utils::fromString< float >( matches[6] ) );
        }
        else if( boost::regex_match( line, matches, vertexRegex ) )
        {
            vertices.push_back( string_utils::fromString< float >( matches[1] ) );
            vertices.push_back( string_utils::fromString< float >( matches[2] ) );
            vertices.push_back( string_utils::fromString< float >( matches[3] ) );
        }
        else if( boost::regex_match( line, matches, normalRegex ) )
        {
            normals.push_back( string_utils::fromString< float >( matches[1] ) );
            normals.push_back( string_utils::fromString< float >( matches[2] ) );
            normals.push_back( string_utils::fromString< float >( matches[3] ) );
        }
        // check whether this is a face definition
        else if( boost::regex_match( line, matches, faceVRegex ) )
        {
            // NOTE: indices are stored beginning at 1
            faces.push_back( string_utils::fromString< size_t >( matches[1] ) - 1 );
            faces.push_back( string_utils::fromString< size_t >( matches[2] ) - 1 );
            faces.push_back( string_utils::fromString< size_t >( matches[3] ) - 1 );
        }
        else if( boost::regex_match( line, matches, faceVTRegex ) )
        {
            // NOTE: indices are stored beginning at 1
            faces.push_back( string_utils::fromString< size_t >( matches[1] ) - 1 );
            faces.push_back( string_utils::fromString< size_t >( matches[3] ) - 1 );
            faces.push_back( string_utils::fromString< size_t >( matches[5] ) - 1 );
        }
        else if( boost::regex_match( line, matches, faceVTNRegex ) )
        {
            // NOTE: indices are stored beginning at 1
            faces.push_back( string_utils::fromString< size_t >( matches[1] ) - 1 );
            faces.push_back( string_utils::fromString< size_t >( matches[4] ) - 1 );
            faces.push_back( string_utils::fromString< size_t >( matches[7] ) - 1 );
        }
        else if( boost::regex_match( line, matches, faceVNRegex ) )
        {
            // NOTE: indices are stored beginning at 1
            faces.push_back( string_utils::fromString< size_t >( matches[1] ) - 1 );
            faces.push_back( string_utils::fromString< size_t >( matches[3] ) - 1 );
            faces.push_back( string_utils::fromString< size_t >( matches[5] ) - 1 );
        }
        // check whether this is a comment
        else if( boost::regex_match( line, matches, commentRegex ) )
        {
            // ignore them
        }
        // check whether this is something else
        else
        {
            numUnsupported++;
        }
    }

    if( numUnsupported )
    {
        wlog::error( "Read Mesh" ) << "There where " << numUnsupported << " unsupported lines.";
    }

    // done. Close file.
    ifs.close();

    // build triMesh instance
    WTriangleMesh::SPtr triMesh( new WTriangleMesh( vertices.size() / 3, faces.size() / 3 ) );
    // this is needed if you want to keep the normals that have been loaded.
    //triMesh->setAutoRecalcNormals( false );

    WAssert( ( vertices.size() == normals.size() ) || ( normals.size() == 0 ), "Number of normals and vertices do not match." );

    for( size_t i = 0; i < vertices.size(); i += 3 )
    {
        triMesh->addVertex( vertices[ i + 0 ], vertices[ i + 1 ], vertices[ i + 2 ] );
    }
    for( size_t i = 0; i < faces.size(); i += 3 )
    {
        triMesh->addTriangle( faces[ i + 0 ], faces[ i + 1 ], faces[ i + 2 ] );
    }
    for( size_t i = 0; i < normals.size(); i += 3 )
    {
        triMesh->setVertexNormal( i / 3, normals[ i + 0 ], normals[ i + 1 ], normals[ i + 2 ] );
    }
    for( size_t i = 0; i < colors.size(); i += 3 )
    {
        triMesh->setVertexColor( i / 3, osg::Vec4( colors[ i + 0 ], colors[ i + 1 ], colors[ i + 2 ], 1.0 ) );
    }

    // done.
    progress->finish();
    parentProgress->removeSubProgress( progress );

    return triMesh;
}
