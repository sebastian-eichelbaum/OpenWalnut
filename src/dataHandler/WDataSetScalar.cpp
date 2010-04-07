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

#include "../common/WAssert.h"
#include "../common/WLimits.h"
#include "WDataSetSingle.h"

#include "WDataSetScalar.h"


WDataSetScalar::WDataSetScalar( boost::shared_ptr< WValueSetBase > newValueSet,
                                boost::shared_ptr< WGrid > newGrid )
    : WDataSetSingle( newValueSet, newGrid )
{
    WAssert( newValueSet, "No value set given." );
    WAssert( newGrid, "No grid given." );
    WAssert( newValueSet->size() == newGrid->size(), "Number of values unequal number of positions in grid." );
    WAssert( newValueSet->order() == 0, "The value set does not contain scalars." );

    double max = wlimits::MIN_DOUBLE;
    double min = wlimits::MAX_DOUBLE;

    for( size_t i = 0; i < newValueSet->size(); ++i )
    {
        double tmp = newValueSet->getScalarDouble( i );
        max = max < tmp ? tmp : max;
        min = min > tmp ? tmp : min;
    }
    m_maximum = max;
    m_minimum = min;
}

WDataSetScalar::WDataSetScalar( boost::shared_ptr< WValueSetBase > newValueSet,
                                boost::shared_ptr< WGrid > newGrid,
                                double max,
                                double min )
    : WDataSetSingle( newValueSet, newGrid )
{
    WAssert( newValueSet, "No value set given." );
    WAssert( newGrid, "No grid given." );
    WAssert( newValueSet->size() == newGrid->size(), "Number of values unequal number of positions in grid." );
    WAssert( newValueSet->order() == 0, "The value set does not contain scalars." );

    WAssert( max >= min, "max must be at least as large as min." );
    m_maximum = max;
    m_minimum = min;
}

WDataSetScalar::~WDataSetScalar()
{
}

double WDataSetScalar::getMax() const
{
    return m_maximum;
}

double WDataSetScalar::getMin() const
{
    return m_minimum;
}
