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

#include "../../common/WPathHelper.h"
#include "../../common/WPropertyHelper.h"
#include "../../kernel/WKernel.h"
#include "../../graphicsEngine/WTriangleMesh.h"

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
    // Put the code for your properties here. See "src/modules/template/" for an extensively documented example.

    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );
    m_meshFile = m_properties->addProperty( "Mesh file", "", WPathHelper::getAppPath() );
    m_readTriggerProp = m_properties->addProperty( "Do read",  "Press!",
                                                  WPVBaseTypes::PV_TRIGGER_READY, m_propCondition );
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

        m_triMesh = read();
        m_output->updateData( m_triMesh );
    }
}

boost::shared_ptr< WTriangleMesh > WMReadMesh::read()
{
    namespace su = string_utils;

    m_readTriggerProp->set( WPVBaseTypes::PV_TRIGGER_READY, false );

    std::string fileName = m_meshFile->get().file_string().c_str();
    WAssert( !fileName.empty(), "No filename specified." );

    boost::shared_ptr< WProgress > progress = boost::shared_ptr< WProgress >( new WProgress( "Marching Cubes", 3 ) );
    m_progress->addSubProgress( progress );

    std::ifstream ifs;
    ifs.open( fileName.c_str(), std::ifstream::in );
    if( !ifs || ifs.bad() )
    {
        WLogger::getLogger()->addLogMessage( "Try load broken file '" + fileName + "'", "Marching Cubes", LL_ERROR );
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
            WLogger::getLogger()->addLogMessage( "Unexpected end of file: " + fileName, "Marching Cubes", LL_ERROR );
        }
        header.push_back( line );
    }
    if( header.at(0) != "# vtk DataFile Version 2.0" )
    {
        WLogger::getLogger()->addLogMessage( "Unsupported format version string: " + header.at( 0 ), "Marching Cubes", LL_WARNING );
    }

    if( su::toUpper( su::trim( header.at( 2 ) ) ) != "ASCII" )
    {
        WLogger::getLogger()->addLogMessage( "Only ASCII files supported, not " + header.at( 2 ), "Marching Cubes", LL_ERROR );
    }

    if(  su::tokenize( header.at( 3 ) ).size() < 2 ||
         su::toUpper( su::tokenize( header.at( 3 ) )[1] ) != "UNSTRUCTURED_GRID" )
    {
        WLogger::getLogger()->addLogMessage( "Invalid VTK DATASET type: " + su::tokenize( header.back() )[1], "Marching Cubes", LL_ERROR );
    }

    // ------ POINTS -------

    std::getline( ifs, line );
    size_t numPoints = 0;
    std::vector< std::string > tokens = su::tokenize( line );
    if( tokens.size() != 3 || su::toLower( tokens.at( 2 ) ) != "float" || su::toLower( tokens.at( 0 ) ) != "points"  )
    {
        WLogger::getLogger()->addLogMessage( "Invalid VTK POINTS declaration: " + line, "Marching Cubes", LL_ERROR );
    }
    try
    {
        numPoints = boost::lexical_cast< size_t >( tokens.at( 1 ) );
    }
    catch( const boost::bad_lexical_cast &e )
    {
        WLogger::getLogger()->addLogMessage( "Invalid number of points: " + tokens.at( 1 ), "Marching Cubes", LL_ERROR );
    }


    std::vector< wmath::WPosition > points;
    points.reserve( numPoints );
    float pointData[3];
    for( unsigned int i = 0; i < numPoints; ++i )
    {
        ifs >> pointData[0] >>  pointData[1] >>  pointData[2];
        points.push_back( wmath::WPosition( pointData[0], pointData[1], pointData[2] ) );
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
            WLogger::getLogger()->addLogMessage( "Number of cell vertices should be 3 but found " + nbCellVerts, "Marching Cubes", LL_ERROR );
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
            WLogger::getLogger()->addLogMessage( "Invalid cell type: " + cellType, "Marching Cubes", LL_ERROR );
        }
    }

    ifs.close();

    progress->finish();

    return triMesh;
}

