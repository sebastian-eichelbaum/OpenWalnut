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

#ifndef WITKIMAGECONVERSION_H
#define WITKIMAGECONVERSION_H

#ifdef OW_USE_ITK

#include <itkImage.h>

#include <vector>

#include <boost/shared_ptr.hpp>

#include "WDataSetScalar.h"

/**
 * Create an itk image from a dataset.
 *
 * \param dataSet The dataset to convert.
 *
 * \return A pointer to a 3D itk image.
 */
template< typename T >
typename itk::Image< T, 3 >::Pointer makeImageFromDataSet( boost::shared_ptr< WDataSetScalar const > dataSet )
{
    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( dataSet->getGrid() );
    WAssert( grid, "" );

    // this is a shared-pointer
    typename itk::Image< T, 3 >::Pointer img = itk::Image< T, 3 >::New();
    typename itk::Image< T, 3 >::IndexType i;
    typename itk::Image< T, 3 >::SizeType s;

    s[ 0 ] = grid->getNbCoordsX();
    s[ 1 ] = grid->getNbCoordsY();
    s[ 2 ] = grid->getNbCoordsZ();
    i[ 0 ] = i[ 1 ] = i[ 2 ] = 0;

    typename itk::Image< T, 3 >::RegionType r;
    r.SetSize( s );
    r.SetIndex( i );

    // TODO( reichenbach ): spacing, orientation
    img->SetRegions( r );
    img->Allocate();

    for( i[ 0 ] = 0; i[ 0 ] < static_cast< int >( s[ 0 ] ); ++i[ 0 ] )
    {
        for( i[ 1 ] = 0; i[ 1 ] < static_cast< int >( s[ 1 ] ); ++i[ 1 ] )
        {
            for( i[ 2 ] = 0; i[ 2 ] < static_cast< int >( s[ 2 ] ); ++i[ 2 ] )
            {
                img->SetPixel( i, dataSet->getValueAt< T >( i[ 0 ], i[ 1 ], i[ 2 ] ) );
            }
        }
    }
    return img;
}

/**
 * Create a dataset from an itk image.
 *
 * \param img The image to convert.
 *
 * \return A pointer to the dataset.
 */
template< typename T >
boost::shared_ptr< WDataSetScalar > makeDataSetFromImage( typename itk::Image< T, 3 >::Pointer img )
{
    typename itk::Image< T, 3 >::SizeType const& s = img->GetLargestPossibleRegion().GetSize();

    // TODO( reichenbach ): spacing, orientation
    boost::shared_ptr< WGrid > grid( new WGridRegular3D( s[ 0 ], s[ 1 ], s[ 2 ], 1.0, 1.0, 1.0 ) );

    std::vector< T > v( s[ 0 ] * s[ 1 ] * s[ 2 ] );

    typename itk::Image< T, 3 >::IndexType i;

    for( i[ 0 ] = 0; i[ 0 ] < static_cast< int >( s[ 0 ] ); ++i[ 0 ] )
    {
        for( i[ 1 ] = 0; i[ 1 ] < static_cast< int >( s[ 1 ] ); ++i[ 1 ] )
        {
            for( i[ 2 ] = 0; i[ 2 ] < static_cast< int >( s[ 2 ] ); ++i[ 2 ] )
            {
                v.at( i[ 0 ] + i[ 1 ] * s[ 0 ] + i[ 2 ] * s[ 0 ] * s[ 1 ] ) = img->GetPixel( i );
            }
        }
    }

    boost::shared_ptr< WValueSetBase > values( new WValueSet< T >( 0, 1, v, DataType< T >::type ) );

    return boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( values, grid ) );
}

#endif  // OW_USE_ITK

#endif  // WITKIMAGECONVERSION_H
