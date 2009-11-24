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

#include "WFiberCluster.h"
#include "../../common/WLimits.h"
#include "../../dataHandler/WDataSetFibers.h"
#include "../../math/fiberSimilarity/WDLTMetric.h"

WFiberCluster::WFiberCluster( const boost::shared_ptr< WDataSetFibers > fibs )
{
    m_fibs = fibs;
}

WFiberCluster::WFiberCluster( size_t index, const boost::shared_ptr< WDataSetFibers > fibs )
{
    m_memberIndices.push_back( index );
    m_fibs = fibs;
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

double WFiberCluster::minDistance( const WFiberCluster& other, const double proximity_t ) const
{
    double result = wlimits::MAX_DOUBLE;
    double dist;
    WDLTMetric dLt( proximity_t * proximity_t );

    std::list< size_t >::const_iterator thisIdx = m_memberIndices.begin();
    for( ; thisIdx != m_memberIndices.end(); ++thisIdx )
    {
        std::list< size_t >::const_iterator otherIdx = other.m_memberIndices.begin();
        for( ; otherIdx != other.m_memberIndices.end(); ++otherIdx )
        {
            dist = dLt.dist( (*m_fibs)[ *thisIdx ], (*m_fibs)[ *otherIdx ] );
            if( dist < result )
            {
                result = dist;
            }
        }
    }
    assert( result < wlimits::MAX_DOUBLE );
    return result;
}

void WFiberCluster::updateClusterIndices( std::vector< size_t >& cid, // NOLINT
                                          const size_t newCID ) const
{
    assert( !this->empty() );
    std::list< size_t >::const_iterator cit = m_memberIndices.begin();
    for( ; cit != m_memberIndices.end(); ++cit )
    {
        cid.at( *cit ) = newCID;
    }
}

// void WFiberCluster::paintIntoFgePrimitive( FgeLineStrips *lstrips) const
// {
//     typedef std::vector< FArray > Fiber;
//     lstrips->setNewColor( m_color );
//     std::list< size_t >::const_iterator fibID = m_memberIndices.begin();
//     for( ; fibID != m_memberIndices.end(); ++fibID )
//     {
//         const Fiber& fiber = (*m_fibs)[ *fibID ].getIntermediateSteps();
//         Fiber::const_iterator vertex = fiber.begin();
//         for( ; vertex != fiber.end(); ++vertex )
//         {
//             lstrips->setNewVertex( *vertex );
//         }
//         lstrips->setNewStrip();
//     }
// }
//
// FgeLineStrips* WFiberCluster::createNewFgePrimitive() const
// {
//     FgeLineStrips *lstrips = new FgeLineStrips;
//     paintIntoFgePrimitive( lstrips );
//     return lstrips;
// }
