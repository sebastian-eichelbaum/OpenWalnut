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

#include "WMeshReaderDIP.h"

WMeshReaderDIP::WMeshReaderDIP():
    WObjectNDIP< WMeshReaderInterface >( "DIP", "Load DIP Meshes." )
{
    // add properties
    m_propDatasetSizeX = m_properties->addProperty( "Dataset size X", "Size of the dataset in x direction", 160 );
    m_propDatasetSizeY = m_properties->addProperty( "Dataset size Y", "Size of the dataset in y direction", 200 );
    m_propDatasetSizeZ = m_properties->addProperty( "Dataset size Z", "Size of the dataset in z direction", 160 );
}

WMeshReaderDIP::~WMeshReaderDIP()
{
    // cleanup
}

WTriangleMesh::SPtr WMeshReaderDIP::operator()( WProgressCombiner::SPtr parentProgress,
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
    size_t numPoints = 0;
    size_t numCells = 0;
    size_t typeCells = 0;
    std::vector< std::string > tokens;

    // first pass, try to read all information that might be anywhere in the file
    while( !ifs.eof() )
    {
        std::getline( ifs, line );
        tokens = su::tokenize( line );
        if( tokens.size() == 2 && su::toLower( tokens.at( 0 ) ) == "numberpositions=" )
        {
            try
            {
                numPoints = string_utils::fromString< size_t >( tokens.at( 1 ) );
            }
            catch( const std::exception &e )
            {
                WLogger::getLogger()->addLogMessage( "Invalid number of points: " + tokens.at( 1 ), "Read Mesh", LL_ERROR );
                progress->finish();
                return boost::shared_ptr< WTriangleMesh >();
            }
        }
        if( tokens.size() == 2 && su::toLower( tokens.at( 0 ) ) == "numberpolygons=" )
        {
            try
            {
                numCells = string_utils::fromString< size_t >( tokens.at( 1 ) );
            }
            catch( const std::exception &e )
            {
                WLogger::getLogger()->addLogMessage( "Invalid number of polygons: " + tokens.at( 1 ), "Read Mesh", LL_ERROR );
                progress->finish();
                return boost::shared_ptr< WTriangleMesh >();
            }
        }
        if( tokens.size() == 2 && su::toLower( tokens.at( 0 ) ) == "typepolygons=" )
        {
            try
            {
                typeCells = string_utils::fromString< size_t >( tokens.at( 1 ) );
            }
            catch( const std::exception &e )
            {
                WLogger::getLogger()->addLogMessage( "Invalid type of polygons: " + tokens.at( 1 ), "Read Mesh", LL_ERROR );
                progress->finish();
                return boost::shared_ptr< WTriangleMesh >();
            }
            if( typeCells != 3 )
            {
                WLogger::getLogger()->addLogMessage( "Invalid type of polygons: " + tokens.at( 1 ), "Read Mesh", LL_ERROR );
                progress->finish();
                return boost::shared_ptr< WTriangleMesh >();
            }
        }
    }
    wlog::debug( "Read Mesh" ) << "Positions: " << numPoints << " Cells: " << numCells;

    //ifs.seekg( 0, std::ios::beg );
    ifs.close();
    ifs.open( fileName.c_str(), std::ifstream::in );
    wlog::debug( "Read Mesh" ) << "current position: " << ifs.tellg();
    while( !ifs.eof() )
    {
        std::getline( ifs, line );
        tokens = su::tokenize( line );
        if( tokens.size() == 1 && su::toLower( tokens.at( 0 ) ) == "positionsfixed" )
        {
            wlog::debug( "Read Mesh" ) << "found vertex info at file position: " << ifs.tellg();
            break;
        }
    }
    if( ifs.eof() )
    {
        WLogger::getLogger()->addLogMessage( "Couldn't find vertex info in the file", "Read Mesh", LL_ERROR );
        progress->finish();
        return boost::shared_ptr< WTriangleMesh >();
    }

    std::vector< osg::Vec3 > points;
    points.reserve( numPoints );

    wlog::debug( "Read Mesh" ) << "Start reading vertex info";
    for( unsigned int i = 0; i < numPoints; ++i )
    {
        std::getline( ifs, line );
        tokens = su::tokenize( line );

        try
        {
            points.push_back( osg::Vec3( string_utils::fromString< float >( tokens.at( 1 ) ) + ( m_propDatasetSizeX->get( true )/2.0 ),
                                         string_utils::fromString< float >( tokens.at( 0 ) ) + ( m_propDatasetSizeY->get( true )/2.0 ),
                                         string_utils::fromString< float >( tokens.at( 2 ) ) + ( m_propDatasetSizeZ->get( true )/2.0 ) ) );
        }
        catch( const std::exception &e )
        {
            WLogger::getLogger()->addLogMessage( "Invalid vertex position", "Read Mesh", LL_ERROR );
            progress->finish();
            return boost::shared_ptr< WTriangleMesh >();
        }
    }
    wlog::debug( "Read Mesh" ) << "Finished reading vertex info";

    boost::shared_ptr< WTriangleMesh > triMesh( new WTriangleMesh( numPoints, numCells ) );

    for( unsigned int i = 0; i < numPoints; ++i )
    {
        triMesh->addVertex( points[i] );
    }
    ++*progress;


    //ifs.seekg( 0, std::ios::beg );
    ifs.close();
    ifs.open( fileName.c_str(), std::ifstream::in );
    while( !ifs.eof() )
    {
        std::getline( ifs, line );
        tokens = su::tokenize( line );
        if( tokens.size() == 1 && su::toLower( tokens.at( 0 ) ) == "polygons" )
        {
            wlog::debug( "Read Mesh" ) << "found polygon info at file postion: " << ifs.tellg();
            break;
        }
    }
    if( ifs.eof() )
    {
        WLogger::getLogger()->addLogMessage( "Couldn't find polygon info in the file", "Read Mesh", LL_ERROR );
        progress->finish();
        return boost::shared_ptr< WTriangleMesh >();
    }


    wlog::debug( "Read Mesh" ) << "Start reading polygon info";
    for( unsigned int i = 0; i < numCells; ++i )
    {
        std::getline( ifs, line );
        tokens = su::tokenize( line );

        try
        {
            triMesh->addTriangle( string_utils::fromString< size_t >( tokens.at( 0 ) ),
                                  string_utils::fromString< size_t >( tokens.at( 1 ) ),
                                  string_utils::fromString< size_t >( tokens.at( 2 ) ) );
        }
        catch( const std::exception &e )
        {
            WLogger::getLogger()->addLogMessage( "Invalid triangle ID", "Read Mesh", LL_ERROR );
            progress->finish();
            return boost::shared_ptr< WTriangleMesh >();
        }
    }
    wlog::debug( "Read Mesh" ) << "Finished reading polygon info";
    ++*progress;
    progress->finish();

    return triMesh;
}
