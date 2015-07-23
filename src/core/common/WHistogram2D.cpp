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

#include <utility>

#include <core/common/math/WMath.h>
#include <core/graphicsEngine/WGETexture.h>
#include "WAssert.h"
#include "WHistogram2D.h"
#include "WLimits.h"
#include "WLogger.h"

WHistogram2D::WHistogram2D( double minX, double maxX, double minY, double maxY, size_t bucketsX, size_t bucketsY )
{
    // use protecte default ctor to workaround missing initializer lists which are part of C++11 and GNU++11 only.
    TArray min = {{ minX, minY }}; // NOLINT braces
    TArray max = {{ maxX, maxY }}; // NOLINT braces
    SizeArray buckets = {{ bucketsX, bucketsY }}; // NOLINT braces
    reset( min, max, buckets );

    m_intervalWidth[0] = std::abs( maxX - minX ) / static_cast< double >( bucketsX );
    m_intervalWidth[1] = std::abs( maxY - minY ) / static_cast< double >( bucketsY );
    m_bins = BinType::Zero( bucketsX, bucketsY );
}

WHistogram2D::~WHistogram2D()
{
}

WHistogram2D::WHistogram2D( const WHistogram2D& other )
    : WHistogramND( other )
{
    m_bins = other.m_bins;
}

size_t WHistogram2D::operator()( SizeArray index ) const
{
    return m_bins( index[0], index[1] );
}

size_t WHistogram2D::operator()( size_t i, size_t j ) const
{
    SizeArray index = {{ i, j }};
    return operator()( index );
}

double WHistogram2D::getBucketSize( SizeArray /* index */ ) const
{
    return m_intervalWidth[0] * m_intervalWidth[1];
}

boost::array< std::pair< double, double >, 2 > WHistogram2D::getIntervalForIndex( SizeArray index ) const
{
    boost::array< std::pair< double, double >, 2 > result;

    for( size_t i = 0; i < 2; ++i )
    {
        result[i] = std::make_pair( m_intervalWidth[i] * index[i] + m_min[i],
                                    m_intervalWidth[i] * ( index[i] + 1 ) + m_min[i] );
    }

    return result;
}

void WHistogram2D::insert( TArray values )
{
    if( values[0] > m_max[0] || values[0] < m_min[0] || values[1] > m_max[1] || values[1] < m_min[1] )
    {
        wlog::warn( "WHistogram2D" ) << std::scientific << std::setprecision( 16 ) << "Inserted value out of bounds, thread: ("
                                     << values[0] << "," << values[1] << ") whereas min,max are: dim0: (" << m_min[0] << "," << m_max[0] << ") "
                                     << "dim1:(" << m_min[1] << "," << m_max[1] << ")";
        return;
    }

    SizeArray coord = {{ 0, 0 }};

    for( size_t i = 0; i < 2; ++i )
    {
        if( std::abs( m_min[i] - m_max[i] ) <= 2.0 * wlimits::DBL_EPS )
        {
            coord[i] = m_buckets[i] - 1;
        }
        else if( values[i] >= ( m_max[i] - m_intervalWidth[i] ) && values[i] <= m_max[i] )
        {
            coord[i] = m_buckets[i] - 1;
        }
        else
        {
            coord[i] = static_cast< size_t >( ( values[i] - m_min[i] ) / std::abs( m_max[i] - m_min[i] ) * ( m_buckets[i] ) );
        }
    }

    m_bins( coord[0], coord[1] )++;
}

void WHistogram2D::insert( double x, double y )
{
    TArray values = {{ x, y }};
    insert( values );
}

WGETexture2D::RPtr WHistogram2D::getTexture()
{
    osg::ref_ptr< osg::Image > image = new osg::Image();
    size_t imageWidth = m_buckets[0];
    size_t imageHeight = m_buckets[1];
    float maxCount = 0;

    for( size_t j = 0; j < imageHeight; ++j ) // get max bin for scaling
    {
        for( size_t i = 0; i < imageWidth; ++i )
        {
            if( m_bins( i, j ) > maxCount )
            {
                maxCount = static_cast< float >( m_bins( i, j ) );
            }
        }
    }

    image->allocateImage( imageWidth, imageHeight, 1, GL_RED, GL_FLOAT );
    image->setInternalTextureFormat( GL_RED );
    float* data = reinterpret_cast< float* >( image->data() );

    for( size_t j = 0; j < imageHeight; ++j )
    {
        for( size_t i = 0; i < imageWidth; ++i )
        {
            data[i + j * imageWidth] = static_cast< float >( m_bins( i, j ) ) / maxCount;
        }
    }

    return WGETexture2D::RPtr( new WGETexture2D( image ) );
}

/**
 * Unnamed namespace for helper functions keeping the code DRY as possible.
 */
namespace
{
    double calcAreaScale( const double bucket, const size_t j )
    {
        double theta = pi() - ( j * bucket + ( bucket / 2.0 ) );
        return 1.0 / sin( theta );
    }
}

WGETexture2D::RPtr WHistogram2D::getSphereTexture()
{
    osg::ref_ptr< osg::Image > image = new osg::Image();
    size_t imageWidth = m_buckets[0];
    size_t imageHeight = m_buckets[1];
    double maxCount = 0.0;
    const double bucket = pi() / static_cast< double >( imageHeight );
    double areaScale = 0.0;

    for( size_t j = 0; j < imageHeight; ++j ) // get max bin for scaling
    {
        areaScale = calcAreaScale( bucket, j );
        for( size_t i = 0; i < imageWidth; ++i )
        {
            if( areaScale * m_bins( i, j ) > maxCount)
            {
                maxCount = areaScale * static_cast< double >( m_bins( i, j ) );
            }
        }
    }

    image->allocateImage( imageWidth, imageHeight, 1, GL_RED, GL_FLOAT );
    image->setInternalTextureFormat( GL_RED );
    float* data = reinterpret_cast< float* >( image->data() );

    for( size_t j = 0; j < imageHeight; ++j )
    {
        areaScale = calcAreaScale( bucket, j );
        for( size_t i = 0; i < imageWidth; ++i )
        {
            data[i + j * imageWidth] = areaScale * static_cast< double >( m_bins( i, j ) ) / maxCount;
        }
    }

    return WGETexture2D::RPtr( new WGETexture2D( image ) );
}

