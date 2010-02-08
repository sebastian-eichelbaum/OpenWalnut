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

#include "../../dataHandler/WDataSetFiberVector.h"
#include "../../math/fiberSimilarity/WDLTMetric.h"
#include "../WLimits.h"
#include "../WTransferable.h"
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
