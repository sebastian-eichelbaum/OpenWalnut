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

#include "WMeshReaderVTK.h"

WMeshReaderVTK::WMeshReaderVTK():
    WObjectNDIP< WMeshReaderInterface >( "VTK", "Load VTK Meshes." )
{
    // add properties
    m_propVectorAsColor = m_properties->addProperty( "Read vectors as color",
                                                     "If the module can load vectors from the file "
                                                     "it will interpret them as colors of the surface.",
                                                     true );
}

WMeshReaderVTK::~WMeshReaderVTK()
{
    // cleanup
}

WTriangleMesh::SPtr WMeshReaderVTK::operator()( WProgressCombiner::SPtr parentProgress,
                                                boost::filesystem::path file )
{
    namespace su = string_utils;

    std::string fileName = file.string();
    WAssert( !fileName.empty(), "No filename specified." );

    boost::shared_ptr< WProgress > progress( new WProgress( "Read Mesh", 3 ) );
    parentProgress->addSubProgress( progress );

    std::ifstream ifs;
    ifs.open( fileName.c_str(), std::ifstream::in );
    if( !ifs || ifs.bad() )
    {
        WLogger::getLogger()->addLogMessage( "Try load broken file '" + fileName + "'", "Read Mesh", LL_ERROR );
        throw std::runtime_error( "Problem during reading file. Probably file not found." );
    }
    std::string line;

    // ------ HEADER -------
    std::vector< std::string > header;
    for( int i = 0; i < 4; ++i )  // strip first four lines
    {
        std::getline( ifs, line );
        if( !ifs.good() )
        {
            WLogger::getLogger()->addLogMessage( "Unexpected end of file: " + fileName, "Read Mesh", LL_ERROR );
            progress->finish();
            return boost::shared_ptr< WTriangleMesh >();
        }
        header.push_back( line );
    }
    if( header.at(0) != "# vtk DataFile Version 2.0" )
    {
        WLogger::getLogger()->addLogMessage( "Unsupported format version string: " + header.at( 0 ), "Read Mesh", LL_WARNING );
    }

    if( su::toUpper( su::trim( header.at( 2 ) ) ) != "ASCII" )
    {
        WLogger::getLogger()->addLogMessage( "Only ASCII files supported, not " + header.at( 2 ), "Read Mesh", LL_ERROR );
        progress->finish();
        return boost::shared_ptr< WTriangleMesh >();
    }

    if(  su::tokenize( header.at( 3 ) ).size() < 2 ||
         su::toUpper( su::tokenize( header.at( 3 ) )[1] ) != "UNSTRUCTURED_GRID" )
    {
        WLogger::getLogger()->addLogMessage( "Invalid VTK DATASET type: " + su::tokenize( header.back() )[1], "Read Mesh", LL_ERROR );
        progress->finish();
        return boost::shared_ptr< WTriangleMesh >();
    }

    // ------ POINTS -------

    std::getline( ifs, line );
    size_t numPoints = 0;
    std::vector< std::string > tokens = su::tokenize( line );
    if( tokens.size() != 3 || su::toLower( tokens.at( 2 ) ) != "float" || su::toLower( tokens.at( 0 ) ) != "points"  )
    {
        WLogger::getLogger()->addLogMessage( "Invalid VTK POINTS declaration: " + line, "Read Mesh", LL_ERROR );
        progress->finish();
        return boost::shared_ptr< WTriangleMesh >();
    }
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


    std::vector< WPosition > points;
    points.reserve( numPoints );
    float pointData[3];
    for( unsigned int i = 0; i < numPoints; ++i )
    {
        ifs >> pointData[0] >>  pointData[1] >>  pointData[2];
        points.push_back( WPosition( pointData[0], pointData[1], pointData[2] ) );
    }

    // ----- Vertex Ids For Cells---------
    char* cellsMarker = new char[30];
    size_t nbCells;
    size_t nbNumbers;
    ifs >> cellsMarker >> nbCells >> nbNumbers;

    boost::shared_ptr< WTriangleMesh > triMesh( new WTriangleMesh( numPoints, nbCells ) );

    for( unsigned int i = 0; i < numPoints; ++i )
    {
        triMesh->addVertex( osg::Vec3( points[i][0], points[i][1], points[i][2] ) );
    }
    ++*progress;

    unsigned int nbCellVerts;
    for( unsigned int i = 0; i < nbCells; ++i )
    {
        unsigned int tri[3];
        ifs >> nbCellVerts >> tri[0]  >> tri[1]  >> tri[2];
        triMesh->addTriangle( tri[0], tri[1], tri[2] );
        if( nbCellVerts != 3 )
        {
            WLogger::getLogger()->addLogMessage( "Number of cell vertices should be 3 but found " +
                                                 string_utils::toString( nbCellVerts ) + ".", "Read Mesh", LL_ERROR );
            progress->finish();
            return boost::shared_ptr< WTriangleMesh >();
        }
    }

    ++*progress;

    // ----- Cell Types ---------
    char* cells_typesMarker = new char[30];
    size_t nbCellTypes;
    ifs >> cells_typesMarker >> nbCellTypes;
    unsigned int cellType;
    for( unsigned int i = 0; i < nbCellTypes; ++i )
    {
        ifs >> cellType;
        if( cellType != 5 )
        {
            WLogger::getLogger()->addLogMessage( "Invalid cell type: " + string_utils::toString( cellType ) + ".", "Read Mesh", LL_ERROR );
            progress->finish();
            return boost::shared_ptr< WTriangleMesh >();
        }
    }

    char* marker = new char[30];
    size_t nbVectors;
    ifs >> marker >> nbVectors;
    if( std::string( marker ) == "POINT_DATA" && nbVectors == numPoints )
    {
        // ----- Vector as color ---------
        char* vectorMarker = new char[30];
        char* vectorName = new char[30];
        char* vectorInfo1 = new char[30];
        char* vectorInfo2 = new char[30];
        ifs >> vectorMarker >> vectorName >> vectorInfo1 >> vectorInfo2;

        if( std::string( vectorMarker ) == "VECTORS"
            && m_propVectorAsColor->get()
            &&  std::string( vectorInfo1 ) == "float" )
        {
            WLogger::getLogger()->addLogMessage( "Reading colors from vectors", "Read Mesh", LL_DEBUG );
            WColor vectorComp;
            for( unsigned int i = 0; i < nbVectors; ++i )
            {
                std::string line;
                std::getline( ifs, line, '\n' );
                ifs >> vectorComp[0] >> vectorComp[1] >> vectorComp[2];
                triMesh->setVertexColor( i, vectorComp );
            }
        }
        if( std::string( vectorMarker ) == "ARRAYS" )
        {
            //WLogger::getLogger()->addLogMessage( std::string( vectorInfo1[0] ), "Read Mesh", LL_DEBUG );
            WAssert( vectorInfo1[0] == '2' || vectorInfo1[0] == '3', "Can only deal with 2D or 3D arrays." );
            if( m_propVectorAsColor->get()
                && std::string( vectorInfo2 ) == "float" )
            {
                WLogger::getLogger()->addLogMessage( "Reading colors from arrays", "Read Mesh", LL_DEBUG );
                WColor vectorComp;
                for( unsigned int i = 0; i < nbVectors; ++i )
                    {
                        std::string line;
                        std::getline( ifs, line, '\n' );
                        if( vectorInfo1[0] == '2' )
                            {
                                ifs >> vectorComp[0] >> vectorComp[1];
                                vectorComp[2] = 0;
                            }
                        if( vectorInfo1[0] == '3' )
                            {
                                ifs >> vectorComp[0] >> vectorComp[1] >> vectorComp[2];
                            }
                        triMesh->setVertexColor( i, vectorComp );
                    }
            }
        }
        triMesh->rescaleVertexColors();
        delete[] vectorMarker;
        delete[] vectorName;
        delete[] vectorInfo1;
        delete[] vectorInfo2;
    }

    ifs.close();

    progress->finish();

    delete[] cellsMarker;
    delete[] cells_typesMarker;
    delete[] marker;

    return triMesh;
}
