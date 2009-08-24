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
#include "../WDataHandler.h"
#include "../WDataSet.h"
#include "../WDataSetSingle.h"
#include "../WGrid.h"
#include "../WGridRegular3D.h"
#include "../WValueSetBase.h"
#include "../WValueSet.hpp"


WLoaderNIfTI::WLoaderNIfTI( std::string fileName, boost::shared_ptr< WDataHandler > dataHandler )
    : m_fileName( fileName ),
    m_dataHandler( dataHandler )
{
}


template< typename T > std::vector< T > WLoaderNIfTI::copyArray( const T* dataArray, const size_t nbValues,
        const size_t vDim )
{
    std::vector< T > data( nbValues * vDim );
    for( unsigned int i = 0; i < nbValues; ++i )
    {
        for ( unsigned int j = 0; j < vDim; ++j )
        {
            data[i * vDim + j] = dataArray[( j * nbValues ) + i];
        }
    }
    return data;
}


void WLoaderNIfTI::operator()()
{
    nifti_image* header = nifti_image_read( m_fileName.c_str(), 0 );
    int columns = header->dim[1];
    int rows = header->dim[2];
    int frames = header->dim[3];

    boost::shared_ptr< WValueSetBase > newValueSet;
    boost::shared_ptr<WGrid> newGrid;

    nifti_image* filedata = nifti_image_read( m_fileName.c_str(), 1 );

    unsigned int vDim = header->dim[4];
    unsigned int nbTens = columns * rows * frames;
    unsigned int nbValues = nbTens * vDim;

    nifti_image_infodump( header );

    switch( header->datatype )
    {
        case DT_UNSIGNED_CHAR:
        {
            std::vector< int8_t > data =
                copyArray( reinterpret_cast<int8_t*>( filedata->data ), nbValues, vDim );
            newValueSet = boost::shared_ptr< WValueSetBase > ( new WValueSet< int8_t >( 0, vDim, data ) );
            newGrid = boost::shared_ptr< WGridRegular3D >( new WGridRegular3D( columns,
                                                                               rows,
                                                                               frames,
                                                                               1.,
                                                                               1.,
                                                                               1. ) );
            break;
        }

        case DT_SIGNED_SHORT:
        {
           std::vector< int16_t > data =
               copyArray( reinterpret_cast<int16_t*>( filedata->data ), nbValues, vDim );
           newValueSet = boost::shared_ptr< WValueSetBase > ( new WValueSet< int16_t >( 0, vDim, data ) );
           newGrid = boost::shared_ptr< WGridRegular3D >( new WGridRegular3D( columns,
                                                                              rows,
                                                                              frames,
                                                                              1.,
                                                                              1.,
                                                                              1. ) );
           break;
        }

        case DT_FLOAT:
        {
            std::vector< float > data =
                copyArray( reinterpret_cast<float*>( filedata->data ), nbValues, vDim );
            newValueSet = boost::shared_ptr< WValueSetBase > ( new WValueSet< float >( 0, vDim, data ) );
            newGrid = boost::shared_ptr< WGridRegular3D >( new WGridRegular3D( columns,
                            rows,
                            frames,
                            1.,
                            1.,
                            1. ) );
            break;
        }

        default:
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
    metaInfo->setValueDim( header->dim[4] );
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
