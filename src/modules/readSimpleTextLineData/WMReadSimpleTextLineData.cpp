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
#include <map>
#include <algorithm>

#include <boost/foreach.hpp>

#include "core/kernel/WKernel.h"
#include "core/kernel/WDataModuleInputFile.h"
#include "core/kernel/WDataModuleInputFilterFile.h"
#include "core/common/WPathHelper.h"
#include "core/common/WStringUtils.h"

#include "WMReadSimpleTextLineData.h"

// This line is needed by the module loader to actually find your module. You need to add this to your module too. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMReadSimpleTextLineData )

WMReadSimpleTextLineData::WMReadSimpleTextLineData():
    WDataModule(),
    m_reload( false )
{
    // Init
}

WMReadSimpleTextLineData::~WMReadSimpleTextLineData()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMReadSimpleTextLineData::factory() const
{
    return boost::shared_ptr< WModule >( new WMReadSimpleTextLineData() );
}

const std::string WMReadSimpleTextLineData::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "Read Simple Text Line Data";
}

const std::string WMReadSimpleTextLineData::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    return "This module reads simple text-based line files.";
}

void WMReadSimpleTextLineData::connectors()
{
    m_output = WModuleOutputData < WDataSetFibers  >::createAndAdd( shared_from_this(), "out", "The loaded dataset" );

    // call WModule's initialization
    WModule::connectors();
}

void WMReadSimpleTextLineData::moduleMain()
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

std::vector< WDataModuleInputFilter::ConstSPtr > WMReadSimpleTextLineData::getInputFilter() const
{
    std::vector< WDataModuleInputFilter::ConstSPtr > filters;

    // NOTE: plain extension. No wildcards or prefixing "."!
    filters.push_back( WDataModuleInputFilter::ConstSPtr( new WDataModuleInputFilterFile( "stld", "Simple text-based line data files" ) ) );

    return filters;
}

void WMReadSimpleTextLineData::handleInputChange()
{
    // notify the module only
    m_reload = true;
    m_moduleState.notify();
}

