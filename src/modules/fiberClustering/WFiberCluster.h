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

#ifndef WFIBERCLUSTER_H
#define WFIBERCLUSTER_H

#include <list>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "../../common/WColor.h"
#include "../../dataHandler/WDataSetFibers.h"

/**
 * Represents a cluster of indices of a WDataSetFibers.
 */
class WFiberCluster
{
public:
    /**
     * Constructs an cluster with one fiber and a reference to the fiber dataset
     * to compute the intercluster distance.
     *
     * \param index The index of the first fiber belonging to this cluster
     * \param fibs Reference to the fiber dataset
     */
    WFiberCluster( size_t index, const boost::shared_ptr< WDataSetFibers > fibs );

    /**
     * Constructs an empty cluster with reference to the fiber dataset.
     *
     * \param fibs Reference to the fiber dataset
     */
    explicit WFiberCluster( const boost::shared_ptr< WDataSetFibers > fibs );

    /**
     * Returns true if there are no fibers in that cluster, false otherwise.
     */
    bool empty() const;

    /**
     * Merge the fibers of the other cluster with the fibers of this cluster.
     * Afterwards the other cluster is empty.
     *
     * \param other The other WFiberCluster which should merged into this one
     */
    void merge( WFiberCluster &other ); // NOLINT

    /**
     * Returns a const reference of all indices inside this cluster
     */
    const std::list< size_t >& getIndices() const;

    /**
     * Find the minimal distance for any pair of fibers where one fiber is
     * in this cluster and the other is in the other cluster.
     *
     * \param other the cluster that will be compared with this cluster
     * \param proximity_t Fiber distances below this threshold are not
     * conisdered
     */
    double minDistance( const WFiberCluster& other,
                        const double proximity_t = 0.0 ) const;

    /**
     * ToDo(math): Refactor! use merge function instead
     * For all indices in this cluster, update their cluster number in the 
     * given cid-vector with the given newCid cluster number.
     */
    void updateClusterIndices( std::vector< size_t >& cid, const size_t newCid ) const; // NOLINT

    /**
     * Sort the indices of fibers associated with this cluster in ascending
     * order.
     */
    void sort();

    /**
     * \return Number of fibers associated with this cluster.
     */
    size_t size() const;

    /**
     * Make this cluster empty. Note: The real fibers from fiber dataset are
     * not deleted.
     */
    void clear();

    /**
     * Sets the color of which all fibers of this clusters should be painted
     * with.
     *
     * \param color The color for all fibers of this cluster.
     */
    void setColor( WColor color );

    /**
     * \param other The other fiber which should be compared
     * \return true If both clusters having same fibers in same order!
     */
    bool operator==( const WFiberCluster& other ) const;

    /**
     * The opposite of the operator==
     *
     * \param other The other fiber which should be compared
     * \return false If both clusters having same fibers in same order!
     */
    bool operator!=( const WFiberCluster& other ) const;

private:
    /**
     * All indices in this set are members of this cluster
     */
    std::list< size_t > m_memberIndices;

    /**
     * Reference to the real fibers, so painting is possible.
     */
    boost::shared_ptr< WDataSetFibers > m_fibs;

    /**
     * Color which is used to paint the members of this cluster.
     */
    WColor m_color;
};

inline bool WFiberCluster::empty() const
{
    return m_memberIndices.empty();
}

inline void WFiberCluster::sort()
{
    m_memberIndices.sort();
}

inline size_t WFiberCluster::size() const
{
    return m_memberIndices.size();
}

inline void WFiberCluster::clear()
{
    m_memberIndices.clear();
}

inline void WFiberCluster::setColor( WColor color )
{
    m_color = color;
}

inline bool WFiberCluster::operator==( const WFiberCluster& other ) const
{
    return m_memberIndices == other.m_memberIndices;
}

inline bool WFiberCluster::operator!=( const WFiberCluster& other ) const
{
    return m_memberIndices != other.m_memberIndices;
}

inline const std::list< size_t >& WFiberCluster::getIndices() const
{
    return m_memberIndices;
}

inline std::ostream& operator<<( std::ostream& os, const WFiberCluster& c )
{
    using string_utils::operator<<;
    return os << c.getIndices();
}

#endif  // WFIBERCLUSTER_H
