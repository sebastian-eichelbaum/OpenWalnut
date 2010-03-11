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

#include <list>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "../../common/WLimits.h"
#include "../../common/WTransferable.h"
#include "../WDataSetFiberVector.h"
#include "WFiberCluster.h"

// TODO(math): The only reason why we store here a Reference to the fiber
// dataset is, we need it in the WMVoxelizer module as well as the clustering
// information. Since we don't have the possibility of multiple
// InputConnectors we must agglomerate those into one object. Please remove this.
// initializes the variable and provides a linker reference
// \cond
boost::shared_ptr< WPrototyped > WFiberCluster::m_prototype = boost::shared_ptr< WPrototyped >();
// \endcond

WFiberCluster::WFiberCluster()
    : WTransferable()
{
}

WFiberCluster::WFiberCluster( size_t index )
    : WTransferable()
{
    m_memberIndices.push_back( index );
}

void WFiberCluster::merge( WFiberCluster& other ) // NOLINT
{
    std::list< size_t >::const_iterator cit = other.m_memberIndices.begin();
    for( ; cit != other.m_memberIndices.end(); ++cit)
    {
        m_memberIndices.push_back( *cit );
    }
    // make sure that those indices aren't occuring anywhere else
    other.clear();
}

// NODOXYGEN
// \cond
void WFiberCluster::setDataSetReference( boost::shared_ptr< const WDataSetFiberVector > fibs )
{
    m_fibs = fibs;
}

boost::shared_ptr< const WDataSetFiberVector > WFiberCluster::getDataSetReference() const
{
    return m_fibs;
}

// TODO(math): The only reason why we store here a Reference to the fiber
// dataset is, we need it in the WMVoxelizer module as well as the clustering
// information. Since we don't have the possibility of multiple
// InputConnectors we must agglomerate those into one object. Please remove this.
boost::shared_ptr< WPrototyped > WFiberCluster::getPrototype()
{
    if ( !m_prototype )
    {
        m_prototype = boost::shared_ptr< WPrototyped >( new WFiberCluster() );
    }
    return m_prototype;
}
// \endcond

void WFiberCluster::generateCenterLine()
{
    // make copies of the fibers
    boost::shared_ptr< WDataSetFiberVector > fibs( new WDataSetFiberVector() );
    size_t avgFiberSize = 0;
    for( std::list< size_t >::const_iterator cit = m_memberIndices.begin(); cit != m_memberIndices.end(); ++cit )
    {
        fibs->push_back( m_fibs->at( *cit ) );
        avgFiberSize += fibs->back().size();
    }
    avgFiberSize /= fibs->size();

    unifyDirection( fibs );

    for( WDataSetFiberVector::iterator cit = fibs->begin(); cit != fibs->end(); ++cit )
    {
        cit->resample( avgFiberSize );
    }

    m_centerLine = boost::shared_ptr< wmath::WFiber >( new wmath::WFiber() );
    m_centerLine->reserve( avgFiberSize );
    for( size_t i = 0; i < avgFiberSize; ++i )
    {
        wmath::WPosition avgPosition( 0, 0, 0 );
        for( WDataSetFiberVector::const_iterator cit = fibs->begin(); cit != fibs->end(); ++cit )
        {
            avgPosition += cit->at( i );
        }
        avgPosition /= fibs->size();
        m_centerLine->push_back( avgPosition );
    }
}

void WFiberCluster::unifyDirection( boost::shared_ptr< WDataSetFiberVector > fibs ) const
{
    if( fibs->size() < 2 )
    {
        return;
    }

    assert( !( fibs->at( 0 ).empty() ) && "WFiberCluster.unifyDirection: Empty fiber processed.. aborting" );

    // first fiber defines direction
    const wmath::WFiber& firstFib = fibs->front();
    const wmath::WPosition start = firstFib.front();
    const wmath::WPosition m1    = firstFib.at( firstFib.size() * 1.0 / 3.0 );
    const wmath::WPosition m2    = firstFib.at( firstFib.size() * 2.0 / 3.0 );
    const wmath::WPosition end   = firstFib.back();

    for( WDataSetFiberVector::iterator cit = fibs->begin() + 1; cit != fibs->end(); ++cit )
    {
        const wmath::WFiber& other = *cit;
        double        distance = ( start - other.front() ).normSquare() +
                                 ( m1 - other.at( other.size() * 1.0 / 3.0 ) ).normSquare() +
                                 ( m2 - other.at( other.size() * 2.0 / 3.0 ) ).normSquare() +
                                 ( end - other.back() ).normSquare();
        double inverseDistance = ( start - other.back() ).normSquare() +
                                 ( m1 - other.at( other.size() * 2.0 / 3.0 ) ).normSquare() +
                                 ( m2 - other.at( other.size() * 1.0 / 3.0 ) ).normSquare() +
                                 ( end - other.front() ).normSquare();
        distance        /= 4.0;
        inverseDistance /= 4.0;
        if( inverseDistance < distance )
        {
            cit->reverseOrder();
        }
    }
}

boost::shared_ptr< wmath::WFiber > WFiberCluster::getCenterLine() const
{
    return m_centerLine;
}
