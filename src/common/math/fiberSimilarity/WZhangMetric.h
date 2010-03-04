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

#ifndef WZHANGMETRIC_H
#define WZHANGMETRIC_H

#include <utility>

#include "WFiberSimilarity.h"
#include "../WFiber.h"

/**
 * Abstract class which prepares the implementation of the thresholded mean min
 * distance as introduced by Zhang etal: http://dx.doi.org/10.1109/TVCG.2008.52
 */
class WZhangMetric : public WFiberSimilarity
{
public:
    /**
     * Constructs new algorithm with its threshold for minimal square
     * distances.
     *
     * \param thresholdSquare Threshold upto which the distances should be
     * ignored given as square for reasons of performance.
     */
    explicit WZhangMetric( double thresholdSquare );

    /**
     * Resets the threshold for minimal point distances used in computation.
     *
     * \param thresholdSquare Threshold upto which the distances should be
     * ignored given as square for reasons of performance.
     */
    void setThreshold( double thresholdSquare );

protected:
    /**
     * Optimized version which computes dt measure from fibers Q and R.
     * Since this is not symmetric it computes both: dt(Q,R) and dt(R,Q) at
     * once. dt(Q,R) computes the mean euclidian distance of the closest
     * points.
     *
     * \param q The first fiber
     * \param r The second fiber
     * \return Both dt(Q,R) and dt(R,Q).
     */
    std::pair< double, double > dXt_optimized( const wmath::WFiber &q, const wmath::WFiber &r ) const;

private:
    /**
     * Threshold upto this point-distances are threaded as zero-distance.
     */
    double m_thresholdSquare;
};

#endif  // WZHANGMETRIC_H
