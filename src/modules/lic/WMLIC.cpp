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

#include "../../kernel/WKernel.h"
#include "WMLIC.h"

WMLIC::WMLIC():
    WModule()
{
}

WMLIC::~WMLIC()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMLIC::factory() const
{
    return boost::shared_ptr< WModule >( new WMLIC() );
}

const std::string WMLIC::getName() const
{
    return "LIC";
}

const std::string WMLIC::getDescription() const
{
    return "Line integrate convolution on TriangleMesh with a WDataSetVector";
}

void WMLIC::connectors()
{
    m_meshIC = boost::shared_ptr< WModuleInputData < WTriangleMesh2 > >(
            new WModuleInputData< WTriangleMesh2 >( shared_from_this(),
                "inMesh", "The triangle mesh used for painting the LIC" )
            );
    m_vectorIC = boost::shared_ptr< WModuleInputData < WDataSetVector > >(
            new WModuleInputData< WDataSetVector >( shared_from_this(),
                "inVectorDS", "The vectors used for computing the Streamlines used for the LIC" )
            );
    m_meshOC = boost::shared_ptr< WModuleOutputData < WTriangleMesh2 > >(
            new WModuleOutputData< WTriangleMesh2 >( shared_from_this(),
                "outMesh", "The LIC" )
            );

    addConnector( m_meshIC );
    addConnector( m_vectorIC );
    addConnector( m_meshOC );
    WModule::connectors();
}

void WMLIC::properties()
{
}

void WMLIC::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_meshIC->getDataChangedCondition() );
    m_moduleState.add( m_vectorIC->getDataChangedCondition() );

    ready();

    while ( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if ( m_shutdownFlag() )
        {
            break;
        }

        boost::shared_ptr< WTriangleMesh2 > newMesh = m_meshIC->getData();
        boost::shared_ptr< WDataSetVector > newVector = m_vectorIC->getData();
        bool dataChanged = ( m_inMesh != newMesh ) || ( m_inVector != newVector );
        bool dataValid   = ( newMesh.get() && newVector.get() );

        if ( dataChanged && dataValid )
        {
            debugLog() << "Received Data.";
            m_inMesh = newMesh;
            m_inVector = newVector;
        }
    }
}
