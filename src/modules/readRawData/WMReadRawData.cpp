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
#include <fstream>

#include "core/common/WPathHelper.h"
//#include "core/common/WPropertyHelper.h"
#include "core/kernel/WKernel.h"
#include "WMReadRawData.xpm"

#include "WMReadRawData.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMReadRawData )

WMReadRawData::WMReadRawData():
    WModule()
{
}

WMReadRawData::~WMReadRawData()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMReadRawData::factory() const
{
    return boost::shared_ptr< WModule >( new WMReadRawData() );
}

const char** WMReadRawData::getXPMIcon() const
{
    return WMReadRawData_xpm; // Please put a real icon here.
}
const std::string WMReadRawData::getName() const
{
    return "Read Raw Data (currently 8 bit voxels only)";
}

const std::string WMReadRawData::getDescription() const
{
    return "Read data defined on uniform lattices"
        "in raw format, i.e., plain three-dimensional arrays of data.  (currently 8 bit voxels only)";
}

void WMReadRawData::connectors()
{
    m_output = boost::shared_ptr< WModuleOutputData< WDataSetScalar > >(
            new WModuleOutputData< WDataSetScalar >( shared_from_this(), "Data", "The loaded data." ) );

    addConnector( m_output );

    WModule::connectors();
}

void WMReadRawData::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );
    m_dataFile = m_properties->addProperty( "File", "", WPathHelper::getAppPath(), m_propCondition );
    WPropertyHelper::PC_PATHEXISTS::addTo( m_dataFile );

    m_X = m_properties->addProperty( "X", "Data sample in X direction.", 256 );
    m_X->setMin( 0 );
    m_X->setMax( 1024 );
    m_Y = m_properties->addProperty( "Y", "Data sample in Y direction.", 256 );
    m_Y->setMin( 0 );
    m_Y->setMax( 1024 );
    m_Z = m_properties->addProperty( "Z", "Data sample in Z direction.", 256 );
    m_Z->setMin( 0 );
    m_Z->setMax( 1024 );

    WModule::properties();
}

void WMReadRawData::requirements()
{
    // Put the code for your requirements here. See "src/modules/template/" for an extensively documented example.
}

void WMReadRawData::moduleMain()
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
        boost::shared_ptr< WProgress > progress = boost::shared_ptr< WProgress >( new WProgress( "Read Raw Data", 2 ) );
        ++*progress;
        m_dataSet = readData( m_dataFile->get().file_string() );
        ++*progress;
        m_output->updateData( m_dataSet );
        progress->finish();
    }
}


boost::shared_ptr< WDataSetScalar > WMReadRawData::readData( std::string fileName )
{
    size_t numX = m_X->get();
    size_t numY = m_Y->get();
    size_t numZ = m_Z->get();
    size_t numVoxels = numX * numY * numZ;
    boost::shared_ptr< std::vector< unsigned char > > values;
    values = boost::shared_ptr< std::vector< unsigned char > >( new std::vector< uint8_t>( numVoxels ) );


    std::ifstream ifs;
    ifs.open( fileName.c_str(), std::ifstream::in|std::ios::binary );
    if( !ifs || ifs.bad() )
    {
        WLogger::getLogger()->addLogMessage( "Try load broken file '" + fileName + "'", "Read Raw Data", LL_ERROR );
        throw std::runtime_error( "Problem during reading file. Probably file not found." );
    }

    unsigned char *pointData = new unsigned char[ numVoxels ];
    ifs.read( reinterpret_cast< char* >( pointData ), 3 * sizeof( unsigned char ) * numVoxels );

    for( size_t voxelId = 0; voxelId < numVoxels; ++voxelId )
    {
        (*values)[voxelId] = pointData[voxelId];
    }
    ifs.close();

    boost::shared_ptr< WValueSetBase > newValueSet;
    newValueSet =  boost::shared_ptr< WValueSetBase >( new WValueSet< unsigned char >( 0, 1, values, W_DT_UNSIGNED_CHAR ) );
    boost::shared_ptr< WGridRegular3D > newGrid;
    newGrid = boost::shared_ptr< WGridRegular3D >( new WGridRegular3D( numX, numY, numZ ) );
    boost::shared_ptr< WDataSetScalar > newDataSet;
    newDataSet = boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( newValueSet, newGrid ) );
    return newDataSet;
}
