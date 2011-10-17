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

#include <string>
#include <algorithm>
#include <vector>

#include "../common/WLogger.h"
#include "../common/datastructures/WFiber.h"
#include "WDataSet.h"
#include "WDataSetFiberVector.h"

// prototype instance as singleton
boost::shared_ptr< WPrototyped > WDataSetFiberVector::m_prototype = boost::shared_ptr< WPrototyped >();

WDataSetFiberVector::WDataSetFiberVector()
    : WMixinVector< WFiber >(),
      WDataSet()
{
}

WDataSetFiberVector::WDataSetFiberVector( boost::shared_ptr< std::vector< WFiber > > fibs )
    : WMixinVector< WFiber >( *fibs ), // COPYING this into this since, WMixinVector has no possibility for references or boost::shared_ptr
      WDataSet()
{
}

WDataSetFiberVector::WDataSetFiberVector( boost::shared_ptr< const WDataSetFibers > fiberDS )
    : WMixinVector< WFiber >(),
      WDataSet()
{
    if( !fiberDS.get() )
    {
        wlog::error( "WDataSetFiberVector" ) << "During constructing a WDataSetFiberVector out of an empty WDataSetFibers";
        return;
    }

    if( fiberDS->getFileName() != "" )
    {
        setFileName( fiberDS->getFileName() );
    }
    size_t numLines = fiberDS->size();
    const std::vector< size_t >& lineLengths = *fiberDS->getLineLengths();
    reserve( numLines );

    while( size() < numLines )
    {
        WFiber fib;
        for( size_t i = 0; i < lineLengths[ size() ]; ++i )
        {
            fib.push_back( fiberDS->getPosition( size(), i ) );
        }
        push_back( fib );
    }
}

WDataSetFiberVector::WDataSetFiberVector( const WDataSetFiberVector& other )
    : WMixinVector< WFiber >( other ),
      WDataSet()
{
}

WDataSetFiberVector& WDataSetFiberVector::operator=( const WDataSetFiberVector& other )
{
    if( this == &other )
    {
        return *this;
    }
    assign( other.begin(), other.end() );
    return *this;
}

WDataSetFiberVector::~WDataSetFiberVector()
{
    // since no pointer deallocation is needed, nothing to do here
}

void WDataSetFiberVector::sortDescLength()
{
    std::sort( begin(), end(), hasMorePointsThen );
}

boost::shared_ptr< WDataSetFiberVector > WDataSetFiberVector::generateDataSetOutOfUsedFibers( const std::vector< bool > &unused ) const
{
    boost::shared_ptr< WDataSetFiberVector > result = boost::shared_ptr< WDataSetFiberVector >( new WDataSetFiberVector() );
    assert( unused.size() == size() );
    for( size_t i = 0 ; i < unused.size(); ++i )
    {
        if( !unused[i] )
        {
            result->push_back( at( i ) );
        }
    }
    return result;
}

bool WDataSetFiberVector::isTexture() const
{
    return false;
}

const std::string WDataSetFiberVector::getName() const
{
    return "WDataSetFiberVector";
}

const std::string WDataSetFiberVector::getDescription() const
{
    return "Contains tracked fiber data.";
}

boost::shared_ptr< WPrototyped > WDataSetFiberVector::getPrototype()
{
    if( !m_prototype )
    {
        m_prototype = boost::shared_ptr< WPrototyped >( new WDataSetFiberVector() );
    }

    return m_prototype;
}

