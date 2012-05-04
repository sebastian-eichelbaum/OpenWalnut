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

#include <fstream>
#include <string>

#include "core/common/WPathHelper.h"
#include "core/dataHandler/WDataSetDipole.h"
#include "core/dataHandler/exceptions/WDHIOFailure.h"
#include "core/kernel/WKernel.h"
#include "modules/readDipoles/WMReadDipoles.xpm"

#include "WMReadDipoles.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMReadDipoles )

WMReadDipoles::WMReadDipoles():
    WModule()
{
}

WMReadDipoles::~WMReadDipoles()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMReadDipoles::factory() const
{
    return boost::shared_ptr< WModule >( new WMReadDipoles() );
}

const char** WMReadDipoles::getXPMIcon() const
{
    return WMReadDipoles_xpm; // Please put a real icon here.
}
const std::string WMReadDipoles::getName() const
{
    return "Read Dipoles";
}

const std::string WMReadDipoles::getDescription() const
{
    return "Reading \".dip\" files containing position and additional information on dipoles reconstructed from EEG..";
}

void WMReadDipoles::connectors()
{
    m_dipoles = boost::shared_ptr< WModuleOutputData< WDataSetDipole > >( new WModuleOutputData< WDataSetDipole >(
        shared_from_this(), "Dipoles", "The loaded dipoles reconstructed from EEG." ) );
    addConnector( m_dipoles );

    WModule::connectors();
}

void WMReadDipoles::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );
    m_dataFile = m_properties->addProperty( "File", "", WPathHelper::getAppPath(), m_propCondition );
    WPropertyHelper::PC_PATHEXISTS::addTo( m_dataFile );

    WModule::properties();
}

void WMReadDipoles::requirements()
{
    // Put the code for your requirements here. See "src/modules/template/" for an extensively documented example.
}

void WMReadDipoles::moduleMain()
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

        boost::shared_ptr< WProgress > progress = boost::shared_ptr< WProgress >( new WProgress( "Read Dipoles", 2 ) );
        ++*progress;
        m_dataSet = readData( m_dataFile->get().string() );
        ++*progress;
        m_dipoles->updateData( m_dataSet );
        progress->finish();
    }
}


boost::shared_ptr< WDataSetDipole > WMReadDipoles::readData( std::string filename )
{
    std::ifstream ifs;
    ifs.open( filename.c_str(), std::ifstream::in );
    if( !ifs || ifs.bad() )
    {
        throw WDHIOFailure( std::string( "Internal error while opening file" ) );
    }

    std::string line;
    std::vector< std::string > tokens;

    std::getline( ifs, line, '\n' );

    while( line.find( "NumberTimeSteps" ) )
    {
       std::getline( ifs, line, '\n' );
    }
    tokens = string_utils::tokenize( line );
    size_t nbTimeSteps = string_utils::fromString< size_t >( tokens[1].c_str() );

    std::getline( ifs, line, '\n' );

    while( line.find( "TimeSteps" ) )
    {
       std::getline( ifs, line, '\n' );
    }
    tokens = string_utils::tokenize( line );
    tokens = string_utils::tokenize( tokens[1], "()" );
    float timeFirst = string_utils::fromString< float >( tokens[0].c_str() );
    float timeDistance = string_utils::fromString< float >( tokens[1].c_str() );
    float timeLast = string_utils::fromString< float >( tokens[2].c_str() );
    std::vector< float > times( nbTimeSteps );
    for( size_t timeStep = 0; timeStep < nbTimeSteps; ++timeStep )
    {
        times[timeStep] = timeFirst + timeStep * timeDistance;
    }
    std::cout<<times[nbTimeSteps-1]<< " " << timeLast << std::endl;
    WAssert( std::abs( times[nbTimeSteps-1] - timeLast ) < 1e-4, "Error during filling times vector." );

    while( line.find( "PositionsFixed" ) )
    {
       std::getline( ifs, line, '\n' );
    }

    WPosition pos;
    ifs >> pos[0] >> pos[1] >> pos[2];

    std::vector< float > magnitudes;

    while( line.find( "Magnitudes" ) )
    {
       std::getline( ifs, line, '\n' );
    }
    std::getline( ifs, line, '\n' );

    tokens = string_utils::tokenize( line );
    for( unsigned int tokenId = 0; tokenId < tokens.size(); ++tokenId )
    {
        magnitudes.push_back( string_utils::fromString< float >( tokens[tokenId].c_str() ) );
    }

    WAssert( magnitudes.size() == nbTimeSteps, "Number of time steps and magnitudes must be equal" );
    WAssert( times.size() == nbTimeSteps, "Number of time steps and times must be equal" );

    ifs.close();

    boost::shared_ptr< WDataSetDipole > loadedData( new WDataSetDipole( pos, magnitudes, times ) );

    return loadedData;
}
