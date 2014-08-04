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

#include <fstream>
#include <string>
#include <vector>

#include "core/kernel/WKernel.h"
#include "core/kernel/WDataModuleInputFile.h"
#include "core/kernel/WDataModuleInputFilterFile.h"
#include "core/common/WPathHelper.h"
#include "core/common/WStringUtils.h"

#include "WMReadVIM.h"

// This line is needed by the module loader to actually find your module. You need to add this to your module too. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMReadVIM )

WMReadVIM::WMReadVIM():
    WDataModule(),
    m_reload( false )
{
    // Init
}

WMReadVIM::~WMReadVIM()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMReadVIM::factory() const
{
    return boost::shared_ptr< WModule >( new WMReadVIM() );
}

const std::string WMReadVIM::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "Read VIM";
}

const std::string WMReadVIM::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    return "This module reads VIM files containing particle data.";
}

void WMReadVIM::connectors()
{
    m_output = WModuleOutputData < WDataSetPoints  >::createAndAdd( shared_from_this(), "out", "The loaded dataset" );

    // call WModule's initialization
    WModule::connectors();
}

void WMReadVIM::moduleMain()
{
    m_moduleState.setResetable( true, true );

    // Signal ready state. Now your module can be connected by the container, which owns the module.
    ready();
    waitRestored();

    // main loop
    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        // woke up since the module is requested to finish
        if( m_shutdownFlag() )
        {
            break;
        }

        if( m_reload )
        {
            load();
        }
    }
}

std::vector< WDataModuleInputFilter::ConstSPtr > WMReadVIM::getInputFilter() const
{
    std::vector< WDataModuleInputFilter::ConstSPtr > filters;

    // NOTE: plain extension. No wildcards or prefixing "."!
    filters.push_back( WDataModuleInputFilter::ConstSPtr( new WDataModuleInputFilterFile( "vim", "VIM point cloud files" ) ) );

    return filters;
}

void WMReadVIM::handleInputChange()
{
    // notify the module only
    m_reload = true;
    m_moduleState.notify();
}

void WMReadVIM::load()
{
    m_reload = false;

    // open file
    WDataModuleInputFile::SPtr inputFile = getInputAs< WDataModuleInputFile >();
    if( !inputFile )
    {
        // No input? Reset output too.
        m_output->updateData( WDataSetPoints::SPtr() );
        return;
    }
    boost::filesystem::path p = inputFile->getFilename();

    std::ifstream ifs;
    ifs.open( p.string().c_str(), std::ifstream::in );
    if( !ifs || ifs.bad() )
    {
        errorLog() << "Could not open file \"" << p.string() << "\".";
        return;
    }

    boost::shared_ptr< WProgress > progress1( new WProgress( "Loading" ) );
    m_progress->addSubProgress( progress1 );

    // target memory
    WDataSetPoints::VertexArray vertices( new WDataSetPoints::VertexArray::element_type() );
    WDataSetPoints::ColorArray colors( new WDataSetPoints::ColorArray::element_type() );
    WBoundingBox bb;

    infoLog() << "Start Loading ...";

    // interpret file
    std::string line;
    std::vector< std::string > tokens;
    size_t numPoints = 0;
    while( !ifs.eof() )
    {
        std::getline( ifs, line );
        tokens = string_utils::tokenize( line );

        if( ( tokens.size() == 9 ) && ( tokens[ 0 ] == "!" ) )    // mathc
        {
            // coordinate:
            WVector3f coord(
                string_utils::fromString< float >( tokens[2] ),
                string_utils::fromString< float >( tokens[3] ),
                string_utils::fromString< float >( tokens[4] )
            );

            // expand bb
            bb.expandBy( coord );

            // read 3rd to 5th number
            vertices->push_back( coord.x() );
            vertices->push_back( coord.y() );
            vertices->push_back( coord.z() );
            colors->push_back( 1.0 );
            colors->push_back( 1.0 );
            colors->push_back( 1.0 );

            numPoints++;
        }
    }

    infoLog() << "Loaded " << numPoints << " points from file. Done.";

    // finally provide output data
    boost::shared_ptr< WDataSetPoints> newOutput( new WDataSetPoints( vertices, colors, bb ) );
    m_output->updateData( newOutput );

    // done. close file and report finish
    progress1->finish();
    ifs.close();
}

