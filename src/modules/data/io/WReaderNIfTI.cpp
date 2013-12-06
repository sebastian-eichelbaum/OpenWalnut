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

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "core/common/WIOTools.h"
#include "core/common/WLogger.h"
#include "core/dataHandler/WDataHandlerEnums.h"
#include "core/dataHandler/WDataSet.h"
#include "core/dataHandler/WDataSetDTI.h"
#include "core/dataHandler/WDataSetRawHARDI.h"
#include "core/dataHandler/WDataSetScalar.h"
#include "core/dataHandler/WDataSetSegmentation.h"
#include "core/dataHandler/WDataSetSingle.h"
#include "core/dataHandler/WDataSetSphericalHarmonics.h"
#include "core/dataHandler/WDataSetTimeSeries.h"
#include "core/dataHandler/WDataSetVector.h"
#include "core/dataHandler/WGrid.h"
#include "core/dataHandler/WGridRegular3D.h"
#include "core/dataHandler/WSubject.h"
#include "core/dataHandler/WValueSet.h"
#include "core/dataHandler/WValueSetBase.h"

#include "WReaderNIfTI.h"

WReaderNIfTI::WReaderNIfTI( std::string fileName )
    : WReader( fileName ),
      m_sform( 4, 4 ),
      m_qform( 4, 4 )
{
}

template< typename T >  boost::shared_ptr< std::vector< T > > WReaderNIfTI::copyArray( const T* dataArray, const size_t countVoxels,
        const size_t vDim )
{
    boost::shared_ptr< std::vector< T > > data( new std::vector< T >( countVoxels * vDim ) );
    for( unsigned int i = 0; i < countVoxels; ++i )
    {
        for( unsigned int j = 0; j < vDim; ++j )
        {
            (*data)[i * vDim + j] = dataArray[( j * countVoxels ) + i];
        }
    }
    return data;
}


WMatrix< double > WReaderNIfTI::convertMatrix( const mat44& in )
{
    WMatrix< double > out( 4, 4 );

    bool isZero = true;
    for( size_t i = 0; i < 4; ++i )
    {
        for( size_t j = 0; j < 4; ++j )
        {
            out( i, j ) = in.m[i][j];
            isZero = isZero && out( i, j ) == 0.0;
        }
    }

    if( isZero )
    {
        out.makeIdentity();
    }
    return out;
}

