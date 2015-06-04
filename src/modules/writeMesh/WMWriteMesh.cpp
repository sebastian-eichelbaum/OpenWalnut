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
#include <fstream>
#include <vector>

#include "core/common/WStringUtils.h"
#include "core/common/math/WMath.h"
#include "core/common/WPathHelper.h"
#include "core/common/WLimits.h"
#include "core/kernel/WKernel.h"
#include "WMWriteMesh.xpm"
#include "WMWriteMesh.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMWriteMesh )

WMWriteMesh::WMWriteMesh():
    WModule()
{
}

WMWriteMesh::~WMWriteMesh()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMWriteMesh::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMWriteMesh() );
}

const char** WMWriteMesh::getXPMIcon() const
{
    return WMWriteMesh_xpm;
}
const std::string WMWriteMesh::getName() const
{
    return "Write Mesh";
}

const std::string WMWriteMesh::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    // See "src/modules/template/" for an extensively documented example.
    return "This module writes a triangle mesh to a file. Different file formats are supported.";
}

void WMWriteMesh::connectors()
{
    m_meshInput = boost::shared_ptr< WModuleInputData < WTriangleMesh > >(
        new WModuleInputData< WTriangleMesh >( shared_from_this(), "mesh", "The mesh to save" )
        );

    addConnector( m_meshInput );

    WModule::connectors();
}

void WMWriteMesh::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );
    m_savePropGroup = m_properties->addPropertyGroup( "Save Surface",  "" );
    m_saveTriggerProp = m_savePropGroup->addProperty( "Do save",  "Press!", WPVBaseTypes::PV_TRIGGER_READY, m_propCondition );
    m_meshFile = m_savePropGroup->addProperty( "Mesh file", "", WPathHelper::getAppPath() );

    m_fileTypeSelectionsList = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_fileTypeSelectionsList->addItem( "VTK ASCII", "" );
    m_fileTypeSelectionsList->addItem( "json", "" );
    m_fileTypeSelectionsList->addItem( "STL", "Ascii Stereo Lithography File." );

    m_fileTypeSelection = m_properties->addProperty( "File type",  "file type.", m_fileTypeSelectionsList->getSelectorFirst() );
       WPropertyHelper::PC_SELECTONLYONE::addTo( m_fileTypeSelection );

    m_writeColors = m_properties->addProperty( "Write colors", "", true );

    WModule::properties();
}

void WMWriteMesh::moduleMain()
{
    // Put the code for your module's main functionality here.
    // See "src/modules/template/" for an extensively documented example.
    m_moduleState.add( m_meshInput->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    // signal ready state
    ready();

    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        // this waits for m_moduleState to fire. By default, this is only the m_shutdownFlag condition.
        // NOTE: you can add your own conditions to m_moduleState using m_moduleState.add( ... )
        m_moduleState.wait();

        if( m_triMesh != m_meshInput->getData() )
        {
            m_triMesh = m_meshInput->getData();
        }

        m_saveTriggerProp->set( WPVBaseTypes::PV_TRIGGER_READY, false );

        switch( m_fileTypeSelection->get( true ).getItemIndexOfSelected( 0 ) )
        {
            case 0:
                debugLog() << "type VTK ASCII file selected";
                saveVTKASCII();
                break;
            case 1:
                debugLog() << "type json file selected";
                saveJson();
                break;
            case 2:
                debugLog() << "type STL file selected";
                saveSTL();
                break;
            default:
                debugLog() << "this shouldn't be reached";
                break;
        }
        m_saveTriggerProp->set( WPVBaseTypes::PV_TRIGGER_READY, true );
    }
}

