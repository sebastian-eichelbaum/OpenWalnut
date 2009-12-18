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

#ifndef WMFIBERCLUSTERING_H
#define WMFIBERCLUSTERING_H

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <osg/Geode>

#include "../../common/datastructures/WDXtLookUpTable.h"
#include "../../common/datastructures/WFiberCluster.h"
#include "../../dataHandler/WDataSetFibers.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../math/WFiber.h"

/**
 * Test module for Clustering fibers
 */
class WMFiberClustering : public WModule
{
friend class WMFiberClusteringTest;
public:
    /**
     * Constructs new FiberTestModule
     */
    WMFiberClustering();

    /**
     * Destructs this FiberTestModule
     */
    virtual ~WMFiberClustering();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description to module.
     */
    virtual const std::string getDescription() const;

    /**
     * Due to the prototype design pattern used to build modules, this method
     * returns a new instance of this method. NOTE: it should never be
     * initialized or modified in some other way. A simple new instance is
     * required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     * Determine what to do if a property was changed.
     * \param propertyName Name of the property.
     */
    void slotPropertyChanged( std::string propertyName );

protected:
    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void moduleMain();

    /**
     * Initialize the connectors this module is using.
     */
    virtual void connectors();

    /**
     * Initialize the properties for this module.
     */
    virtual void properties();

    /**
     * Reclusters the scene.
     */
    void update();

private:
    /**
     * Group fibers into WFiberCluster.
     */
    void cluster();

    /**
     * Generates an osg geode for the given cluster with the given color
     *
     * \param cluster The WFiberCluster which should be drawn
     * \return geode containing the graphical representation of the cluster
     */
    osg::ref_ptr< osg::Geode > genFiberGeode( const WFiberCluster &cluster ) const;

    /**
     * Choose colors and build and commit new FgePrimitve
     */
    void paint();

    /**
     * Checks if the look up table is still valid (e.g. size() of m_fibs has
     * changed.
     */
    void checkDLtLookUpTable();

    /**
     * Melds the given two clusters to the cluster with the lower ID.
     *
     * \param qClusterID ID of the first cluster
     * \param rClusterID ID of the second cluster
     */
    void meld( size_t qClusterID, size_t rClusterID );

    /**
     * Stores the cluster id of every fiber so it is fast to get the cluster
     * of a given fiber.
     */
    std::vector< size_t > m_clusterIDs;

    boost::shared_ptr< WDataSetFibers > m_fibs; //!< Reference to the WDataSetFibers object

    double m_maxDistance_t; //!< Maximum distance of two fibers in one cluster.

    std::vector< WFiberCluster > m_clusters; //!< Stores all WFiberClusters

    bool m_dLtTableExists; //!< Flag whether there is already a dLt look up table or not.

    boost::shared_ptr< WDXtLookUpTable > m_dLtTable; //!< Distance matrix lookUpTable

    unsigned int m_minClusterSize; //!< All clusters up to this size will be discarded

    bool m_separatePrimitives; //!< If true each cluster has its own OSG node

    /**
     * Proximity threshold, which defines the minimum distance which should be
     * considered in the calculation of the mean-minimum-distance of two fibers.
     */
    double m_proximity_t;

    size_t m_lastFibsSize; //!< Last known number of fibers

    /**
     * Input connector for a fiber dataset.
     */
    boost::shared_ptr< WModuleInputData< WDataSetFibers > > m_fiberInput;

    /**
     * OSG node for this module. All other OSG nodes of this module should be
     * placed as child to this node.
     */
    osg::ref_ptr< osg::Group > m_osgNode;
};

inline const std::string WMFiberClustering::getName() const
{
    return std::string( "Fiber Clustering Module" );
}

inline const std::string WMFiberClustering::getDescription() const
{
    return std::string( "Clusters fibers from a WDataSetFibers" );
}

#endif  // WMFIBERCLUSTERING_H
