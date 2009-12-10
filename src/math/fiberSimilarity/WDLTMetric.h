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

#ifndef WDLTMETRIC_H
#define WDLTMETRIC_H

#include "WZhangMetric.h"
#include "../WFiber.h"

/**
 * This is the Larger thresholded distance as described by Zhang.
 */
class WDLTMetric : public WZhangMetric
{
public:
    /**
     * Constructs this metric with a certain threshold.
     */
    explicit WDLTMetric( double thresholdSquare );

    /**
     * \param q First fiber
     * \param r Second fiber
     * \return The maximum of dt(Q,R) and dt(R,Q)
     */
    virtual double dist( const wmath::WFiber &q, const wmath::WFiber &r ) const;

private:
};

#endif  // WDLTMETRIC_H
