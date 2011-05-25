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

#include "core/common/math/WMath.h"
#include "core/common/WPathHelper.h"
#include "core/kernel/WKernel.h"
#include "../emptyIcon.xpm" // Please put a real icon here.
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
    return emptyIcon_xpm; // Please put a real icon here.
}
const std::string WMWriteMesh::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "WriteMesh";
}

const std::string WMWriteMesh::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    // See "src/modules/template/" for an extensively documented example.
    return "Someone should add some documentation here. "
    "Probably the best person would be the modules's creator, i.e. \"wiebel\"";
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
    m_savePropGroup = m_properties->addPropertyGroup( "Save Surface",  "" );
    m_saveTriggerProp = m_savePropGroup->addProperty( "Do Save",  "Press!",
                                                  WPVBaseTypes::PV_TRIGGER_READY );
    m_saveTriggerProp->getCondition()->subscribeSignal( boost::bind( &WMWriteMesh::save, this ) );

    m_meshFile = m_savePropGroup->addProperty( "Mesh File", "", WPathHelper::getAppPath() );

    WModule::properties();
}

void WMWriteMesh::moduleMain()
{
    // Put the code for your module's main functionality here.
    // See "src/modules/template/" for an extensively documented example.
    m_moduleState.add( m_meshInput->getDataChangedCondition() );

    // signal ready state
    ready();

    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        if( !m_meshInput->getData() )
        {
            // ok, the output has not yet sent data
            // NOTE: see comment at the end of this while loop for m_moduleState
            debugLog() << "Waiting for data ...";
            m_moduleState.wait();
            continue;
        }

        m_triMesh = m_meshInput->getData();

        // this waits for m_moduleState to fire. By default, this is only the m_shutdownFlag condition.
        // NOTE: you can add your own conditions to m_moduleState using m_moduleState.add( ... )
        m_moduleState.wait();
    }
}

bool WMWriteMesh::save() const
{
    m_saveTriggerProp->set( WPVBaseTypes::PV_TRIGGER_READY, false );

    if( !m_triMesh )
    {
        return false;
    }

    if( m_triMesh->vertSize() == 0 )
    {
        WLogger::getLogger()->addLogMessage( "Will not write file that contains 0 vertices.", "Marching Cubes", LL_ERROR );
        return false;
    }

    if( m_triMesh->triangleSize() == 0 )
    {
        WLogger::getLogger()->addLogMessage( "Will not write file that contains 0 triangles.", "Marching Cubes", LL_ERROR );
        return false;
    }

    const char* c_file = m_meshFile->get().file_string().c_str();
    std::ofstream dataFile( c_file, std::ios_base::binary );

    if( dataFile )
    {
        WLogger::getLogger()->addLogMessage( "opening file", "Marching Cubes", LL_DEBUG );
    }
    else
    {
        WLogger::getLogger()->addLogMessage( "open file failed" + m_meshFile->get().file_string() , "Marching Cubes", LL_ERROR );
        return false;
    }

    dataFile.precision( 16 );

    WLogger::getLogger()->addLogMessage( "start writing file", "Marching Cubes", LL_DEBUG );
    dataFile << ( "# vtk DataFile Version 2.0\n" );
    dataFile << ( "generated using OpenWalnut\n" );
    dataFile << ( "ASCII\n" );
    dataFile << ( "DATASET UNSTRUCTURED_GRID\n" );

    WPosition point;
    dataFile << "POINTS " << m_triMesh->vertSize() << " float\n";
    for( size_t i = 0; i < m_triMesh->vertSize(); ++i )
    {
        point = m_triMesh->getVertexAsPosition( i );
        if( !( myIsfinite( point[0] ) && myIsfinite( point[1] ) && myIsfinite( point[2] ) ) )
        {
            WLogger::getLogger()->addLogMessage( "Will not write file from data that contains NAN or INF.", "Marching Cubes", LL_ERROR );
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
