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
#endif

#include <vector>

#include <boost/shared_ptr.hpp>

#include "WDataSetScalar.h"

#ifdef OW_USE_ITK

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
    boost::shared_ptr< WGridRegular3D > grid = boost::dynamic_pointer_cast< WGridRegular3D >( dataSet->getGrid() );
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

    typename itk::Image< T, 3 >::SpacingType spacing;
    spacing[ 0 ] = grid->getOffsetX();
    spacing[ 1 ] = grid->getOffsetY();
    spacing[ 2 ] = grid->getOffsetZ();

    typename itk::Image< T, 3 >::PointType orig;
    orig[ 0 ] = grid->getOrigin()[ 0 ];
    orig[ 1 ] = grid->getOrigin()[ 1 ];
    orig[ 2 ] = grid->getOrigin()[ 2 ];

    img->SetRegions( r );
    img->SetSpacing( spacing );
    img->SetOrigin( orig );
    img->Allocate();

    // copy direction matrix
    typename itk::Image< T, 3 >::DirectionType dirMat;
    dirMat( 0, 0 ) = grid->getDirectionX()[ 0 ];
    dirMat( 0, 1 ) = grid->getDirectionY()[ 0 ];
    dirMat( 0, 2 ) = grid->getDirectionZ()[ 0 ];
    dirMat( 1, 0 ) = grid->getDirectionX()[ 1 ];
    dirMat( 1, 1 ) = grid->getDirectionY()[ 1 ];
    dirMat( 1, 2 ) = grid->getDirectionZ()[ 1 ];
    dirMat( 2, 0 ) = grid->getDirectionX()[ 2 ];
    dirMat( 2, 1 ) = grid->getDirectionY()[ 2 ];
    dirMat( 2, 2 ) = grid->getDirectionZ()[ 2 ];

    img->SetDirection( dirMat );

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

    WMatrix< double > smat( 4, 4 );
    typename itk::Image< T, 3 >::DirectionType dirMat = img->GetDirection();

    smat( 0, 0 ) = dirMat( 0, 0 );
    smat( 0, 1 ) = dirMat( 0, 1 );
    smat( 0, 2 ) = dirMat( 0, 2 );
    smat( 1, 0 ) = dirMat( 1, 0 );
    smat( 1, 1 ) = dirMat( 1, 1 );
    smat( 1, 2 ) = dirMat( 1, 2 );
    smat( 2, 0 ) = dirMat( 2, 0 );
    smat( 2, 1 ) = dirMat( 2, 1 );
    smat( 2, 2 ) = dirMat( 2, 2 );
    smat( 0, 3 ) = img->GetOrigin()[ 0 ];
    smat( 1, 3 ) = img->GetOrigin()[ 1 ];
    smat( 2, 3 ) = img->GetOrigin()[ 2 ];
    smat( 3, 3 ) = 1.0;
    smat( 3, 0 ) = 0.0;
    smat( 3, 1 ) = 0.0;
    smat( 3, 2 ) = 0.0;

    WGridTransformOrtho t( smat );
    boost::shared_ptr< WGrid > grid( new WGridRegular3D( s[ 0 ], s[ 1 ], s[ 2 ], t ) );
    boost::shared_ptr< std::vector< T > > v( new std::vector< T >( s[ 0 ] * s[ 1 ] * s[ 2 ] ) );

    typename itk::Image< T, 3 >::IndexType i;

    for( i[ 0 ] = 0; i[ 0 ] < static_cast< int >( s[ 0 ] ); ++i[ 0 ] )
    {
        for( i[ 1 ] = 0; i[ 1 ] < static_cast< int >( s[ 1 ] ); ++i[ 1 ] )
        {
            for( i[ 2 ] = 0; i[ 2 ] < static_cast< int >( s[ 2 ] ); ++i[ 2 ] )
            {
                v->at( i[ 0 ] + i[ 1 ] * s[ 0 ] + i[ 2 ] * s[ 0 ] * s[ 1 ] ) = img->GetPixel( i );
            }
        }
    }

    boost::shared_ptr< WValueSetBase > values( new WValueSet< T >( 0, 1, v, DataType< T >::type ) );

    return boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( values, grid ) );
}

#endif  // OW_USE_ITK

#endif  // WITKIMAGECONVERSION_H
