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

#include <limits>
#include <string>
#include <vector>
#include <fstream>

#include "core/common/WIOTools.h"
#include "core/common/WStringUtils.h"
#include "core/common/WPathHelper.h"
#include "core/common/WPropertyHelper.h"
#include "core/kernel/WKernel.h"
#include "core/graphicsEngine/WTriangleMesh.h"

#include "WMeshReaderVTK.h"
#include "WMeshReaderFiberNavigator.h"
#include "WMeshReaderDIP.h"
#include "WMeshReaderBrainVISA.h"
#include "WMeshReaderFreesurfer.h"
#include "WMeshReaderOBJ.h"

#include "WMReadMesh.h"
#include "WMReadMesh.xpm"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMReadMesh )

WMReadMesh::WMReadMesh():
    WModule(),
    m_strategy( "Mesh Type", "Select one of the mesh types and configure it to your needs.", NULL,
                             "Loader", "A list of all known mesh loader." )
{
    // add some strategies here
    m_strategy.addStrategy( WMeshReaderVTK::SPtr( new WMeshReaderVTK() ) );
    m_strategy.addStrategy( WMeshReaderFiberNavigator::SPtr( new WMeshReaderFiberNavigator() ) );
    m_strategy.addStrategy( WMeshReaderDIP::SPtr( new WMeshReaderDIP() ) );
    m_strategy.addStrategy( WMeshReaderBrainVISA::SPtr( new WMeshReaderBrainVISA() ) );
    m_strategy.addStrategy( WMeshReaderFreesurfer::SPtr( new WMeshReaderFreesurfer() ) );
    m_strategy.addStrategy( WMeshReaderOBJ::SPtr( new WMeshReaderOBJ() ) );
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
    return "Loads a triangle mesh from several types of files.";
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
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );
    m_meshFile = m_properties->addProperty( "Mesh file", "", WPathHelper::getAppPath() );
    WPropertyHelper::PC_PATHEXISTS::addTo( m_meshFile );

    // now, setup the strategy helper.
    m_properties->addProperty( m_strategy.getProperties() );

    m_readTriggerProp = m_properties->addProperty( "Do read",  "Press!", WPVBaseTypes::PV_TRIGGER_READY, m_propCondition );

    m_nbTriangles = m_infoProperties->addProperty( "Triangles", "The number of triangles in the loaded mesh.", 0 );
    m_nbTriangles->setMax( std::numeric_limits< int >::max() );

    m_nbVertices = m_infoProperties->addProperty( "Vertices", "The number of vertices in the loaded mesh.", 0 );
    m_nbVertices->setMax( std::numeric_limits< int >::max() );

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

        // issue the current strategy
        m_triMesh = m_strategy()->operator()( m_progress, m_meshFile->get() );

        // update the info
        m_nbTriangles->set( m_triMesh->triangleSize() );
        m_nbVertices->set( m_triMesh->vertSize() );

        // update the output
        m_output->updateData( m_triMesh );

        // ready to be triggered again
        m_readTriggerProp->set( WPVBaseTypes::PV_TRIGGER_READY, true );
    }
}

