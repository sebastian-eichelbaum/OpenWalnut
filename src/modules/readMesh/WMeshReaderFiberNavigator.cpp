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

#include "WMeshReaderFiberNavigator.h"

WMeshReaderFiberNavigator::WMeshReaderFiberNavigator():
    WObjectNDIP< WMeshReaderInterface >( "FiberNavigator", "Load FiberNavigator Meshes." )
{
    // add properties
}

WMeshReaderFiberNavigator::~WMeshReaderFiberNavigator()
{
    // cleanup
}

WTriangleMesh::SPtr WMeshReaderFiberNavigator::operator()( WProgressCombiner::SPtr parentProgress,
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
        WLogger::getLogger()->addLogMessage( "Try load broken file '" + fileName + "'", "Read Mesh", LL_ERROR );
        throw std::runtime_error( "Problem during reading file. Probably file not found." );
    }
    std::string line;

    std::vector< std::string > tokens;
    size_t numPoints = 0;
    while( !ifs.eof() )
    {
        std::getline( ifs, line );
        tokens = su::tokenize( line );
        if( tokens.size() > 1 && su::toLower( tokens.at( 0 ) ) == "point_data" )
        {
            try
            {
                numPoints = string_utils::fromString< size_t >( tokens.at( 1 ) );
                break;
            }
            catch( const std::exception &e )
            {
                WLogger::getLogger()->addLogMessage( "Invalid number of points: " + tokens.at( 1 ), "Read Mesh", LL_ERROR );
                progress->finish();
                return boost::shared_ptr< WTriangleMesh >();
            }
        }
    }
    // ------ POINTS -------
    std::vector< osg::Vec3 > points;
    points.reserve( numPoints );

    WLogger::getLogger()->addLogMessage( "Start reading vertex info", "Read Mesh", LL_DEBUG );
    for( unsigned int i = 0; i < numPoints; ++i )
    {
        std::getline( ifs, line );
        tokens = su::tokenize( line );

        try
        {
            points.push_back( osg::Vec3( string_utils::fromString< float >( tokens.at( 0 ) ),
                                         string_utils::fromString< float >( tokens.at( 1 ) ),
                                         string_utils::fromString< float >( tokens.at( 2 ) ) ) );
        }
        catch( const std::exception &e )
        {
            WLogger::getLogger()->addLogMessage( "Invalid vertex position", "Read Mesh", LL_ERROR );
            progress->finish();
            return boost::shared_ptr< WTriangleMesh >();
        }
    }
    WLogger::getLogger()->addLogMessage( "Finished reading vertex info", "Read Mesh", LL_DEBUG );
    // ----- Vertex Ids For Cells---------

    size_t numCells = 0;

    std::getline( ifs, line );
    tokens = su::tokenize( line );
    if( tokens.size() > 1 && su::toLower( tokens.at( 0 ) ) == "cells" )
    {
        try
        {
            numCells = string_utils::fromString< size_t >( tokens.at( 1 ) );
        }
        catch( const std::exception &e )
        {
            WLogger::getLogger()->addLogMessage( "Invalid number of cells: " + tokens.at( 1 ), "Read Mesh", LL_ERROR );
            progress->finish();
            return boost::shared_ptr< WTriangleMesh >();
        }
    }


    boost::shared_ptr< WTriangleMesh > triMesh( new WTriangleMesh( numPoints, numCells ) );
    for( unsigned int i = 0; i < numPoints; ++i )
    {
        triMesh->addVertex( points[i] );
    }

    ++*progress;
    WLogger::getLogger()->addLogMessage( "Finished reading polygon info", "Read Mesh", LL_DEBUG );
    for( unsigned int i = 0; i < numCells; ++i )
    {
        std::getline( ifs, line );
        tokens = su::tokenize( line );

        try
        {
            triMesh->addTriangle( string_utils::fromString< size_t >( tokens.at( 1 ) ),
                                  string_utils::fromString< size_t >( tokens.at( 2 ) ),
                                  string_utils::fromString< size_t >( tokens.at( 3 ) ) );
        }
        catch( const std::exception &e )
        {
            WLogger::getLogger()->addLogMessage( "Invalid triangle ID", "Read Mesh", LL_ERROR );
            progress->finish();
            return boost::shared_ptr< WTriangleMesh >();
        }
    }
    WLogger::getLogger()->addLogMessage( "Finished reading polygon info", "Read Mesh", LL_DEBUG );
    ++*progress;

    ifs.close();

    progress->finish();

    return triMesh;
}
