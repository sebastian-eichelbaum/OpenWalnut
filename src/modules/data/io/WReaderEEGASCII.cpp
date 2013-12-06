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
#include <iostream>
#include <string>
#include <vector>


#include "core/common/WAssert.h"
#include "core/common/WException.h"
#include "core/common/WStringUtils.h"
#include "core/dataHandler/WEEG.h"
#include "core/dataHandler/WSubject.h"

#include "WReaderEEGASCII.h"

WReaderEEGASCII::WReaderEEGASCII( std::string fileName )
    : WReaderEEG( fileName )
{
}

boost::shared_ptr< WDataSet > WReaderEEGASCII::load()
{
    std::ifstream in( m_fname.c_str() );
    if( in.fail() )
        throw WException( std::string( "Could not read file \"" + m_fname + "\"" ) );

    std::string tmp;
    getline( in, tmp );
    std::vector< std::string > tokens = string_utils::tokenize( tmp );

    const unsigned int nbChannels = tokens.size() / 3;
    WEEGChannelLabels labels( nbChannels );
    for( unsigned int i = 0; i < nbChannels; ++i )
    {
        labels[i].first = tokens[3*i] + " " + tokens[3*i+1] + " " + tokens[3*i+2];
        // TODO(wiebel): set second channel
    }

    WEEGSegmentArray segments( 1 );
    segments[0].clear();
    for( unsigned int i = 0; i < nbChannels; ++i )
    {
        segments[0].push_back( WEEGElectrode( 0 ) );
    }

    getline( in, tmp );
    while( !in.eof() )
    {
        tokens = string_utils::tokenize( tmp );
        WAssert( tokens.size() == nbChannels, "Error." );
        for( unsigned int i = 0; i < nbChannels; ++i )
        {
            segments[0][i].push_back( string_utils::fromString< double >( tokens[i].c_str() ) );
        }
        getline( in, tmp );
    }

    in.close();


    WEEGElectrodeLibrary lib;  // TODO(wiebel): this is a dummy

    boost::shared_ptr< WEEG > eeg( new WEEG( segments, lib, labels ) );
    eeg->setFilename( m_fname );
    return eeg;
}
