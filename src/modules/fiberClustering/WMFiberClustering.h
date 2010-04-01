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
#include "../../dataHandler/datastructures/WFiberCluster.h"
#include "../../dataHandler/WDataSetFiberVector.h"
#include "../../graphicsEngine/WGEGroupNode.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../common/datastructures/WFiber.h"

/**
 * Test module for Clustering fibers
 * \ingroup modules
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
     * Get the icon for this module in XPM format.
     */
    virtual const char** getXPMIcon() const;

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

    /**
     * Callback for m_active. Overwrite this in your modules to handle m_active changes separately.
     */
    virtual void activate();

private:
    /**
     * Group fibers into WFiberCluster.
     */
    void cluster();

    /**
     * Generates an osg geode for the given cluster with the given color
     *
     * \param cluster The WFiberCluster which should be drawn
     * \param color The color of alle fibers of the given cluster
     * \return geode containing the graphical representation of the cluster
     */
    osg::ref_ptr< osg::Geode > genFiberGeode( const WFiberCluster &cluster, const WColor& color ) const;

    /**
     * Choose colors and build and commit new FgePrimitve.
     *
     * \return The OSG group node with all clusters:w
     */
    osg::ref_ptr< WGEGroupNode > paint() const;

    /**
     * Checks if the look up table exists. This is done via the original file
     * name containing the fibers but different suffix.
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
     * in the same directory as the fib file.
     *
     * \return fib file name where the extension is changed to ".dlt"
     */
    std::string lookUpTableFileName() const;

    /**
     * Updates the output with new cluster.
     */
    void updateOutput();

    size_t                       m_lastFibsSize; //!< Last known number of fibers
    bool                         m_dLtTableExists; //!< Flag whether there is already a dLt look up table or not.
    std::vector< size_t >        m_clusterIDs; //!< Stores the cluster id of every fiber so it is fast to get the cluster of a given fiber.
    std::vector< WFiberCluster > m_clusters; //!< Stores all WFiberClusters


    /**
     * Proximity threshold, which defines the minimum distance which should be
     * considered in the calculation of the mean-minimum-distance of two fibers.
     */
    WPropDouble m_proximity_t;
    WPropDouble m_maxDistance_t; //!< Maximum distance of two fibers in one cluster.
    WPropInt    m_minClusterSize; //!< All clusters up to this size will be discarded
    WPropInt    m_clusterOutputID; //!< Specifies which cluster should be connected to the Output
    WPropBool   m_invisibleFibers; //!< If true the fibers of all clusters are not shown
    WPropBool   m_run; //!< "Button" to initiate clustering with the given properties
    // bool m_separatePrimitives; //!< If true each cluster has its own OSG node

    boost::shared_ptr< WDataSetFiberVector >                m_fibs; //!< Reference to the WDataSetFiberVector object
    boost::shared_ptr< WDataSetFibers >                     m_rawFibs; //!< Reference to the WDataSetFibers object
    boost::shared_ptr< WModuleInputData< WDataSetFibers > > m_fiberInput; //!< Input connector for a fiber dataset.
    boost::shared_ptr< WModuleOutputData< WFiberCluster > > m_output; //!< Output connector for the first cluster.
    boost::shared_ptr< WDXtLookUpTable >                    m_dLtTable; //!< Distance matrix lookUpTable

    boost::shared_ptr< WCondition > m_update; //!< Fires to indicate a full update (reclustering, repainting, etc...)
    boost::shared_ptr< WCondition > m_updateOutput; //!< Fires to indicate an update of output data only

    /**
     * OSG node for this module. All other OSG nodes of this module should be
     * placed as child to this node.
     */
    osg::ref_ptr< WGEGroupNode > m_osgNode;

    /**
     * Inidcates if a given output ID is valid!
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
         * You need to overwrite this method. It decides whether the specified new value should be accepted or not.
         * We don't use the setMin setMax since the boundaries may change during runtime!
         *
         * \param property the property thats going to be changed.
         * \param value the new value
         *
         * \return true if the new value is OK.
         */
        virtual bool accept( boost::shared_ptr< WPropertyVariable< WPVBaseTypes::PV_INT > >  property, WPVBaseTypes::PV_INT value );
    private:
        const std::vector< WFiberCluster >& m_clusters; //!< Accept need to look into the cluster array for max size constraint
    };
};

inline const std::string WMFiberClustering::getName() const
{
    return std::string( "Fiber Clustering" );
}

inline const std::string WMFiberClustering::getDescription() const
{
    return std::string( "Clusters fibers from a WDataSetFiberVector" );
}

#endif  // WMFIBERCLUSTERING_H