bool WMWriteMesh::saveVTKASCII() const
{
    if( !m_triMesh )
    {
        return false;
    }

    if( m_triMesh->vertSize() == 0 )
    {
        WLogger::getLogger()->addLogMessage( "Will not write file that contains 0 vertices.", "Write Mesh", LL_ERROR );
        return false;
    }

    if( m_triMesh->triangleSize() == 0 )
    {
        WLogger::getLogger()->addLogMessage( "Will not write file that contains 0 triangles.", "Write Mesh", LL_ERROR );
        return false;
    }

    const char* c_file = m_meshFile->get().string().c_str();
    std::ofstream dataFile( c_file, std::ios_base::binary );

    if( dataFile )
    {
        WLogger::getLogger()->addLogMessage( "opening file", "Write Mesh", LL_DEBUG );
    }
    else
    {
        WLogger::getLogger()->addLogMessage( "open file failed" + m_meshFile->get().string() , "Write Mesh", LL_ERROR );
        return false;
    }

    dataFile.precision( 16 );

    WLogger::getLogger()->addLogMessage( "start writing file", "Write Mesh", LL_DEBUG );
    dataFile << ( "# vtk DataFile Version 2.0\n" );
    dataFile << ( "generated using OpenWalnut\n" );
    dataFile << ( "ASCII\n" );
    dataFile << ( "DATASET UNSTRUCTURED_GRID\n" );

    WPosition point;
    dataFile << "POINTS " << m_triMesh->vertSize() << " float\n";
    for( size_t i = 0; i < m_triMesh->vertSize(); ++i )
    {
        point = m_triMesh->getVertex( i );
        if( !( !wlimits::isInf( point[0] ) && !wlimits::isInf( point[1] ) && !wlimits::isInf( point[2] ) ) )
        {
            WLogger::getLogger()->addLogMessage( "Will not write file from data that contains NAN or INF.", "Write Mesh", LL_ERROR );
            return false;
        }
        dataFile << point[0] << " " << point[1] << " " << point[2] << "\n";
    }

    dataFile << "CELLS " << m_triMesh->triangleSize() << " " << m_triMesh->triangleSize() * 4 << "\n";
    for( size_t i = 0; i < m_triMesh->triangleSize(); ++i )
    {
        dataFile << "3 " << m_triMesh->getTriVertId0( i ) << " "
                 <<  m_triMesh->getTriVertId1( i ) << " "
                 <<  m_triMesh->getTriVertId2( i ) << "\n";
    }
    dataFile << "CELL_TYPES "<< m_triMesh->triangleSize() <<"\n";
    for( size_t i = 0; i < m_triMesh->triangleSize(); ++i )
    {
        dataFile << "5\n";
    }
    dataFile << "POINT_DATA " << m_triMesh->vertSize() << "\n";
    dataFile << "SCALARS scalars float\n";
    dataFile << "LOOKUP_TABLE default\n";
    for( size_t i = 0; i < m_triMesh->vertSize(); ++i )
    {
        dataFile << "0\n";
    }
    dataFile.close();
    WLogger::getLogger()->addLogMessage( "saving done", "Write Mesh", LL_DEBUG );
    return true;
}

bool WMWriteMesh::saveSTL()
{
    if( !m_triMesh )
    {
        return false;
    }

    if( m_triMesh->vertSize() == 0 )
    {
        WLogger::getLogger()->addLogMessage( "Will not write file that contains 0 vertices.", "Write Mesh", LL_ERROR );
        return false;
    }

    if( m_triMesh->triangleSize() == 0 )
    {
        WLogger::getLogger()->addLogMessage( "Will not write file that contains 0 triangles.", "Write Mesh", LL_ERROR );
        return false;
    }

    const char* c_file = m_meshFile->get().string().c_str();
    std::ofstream dataFile( c_file, std::ios_base::binary );

    if( dataFile )
    {
        WLogger::getLogger()->addLogMessage( "Opening file", "Write Mesh", LL_DEBUG );
    }
    else
    {
        WLogger::getLogger()->addLogMessage( "Open file failed" + m_meshFile->get().string() , "Write Mesh", LL_ERROR );
        return false;
    }

    WLogger::getLogger()->addLogMessage( "Start writing file", "Write Mesh", LL_DEBUG );
    dataFile << ( "solid OpenWalnut_TriangleMesh\n" );

    // STL requires special number format. Look at Wikipedia: "in sign-mantissa 'e'-sign-exponent format, e.g., "-2.648000e-002"
    dataFile.precision( 16 );
    dataFile << std::scientific;

    // write each triangle. Remember: only one normal per facet. If 0,0,0, viewer program calculates them.
    for( size_t i = 0; i < m_triMesh->triangleSize(); ++i )
    {
        osg::Vec3f v1 = m_triMesh->getTriVert( i, 0 );
        osg::Vec3f v2 = m_triMesh->getTriVert( i, 1 );
        osg::Vec3f v3 = m_triMesh->getTriVert( i, 2 );

        // write face, use right hand rule
        dataFile << "facet normal 0 0 0 " << std::endl;
        dataFile << "  outer loop" << std::endl;
        dataFile << "    vertex " << v1.x() << " " << v1.y() << " " << v1.z() << std::endl;
        dataFile << "    vertex " << v3.x() << " " << v3.y() << " " << v3.z() << std::endl;
        dataFile << "    vertex " << v2.x() << " " << v2.y() << " " << v2.z() << std::endl;
        dataFile << "  endloop" << std::endl;
        dataFile << "endfacet" << std::endl;
    }

    dataFile << ( "endsolid OpenWalnut_TriangleMesh\n" );
    dataFile.close();
    WLogger::getLogger()->addLogMessage( "Saving done", "Write Mesh", LL_DEBUG );
    return true;
}

