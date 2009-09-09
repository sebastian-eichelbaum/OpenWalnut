//---------------------------------------------------------------------------
//
// Project: OpenWalnut
//
// Copyright 2009 SomeCopyrightowner
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

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>


#include "WLoaderBiosig.h"
#include "../WDataHandler.h"
#include "../WMetaInfo.h"
#include "../WEEG.h"

typedef std::vector< double > WEEGElectrode;
typedef std::vector< WEEGElectrode > WEEGSegment;
typedef std::vector< WEEGSegment > WEEGSegmentArray;  // kann ich das auch irgendwie aus WEEG.h bekommen?

WLoaderBiosig::WLoaderBiosig( std::string fileName, boost::shared_ptr< WDataHandler > dataHandler )
    : WLoader( fileName ),
      m_dataHandler( dataHandler ),
      m_columns( 0 ),
      m_rows( 0 )
{
}

void WLoaderBiosig::fillSegment( std::vector<std::vector<double> >* segment, biosig_data_type* data, size_t nbSamples )
{
    for( unsigned int i = 0; i < m_rows; ++i )
    {
        WEEGElectrode channel( 0 );
        for( unsigned int j = 0; j < m_columns; ++j )
        {
            channel.push_back( data[i*m_columns+j] );
        }
        segment->push_back( channel );
    }
}
void WLoaderBiosig::biosigLoader()
{
    std::cout << "BIOSIGLOADER" << std::endl;
    HDRTYPE* hd;
    hd =  sopen( m_fileName.c_str(), "r", 0 );

#warning check if format is supported here
    std::cout << "BIOSIG ERRNUM       : " << B4C_ERRNUM << std::endl;
    std::cout << "BIOSIG Filename     : " << hd->FileName << std::endl;
    std::cout << "BIOSIG Type         : " << hd->TYPE << " " << EEProbe << std::endl;
    std::cout << "BIOSIG HeaderLength : " << hd->HeadLen << std::endl;
    std::cout << "BIOSIG NbChannels   : " << hd->NS << std::endl;
    std::cout << "BIOSIG SamsPerBlock : " << hd->SPR << std::endl;
    std::cout << "BIOSIG NbBlocks     : " << hd->NRec << std::endl;
    if( hd->FLAG.ROW_BASED_CHANNELS )
        std::cout << "BIOSIG channels stored as rows." << std::endl;
    else
        std::cout << "BIOSIG channels stored as cols." << std::endl;
    biosig_data_type* DATA = 0;
    size_t LEN = 1;
#warning LOOK TO Notizen.txt on the Desktop.
#warning provides start and length parameters ...
#warning these should be used as EEG might not fit or at
#warning least is not intended to be completely in the main memory.
    size_t alex = sread( DATA, 0, LEN, hd );
    size_t nbSamples = LEN*hd->SPR*hd->NS;
    m_columns =  hd->data.size[0];
    m_rows = hd->data.size[1];
    std::cout << "BIOSIG data.size[0] : " << m_columns << std::endl;
    std::cout << "BIOSIG data.size[1] : " << m_rows << std::endl;
    std::cout << "BIOSIG data.block[0]: " << hd->data.block[0] << std::endl;
    std::cout << "BIOSIG DataSize     : " << alex << std::endl;

    std::cout << alex << std::endl;
    std::cout << "BIOSIGLOADER_some_data" << std::endl;

    std::vector<std::vector<double> > segment;
    fillSegment( &segment, hd->data.block, nbSamples );


    std::vector<std::vector<std::vector<double> > > segments( 0 );
    segments.push_back( segment );


    boost::shared_ptr< WMetaInfo > metaInfo = boost::shared_ptr<WMetaInfo>( new WMetaInfo() );
    boost::shared_ptr< WEEG > eeg = boost::shared_ptr< WEEG >( new WEEG( metaInfo, segments ) );

    m_dataHandler->addDataSet( eeg );

    std::cout << "BIOSIG loading done." << std::endl;
}

void WLoaderBiosig::operator()()
{
    biosigLoader();
}
