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

#ifndef WFIBERCLUSTERING_H
#define WFIBERCLUSTERING_H

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <osg/Geode>

#include "WFiberCluster.h"
#include "WDXtLookUpTable.h"
#include "../../dataHandler/WDataSetFibers.h"
#include "../../kernel/WModule.h"
#include "../../math/WFiber.h"

/**
 * Test module for Clustering fibers
 */
class WFiberClustering : public WModule
{
friend class WFiberClusteringTest;
public:
    /**
     * Constructs new FiberTestModule
     */
    WFiberClustering();

    /**
     * Destructs this FiberTestModule
     */
    virtual ~WFiberClustering();

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

protected:
    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void threadMain();

private:
    /**
     * Group fibers into WFiberCluster.
     */
    void cluster();

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
     * Reference to the WDataSetFibers object to work with.
     */
    boost::shared_ptr< WDataSetFibers > m_fibs;

    /**
     * Maximum distance of two fibers in one cluster.
     */
    double m_maxDistance_t;

    /**
     * Collection of all WFiberClusters containing their fiber indices.
     */
    std::vector< WFiberCluster > m_clusters;

    /**
     * Flag whether there is already a dLt look up table or not.
     */
    bool m_dLtTableExists;

    /**
     * Look up table for the dLt fiber distance metric, so reclustering is very
     * fast.
     */
    boost::shared_ptr< WDXtLookUpTable > m_dLtTable;

    /**
     * All clusters up to this size will be discarded after the clustering step
     */
    unsigned int m_minClusterSize;

    /**
     * If true all clusters have their own graphic primitives, so you may view
     * or select them solitary.
     */
    bool m_separatePrimitives;

    /**
     * Proximity threshold, which defines the minimum distance which should be
     * considered in the calculation of the mean-minimum-distance of two fibers.
     */
    double m_proximity_t;

    /**
     * Last known number of fibers
     */
    size_t m_lastFibsSize;
};

#endif  // WFIBERCLUSTERING_H