void WMReadSimpleTextLineData::load()
{
    /*
     * Tiny Format Description
     *
     * Purpose: easy write, easy read -> interchange data between different tools.
     *
     * Storing line-strips as a bunch of points and the corresponding point-index-list per strip.
     *
     * A point in the file starts with "P", followed by space and three coordinates (space separated too).
     * A line strip starts with "L", followed by space and a list of indices (to points).
     * Everything else is ignored right now.
     *
     * Example:
     *
     * # Comment
     * P 134 123.55 122.4563213123
     * P 1.34 1.2355 1.2245
     * P 3.34 14.2355 12.2
     * L 0 2 1
     * L 0 1
     * L 2 0
     */
    m_reload = false;

    // open file
    WDataModuleInputFile::SPtr inputFile = getInputAs< WDataModuleInputFile >();
    if( !inputFile )
    {
        // No input? Reset output too.
        m_output->updateData( WDataSetFibers::SPtr() );
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


    // Keep track of the BB
    WBoundingBox bb;

    // Walk through the file -> line by line
    std::string line;
    std::vector< std::string > tokens;
    size_t numLines = 0;

    // Some storage for loading the data
    std::vector< WVector3f > loadedVertices;
    typedef std::vector< size_t > LineStrip;
    std::vector< LineStrip > loadedLineStrips;
    std::map< size_t, std::vector< double > > loadedPointAttribs;
    std::map< size_t, std::vector< double > > loadedLineAttribs;

    while( !ifs.eof() )
    {
        std::getline( ifs, line );
        tokens = string_utils::tokenize( string_utils::trim( line ) );
        numLines++;

        // first token describes meaning.
        // NOTE: be kind and ignore unknown tokens (like comments)
        if( tokens.size() < 2 ) // < 2? Each line is at least a describing token and a value for it == 2
        {
            continue;
        }

        // It is a point.
        if( string_utils::toLower( tokens[0] ) == "p" )
        {
            WVector3f coord(
                string_utils::fromString< float >( tokens[1] ),
                string_utils::fromString< float >( tokens[2] ),
                string_utils::fromString< float >( tokens[3] )
            );

            // expand bb
            bb.expandBy( coord );

            loadedVertices.push_back( coord );
        }

        // It is a line.
        if( string_utils::toLower( tokens[0] ) == "l" )
        {
            LineStrip ls;

            // Copy all the indices into the linestrip vector
            for( size_t i = 1; i < tokens.size(); ++i )
            {
                size_t idx = string_utils::fromString< size_t >( tokens[i] );
                ls.push_back( idx );
            }

            // NOTE: a lot of copying, but when considering overall bad performance of hard disk IO vs RAM ...
            loadedLineStrips.push_back( ls );
        }

        // It is a point attribute.
        if( string_utils::toLower( tokens[0] ) == "pa" )
        {
            size_t attrIdx = string_utils::fromString< size_t >( tokens[1] );
            loadedPointAttribs[ attrIdx ].push_back( string_utils::fromString< float >( tokens[2] ) );
        }

        // It is a line attribute.
        if( string_utils::toLower( tokens[0] ) == "la" )
        {
            size_t attrIdx = string_utils::fromString< size_t >( tokens[1] );
            loadedLineAttribs[ attrIdx ].push_back( string_utils::fromString< float >( tokens[2] ) );
        }
    }

    // As the DataSetFibers uses run-length encoded linestrips, we need to transform the stuff now.

    // target memory
    WDataSetFibers::VertexArray vertices( new WDataSetFibers::VertexArray::element_type() );
    WDataSetFibers::LengthArray lengths( new WDataSetFibers::LengthArray::element_type() );
    WDataSetFibers::IndexArray lineStartIndices( new WDataSetFibers::IndexArray::element_type() );
    WDataSetFibers::IndexArray verticesReverse( new WDataSetFibers::IndexArray::element_type() );

    std::vector< WDataSetFibers::VertexParemeterArray > pAttribs;
    std::vector< WDataSetFibers::LineParemeterArray > lAttribs;

    for( std::map< size_t, std::vector< double > >::const_iterator i = loadedLineAttribs.begin(); i != loadedLineAttribs.end(); ++i )
    {
        size_t desiredSize = loadedLineStrips.size();
        size_t realSize = ( *i ).second.size();

        if( desiredSize != realSize )
        {
            warnLog() << "Ignoring line attribute " << ( *i ).first << " as there are too few/too much items.";
        }

        // Create and copy
        boost::shared_ptr< WDataSetFibers::LineParemeterArray::element_type > vec(
            new WDataSetFibers::LineParemeterArray::element_type( realSize ) );
        std::copy( ( *i ).second.begin(), ( *i ).second.end(), vec->begin() );
        lAttribs.push_back( vec );
    }

    // map between the indices in the source attribute list and the really used attributes
    std::map< size_t, size_t > pAttribIdxMap;
    for( std::map< size_t, std::vector< double > >::const_iterator i = loadedPointAttribs.begin(); i != loadedPointAttribs.end(); ++i )
    {
        size_t desiredSize = loadedVertices.size();
        size_t realSize = ( *i ).second.size();

        if( desiredSize != realSize )
        {
            warnLog() << "Ignoring point attribute " << ( *i ).first << " as there are too few/too much items.";
        }

        // Create and do NOT copy -> why? The vertex attributes need to be "de-indexed" below. So only create the target vector and go on.
        boost::shared_ptr< WDataSetFibers::VertexParemeterArray::element_type > vec(
            new WDataSetFibers::VertexParemeterArray::element_type() );
        pAttribIdxMap[ pAttribs.size() ] = ( *i ).first;
        pAttribs.push_back( vec );
    }


    size_t currentStartIndex = 0;

    // For each lineStrip, we need to add vertices and fill the run-lenght info in lengths and lineStartIndices.
    for( std::vector< LineStrip >::const_iterator iter = loadedLineStrips.begin(); iter != loadedLineStrips.end(); ++iter )
    {
        const LineStrip& ls = *iter;

        // For each index in the strip, resolve the indexed point coordinates.
        for( LineStrip::const_iterator indexIter = ls.begin(); ls.end() != indexIter; ++indexIter )
        {
            // Get loaded data
            size_t pIdx = *indexIter;
            WVector3f p = loadedVertices[ pIdx ];

            // Add point to vertices
            vertices->push_back( p[ 0 ] );
            vertices->push_back( p[ 1 ] );
            vertices->push_back( p[ 2 ] );

            for( std::vector< WDataSetFibers::VertexParemeterArray >::const_iterator i = pAttribs.begin(); i != pAttribs.end(); ++i )
            {
                size_t attrIdx = pAttribIdxMap[ i - pAttribs.begin() ];
                ( *i )->push_back( loadedPointAttribs[ attrIdx ][ pIdx ] );
            }

            // store the current line index for each vertex
            verticesReverse->push_back( iter - loadedLineStrips.begin() );
        }

        // set length of the strip
        lengths->push_back( ls.size() );
        // index where it starts inside the vertex array
        lineStartIndices->push_back( currentStartIndex );
        currentStartIndex += ls.size();
    }

    // bb.expandBy( WVector3f( 0, 0, 0 ) );

    WDataSetFibers::SPtr ds( new WDataSetFibers( vertices, lineStartIndices, lengths, verticesReverse, bb ) );
    ds->setVertexParameters( pAttribs );
    ds->setLineParameters( lAttribs );

    m_output->updateData( ds );

    // done. close file and report finish
    progress1->finish();
    ifs.close();

    infoLog() << "Loaded " << loadedLineStrips.size() << " line strips from file, having " << pAttribs.size() << " point attributes and "
              << lAttribs.size() << " line attributes. Done.";
}

