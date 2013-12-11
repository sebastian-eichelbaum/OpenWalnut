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

#include <cstdlib>
#include <fstream>
#include <list>
#include <stdint.h>
#include <string>
#include <vector>

#include <boost/regex.hpp>
#include <boost/shared_ptr.hpp>

#include "core/common/WAssert.h"
#include "core/common/WIOTools.h"
#include "core/common/WLimits.h"
#include "core/common/WLogger.h"
#include "core/common/WStringUtils.h"
#include "core/dataHandler/datastructures/WFiberCluster.h"
#include "core/dataHandler/exceptions/WDHIOFailure.h"
#include "core/dataHandler/exceptions/WDHNoSuchFile.h"
#include "core/dataHandler/exceptions/WDHParseError.h"
#include "core/dataHandler/WDataSetHierarchicalClustering.h"

#include "WReaderClustering.h"

WReaderClustering::WReaderClustering( std::string fname )
    : WReader( fname )
{
}

WReaderClustering::~WReaderClustering() throw()
{
}


boost::shared_ptr< WDataSetHierarchicalClustering > WReaderClustering::read()
{
    //Read file to line list
    boost::shared_ptr< std::ifstream > ifs( new std::ifstream() );
    ifs->open( m_fname.c_str(), std::ifstream::in | std::ifstream::binary );
    if( !ifs || ifs->bad() )
    {
        throw WDHIOFailure( std::string( "internal error while opening file" ) );
    }

    std::vector< std::string > lines;
    std::string line;


    while( !ifs->eof() )
    {
        getline( *ifs, line );
        lines.push_back( std::string( line ) );
    }

    ifs->close();

    for( size_t i = 0; i < lines.size() - 1; ++i )
    {
        std::string &ls = lines[i];

        boost::regex reg1( "\\(" );
        ls = boost::regex_replace( ls, reg1, "(," );

        boost::regex reg2( "\\)" );
        ls = boost::regex_replace( ls, reg2, ",)" );

        boost::regex reg3( "\\ " );
        ls = boost::regex_replace( ls, reg3, "" );
    }

    std::vector<std::string>svec;
    WTreeNode::SPtr rootNode;

    for( size_t i = 0; i < lines.size() - 1; ++i )
    {
        svec.clear();
        boost::regex reg( "," );
        boost::sregex_token_iterator it( lines[i].begin(), lines[i].end(), reg, -1 );
        boost::sregex_token_iterator end;
        while( it != end )
        {
            svec.push_back( *it++ );
        }

        size_t level = string_utils::fromString< size_t >( svec[1] );

        if( level == 0 )
        {
            std::list< size_t > indices;
            // start after ( level (
            size_t k = 3;
            while( svec[k] != ")" && svec[k] != "" )
            {
                indices.push_back( string_utils::fromString< size_t >( svec[k] ) );
                ++k;
            }
            WColor color( 1.0, 1.0, 0.0, 1.0 );
            WFiberCluster::SPtr cluster( new WFiberCluster( indices, color ) );
            m_clustering[ i ] = cluster;
            m_nodes[ i ] = WTreeNode::SPtr( new WTreeNode( i, level ) );
            rootNode = m_nodes[ i ];
        }
        else
        {
            std::list< size_t > indices;
            // start after ( level (
            size_t k = 3;
            while( svec[k] != ")" )
            {
                indices.push_back( string_utils::fromString< size_t >( svec[k] ) );
                ++k;
            }

            WColor color( 0.1 * ( i % 10 ), 0.1 * ( ( i + 5 ) % 10 ), 0.0, 1.0 );
            WFiberCluster::SPtr cluster( new WFiberCluster( indices, color ) );
            m_clustering[ i ] = cluster;
            m_nodes[ i ] = WTreeNode::SPtr( new WTreeNode( i, level ) );
            rootNode = m_nodes[ i ];

            //add Child cluster
            k += 2;
            std::vector< size_t > childIndices;
            while( svec[k] != ")" )
            {
                childIndices.push_back( string_utils::fromString< size_t >( svec[k] ) );
                ++k;
            }

            for( size_t j = 0; j < childIndices.size(); j++ )
            {
                WTreeNode::SPtr childNode = m_nodes[ childIndices[ j ] ];
                m_nodes[ i ]->addChild( childNode );
            }
        }
    }

    //Return the WDataSetHierarchicalClustering
    WDataSetHierarchicalClustering::SPtr clustering( new WDataSetHierarchicalClustering( rootNode, m_clustering ) );
    return clustering;
}



