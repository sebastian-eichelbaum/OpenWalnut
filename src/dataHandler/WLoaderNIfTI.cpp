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
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "WLoaderNIfTI.h"
#include "WDataHandler.h"
#include "WDataSet.h"
#include "WDataSetSingle.h"
#include "WGrid.h"
#include "WValueSetBase.h"
#include "WValueSet.hpp"


WLoaderNIfTI::WLoaderNIfTI( std::string fileName, boost::shared_ptr< WDataHandler > dataHandler )
    : m_fileName( fileName ),
    m_dataHandler( dataHandler )
{
}

void WLoaderNIfTI::operator()()
{
    nifti_image* header = nifti_image_read( m_fileName.c_str(), 0 );
    int columns = header->dim[1];
    int rows = header->dim[2];
    int frames = header->dim[3];
    std::cout << "grid dimensions: " << columns << " " << rows << " " << frames
                    << std::endl;
    switch( header->datatype )
    {
        case DT_SIGNED_SHORT:
            std::cout << "data type: DT_SIGNED_SHORT" << std::endl;
            break;
        default:
            std::cout << "unknown data type " << header->datatype << std::endl;
            assert( 0 );
    }
    boost::shared_ptr< WValueSetBase > newValueSet;
    boost::shared_ptr<WGrid> newGrid;

    nifti_image* filedata = nifti_image_read( m_fileName.c_str(), 1 );

    unsigned int valueDim = header->dim[4];
    unsigned int nbTens = columns * rows * frames;
    unsigned int nbValues = nbTens * valueDim;

    nifti_image_infodump( header );

    if( header->datatype == DT_SIGNED_SHORT && valueDim == 1 )
    {
        std::cout << "DataType SIGNED_SHORT: " << header->datatype << std::endl;
        int16_t* myImage = reinterpret_cast<int16_t*>( filedata->data );
        std::vector< int16_t > data( nbValues );
        for( unsigned int i = 0; i < nbValues; i++ )
        {
            data[i] = myImage[i];
        }
        newValueSet = boost::shared_ptr< WValueSetBase > ( new WValueSet< int16_t >( 0, valueDim, data ) );
        newGrid = boost::shared_ptr< WGrid >( new WGrid( nbValues ) );
    }
    else
    {
        std::cout << "unknown data type " << header->datatype << std::endl;
        newValueSet = boost::shared_ptr< WValueSetBase >();
        newGrid = boost::shared_ptr< WGrid >();
    }

    boost::shared_ptr< WMetaInfo > metaInfo = boost::shared_ptr<WMetaInfo>( new WMetaInfo() );
    fillMetaInfo( metaInfo, header );

    boost::shared_ptr< WDataSet > newDataSet = boost::shared_ptr< WDataSet >(
                    new WDataSetSingle( newValueSet, newGrid, metaInfo ) );
    m_dataHandler->addDataSet( newDataSet );
}


void WLoaderNIfTI::fillMetaInfo( boost::shared_ptr< WMetaInfo > metaInfo, nifti_image* header )
{
    metaInfo->setName( m_fileName );
    metaInfo->setFileName( m_fileName );
    metaInfo->setDataType( header->datatype );
    metaInfo->setNx( header->nx );
    metaInfo->setNy( header->ny );
    metaInfo->setNz( header->nz );
    metaInfo->setDx( header->dx );
    metaInfo->setDy( header->dy );
    metaInfo->setDz( header->dz );
    metaInfo->setXyzUnits( header->xyz_units );
    metaInfo->setQformCode( header->qform_code );
    metaInfo->setQuaternB( header->quatern_b );
    metaInfo->setQuaternC( header->quatern_c );
    metaInfo->setQuaternD( header->quatern_d );
    metaInfo->setQoffsetX( header->qoffset_x );
    metaInfo->setQoffsetY( header->qoffset_y );
    metaInfo->setQoffsetZ( header->qoffset_z );
    metaInfo->setQfac( header->qfac );
    metaInfo->setFreqDim( header->freq_dim );
    metaInfo->setPhaseDim( header->phase_dim );
    metaInfo->setSliceDim( header->slice_dim );

    std::vector<float>xyzMatrix( 16, 0 );
    for ( int i = 0; i < 4; ++i )
    {
        for ( int j = 0; j < 4; ++j)
        {
            xyzMatrix[ i * 4 + j ] = header->qto_xyz.m[i][j];
        }
    }
    metaInfo->setQtoXyzMatrix( xyzMatrix );

    std::vector<float>ijkMatrix( 16, 0 );
    for ( int i = 0; i < 4; ++i )
    {
        for ( int j = 0; j < 4; ++j)
        {
            ijkMatrix[ i * 4 + j ] = header->qto_ijk.m[i][j];
        }
    }
    metaInfo->setQtoIjkMatrix( ijkMatrix );
}
