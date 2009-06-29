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
#include "WMetaInfo.h"
#include "io/nifti/nifti1_io.h"

WLoaderNIfTI::WLoaderNIfTI( std::string fileName, boost::shared_ptr< WDataHandler > dataHandler )
: m_fileName( fileName ), m_dataHandler( dataHandler )
{
}

void WLoaderNIfTI::operator()()
{
    nifti_image* image = nifti_image_read( m_fileName.c_str(), 0 );
    int columns = image->dim[1];
    int rows = image->dim[2];
    int frames = image->dim[3];
    std::cout << "grid dimensions: " << columns << " " << rows << " " << frames
                    << std::endl;
    switch( image->datatype )
    {
    case DT_SIGNED_SHORT:
        std::cout << "data type: DT_SIGNED_SHORT" << std::endl;
        break;
    default:
        std::cout << "unknown data type " << image->datatype << std::endl;
        assert( 0 );
    }
    boost::shared_ptr< WValueSetBase > newValueSet;
    boost::shared_ptr<WGrid> newGrid;

    nifti_image* filedata = nifti_image_read( m_fileName.c_str(), 1 );

    unsigned int valueDim = image->dim[4];
    unsigned int nbTens = columns * rows * frames;
    unsigned int nbValues = nbTens * valueDim;

    nifti_image_infodump( image );

    if( image->datatype == DT_SIGNED_SHORT && valueDim == 1 )
    {
        std::cout << "DataType SIGNED_SHORT: " << image->datatype << std::endl;
        int16_t* myImage = reinterpret_cast<int16_t*>( filedata->data );
        std::vector< int16_t > data( nbValues );
        for( unsigned int i = 0; i < nbValues; i++ )
        {
            data[i] = myImage[i];
        }
        newValueSet = boost::shared_ptr< WValueSetBase > (
                        new WValueSet< int16_t >( 0, valueDim, data ) );
        newGrid = boost::shared_ptr< WGrid >( new WGrid( nbValues ) );
    }
    else
    {
        std::cout << "unknown data type " << image->datatype << std::endl;
        newValueSet = boost::shared_ptr< WValueSetBase >();
        newGrid = boost::shared_ptr< WGrid >();
    }

    boost::shared_ptr< WMetaInfo > metaInfo = boost::shared_ptr<WMetaInfo>(
                    new WMetaInfo() );
    boost::shared_ptr< WDataSet > newDataSet = boost::shared_ptr< WDataSet >(
                    new WDataSetSingle( newValueSet, newGrid, metaInfo ) );
    m_dataHandler->addDataSet( newDataSet );
}

