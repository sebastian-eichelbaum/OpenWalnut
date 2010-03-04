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

#ifndef WDSTMETRIC_H
#define WDSTMETRIC_H

#include "WZhangMetric.h"
#include "../../datastructures/WFiber.h"

/**
 * This is the Smaller thresholded distance as described by Zhang.
 */
class WDSTMetric : WZhangMetric
{
public:
    /**
     * Constructs this metric with a certain threshold.
     *
     * \param thresholdSquare Threshold upto which the distances should be
     * ignored given as square for reasons of performance.
     */
    explicit WDSTMetric( double thresholdSquare );

    /**
     * \param q First fiber
     * \param r Second fiber
     * \return The minimum of dt(Q,R) and dt(R,Q)
     */
    virtual double dist( const wmath::WFiber &q, const wmath::WFiber &r ) const;

private:
};

#endif  // WDSTMETRIC_H
