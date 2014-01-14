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

#include <vector>
#include <cmath>

#include <osg/Vec3>

#include <core/common/math/WMath.h>

#include "WResampleByMaxPoints.h"

WResampleByMaxPoints::WResampleByMaxPoints()
    : WObjectNDIP< WResampling_I >( "Maximal Points Resampling", "Each fiber has no more than the given number "
            " of points and if a fiber has already lesser points, it is tested if additional segment length can be applied" )
{
    m_numPoints = m_properties->addProperty( "Max. #Points", "Maximal number of points", 20 );
    m_numPoints->setMin( 2 );
    m_segLength = m_properties->addProperty( "Min. Length", "If fiber has less points it is also resampled to the given segment length", 0.0 );
    m_segLength->setMin( 0.0 );
}

WFiber WResampleByMaxPoints::resample( WFiber fib ) const
{
    if( static_cast< int >( fib.size() ) > m_numPoints->get( true ) )
    {
        fib.resampleByNumberOfPoints( m_numPoints->get( true ) );
    }

    WFiber test( fib );
    if( m_segLength->get( true ) != 0.0 )
    {
        test.resampleBySegmentLength( m_segLength->get( true ) );
    }

    if( static_cast< int >( test.size() ) < m_numPoints->get( true ) && test.size() < fib.size() )
    {
        return test;
    }
    return fib;
}
