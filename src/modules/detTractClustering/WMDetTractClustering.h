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

#ifndef WMDETTRACTCLUSTERING_H
#define WMDETTRACTCLUSTERING_H

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <osg/Geode>

#include "../../common/datastructures/WDXtLookUpTable.h"
#include "../../common/datastructures/WFiber.h"
#include "../../dataHandler/datastructures/WFiberCluster.h"
#include "../../dataHandler/WDataSetFiberVector.h"
#include "../../graphicsEngine/WGEManagedGroupNode.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"

/**
 * Clusters deterministic tractograms ala Zhang: http://dx.doi.org/10.1109/TVCG.2008.52 .
 * In detail this modules decomposes all tracts into groups which should represent anatomical
 * meaning full white matter fiber bundles.
 *
 * \ingroup modules
 */
class WMDetTractClustering : public WModule
{
friend class WMDetTractClusteringTest;
public:
    /**
     * Constructs new clustering module instance.
     */
    WMDetTractClustering();

    /**
     * Destructs this clustering instance.
     */
    virtual ~WMDetTractClustering();

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
     * Get the icon for this module in XPM format.
     */
    virtual const char** getXPMIcon() const;

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
     * Runs the clustering to update all data.
     *
     * \note This is very time consuming.
     */
    void update();

private:
    /**
     * Group tracts into a WFiberCluster.
     */
    void cluster();

    /**
     * Generates an osg geode for the given cluster with the given color
     *
     * \param cluster The WFiberCluster which should be drawn
     * \param color The color of all tracts of the given cluster
     * \return geode containing the graphical representation of the cluster
     */
    osg::ref_ptr< osg::Geode > genTractGeode( const WFiberCluster &cluster, const WColor& color ) const;

    /**
     * Choose colors and build and commit new OSG node.
     *
     * \return The OSG group node with all clusters
     */
    osg::ref_ptr< WGEManagedGroupNode > paint() const;

    /**
     * Checks if the look up table exists. This is done via the original file
     * name containing the tracts but different suffix.
     *
     * \return True if it look up table detection was successfull.
     */
    bool dLtTableExists();

    /**
     * Melds the given two clusters to the cluster with the lower ID.
     *
     * \param qClusterID ID of the first cluster
     * \param rClusterID ID of the second cluster
     */
    void meld( size_t qClusterID, size_t rClusterID );

    /**
     * Computes from the file name inside the given WDataSetFiberVector the
     * corresponding file name for the lookup table. This has the same
     * basename but the extension is now '.dlt' not '.fib' and resides
     * in the same directory as the tract file.
     *
     * \return Tract file name where the extension is changed to ".dlt"
     */
    std::string lookUpTableFileName() const;

    /**
     * Updates the output connector with new cluster. This member function is used by update() and as well from the moduleMain loop.
     */
    void updateOutput();

    size_t                       m_lastTractsSize; //!< Last known number of tracts
    bool                         m_dLtTableExists; //!< Flag whether there is already a dLt look up table or not.
    std::vector< size_t >        m_clusterIDs; //!< Stores the cluster id of every tract so it is fast to get the cluster of a given tract.
    std::vector< WFiberCluster > m_clusters; //!< Stores all WFiberClusters

    WPropDouble  m_proximity_t; //!< Minimum distance of points of two tracts which should be considered
    WPropDouble  m_maxDistance_t; //!< Maximum distance of two tracts in one cluster.
    WPropInt     m_minClusterSize; //!< All clusters up to this size will be discarded
    WPropInt     m_clusterOutputID; //!< Specifies which cluster should be connected to the Output
    WPropTrigger m_run; //!< Button to initiate clustering with the given properties

    // information properties
    WPropInt     m_numTracts; //!< Number of tracts given from input
    WPropInt     m_numUsedTracts; //!< Number of tracts used for rendering
    WPropInt     m_numClusters; //!< Number of clusters computed
    WPropInt     m_numValidClusters; //!< Number of clusters used for rendering
    WPropString  m_clusterSizes; //!< Sizes of the clusters

    boost::shared_ptr< WDataSetFiberVector >                m_tracts; //!< Reference to the WDataSetFiberVector object
    boost::shared_ptr< WDataSetFibers >                     m_rawTracts; //!< Reference to the WDataSetFibers object
    boost::shared_ptr< WModuleInputData< WDataSetFibers > > m_tractInput; //!< Input connector for a tract dataset.
    boost::shared_ptr< WModuleOutputData< WFiberCluster > > m_output; //!< Output connector for the first cluster.
    boost::shared_ptr< WDXtLookUpTable >                    m_dLtTable; //!< Distance matrix lookUpTable

    boost::shared_ptr< WCondition > m_update; //!< Used for register properties indicating a rerun of the moduleMain loop

    osg::ref_ptr< WGEManagedGroupNode > m_osgNode; //!< OSG node for this module.

    /**
     * Validates the output cluster ID!
     */
    class OutputIDBound: public WPropertyVariable< WPVBaseTypes::PV_INT >::PropertyConstraint
    {
        /**
         * Stores the reference to the cluster array so the accept() may look up every time the max size
         *
         * \param clusters const reference to the cluster vector
         */
        explicit OutputIDBound( const std::vector< WFiberCluster >& clusters );

        /**
         * Decides whether the specified new value should be accepted or not.
         *
         * \param property the property thats going to be changed.
         * \param value the new value
         *
         * \return true if the new value is OK.
         */
        virtual bool accept( boost::shared_ptr< WPropertyVariable< WPVBaseTypes::PV_INT > >  property, WPVBaseTypes::PV_INT value );
    private:
        const std::vector< WFiberCluster >& m_clusters; //!< accept() need to look into the cluster array for max size constraint
    };
};

inline const std::string WMDetTractClustering::getName() const
{
    return std::string( "Deterministic Tract Clustering" );
}

inline const std::string WMDetTractClustering::getDescription() const
{
    return std::string( "Clusters deterministic tractograms from a WDataSetFiberVector" );
}

#endif  // WMDETTRACTCLUSTERING_H