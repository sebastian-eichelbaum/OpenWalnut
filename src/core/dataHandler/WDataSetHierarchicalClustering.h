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

#ifndef WDATASETHIERARCHICALCLUSTERING_H_
#define WDATASETHIERARCHICALCLUSTERING_H_

#include <map>
#include <string>
#include <list>

#include <boost/shared_ptr.hpp>

#include "../common/exceptions/WInvalidID.h"
#include "../common/WTransferable.h"
#include "datastructures/WFiberCluster.h"
#include "datastructures/WTreeNode.h"
#include "WDataSet.h"

/**
 * Represents a hierarchy of clusters
 */
class WDataSetHierarchicalClustering : public WDataSet // NOLINT
{
public:
    // some type alias for the used arrays.
    /**
     * Pointer to dataset.
     */
    typedef boost::shared_ptr< WDataSetHierarchicalClustering > SPtr;

    /**
     * Pointer to const dataset.
     */
    typedef boost::shared_ptr< const WDataSetHierarchicalClustering > ConstSPtr;

     /**
     * Constructs a hierarchical clustering dataset
     *
     * \param rootNode the root node of the WTreeNode-tree holding indices to the clusterMap
     * \param clusterMap a map of fiber clusters accessible via index
     */
    WDataSetHierarchicalClustering( WTreeNode::SPtr rootNode, std::map< size_t, WFiberCluster::SPtr > clusterMap );

    /**
     * Constructs a new set of tracts. The constructed instance is not usable but needed for prototype mechanism.
     */
    WDataSetHierarchicalClustering();

    /**
     * Destructor.
     */
    virtual ~WDataSetHierarchicalClustering();

    /**
     * Gets the name of this prototype.
     *
     * \return the name.
     */
    virtual const std::string getName() const;

    /**
     * Gets the description for this prototype.
     *
     * \return the description
     */
    virtual const std::string getDescription() const;

    /**
     * Returns a prototype instantiated with the true type of the deriving class.
     *
     * \return the prototype.
     */
    static boost::shared_ptr< WPrototyped > getPrototype();

    /**
     * Returns the root cluster
     *
     * \return the root cluster
     */
    WTreeNode::SPtr getRootNode();

    /**
     * Returns the whole cluster map
     *
     * \return the whole cluster map
     */
    std::map< size_t, WFiberCluster::SPtr > getClusterMap();

    /**
     * Returns all clusters down (root node has highest level) to a certain level in the hierarchy
     *
     * \param node the node used as a starting point for the recursive lookup
     * \param level the maximum level for a node to be selected
     * \return the clusters down to a certain level in the hierarchy
     */
    std::list< WTreeNode::SPtr > getClustersDownToLevel( WTreeNode::SPtr node, size_t level );

protected:
    /**
     * The prototype as singleton.
     */
    static boost::shared_ptr< WPrototyped > m_prototype;

private:
    /**
     * Pointer to the root cluster
     */
    WTreeNode::SPtr m_rootNode;

    /**
     * Stores the cluster map
     */
    std::map< size_t, WFiberCluster::SPtr > m_clusters;
};

#endif  // WDATASETHIERARCHICALCLUSTERING_H
