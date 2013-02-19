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

#include "core/kernel/WKernel.h"
#include "core/common/datastructures/WFiber.h"


#include "WMWriteAmiraMesh.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMWriteAmiraMesh )

WMWriteAmiraMesh::WMWriteAmiraMesh():
    WModule()
{
}

WMWriteAmiraMesh::~WMWriteAmiraMesh()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMWriteAmiraMesh::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMWriteAmiraMesh() );
}

const std::string WMWriteAmiraMesh::getName() const
{
    return "Write Amira Mesh";
}

const std::string WMWriteAmiraMesh::getDescription() const
{
    return "Write AmiraMesh file format. At the moment only line sets are supported.";
}

void WMWriteAmiraMesh::connectors()
{
    // Put the code for your connectors here. See "src/modules/template/" for an extensively documented example.
    m_tractConnector = WModuleInputData< const WDataSetFibers >::createAndAdd( shared_from_this(), "tractInput", "A dataset of tracts" );

    WModule::connectors();
}

void WMWriteAmiraMesh::properties()
{
    // Put the code for your properties here. See "src/modules/template/" for an extensively documented example.

    m_savePath = m_properties->addProperty( "Save Path", "Where to save the result", boost::filesystem::path( "/no/such/file" ) );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_savePath );
    m_run      = m_properties->addProperty( "Save", "Start saving", WPVBaseTypes::PV_TRIGGER_READY );
    WModule::properties();
}

void WMWriteAmiraMesh::requirements()
{
}

void WMWriteAmiraMesh::moduleMain()
{
    m_moduleState.add( m_tractConnector->getDataChangedCondition() );
    m_moduleState.add( m_run->getCondition() );

    ready();

    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if( !m_tractConnector->getData() )
        {
            continue;
        }

        if( m_run->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            writeFile();
            m_run->set( WPVBaseTypes::PV_TRIGGER_READY, true );
        }
    }
}

void WMWriteAmiraMesh::writeFile()
{
    // open file
    boost::filesystem::path meshFile( m_savePath->get() );
    std::string fnPath = meshFile.string();

    debugLog() << "Opening " << fnPath << " for writing.";
    std::ofstream dataFile( fnPath.c_str(), std::ios_base::binary );
    if( !dataFile )
    {
        errorLog() << "Opening " << fnPath << " failed.";
        return;
    }

    // needed arrays for iterating the fibers
    //   WDataSetFibers::IndexArray  fibStart = fibers->getLineStartIndexes();
    WDataSetFibers::LengthArray fibLen   =  m_tractConnector->getData()->getLineLengths();
    WDataSetFibers::VertexArray fibVerts = m_tractConnector->getData()->getVertices();
    //WDataSetFibers::TangentArray fibTangents = fibers->getTangents();

    std::size_t numPoints = fibVerts->size() / 3;

    debugLog() << "Writing ...";

    // write some head data
    dataFile << "# AmiraMesh 3D ASCII 2.0" << std::endl << std::endl;

    dataFile << "define Lines " << numPoints + m_tractConnector->getData()->size() << std::endl;
    dataFile << "nVertices " << numPoints << std::endl;
    dataFile << std::endl;
    dataFile << "Parameters {" << std::endl;
    dataFile << "    ContentType \"HxLineSet\"" << std::endl;
    dataFile << "}" << std::endl;
    dataFile << std::endl;
    dataFile << "Lines { int LineIdx } @1" << std::endl;
    dataFile << "Vertices { float[3] Coordinates } @2" << std::endl;
    dataFile << std::endl;
    dataFile << "# Data section follows" << std::endl;
    dataFile << std::endl;
    dataFile << "@1" << std::endl;
    size_t vertCounter = 0;
    size_t lineId = 0;
    for( size_t vertId = 0; vertId < numPoints; ++vertId )
    {
        dataFile << vertId << std::endl;
        ++vertCounter;
        if( vertCounter == (*fibLen)[lineId] )
        {
            ++lineId;
            vertCounter = 0;
            dataFile << -1 << std::endl;
        }
    }
    dataFile << std::endl;
    dataFile << "@2" << std::endl;
    for( size_t vertId = 0; vertId < numPoints; ++vertId )
    {
        dataFile << (*fibVerts)[vertId*3+0] << " " << (*fibVerts)[vertId*3+1] << " " << (*fibVerts)[vertId*3+2] << " " << std::endl;
    }
    debugLog() << "Writing done.";
}
