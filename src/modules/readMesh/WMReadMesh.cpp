//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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
#include <fstream>

#include "core/common/WIOTools.h"
#include "core/common/WStringUtils.h"
#include "core/common/WPathHelper.h"
#include "core/common/WPropertyHelper.h"
#include "core/kernel/WKernel.h"
#include "core/graphicsEngine/WTriangleMesh.h"

#include "core/dataHandler/exceptions/WDHIOFailure.h"
#include "core/dataHandler/exceptions/WDHNoSuchFile.h"
#include "core/dataHandler/exceptions/WDHParseError.h"

#include "WMReadMesh.h"
#include "WMReadMesh.xpm"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMReadMesh )

WMReadMesh::WMReadMesh():
    WModule()
{
}

WMReadMesh::~WMReadMesh()
{
    // cleanup
    removeConnectors();
}

boost::shared_ptr< WModule > WMReadMesh::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMReadMesh() );
}

const char** WMReadMesh::getXPMIcon() const
{
    return WMReadMesh_xpm;
}

const std::string WMReadMesh::getName() const
{
    return "Read Mesh";
}

const std::string WMReadMesh::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    // See "src/modules/template/" for an extensively documented example.
    return "Loads a triangle mesh from a vtk file.";
}

void WMReadMesh::connectors()
{
    m_output = boost::shared_ptr< WModuleOutputData< WTriangleMesh > >(
                new WModuleOutputData< WTriangleMesh >( shared_from_this(), "mesh", "The loaded mesh." ) );

    addConnector( m_output );
    // call WModules initialization
    WModule::connectors();
}

void WMReadMesh::properties()
{
    m_nbTriangles = m_infoProperties->addProperty( "Triangles", "The number of triangles in the loaded mesh.", 0 );
    m_nbTriangles->setMax( std::numeric_limits< int >::max() );

    m_nbVertices = m_infoProperties->addProperty( "Vertices", "The number of vertices in the loaded mesh.", 0 );
    m_nbVertices->setMax( std::numeric_limits< int >::max() );

    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );
    m_meshFile = m_properties->addProperty( "Mesh file", "", WPathHelper::getAppPath() );
    m_fileTypeSelectionsList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_fileTypeSelectionsList->addItem( "Mesh (VTK)", "" );
    m_fileTypeSelectionsList->addItem( "Mesh fibernavigator", "" );
    m_fileTypeSelectionsList->addItem( "DIP", "" );
    m_fileTypeSelectionsList->addItem( "BrainVISA", "" );
    m_fileTypeSelectionsList->addItem( "Freesurfer", "" );

    m_fileTypeSelection = m_properties->addProperty( "File type",
                                                     "file type.",
                                                     m_fileTypeSelectionsList->getSelectorFirst(),
                                                     boost::bind( &WMReadMesh::meshTypeSelected, this ) );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_fileTypeSelection );

    m_propDatasetSizeX = m_properties->addProperty( "Dataset size X", "Size of the dataset in x direction", 160, m_propCondition );
    m_propDatasetSizeY = m_properties->addProperty( "Dataset size Y", "Size of the dataset in y direction", 200, m_propCondition );
    m_propDatasetSizeZ = m_properties->addProperty( "Dataset size Z", "Size of the dataset in z direction", 160, m_propCondition );

    m_propDatasetSizeX->setHidden( true );
    m_propDatasetSizeY->setHidden( true );
    m_propDatasetSizeZ->setHidden( true );


    m_propVectorAsColor = m_properties->addProperty( "Read vectors as color",
                                                     "If the module can load vectors from the file "
                                                     "it will interpret them as colors of the surface.",
                                                     true );
    m_propVectorAsColor->setHidden( false );

    m_readTriggerProp = m_properties->addProperty( "Do read",  "Press!", WPVBaseTypes::PV_TRIGGER_READY, m_propCondition );
    WPropertyHelper::PC_PATHEXISTS::addTo( m_meshFile );



    WModule::properties();
}

