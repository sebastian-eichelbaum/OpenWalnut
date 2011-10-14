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

#include "core/dataHandler/WFiberAccumulator.h"

#include "core/kernel/WKernel.h"

#include "WMFiberTranslator.xpm"

#include "WMFiberTranslator.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMFiberTranslator )

WMFiberTranslator::WMFiberTranslator():
    WModule()
{
}

WMFiberTranslator::~WMFiberTranslator()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMFiberTranslator::factory() const
{
    return boost::shared_ptr< WModule >( new WMFiberTranslator() );
}

const char** WMFiberTranslator::getXPMIcon() const
{
    return WMFiberTranslator_xpm;
}
const std::string WMFiberTranslator::getName() const
{
    return "Fiber translator";
}

const std::string WMFiberTranslator::getDescription() const
{
    return "Creates a fiber dataset from various data sources.";
}

void WMFiberTranslator::connectors()
{
    m_output = boost::shared_ptr< WModuleOutputData< WDataSetFibers > >(
            new WModuleOutputData< WDataSetFibers >( shared_from_this(), "outFibers", "" ) );

    addConnector( m_output );

    WModule::connectors();
}

void WMFiberTranslator::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_propInputDirectory = m_properties->addProperty( "Input data directory", "A directory containing .txt files, each file"
                                                      " contains the coordinates of one fiber (e.g. produced by mrtrix).",
                                                       boost::filesystem::path( "/" ), m_propCondition );
    WPropertyHelper::PC_PATHEXISTS::addTo( m_propInputDirectory );
    WPropertyHelper::PC_ISDIRECTORY::addTo( m_propInputDirectory );

    m_propInputFile = m_properties->addProperty( "Input txt file", "A .txt file that contains the fiber coords (ExploreDTI format).",
                                                 boost::filesystem::path( "/" ), m_propCondition );
    WPropertyHelper::PC_PATHEXISTS::addTo( m_propInputFile );

    WModule::properties();
}

void WMFiberTranslator::requirements()
{
}

void WMFiberTranslator::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );

    ready();

    // remove changed flag
    m_propInputDirectory->get( true );

    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if ( m_shutdownFlag() )
        {
            break;
        }

        if( m_propInputDirectory->changed() )
        {
            boost::shared_ptr< WDataSetFibers > fibers = mergeFibers( m_propInputDirectory->get( true ) );
            // boost::shared_ptr< WDataSetFibers > fibers = mergeFibersBinary( m_propInputDirectory->get( true ) );
            if( fibers )
            {
                m_output->updateData( fibers );
            }
        }

        if( m_propInputFile->changed() )
        {
            boost::shared_ptr< WDataSetFibers > fibers = loadExploreDTIFibers( m_propInputFile->get( true ) );
            if( fibers )
            {
                m_output->updateData( fibers );
            }
        }
    }
}

boost::shared_ptr< WDataSetFibers > WMFiberTranslator::mergeFibers( boost::filesystem::path dir )
{
    debugLog() << "Merging fibers.";

    if( !boost::filesystem::exists( dir ) || !boost::filesystem::is_directory( dir ) )
    {
        errorLog() << "Directory " << dir.file_string() << " does not exist!";
        return boost::shared_ptr< WDataSetFibers >();
    }

    WFiberAccumulator accu;

    boost::filesystem::directory_iterator it( dir );
    boost::filesystem::directory_iterator end;
    while( it != end )
    {
        std::vector< WVector3d > fiber;

        if( boost::filesystem::is_directory( it->path() ) || it->path().extension() != ".txt" )
        {
            debugLog() << "Ignoring file " << it->path();
            continue;
        }

        std::fstream f( it->path().file_string().c_str(), std::ios::in );

        WVector3d v;
        while( f >> v[ 0 ] >> v[ 1 ] >> v[ 2 ] )
        {
            fiber.push_back( v );
        }

        f.close();

        accu.add( fiber );

        ++it;
    }

    debugLog() << "Finished.";

    return accu.buildDataSet();
}

boost::shared_ptr< WDataSetFibers > WMFiberTranslator::mergeFibersNrrd( boost::filesystem::path dir )
{
    debugLog() << "Merging fibers (binary).";

    if( !boost::filesystem::exists( dir ) || !boost::filesystem::is_directory( dir ) )
    {
        errorLog() << "Directory " << dir.file_string() << " does not exist!";
        return boost::shared_ptr< WDataSetFibers >();
    }

    WFiberAccumulator accu;

    boost::filesystem::directory_iterator it( dir );
    boost::filesystem::directory_iterator end;
    while( it != end )
    {
        std::vector< WVector3d > fiber;

        if( boost::filesystem::is_directory( it->path() ) )
        {
            debugLog() << "Ignoring file " << it->path();
            continue;
        }

        std::fstream f( it->path().file_string().c_str(), std::ios::in | std::ios::binary );

        WVector3d v;
        while( f >> v[ 0 ] >> v[ 1 ] >> v[ 2 ] )
        {
            fiber.push_back( v );
        }

        f.close();

        infoLog() << "Fiber with " << fiber.size() << " points.";

        accu.add( fiber );

        ++it;
    }

    debugLog() << "Finished.";

    return accu.buildDataSet();
}

boost::shared_ptr< WDataSetFibers > WMFiberTranslator::loadExploreDTIFibers( boost::filesystem::path file )
{
    debugLog() << "Loading fibers from ExploreDTI text format.";

    if( !boost::filesystem::exists( file ) || boost::filesystem::is_directory( file ) )
    {
        errorLog() << "File " << file.file_string() << " does not exist!";
        return boost::shared_ptr< WDataSetFibers >();
    }

    // the file should end in "_coordinates.txt"
    if( file.file_string().length() < 16 || file.file_string().find( "_coordinates.txt" ) != file.file_string().length() - 16 )
    {
        errorLog() << "File " << file << " does not end in _coordinates.txt. Ignoring.";
        return boost::shared_ptr< WDataSetFibers >();
    }

    // get the name of the corresponding fiber length file
    std::string lengthFile = file.file_string();
    lengthFile.resize( lengthFile.length() - 16 );
    lengthFile += "_lengths.txt";

    WFiberAccumulator accu;

    // open files
    std::fstream coords( file.file_string().c_str(), std::ios::in );
    std::fstream lengths( lengthFile.c_str(), std::ios::in );

    while( !lengths.eof() && !lengths.bad() )
    {
        double l;
        lengths >> l;

        if( l < 2.0 )
        {
            errorLog() << "Lengths file contains an invalid length of " << l << ", aborting.";
            return boost::shared_ptr< WDataSetFibers >();
        }

        std::vector< WVector3d > fiber;
        for( int i = 0; i < static_cast< int >( l ); ++i )
        {
            WVector3d v;
            coords >> v[ 0 ] >> v[ 1 ] >> v[ 2 ];
            fiber.push_back( v );
        }

        accu.add( fiber );
    }

    lengths.close();
    coords.close();

    debugLog() << "Finished.";

    return accu.buildDataSet();
}
