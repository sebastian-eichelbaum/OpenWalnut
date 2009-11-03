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

#ifndef WFIBER_H
#define WFIBER_H

#include <vector>
#include <utility>

#include "WLine.h"

// we need this to find the WFiberTest class which is not inside wmath namespace
// Additionally I found no way to put the WFiberTest class into wmath and CXXTest
// finds it by it self. Hence this is the only solution that might work
class WFiberTest;

namespace wmath
{
/**
 * Represents a neural pathway.
 */
class WFiber : public WLine
{
friend class WFiberTest;
public:
    explicit WFiber( const std::vector< WPosition > &points );

    /**
     * TODO(math): document this method in more detail
     * Optimized version which computes dt measure from qr and rq at once.
     */
    std::pair< double, double > dXt_optimized( const WFiber &other,
                                               const double thresholdSquare ) const;

    /**
     * Computes dt(Q, R, t) and dt(R, Q, t) and then the one which is smaller
     * is returned. Note this distance metric is symmetric.
     *
     * \param other The other fiber "R", while this fiber is taken as "Q".
     * \param thresholdSquare The threshold upto which the accumulated
     * distances should be considered
     *
     * \return The minimum of dt(Q, R, t) and dt(R, Q, t)
     */
    double dSt( const WFiber &other, const double thresholdSquare = 0.0 ) const;

    /**
     * Computes dt(Q, R, t) and dt(R, Q, t) and then the one which is smaller
     * is returned. Note this distance metric is symmetric.
     *
     * \param other The other fiber "R", while this fiber is taken as "Q".
     * \param thresholdSquare The threshold upto which the accumulated
     * distances should be considered
     *
     * \return The maximum dt(Q, R, t) and dt(R, Q, t)
     */
    double dLt( const WFiber &other, const double thresholdSquare = 0.0 ) const;

protected:
private:
};

/**
 * Boolean predicate indicating that the first fiber has a greater length then
 * the second one.
 *
 * \param first First fiber
 * \param second Second fiber
 */
inline bool hasGreaterLengthThen( const WFiber &first,
                           const WFiber &second )
{
    return first.size() > second.size();
}
}
#endif  // WFIBER_H