bool WMWriteMesh::saveJson()
{
    if( !m_triMesh )
    {
        return false;
    }

    if( m_triMesh->vertSize() == 0 )
    {
        WLogger::getLogger()->addLogMessage( "Will not write file that contains 0 vertices.", "Write Mesh", LL_ERROR );
        return false;
    }

    if( m_triMesh->triangleSize() == 0 )
    {
        WLogger::getLogger()->addLogMessage( "Will not write file that contains 0 triangles.", "Write Mesh", LL_ERROR );
        return false;
    }

    std::vector< boost::shared_ptr< WTriangleMesh > >meshes;
    meshes = splitMesh( m_triMesh, 65000 );

    for( size_t k = 0; k < meshes.size(); ++k )
    {
        std::string fn = m_meshFile->get().string() + "_" + string_utils::toString( k );
        const char* c_file = fn.c_str();
        std::ofstream dataFile( c_file, std::ios_base::binary );

        if( dataFile )
        {
            WLogger::getLogger()->addLogMessage( "opening file", "Write Mesh", LL_DEBUG );
        }
        else
        {
            WLogger::getLogger()->addLogMessage( "open file failed" + m_meshFile->get().string() , "Write Mesh", LL_ERROR );
            return false;
        }

        dataFile.precision( 5 );

        WLogger::getLogger()->addLogMessage( "start writing file", "Write Mesh", LL_DEBUG );

        std::cout << meshes[k]->vertSize() << " vertices and " << meshes[k]->triangleSize() << " triangles" << std::endl;
        dataFile << ( "{\n" );
        dataFile << ( "    \"vertices\" : [" );
        WPosition point;
        for( size_t i = 0; i < meshes[k]->vertSize() - 1; ++i )
        {
            point = meshes[k]->getVertex( i );
            if( !( !wlimits::isInf( point[0] ) && !wlimits::isInf( point[1] ) && !wlimits::isInf( point[2] ) ) )
            {
                WLogger::getLogger()->addLogMessage( "Will not write file from data that contains NAN or INF.", "Write Mesh", LL_ERROR );
                return false;
            }
            dataFile << point[0] << "," << point[1] << "," << point[2] << ",";
        }
        point = meshes[k]->getVertex( meshes[k]->vertSize() - 1 );
        dataFile << point[0] << "," << point[1] << "," << point[2] << "],\n";

        dataFile << ( "    \"normals\" : [" );
        WPosition normal;
        for( size_t i = 0; i < meshes[k]->vertSize() - 1; ++i )
        {
            normal = meshes[k]->getNormal( i );
            if( !( !wlimits::isInf( normal[0] ) && !wlimits::isInf( normal[1] ) && !wlimits::isInf( normal[2] ) ) )
            {
                WLogger::getLogger()->addLogMessage( "Will not write file from data that contains NAN or INF.", "Write Mesh", LL_ERROR );
                return false;
            }
            dataFile << normal[0] << "," << normal[1] << "," << normal[2] << ",";
        }
        normal = meshes[k]->getNormal( meshes[k]->vertSize() - 1 );
        dataFile << normal[0] << "," << normal[1] << "," << normal[2] << "],\n";

        dataFile << ( "    \"indices\" : [" );
        for( size_t i = 0; i < meshes[k]->triangleSize() - 1; ++i )
        {
            dataFile << meshes[k]->getTriVertId0( i ) << "," <<  meshes[k]->getTriVertId1( i ) << "," <<  meshes[k]->getTriVertId2( i ) << ",";
        }
        size_t i = meshes[k]->triangleSize() - 1;
        dataFile << meshes[k]->getTriVertId0( i ) << "," <<  meshes[k]->getTriVertId1( i ) << "," <<  meshes[k]->getTriVertId2( i ) << "]";


        if( m_writeColors->get() )
        {
            dataFile << ",\n";
            dataFile << ( "    \"colors\" : [" );
            for( size_t j = 0; j < meshes[k]->vertSize() - 1; ++j )
            {
                osg::Vec4 color = meshes[k]->getVertColor( j );

                dataFile << color[0] << "," <<  color[1] << "," <<  color[2] << "," <<  color[3] << ",";
            }
            osg::Vec4 color = meshes[k]->getVertColor( meshes[k]->vertSize() - 1 );
            dataFile << color[0] << "," <<  color[1] << "," <<  color[2] << "," <<  color[3] << "]";
        }
        dataFile <<  "\n}";

        dataFile.close();
    }
    WLogger::getLogger()->addLogMessage( "saving done", "Write Mesh", LL_DEBUG );
    return true;
}

