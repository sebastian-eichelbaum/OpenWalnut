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

#include "core/common/WPathHelper.h"
#include "core/kernel/WKernel.h"
#include "core/common/WStringUtils.h"

#include "WMReadVCL.h"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMReadVCL )

WMReadVCL::WMReadVCL():
    WModule()
{
}

WMReadVCL::~WMReadVCL()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMReadVCL::factory() const
{
    return boost::shared_ptr< WModule >( new WMReadVCL() );
}

const std::string WMReadVCL::getName() const
{
    return "Read VCL";
}

const std::string WMReadVCL::getDescription() const
{
    return "Read VCL line data and their attributes.";
}

void WMReadVCL::connectors()
{
    m_output = WModuleOutputData< WDataSetFibers >::createAndAdd( shared_from_this(), "out", "The data that has been loaded." );

    WModule::connectors();
}

void WMReadVCL::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );
    m_dataFile = m_properties->addProperty( "File", "File to load.", WPathHelper::getAppPath() );
    WPropertyHelper::PC_PATHEXISTS::addTo( m_dataFile );

    m_loadAttrib = m_properties->addProperty( "Load Attribute", "If true, load the specified attribute from file.", false );
    m_attrib = m_properties->addProperty( "Attribute", "Index of the attribute to use if there are any. Begins at 1.", 1 );
    m_attrib->setMin( 1 );

    m_readTriggerProp = m_properties->addProperty( "Do read",  "Press!", WPVBaseTypes::PV_TRIGGER_READY, m_propCondition );
    WModule::properties();
}

void WMReadVCL::moduleMain()
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

        debugLog() << "Loading " << m_dataFile->get().string() << ".";
        boost::shared_ptr< WProgress > progress = boost::shared_ptr< WProgress >( new WProgress( "Read Data" ) );
        m_progress->addSubProgress( progress );
        readData( m_dataFile->get().string() );
        m_output->updateData( m_dataSet );
        progress->finish();
        m_readTriggerProp->set( WPVBaseTypes::PV_TRIGGER_READY );
    }
}

void WMReadVCL::readData( std::string filename )
{
    std::ifstream ifs;
    ifs.open( filename.c_str(), std::ifstream::in );
    if( !ifs || ifs.bad() )
    {
        errorLog() << "Could not open file \"" << filename << "\".";
        return;
    }

    // target memory
    WDataSetFibers::VertexArray vertices( new WDataSetFibers::VertexArray::element_type() );
    WDataSetFibers::LengthArray lengths( new WDataSetFibers::LengthArray::element_type() );
    WDataSetFibers::IndexArray lineStartIndices( new WDataSetFibers::IndexArray::element_type() );
    WDataSetFibers::IndexArray verticesReverse( new WDataSetFibers::IndexArray::element_type() );
    WDataSetFibers::VertexParemeterArray attribs( new WDataSetFibers::VertexParemeterArray::element_type() );

    // Go through, line by line
    std::string line;
    std::vector< std::string > tokens;
    size_t numLines = 0;
    WBoundingBox bb;

    unsigned int attribIndex = m_attrib->get( true ) - 1;
    bool loadAttrib = m_loadAttrib->get( true );
    size_t currentIndex = 0;
    size_t currentLength = 0;
    size_t currentStartIndex = 0;
    size_t currentLineCount = 0;

    while( !ifs.eof() )
    {
        std::getline( ifs, line );
        tokens = string_utils::tokenize( line );
        numLines++;

        // point line
        if( ( tokens.size() >= 4 ) && ( tokens[ 0 ] == "p" ) )
        {
            // coordinate:
            WVector3f coord(
                string_utils::fromString< float >( tokens[1] ),
                string_utils::fromString< float >( tokens[2] ),
                string_utils::fromString< float >( tokens[3] )
            );

            // store attributes
            if( loadAttrib )
            {
                if( tokens.size() - 1 >= 4 + attribIndex )
                {
                    attribs->push_back( string_utils::fromString< double >( tokens[ 4 + attribIndex ] ) );
                }
                else
                {
                    attribs->push_back( 0.5 );
                }
            }

            // expand bb
            bb.expandBy( coord );

            vertices->push_back( coord.x() );
            vertices->push_back( coord.y() );
            vertices->push_back( coord.z() );
            verticesReverse->push_back( currentLineCount );

            // keep track of indices until we reach an "n" line.
            currentIndex++;
            currentLength++;
        }

        // new line? (or beginning of file
        if( ( tokens.size() == 1 ) && ( tokens[ 0 ] == "n" ) )
        {
            // new start index
            lineStartIndices->push_back( currentStartIndex );
            // record length of last line
            lengths->push_back( currentLength );

            currentStartIndex = currentIndex;
            currentLineCount++;
            currentLength = 0;
        }
    }

    ifs.close();

    debugLog() << " = Num Lines: " << currentLineCount;
    debugLog() << " = Num Vertices: " << currentIndex;

    // create dataset
    if( loadAttrib )
    {
        m_dataSet = WDataSetFibers::SPtr( new WDataSetFibers( vertices, lineStartIndices, lengths, verticesReverse, bb, attribs ) );
    }
    else
    {
        m_dataSet = WDataSetFibers::SPtr( new WDataSetFibers( vertices, lineStartIndices, lengths, verticesReverse, bb ) );
    }
}
