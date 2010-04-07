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

#include <stdint.h>

#include <iostream>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "WLoaderNIfTI.h"
#include "../WDataSet.h"
#include "../WSubject.h"
#include "../WDataSetSingle.h"
#include "../WDataSetVector.h"
#include "../WDataSetScalar.h"
#include "../WGrid.h"
#include "../WGridRegular3D.h"
#include "../WValueSetBase.h"
#include "../WValueSet.h"
#include "../WDataHandlerEnums.h"
#include "../../common/WLogger.h"


WLoaderNIfTI::WLoaderNIfTI( std::string fileName )
    : WLoader( fileName )
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

#ifdef DEBUG
    //nifti_image_infodump( header );
#endif

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

        case DT_INT32:
        {
            std::vector< int32_t > data = copyArray( reinterpret_cast< int32_t* >( filedata->data ), countVoxels, vDim );
            newValueSet = boost::shared_ptr< WValueSetBase >( new WValueSet< int32_t >( order, vDim, data, W_DT_SIGNED_INT ) );
            break;
        }


        case DT_FLOAT:
        {
            std::vector< float > data = copyArray( reinterpret_cast< float* >( filedata->data ), countVoxels, vDim );
            newValueSet = boost::shared_ptr< WValueSetBase >( new WValueSet< float >( order, vDim, data, W_DT_FLOAT ) );
            break;
        }

        case DT_DOUBLE:
        {
            std::vector< double > data = copyArray( reinterpret_cast< double* >( filedata->data ), countVoxels, vDim );
            newValueSet = boost::shared_ptr< WValueSetBase >( new WValueSet< double >( order, vDim, data, W_DT_DOUBLE ) );
            break;
        }

        default:
            wlog::error( "WLoaderNIfTI" ) << "unknown data type " << header->datatype << std::endl;
            newValueSet = boost::shared_ptr< WValueSetBase >();
    }

    if( header->sform_code > 0 )
    {
        // method 3 (affine transform)
        newGrid = boost::shared_ptr< WGridRegular3D >( new WGridRegular3D(
            columns, rows, frames, convertMatrix( header->sto_xyz ), header->dx, header->dy, header->dz ) );
    }
    else if( header->qform_code > 0 )
    {
        // method 2 (rigid body transform)
        newGrid = boost::shared_ptr< WGridRegular3D >( new WGridRegular3D(
            columns, rows, frames, convertMatrix( header->qto_xyz ), header->dx, header->dy, header->dz ) );
    }
    else
    {
        // method 1 (only scaling)
        newGrid = boost::shared_ptr< WGridRegular3D >( new WGridRegular3D(
            columns, rows, frames, header->dx, header->dy, header->dz ) );
    }

    boost::shared_ptr< WDataSet > newDataSet;
    if( vDim == 3 )
    {
        newDataSet = boost::shared_ptr< WDataSet >( new WDataSetVector( newValueSet, newGrid ) );
    }
    else if( vDim == 1 )
    {
        newDataSet = boost::shared_ptr< WDataSet >( new WDataSetScalar( newValueSet, newGrid ) );
    }
    else
    {
        newDataSet = boost::shared_ptr< WDataSet >( new WDataSetSingle( newValueSet, newGrid ) );
    }
    newDataSet->setFileName( m_fileName );

    return newDataSet;
}
