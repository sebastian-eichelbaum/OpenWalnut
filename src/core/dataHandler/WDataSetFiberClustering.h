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

#ifndef WDATASETFIBERCLUSTERING_H
#define WDATASETFIBERCLUSTERING_H

#include <map>
#include <string>

#include <boost/shared_ptr.hpp>

#include "datastructures/WFiberCluster.h"

#include "../common/exceptions/WInvalidID.h"
#include "../common/WTransferable.h"

/**
 * This is a dataset which represent a clustering of fibers. It does not itself contain the fiber-data. This dataset only contains the indices of
 * fibers belonging to the clusters.
 *
 * Each cluster has its own ID. A mapping between names and IDs is possible.
 */
class WDataSetFiberClustering: public WTransferable
{
public:
    /**
     * Convenience typedef for a boost::shared_ptr< WDataSetFiberClustering >.
     */
    typedef boost::shared_ptr< WDataSetFiberClustering > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WDataSetFiberClustering >.
     */
    typedef boost::shared_ptr< const WDataSetFiberClustering > ConstSPtr;

    /**
     * The type of the cluster map
     */
    typedef std::map< size_t, WFiberCluster::SPtr > ClusterMap;

    /**
     * Default constructor.
     */
    WDataSetFiberClustering();

    /**
     * constructor with cluster list.
     *
     * \param clustering the cluster map defining the clustering
     */
    explicit WDataSetFiberClustering( const ClusterMap &clustering );

    /**
     * Destructor.
     */
    virtual ~WDataSetFiberClustering();

    /**
     * The name of this transferable. This is useful information for the users.
     *
     * \return the name.
     */
    virtual const std::string getName() const;

    /**
     *
     * The description of this transferable. This is useful information for the users.
     *
     * \return A description
     */
    virtual const std::string getDescription() const;

    /**
     * Returns a prototype instantiated with the true type of the deriving class.
     *
     * \return the prototype.
     */
    static boost::shared_ptr< WPrototyped > getPrototype();

    /**
     * Sets the cluster at the given ID. If there is a cluster at this ID, it is replaced.
     *
     * \param id the ID of the cluster
     * \param cluster the cluster
     */
    virtual void setCluster( size_t id, WFiberCluster::SPtr cluster );

    /**
     * Returns the cluster with the given ID.
     *
     * \throw WInvalidID if the ID is not known.
     *
     * \param id the ID of the cluster to get
     *
     * \return the cluster
     */
    virtual WFiberCluster::SPtr getCluster( size_t id );

    /**
     * Returns the cluster with the given ID.
     *
     * \throw WInvalidID if the ID is not known.
     *
     * \param id the ID of the cluster to get
     *
     * \return the cluster
     */
    virtual WFiberCluster::ConstSPtr getCluster( size_t id ) const;

    /**
     * Returns the cluster with the given ID. If there is no cluster with this ID, an empty one is returned.
     *
     * \param id the ID of the cluster to get
     *
     * \return the cluster
     */
    virtual WFiberCluster::SPtr getOrCreateCluster( size_t id );

    /**
     * Removes the cluster with the specified ID. If it does not exist, nothing happens.
     *
     * \param id the id of the cluster
     */
    virtual void removeCluster( size_t id );

    /**
     * The begin iterator of the clustering for const iteration.
     *
     * \return the begin iterator
     */
    ClusterMap::const_iterator begin() const;

    /**
     * The begin iterator of the clustering for non-const iteration.
     *
     * \return the begin iterator
     */
    ClusterMap::iterator begin();

    /**
     * The end iterator of the clustering for const iteration.
     *
     * \return the begin iterator
     */
    ClusterMap::const_iterator end() const;

    /**
     * The end iterator of the clustering for non-const iteration.
     *
     * \return the end iterator
     */
    ClusterMap::iterator end();

    /**
     * Returns the amount of clusters in the clustering
     *
     * \return the amount of clusters in the clustering
     */
    size_t size() const;

protected:
    /**
     * Prototype for this dataset
     */
    static boost::shared_ptr< WPrototyped > m_prototype;
private:
    /**
     * The map between ID and cluster.
     */
    std::map< size_t, WFiberCluster::SPtr > m_clusters;
};

#endif  // WDATASETFIBERCLUSTERING_H