boost::shared_ptr< WDataSetFibers > WDataSetFiberVector::toWDataSetFibers() const
{
    boost::shared_ptr< std::vector< float > > points = boost::shared_ptr< std::vector< float > >( new std::vector< float > );
    boost::shared_ptr< std::vector< size_t > > fiberStartIndices = boost::shared_ptr< std::vector< size_t > >( new std::vector< size_t > );
    boost::shared_ptr< std::vector< size_t > > fiberLengths = boost::shared_ptr< std::vector< size_t > >( new std::vector< size_t > );
    boost::shared_ptr< std::vector< size_t > > pointFiberMapping = boost::shared_ptr< std::vector< size_t > >( new std::vector< size_t > );

    fiberStartIndices->reserve( size() );
    fiberLengths->reserve( size() );
    // other reserving for points and pointFiberMapping is not possible here without cycling through the whole data

    size_t fiberID = 0;
    for( const_iterator cit = begin(); cit != end(); ++cit, ++fiberID )
    {
        const WFiber& fib = *cit;
        // the division by 3 is necessary since it carries the point numbers not the number of the i'th component
        fiberStartIndices->push_back( points->size() / 3 );
        fiberLengths->push_back( fib.size() );
        for( WFiber::const_iterator fit = fib.begin(); fit != fib.end(); ++fit )
        {
            points->push_back( ( *fit )[0] );
            points->push_back( ( *fit )[1] );
            points->push_back( ( *fit )[2] );
            pointFiberMapping->push_back( fiberID );
        }
    }

    return boost::shared_ptr< WDataSetFibers >( new WDataSetFibers( points, fiberStartIndices, fiberLengths, pointFiberMapping ) );
}

boost::shared_ptr< WFiber > centerLine( boost::shared_ptr< const WDataSetFiberVector > tracts )
{
    if( !tracts || tracts->empty() ) // invalid data produces invalid center lines
    {
        return boost::shared_ptr< WFiber >( new WFiber() );
    }

    size_t avgTractSize = 0;
    for( WDataSetFiberVector::const_iterator cit = tracts->begin(); cit != tracts->end(); ++cit )
    {
        avgTractSize += cit->size();
    }
    avgTractSize /= tracts->size();

    WFiber firstTract( tracts->front() );
    firstTract.resampleByNumberOfPoints( avgTractSize );
    boost::shared_ptr< WFiber > result( new WFiber( firstTract ) ); // copy the first tract into result centerline

    for( size_t tractIndex = 1; tractIndex < tracts->size(); ++tractIndex )
    {
        WFiber other( tracts->at( tractIndex ) );
        other.resampleByNumberOfPoints( avgTractSize );
        other.unifyDirectionBy( firstTract );

        for( size_t pointIndex = 0; pointIndex < avgTractSize; ++pointIndex )
        {
            result->at( pointIndex ) += other[ pointIndex ];
        }
    }

    for( size_t pointIndex = 0; pointIndex < avgTractSize; ++pointIndex )
    {
        result->at( pointIndex ) /= static_cast< double >( tracts->size() );
    }

    return result;
}

boost::shared_ptr< WFiber > longestLine( boost::shared_ptr< const WDataSetFiberVector > tracts )
{
    if( !tracts || tracts->empty() ) // invalid data produces invalid longest lines
    {
        return boost::shared_ptr< WFiber >( new WFiber() );
    }

    size_t maxSize = 0;
    size_t maxIndex = 0;

    for( size_t tractIndex = 0; tractIndex < tracts->size(); ++tractIndex )
    {
        if( maxSize < tracts->at( tractIndex ).size() )
        {
            maxSize = tracts->at( tractIndex ).size();
            maxIndex = tractIndex;
        }
    }

    return boost::shared_ptr< WFiber >( new WFiber( tracts->at( maxIndex ) ) );
}

boost::shared_ptr< WFiber > centerLine( boost::shared_ptr< const WDataSetFibers > tracts )
{
    return centerLine( boost::shared_ptr< WDataSetFiberVector >( new WDataSetFiberVector( tracts ) ) );
}

boost::shared_ptr< WFiber > longestLine( boost::shared_ptr< const WDataSetFibers > tracts )
{
    return longestLine( boost::shared_ptr< WDataSetFiberVector >( new WDataSetFiberVector( tracts ) ) );
}