void WMReadMesh::moduleMain()
{
    m_moduleState.add( m_propCondition );
    ready();
    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if( m_shutdownFlag() )
        {
            break;
        }

        m_readTriggerProp->set( WPVBaseTypes::PV_TRIGGER_READY, false );

        switch( m_fileTypeSelection->get( true ).getItemIndexOfSelected( 0 ) )
        {
        case 0:
            m_triMesh = readMesh();
            break;
        case 1:
            m_triMesh = readMeshFnav();
            break;
        case 2:
            m_triMesh = readDip();
            break;
        case 3:
            m_triMesh = readBrainVISA();
            break;
        case 4:
            m_triMesh = readFreesurfer();
            break;
        default:
            debugLog() << "this shouldn't be reached";
            break;
        }
        m_nbTriangles->set( m_triMesh->triangleSize() );
        m_nbVertices->set( m_triMesh->vertSize() );

        m_output->updateData( m_triMesh );

        m_readTriggerProp->set( WPVBaseTypes::PV_TRIGGER_READY, true );
    }
}

boost::shared_ptr< WTriangleMesh > WMReadMesh::readMeshFnav()
{
    namespace su = string_utils;

    std::string fileName = m_meshFile->get().string().c_str();
    WAssert( !fileName.empty(), "No filename specified." );

    boost::shared_ptr< WProgress > progress = boost::shared_ptr< WProgress >( new WProgress( "Read Mesh", 3 ) );
    m_progress->addSubProgress( progress );

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
                debugLog() << numPoints << " points";
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

    debugLog() << "Start reading vertex info";
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
    debugLog() << "Finished reading vertex info";
    // ----- Vertex Ids For Cells---------

    size_t numCells = 0;

    std::getline( ifs, line );
    tokens = su::tokenize( line );
    if( tokens.size() > 1 && su::toLower( tokens.at( 0 ) ) == "cells" )
    {
        try
        {
            numCells = string_utils::fromString< size_t >( tokens.at( 1 ) );
            debugLog() << numCells << " cells";
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


    debugLog() << "Start reading polygon info";
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
    debugLog() << "Finished reading polygon info";
    ++*progress;

    ifs.close();

    progress->finish();

    return triMesh;
}


boost::shared_ptr< WTriangleMesh > WMReadMesh::readMesh()
{
    namespace su = string_utils;

    std::string fileName = m_meshFile->get().string().c_str();
    WAssert( !fileName.empty(), "No filename specified." );

    boost::shared_ptr< WProgress > progress = boost::shared_ptr< WProgress >( new WProgress( "Read Mesh", 3 ) );
    m_progress->addSubProgress( progress );

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
            WLogger::getLogger()->addLogMessage( "Number of cell vertices should be 3 but found " + nbCellVerts, "Read Mesh", LL_ERROR );
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
            WLogger::getLogger()->addLogMessage( "Invalid cell type: " + cellType, "Read Mesh", LL_ERROR );
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
        char* vectorDataType = new char[30];
        ifs >> vectorMarker >> vectorName >> vectorDataType;

        if( std::string( vectorMarker ) == "VECTORS"
            && m_propVectorAsColor->get()
            &&  std::string( vectorDataType ) == "float" )
        {
            WColor vectorComp;
            for( unsigned int i = 0; i < nbVectors; ++i )
            {
                std::string line;
                std::getline( ifs, line, '\n' );
                ifs >> vectorComp[0] >> vectorComp[1] >> vectorComp[2];
                triMesh->setVertexColor( i, vectorComp );
            }
        }
    }

    ifs.close();

    progress->finish();

    return triMesh;
}


boost::shared_ptr< WTriangleMesh > WMReadMesh::readBrainVISA()
{
    debugLog() << "brainVISA reader (don't forget to fix switch statement)";
    namespace su = string_utils;

    std::string fileName = m_meshFile->get().string().c_str();
    WAssert( !fileName.empty(), "No filename specified." );

    boost::shared_ptr< WProgress > progress = boost::shared_ptr< WProgress >( new WProgress( "Read Mesh", 3 ) );
    m_progress->addSubProgress( progress );

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
    debugLog() << count[0] << " vertices";

    ifs.seekg( 33 + count[0] * 12 );
    ifs.read( reinterpret_cast< char* >( count ), 4 );
    numNormals = count[0];
    debugLog() << count[0] << " normals";

    ifs.seekg( 41 + numVertices * 12 + numNormals * 12 );
    ifs.read( reinterpret_cast< char* >( count ), 4 );
    debugLog() << count[0] << " triangles";
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

osg::Vec4 WMReadMesh::blueGreenPurpleColorMap( float value )
{
    value *= 5.0;
    osg::Vec4 color;
    if( value < 0.0 )
    {
        color = osg::Vec4( 0.0, 0.0, 0.0, 1.0 );
    }
    else if( value < 1.0 )
    {
        color = osg::Vec4( 0.0, value, 1.0, 1.0 );
    }
    else if( value < 2.0 )
    {
        color = osg::Vec4( 0.0, 1.0, 2.0-value, 1.0 );
    }
    else if( value < 3.0 )
    {
        color =  osg::Vec4( value-2.0, 1.0, 0.0, 1.0 );
    }
    else if( value < 4.0 )
    {
        color = osg::Vec4( 1.0, 4.0-value, 0.0, 1.0 );
    }
    else if( value <= 5.0 )
    {
        color = osg::Vec4( 1.0, 0.0, value-4.0, 1.0 );
    }
    else
    {
        color =  osg::Vec4( 1.0, 0.0, 1.0, 1.0 );
    }

    return color;
}

boost::shared_ptr< WTriangleMesh > WMReadMesh::readDip()
{
    namespace su = string_utils;

    std::string fileName = m_meshFile->get().string().c_str();
    WAssert( !fileName.empty(), "No filename specified." );

    boost::shared_ptr< WProgress > progress = boost::shared_ptr< WProgress >( new WProgress( "Read Mesh", 3 ) );
    m_progress->addSubProgress( progress );

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
    debugLog() << "Positions: " << numPoints << " Cells: " << numCells;

    //ifs.seekg( 0, std::ios::beg );
    ifs.close();
    ifs.open( fileName.c_str(), std::ifstream::in );
    debugLog() << "current position: " << ifs.tellg();
    while( !ifs.eof() )
    {
        std::getline( ifs, line );
        tokens = su::tokenize( line );
        if( tokens.size() == 1 && su::toLower( tokens.at( 0 ) ) == "positionsfixed" )
        {
            debugLog() << "found vertex info at file position: " << ifs.tellg();
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

    debugLog() << "Start reading vertex info";
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
    debugLog() << "Finished reading vertex info";

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
            debugLog() << "found polygon info at file postion: " << ifs.tellg();
            break;
        }
    }
    if( ifs.eof() )
    {
        WLogger::getLogger()->addLogMessage( "Couldn't find polygon info in the file", "Read Mesh", LL_ERROR );
        progress->finish();
        return boost::shared_ptr< WTriangleMesh >();
    }


    debugLog() << "Start reading polygon info";
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
    debugLog() << "Finished reading polygon info";
    ++*progress;
    progress->finish();

    return triMesh;
}

boost::shared_ptr< WTriangleMesh > WMReadMesh::readFreesurfer()
{
    namespace su = string_utils;

    std::string fileName = m_meshFile->get().string();
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

std::string WMReadMesh::getLine( boost::shared_ptr< std::ifstream > ifs, const std::string& desc )
{
    std::string line;
    try
    {
        // we use '\n' as line termination under every platform so our files (which are most likely to be generated on Unix systems)
        // can be read from all platforms not having those line termination symbols like e.g. windows ('\r\n').
        std::getline( *ifs, line, '\n' );
    }
    catch( const std::ios_base::failure &e )
    {
        throw WDHIOFailure( std::string( "IO error while " + desc + " of VTK fiber file: , " + e.what() ) );
    }
    if( !ifs->good() )
    {
        throw WDHParseError( std::string( "Unexpected end of VTK fiber file." ) );
    }
    return line;
}

void WMReadMesh::meshTypeSelected()
{
    m_propVectorAsColor->setHidden( m_fileTypeSelection->get( true ).getItemIndexOfSelected( 0 ) != 0 );

    if( m_fileTypeSelection->get().getItemIndexOfSelected( 0 )  == 2 ||
        m_fileTypeSelection->get().getItemIndexOfSelected( 0 )  == 3 ||
        m_fileTypeSelection->get().getItemIndexOfSelected( 0 )  == 4 )

    {
        m_propDatasetSizeX->setHidden( false );
        m_propDatasetSizeY->setHidden( false );
        m_propDatasetSizeZ->setHidden( false );
    }
    else
    {
        m_propDatasetSizeX->setHidden( true );
        m_propDatasetSizeY->setHidden( true );
        m_propDatasetSizeZ->setHidden( true );
    }
}
