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

#include <iostream>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "WLoaderNIfTI.h"
#include "../WDataHandler.h"
#include "../WDataSet.h"
#include "../WSubject.h"
#include "../WDataSetSingle.h"
#include "../WGrid.h"
#include "../WGridRegular3D.h"
#include "../WValueSetBase.h"
#include "../WValueSet.hpp"
#include "../WDataHandlerEnums.h"


WLoaderNIfTI::WLoaderNIfTI( std::string fileName, boost::shared_ptr< WDataHandler > dataHandler )
    : WLoader( fileName, dataHandler )
{
}


template< typename T > std::vector< T > WLoaderNIfTI::copyArray( const T* dataArray, const size_t countVoxels,
        const size_t vDim )
{
    std::vector< T > data( countVoxels * vDim );
    for( unsigned int i = 0; i < countVoxels; ++i )
    {
        for ( unsigned int j = 0; j < vDim; ++j )
        {
            data[i * vDim + j] = dataArray[( j * countVoxels ) + i];
        }
    }
    return data;
}


wmath::WMatrix< double > WLoaderNIfTI::convertMatrix( const mat44& in )
{
    wmath::WMatrix< double > out( 4, 4 );
    for( size_t i = 0; i < 4; ++i )
    {
        for( size_t j = 0; j < 4; ++j )
        {
            out( i, j ) = in.m[i][j];
        }
    }
    return out;
}

boost::shared_ptr< WDataSet > WLoaderNIfTI::load()
{
    nifti_image* header = nifti_image_read( m_fileName.c_str(), 0 );
    int columns = header->dim[1];
    int rows = header->dim[2];
    int frames = header->dim[3];

    boost::shared_ptr< WValueSetBase > newValueSet;
    boost::shared_ptr<WGrid> newGrid;

    nifti_image* filedata = nifti_image_read( m_fileName.c_str(), 1 );

    unsigned int vDim = header->dim[4];
    unsigned int order = ( ( vDim == 1 ) ? 0 : 1 );  // TODO(all): Does recognize vectors and scalars only so far.
    unsigned int countVoxels = columns * rows * frames;

    // nifti_image_infodump( header );

    switch( header->datatype )
    {
        case DT_UNSIGNED_CHAR:
        {
            std::vector< unsigned char > data = copyArray( reinterpret_cast< unsigned char* >( filedata->data ), countVoxels, vDim );
            newValueSet = boost::shared_ptr< WValueSetBase >( new WValueSet< unsigned char >( order, vDim, data, W_DT_UNSIGNED_CHAR ) );
            break;
        }

        case DT_SIGNED_SHORT:
        {
            std::vector< int16_t > data = copyArray( reinterpret_cast< int16_t* >( filedata->data ), countVoxels, vDim );
            newValueSet = boost::shared_ptr< WValueSetBase >( new WValueSet< int16_t >( order, vDim, data, W_DT_INT16 ) );
            break;
        }

        case DT_FLOAT:
        {
            std::vector< float > data = copyArray( reinterpret_cast< float* >( filedata->data ), countVoxels, vDim );
            newValueSet = boost::shared_ptr< WValueSetBase >( new WValueSet< float >( order, vDim, data, W_DT_FLOAT ) );
            break;
        }

        default:
            std::cout << "unknown data type " << header->datatype << std::endl;
            newValueSet = boost::shared_ptr< WValueSetBase >();
    }

    if( header->sform_code > 0 )
    {
        // method 3 (affine transform)
        newGrid = boost::shared_ptr< WGridRegular3D >( new WGridRegular3D(
            columns, rows, frames, convertMatrix( header->sto_xyz ) ) );
    }
    else if( header->qform_code > 0 )
    {
        // method 2 (rigid body transform)
        newGrid = boost::shared_ptr< WGridRegular3D >( new WGridRegular3D(
            columns, rows, frames, convertMatrix( header->qto_xyz ) ) );
    }
    else
    {
        // method 1 (only scaling)
        newGrid = boost::shared_ptr< WGridRegular3D >( new WGridRegular3D(
            columns, rows, frames, header->dx, header->dy, header->dz ) );
    }

    boost::shared_ptr< WDataSet > newDataSet = boost::shared_ptr< WDataSet >( new WDataSetSingle( newValueSet, newGrid ) );
    newDataSet->setFileName( m_fileName );
    commitDataSet( newDataSet );

    return newDataSet;
}


// TODO(wiebel): fill this info into the subject instead
// void WLoaderNIfTI::fillMetaInfo( boost::shared_ptr< WMetaInfo > metaInfo, nifti_image* header )
// {
//     metaInfo->setNx( header->nx );
//     metaInfo->setNy( header->ny );
//     metaInfo->setNz( header->nz );
//     metaInfo->setXyzUnits( header->xyz_units );
//     metaInfo->setQuaternB( header->quatern_b );
//     metaInfo->setQuaternC( header->quatern_c );
//     metaInfo->setQuaternD( header->quatern_d );
//     metaInfo->setQoffsetX( header->qoffset_x );
//     metaInfo->setQoffsetY( header->qoffset_y );
//     metaInfo->setQoffsetZ( header->qoffset_z );
//     metaInfo->setQfac( header->qfac );
//     metaInfo->setFreqDim( header->freq_dim );
//     metaInfo->setPhaseDim( header->phase_dim );
//     metaInfo->setSliceDim( header->slice_dim );

//     std::vector<float>ijkMatrix( 16, 0 );
//     for ( int i = 0; i < 4; ++i )
//     {
//         for ( int j = 0; j < 4; ++j)
//         {
//             ijkMatrix[ i * 4 + j ] = header->qto_ijk.m[i][j];
//         }
//     }
//     metaInfo->setQtoIjkMatrix( ijkMatrix );
// }