boost::shared_ptr< WDataSet > WReaderNIfTI::load( DataSetType dataSetType )
{
    boost::shared_ptr< nifti_image > filedata( nifti_image_read( m_fname.c_str(), 1 ), &nifti_image_free );

    WAssert( filedata, "Error during file access to NIfTI file. This probably means that the file is corrupted." );

    WAssert( filedata->ndim >= 3,
             "The NIfTI file contains data that has less than the three spatial dimension. OpenWalnut is not able to handle this." );

    int columns = filedata->dim[1];
    int rows = filedata->dim[2];
    int frames = filedata->dim[3];

    boost::shared_ptr< WValueSetBase > newValueSet;
    boost::shared_ptr< WGrid > newGrid;

    unsigned int vDim;
    if( filedata->ndim >= 4 )
    {
        vDim = filedata->dim[4];
    }
    else
    {
        vDim = 1;
    }

    unsigned int order = ( ( vDim == 1 ) ? 0 : 1 );  // TODO(all): Does recognize vectors and scalars only so far.
    unsigned int countVoxels = columns * rows * frames;

    // don't rearrange if this is a time series
    if( filedata->dim[ 5 ] <= 1 )
    {
        switch( filedata->datatype )
        {
            case DT_UINT8:
            {
                boost::shared_ptr< std::vector< uint8_t > > data = copyArray( reinterpret_cast< uint8_t* >( filedata->data ), countVoxels, vDim );
                newValueSet = boost::shared_ptr< WValueSetBase >( new WValueSet< uint8_t >( order, vDim, data, W_DT_UINT8 ) );
                break;
            }
            case DT_INT8:
            {
                boost::shared_ptr< std::vector< int8_t > > data = copyArray( reinterpret_cast< int8_t* >( filedata->data ), countVoxels, vDim );
                newValueSet = boost::shared_ptr< WValueSetBase >( new WValueSet< int8_t >( order, vDim, data, W_DT_INT8 ) );
                break;
            }
            case DT_INT16:
            {
                boost::shared_ptr< std::vector< int16_t > > data = copyArray( reinterpret_cast< int16_t* >( filedata->data ), countVoxels, vDim );
                newValueSet = boost::shared_ptr< WValueSetBase >( new WValueSet< int16_t >( order, vDim, data, W_DT_INT16 ) );
                break;
            }
            case DT_UINT16:
            {
                boost::shared_ptr< std::vector< uint16_t > > data
                    = copyArray( reinterpret_cast< uint16_t* >( filedata->data ), countVoxels, vDim );
                newValueSet = boost::shared_ptr< WValueSetBase >( new WValueSet< uint16_t >( order, vDim, data, W_DT_UINT16 ) );
                break;
            }
            case DT_SIGNED_INT:
            {
                boost::shared_ptr< std::vector< int32_t > > data = copyArray( reinterpret_cast< int32_t* >( filedata->data ), countVoxels, vDim );
                newValueSet = boost::shared_ptr< WValueSetBase >( new WValueSet< int32_t >( order, vDim, data, W_DT_SIGNED_INT ) );
                break;
            }
            case DT_UINT32:
            {
                boost::shared_ptr< std::vector< uint32_t > > data
                    = copyArray( reinterpret_cast< uint32_t* >( filedata->data ), countVoxels, vDim );
                newValueSet = boost::shared_ptr< WValueSetBase >( new WValueSet< uint32_t >( order, vDim, data, W_DT_UINT32 ) );
                break;
            }
            case DT_INT64:
            {
                boost::shared_ptr< std::vector< int64_t > > data = copyArray( reinterpret_cast< int64_t* >( filedata->data ), countVoxels, vDim );
                newValueSet = boost::shared_ptr< WValueSetBase >( new WValueSet< int64_t >( order, vDim, data, W_DT_INT64 ) );
                break;
            }
            case DT_UINT64:
            {
                boost::shared_ptr< std::vector< uint64_t > > data =
                    copyArray( reinterpret_cast< uint64_t* >( filedata->data ), countVoxels, vDim );
                newValueSet = boost::shared_ptr< WValueSetBase >( new WValueSet< uint64_t >( order, vDim, data, W_DT_UINT64 ) );
                break;
            }
            case DT_FLOAT:
            {
                boost::shared_ptr< std::vector< float > > data = copyArray( reinterpret_cast< float* >( filedata->data ), countVoxels, vDim );
                newValueSet = boost::shared_ptr< WValueSetBase >( new WValueSet< float >( order, vDim, data, W_DT_FLOAT ) );
                break;
            }
            case DT_DOUBLE:
            {
                boost::shared_ptr< std::vector< double > > data = copyArray( reinterpret_cast< double* >( filedata->data ), countVoxels, vDim );
                newValueSet = boost::shared_ptr< WValueSetBase >( new WValueSet< double >( order, vDim, data, W_DT_DOUBLE ) );
                break;
            }
            case DT_FLOAT128:
            {
                boost::shared_ptr< std::vector< long double > > data =
                    copyArray( reinterpret_cast< long double* >( filedata->data ), countVoxels, vDim );
                newValueSet = boost::shared_ptr< WValueSetBase >( new WValueSet< long double >( order, vDim, data, W_DT_FLOAT128 ) );
                break;
            }
            default:
                wlog::error( "WReaderNIfTI" ) << "unknown data type " << filedata->datatype << std::endl;
                newValueSet = boost::shared_ptr< WValueSetBase >();
        }
    }

    m_sform = convertMatrix( filedata->sto_xyz );
    m_qform = convertMatrix( filedata->qto_xyz );
    newGrid = boost::shared_ptr< WGridRegular3D >(
                        new WGridRegular3D( columns, rows, frames, WGridTransformOrtho( getStandardTransform() ) ) );

    boost::shared_ptr< WDataSet > newDataSet;
    // known description
    std::string description( filedata->descrip );
    if( !description.compare( "WDataSetSegmentation" ) )
    {
        wlog::debug( "WReaderNIfTI" ) << "Load as segmentation" << std::endl;
        newDataSet = boost::shared_ptr< WDataSet >( new WDataSetSegmentation( newValueSet, newGrid ) );
    }
    else if( description.compare( "WDataSetSphericalHarmonics" ) == 0 || dataSetType == W_DATASET_SPHERICALHARMONICS )
    {
        wlog::debug( "WReaderNIfTI" ) << "Load as spherical harmonics" << std::endl;
        newDataSet = boost::shared_ptr< WDataSet >( new WDataSetSphericalHarmonics( newValueSet, newGrid ) );
    }
    // 4th dimension is the time
    // note that in the nifti standard, dim[ 4 ] is the temporal dimension
    // we use dim[ 5 ] here
    else if( filedata->dim[ 5 ] > 1 )
    {
        WAssert( filedata->dim[ 4 ] == 1, "Only scalar datasets are supported for time series so far." );
        wlog::debug( "WReaderNIfTI" ) << "Load as WDataSetTimeSeries";
        std::size_t numTimeSlices = filedata->dim[ 5 ];
        float tw = filedata->pixdim[ 5 ];
        WAssert( tw != 0.0f, "" );

        std::vector< boost::shared_ptr< WDataSetScalar const > > ds;
        std::vector< float > times;
        float t = 0.0f;
        for( std::size_t k = 0; k < numTimeSlices; ++k )
        {
            times.push_back( t );
            t += tw;
            boost::shared_ptr< WValueSetBase > vs;
            switch( filedata->datatype )
            {
                case DT_UINT8:
                {
                    uint8_t* ptr = reinterpret_cast< uint8_t* >( filedata->data );
                    boost::shared_ptr< std::vector< uint8_t > > values =
                        boost::shared_ptr< std::vector< uint8_t > >(
                            new std::vector< uint8_t >( ptr + k * countVoxels, ptr + ( k + 1 ) * countVoxels ) );
                    vs = boost::shared_ptr< WValueSetBase >( new WValueSet< uint8_t >( 0, 1, values, W_DT_UINT8 ) );
                }
                break;
                case DT_INT8:
                {
                    int8_t* ptr = reinterpret_cast< int8_t* >( filedata->data );
                    boost::shared_ptr< std::vector< int8_t > > values =
                        boost::shared_ptr< std::vector< int8_t > >(
                            new std::vector< int8_t >( ptr + k * countVoxels, ptr + ( k + 1 ) * countVoxels ) );
                    vs = boost::shared_ptr< WValueSetBase >( new WValueSet< int8_t >( 0, 1, values, W_DT_INT8 ) );
                }
                break;
                case DT_UINT16:
                {
                    uint16_t* ptr = reinterpret_cast< uint16_t* >( filedata->data );
                    boost::shared_ptr< std::vector< uint16_t > >values =
                        boost::shared_ptr< std::vector< uint16_t > >(
                            new std::vector< uint16_t >( ptr + k * countVoxels, ptr + ( k + 1 ) * countVoxels ) );
                    vs = boost::shared_ptr< WValueSetBase >( new WValueSet< uint16_t >( 0, 1, values, W_DT_UINT16 ) );
                }
                break;
                case DT_INT16:
                {
                    int16_t* ptr = reinterpret_cast< int16_t* >( filedata->data );
                    boost::shared_ptr< std::vector< int16_t > > values =
                        boost::shared_ptr< std::vector< int16_t > >(
                            new std::vector< int16_t >( ptr + k * countVoxels, ptr + ( k + 1 ) * countVoxels ) );
                    vs = boost::shared_ptr< WValueSetBase >( new WValueSet< int16_t >( 0, 1, values, W_DT_INT16 ) );
                }
                break;
                case DT_UINT32:
                {
                    uint32_t* ptr = reinterpret_cast< uint32_t* >( filedata->data );
                    boost::shared_ptr< std::vector< uint32_t > > values =
                        boost::shared_ptr< std::vector< uint32_t > >(
                            new std::vector< uint32_t >( ptr + k * countVoxels, ptr + ( k + 1 ) * countVoxels ) );
                    vs = boost::shared_ptr< WValueSetBase >( new WValueSet< uint32_t >( 0, 1, values, W_DT_UINT32 ) );
                }
                break;
                case DT_SIGNED_INT:
                {
                    int32_t* ptr = reinterpret_cast< int32_t* >( filedata->data );
                    boost::shared_ptr< std::vector< int32_t > > values =
                        boost::shared_ptr< std::vector< int32_t > >(
                            new std::vector< int32_t >( ptr + k * countVoxels, ptr + ( k + 1 ) * countVoxels ) );
                    vs = boost::shared_ptr< WValueSetBase >( new WValueSet< int32_t >( 0, 1, values, W_DT_SIGNED_INT ) );
                }
                break;
                case DT_UINT64:
                {
                    uint64_t* ptr = reinterpret_cast< uint64_t* >( filedata->data );
                    boost::shared_ptr< std::vector< uint64_t > > values =
                        boost::shared_ptr< std::vector< uint64_t > >(
                            new std::vector< uint64_t >( ptr + k * countVoxels, ptr + ( k + 1 ) * countVoxels ) );
                    vs = boost::shared_ptr< WValueSetBase >( new WValueSet< uint64_t >( 0, 1, values, W_DT_UINT64 ) );
                }
                break;
                case DT_INT64:
                {
                    int64_t* ptr = reinterpret_cast< int64_t* >( filedata->data );
                    boost::shared_ptr< std::vector< int64_t > > values =
                        boost::shared_ptr< std::vector< int64_t > >(
                            new std::vector< int64_t >( ptr + k * countVoxels, ptr + ( k + 1 ) * countVoxels ) );
                    vs = boost::shared_ptr< WValueSetBase >( new WValueSet< int64_t >( 0, 1, values, W_DT_INT64 ) );
                }
                break;
                case DT_FLOAT:
                {
                    float* ptr = reinterpret_cast< float* >( filedata->data );
                    boost::shared_ptr< std::vector< float > > values =
                        boost::shared_ptr< std::vector< float > >(
                            new std::vector< float >( ptr + k * countVoxels, ptr + ( k + 1 ) * countVoxels ) );
                    vs = boost::shared_ptr< WValueSetBase >( new WValueSet< float >( 0, 1, values, W_DT_FLOAT ) );
                }
                break;
                case DT_DOUBLE:
                {
                    double* ptr = reinterpret_cast< double* >( filedata->data );
                    boost::shared_ptr< std::vector< double > > values =
                        boost::shared_ptr< std::vector< double > >(
                            new std::vector< double >( ptr + k * countVoxels, ptr + ( k + 1 ) * countVoxels ) );
                    vs = boost::shared_ptr< WValueSetBase >( new WValueSet< double >( 0, 1, values, W_DT_DOUBLE ) );
                }
                break;
                default:
                    throw WException( std::string( "Unsupported datatype in WReaderNIfTI" ) );
                    break;
            }
            ds.push_back( boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( vs, newGrid ) ) );
        }
        newDataSet = boost::shared_ptr< WDataSet >( new WDataSetTimeSeries( ds, times ) );
    }
    // unknown description
    else
    {
        if( vDim == 3 )
        {
            wlog::debug( "WReaderNIfTI" ) << "Load as WDataSetVector";
            newDataSet = boost::shared_ptr< WDataSet >( new WDataSetVector( newValueSet, newGrid ) );
        }
        else if( vDim == 1 )
        {
            wlog::debug( "WReaderNIfTI" ) << "Load as WDataSetScalar";
            newDataSet = boost::shared_ptr< WDataSet >( new WDataSetScalar( newValueSet, newGrid ) );
        }
        else if( vDim > 20 && filedata->dim[ 5 ] <= 1 ) // hardi data, order 1
        {
            wlog::debug( "WReaderNIfTI" ) << "Load as WDataSetRawHARDI";

            std::string gradientFileName = m_fname;
            std::string suffix = getSuffix( m_fname );

            if( suffix == ".gz" )
            {
                WAssert( gradientFileName.length() > 3, "" );
                gradientFileName.resize( gradientFileName.length() - 3 );
                suffix = getSuffix( gradientFileName );
            }
            WAssert( suffix == ".nii", "Input file is not a nifti file." );

            WAssert( gradientFileName.length() > 4, "" );
            gradientFileName.resize( gradientFileName.length() - 4 );
            gradientFileName += ".bvec";

            // check if the file exists
            std::ifstream i( gradientFileName.c_str() );
            if( i.bad() || !i.is_open() )
            {
                if( i.is_open() )
                {
                    i.close();
                }
                // cannot find the appropriate gradient vectors, build a dataSetSingle instead of hardi
                newDataSet = boost::shared_ptr< WDataSet >( new WDataSetSingle( newValueSet, newGrid ) );
                wlog::debug( "WReaderNIfTI" ) << "Could not find corresponding gradients file \"" << gradientFileName.c_str() <<
                                                 "\", loading as WDataSetSingle instead.";
            }
            else
            {
                // read gradients, there should be 3 * vDim values in the file
                typedef std::vector< WVector3d > GradVec;
                boost::shared_ptr< GradVec > newGradients( new GradVec( vDim ) );

                // the file should contain the x-coordinates in line 0, y-coordinates in line 1,
                // z-coordinates in line 2
                for( unsigned int j = 0; j < 3; ++j )
                {
                    for( unsigned int k = 0; k < vDim; ++k )
                    {
                        i >> newGradients->operator[] ( k )[ j ];
                    }
                }
                bool success = !i.eof();
                i.close();

                WAssert( success, "Gradient file did not contain enough gradients." );

                newDataSet = boost::shared_ptr< WDataSet >( new WDataSetRawHARDI( newValueSet, newGrid, newGradients ) );
            }
        }
        else if( filedata->intent_code == NIFTI_INTENT_SYMMATRIX )
        {
            wlog::debug( "WReaderNIfTI" ) << "Load as WDataSetDTI";
            newDataSet = boost::shared_ptr< WDataSetDTI >( new WDataSetDTI( newValueSet, newGrid ) );
        }
        else
        {
            wlog::debug( "WReaderNIfTI" ) << "Load as WDataSetSingle";
            newDataSet = boost::shared_ptr< WDataSet >( new WDataSetSingle( newValueSet, newGrid ) );
        }
    }
    newDataSet->setFilename( m_fname );

    return newDataSet;
}

WMatrix< double > WReaderNIfTI::getStandardTransform() const
{
    return WMatrix< double >( 4, 4 ).makeIdentity();
}

WMatrix< double > WReaderNIfTI::getSFormTransform() const
{
    return m_sform;
}

WMatrix< double > WReaderNIfTI::getQFormTransform() const
{
    return m_qform;
}
