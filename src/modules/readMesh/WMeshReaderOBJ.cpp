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

    boost::shared_ptr< WProgress > progress = boost::shared_ptr< WProgress >( new WProgress( "Read Mesh", 3 ) );
    parentProgress->addSubProgress( progress );

    std::ifstream ifs;
    ifs.open( fileName.c_str(), std::ifstream::in );
    if( !ifs || ifs.bad() )
    {
        throw  WDHIOFailure( "Could not open \"" + fileName + "\" for reading." );
    }

    // regex for the different lines possible in OBJ
    static const boost::regex faceRegex(   "^ *[f,F] *([0-9]*) *([0-9]*) *([0-9]*) *$" );
    static const boost::regex vertexRegex(   "^ *[v,V] *(-?[0-9]*\\.?[0-9]*) *(-?[0-9]*\\.?[0-9]*) *(-?[0-9]*\\.?[0-9]*).*$" );
    static const boost::regex commentRegex( "^ *#.*$" );
    // please note that there are several more possible definitions ... Please see http://en.wikipedia.org/wiki/Wavefront_.obj_file

    // read contents
    size_t numUnsupported = 0;
    std::string line = "";

    std::vector< float > vertices;
    std::vector< size_t > faces;

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
        if( boost::regex_match( line, matches, vertexRegex ) )
        {
            vertices.push_back( string_utils::fromString< float >( matches[1] ) );
            vertices.push_back( string_utils::fromString< float >( matches[2] ) );
            vertices.push_back( string_utils::fromString< float >( matches[3] ) );
        }
        // check whether this is a face definition
        else if( boost::regex_match( line, matches, faceRegex ) )
        {
            // NOTE: indices are stored beginning at 1
            faces.push_back( string_utils::fromString< size_t >( matches[1] ) - 1 );
            faces.push_back( string_utils::fromString< size_t >( matches[2] ) - 1 );
            faces.push_back( string_utils::fromString< size_t >( matches[3] ) - 1 );
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

    for( size_t i = 0; i < vertices.size(); i += 3 )
    {
        triMesh->addVertex( vertices[ i + 0 ], vertices[ i + 1 ], vertices[ i + 2 ] );
    }
    for( size_t i = 0; i < faces.size(); i += 3 )
    {
        triMesh->addTriangle( faces[ i + 0 ], faces[ i + 1 ], faces[ i + 2 ] );
    }

    // done.
    progress->finish();
    parentProgress->removeSubProgress( progress );

    return triMesh;
}
