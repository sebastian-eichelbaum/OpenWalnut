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

#include "WResampleBySegLength.h"

WResampleBySegLength::WResampleBySegLength()
    : WObjectNDIP< WResampling_I >( "#Points Resampling", "Each fiber has a an ensured number of points after resampling" )
{
    m_segLength = m_properties->addProperty( "#Points", "Number of points each fiber should have afterwards", 1.0 );
    m_segLength->setMin( 0.0 );
}

WFiber WResampleBySegLength::resample( WFiber fib ) const
{
    fib.resampleBySegmentLength( m_segLength->get( true ) );
    return fib;
}