std::vector< boost::shared_ptr< WTriangleMesh > >WMWriteMesh::splitMesh( boost::shared_ptr< WTriangleMesh > triMesh, size_t targetSize )
{
    std::vector< boost::shared_ptr< WTriangleMesh > >meshes;
    if( triMesh->vertSize() <= targetSize )
    {
        meshes.push_back( triMesh );
        return meshes;
    }
    size_t currentTri( 0 );
    size_t id0, id1, id2;

    while( currentTri < triMesh->triangleSize() )
    {
        boost::shared_ptr< WTriangleMesh > newMesh( new WTriangleMesh( 0, 0 ) );
        std::vector<int>newIds( triMesh->vertSize(), -1 );
        while( newMesh->vertSize() < targetSize )
        {
            id0 = triMesh->getTriVertId0( currentTri );
            id1 = triMesh->getTriVertId1( currentTri );
            id2 = triMesh->getTriVertId2( currentTri );
            if( newIds[id0] == -1 )
            {
                newIds[id0] = newMesh->vertSize();
                osg::Vec3 vert = triMesh->getTriVert( currentTri, 0 );
                newMesh->addVertex( vert.x(), vert.y(), vert.z() );
                newMesh->setVertexColor( newMesh->vertSize() - 1, triMesh->getVertColor( id0 ) );
            }
            if( newIds[id1] == -1 )
            {
                newIds[id1] = newMesh->vertSize();
                osg::Vec3 vert = triMesh->getTriVert( currentTri, 1 );
                newMesh->addVertex( vert.x(), vert.y(), vert.z() );
                newMesh->setVertexColor( newMesh->vertSize() - 1, triMesh->getVertColor( id1 ) );
            }
            if( newIds[id2] == -1 )
            {
                newIds[id2] = newMesh->vertSize();
                osg::Vec3 vert = triMesh->getTriVert( currentTri, 2 );
                newMesh->addVertex( vert.x(), vert.y(), vert.z() );
                newMesh->setVertexColor( newMesh->vertSize() - 1, triMesh->getVertColor( id2 ) );
            }
            newMesh->addTriangle( newIds[id0], newIds[id1], newIds[id2] );
            ++currentTri;
            if( currentTri == triMesh->triangleSize() )
            {
                break;
            }
        }

        meshes.push_back( newMesh );
    }
    return meshes;
}
