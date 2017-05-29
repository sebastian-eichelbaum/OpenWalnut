//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2017 OpenWalnut Community
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

#ifndef WVISITRACE_H
#define WVISITRACE_H

#include <utility>
#include <vector>

/**
 * Class performing graph computations for VisiTrace algorithm.
 * The algorithm has been published here:
 * http://dx.doi.org/10.2312/PE.EuroVisShort.EuroVisShort2013.091-095
 *
 * The aim is to draw smooth lines on top visible structures in direct volume
 * renderings.
 */
class WVisiTrace
{
public:
    /**
     * Simple constructor performing initializations
     */
    WVisiTrace();

    /**
     * Get the final 3D line a vector of \ref WPosition.
     * If data has changed since last call a recomputation will be triggered.
     *
     * \return 3D line computed by visitrace.
     */
    std::vector<WPosition> getLine();

    /**
     * Add candidate positions and corresponding opacity jump values
     * for one viewing ray. These values are the basis for visiTrace
     * computation.
     *
     * \param candidates candidate positions and corresponding opacity jumps
     */
    void addCandidatesForRay( const std::vector< std::pair< double, WPosition > > candidates );

    /**
     * Erases all data to be able to start a new VisiTrace computation.
     */
    void reset();

protected:
private:
    /**
     * Optimization resulting in the desired 3D curve (\ref m_curve3D).
     */
    void performVisiTrace();

    std::vector< std::vector< WPosition > > m_candidatePositions; //!< The candidate positions for all rays
    std::vector< std::vector< double > > m_candidateJumps; //!< The opacity jumps belonging to the intervals of the candidate positions.
    std::vector< WPosition > m_curve3D; //!< The 3D curve computed by VisiTrace.

    bool m_dataChanged; //!< Indicates whether new data has been added since last VisiTrace computation.
};

#endif  // WVISITRACE_H
