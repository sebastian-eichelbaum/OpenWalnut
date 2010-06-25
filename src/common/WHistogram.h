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

#ifndef WHISTOGRAM_H
#define WHISTOGRAM_H

#include <vector>

/**
 * Container which associate values with (uniform width) bins (aka intervals or buckets).
 */
class WHistogram
{
public:
    /**
     * Default constructor.
     *
     * \param min
     * \param max
     * \param size
     */
    WHistogram( double min, double max, size_t size = 1000 );

    /**
     * Default destructor.
     */
    ~WHistogram();

    /**
     * Inserts a given value within the given range (min, max) into exactly one bin and increment its size.
     *
     * \param value Value to insert.
     */
    void insert( double value );

    /**
     * Computes number of invervals.
     *
     * \return Number of intervals.
     */
    size_t binSize() const;

    /**
     * Computes the number of inserted values so far.
     *
     * \return Number of values so far.
     */
    size_t valuesSize() const;

    /**
     * LooksUp how many elements are in this bin with the given index.
     *
     * \param index The bin number to look up for
     *
     * \return Number of values associated with this interval.
     */
    size_t operator[]( size_t index ) const;

protected:

private:
    /**
     * Bins to associate with the values. Each bin has the width of m_intervalWidth;
     */
    std::vector< size_t > m_bins;

    /**
     * Minimal value
     */
    double m_min;

    /**
     * Maximal value
     */
    double m_max;

    /**
     * The width of an interval is precomputed to save performance.
     */
    double m_intervalWidth;
};

#endif  // WHISTOGRAM_H
