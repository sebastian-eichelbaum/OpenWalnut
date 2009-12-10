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

WFiberCluster::WFiberCluster()
{
}

WFiberCluster::WFiberCluster( size_t index )
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
