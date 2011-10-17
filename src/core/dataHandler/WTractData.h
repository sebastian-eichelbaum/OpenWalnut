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

#ifndef WTRACTDATA_H
#define WTRACTDATA_H

#include <vector>

#include <boost/shared_ptr.hpp>

/**
 * Stores the data of deterministic fiber tractograms. Derived or optional data as tangents, FA,
 * etc. are not saved in here, and never will be! Just the polylines.
 */
class WTractData
{
public:
    /**
     * Constructs a new WTractData.
     * \param pointComponents x, y, and z components of each position of each tract
     * \param startIndices For each tract the index of the first x component in pointComponents.
     */
    WTractData( boost::shared_ptr< std::vector< float > > pointComponents, boost::shared_ptr< std::vector< size_t > > startIndices );

    /**
     * \return Number of tracts.
     */
    size_t numTracts() const;

protected:
private:
    /**
     * Stores the all components of all vertices of all tracts. First x, y and finally z component
     * are arranged in this manner: \f$[x_0, y_0, z_0, ..., x_{m_0}, y_{m_0}, z_{m_0}, ... , ..., x_{m_k},
     * y_{m_k}, z_{m_k}]\f$ where there are \f$k\f$ many tracts where the i'th tract has \f$m_i\f$
     * vertices, but \f$3m_i\f$ compontents. In other words: m_points.size() / 3 == number of
     * vertices.
     *
     * \note the reason for beeing restricted to float is, that graphic boards and also the tracking
     * algorithms which produce those tracks are just using floats.
     */
    boost::shared_ptr< std::vector< float > > m_pointComponents;

    /**
     * Stores for every tract the index number where it starts in the \ref m_pointComponents array.
     * This means the index of each tracts first component \f$x_0\f$.
     *
     * \note the reason for using \c size_t instead of \c unsigned \c int is that more tracts with
     * more points are in sight.
     */
    boost::shared_ptr< std::vector< size_t > > m_startIndices;
};

inline size_t WTractData::numTracts() const
{
    if( m_startIndices )
    {
        return m_startIndices->size();
    }
    return 0;
}

#endif  // WTRACTDATA_H
