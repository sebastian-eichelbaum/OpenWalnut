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

#ifndef WMVOXELIZER_H
#define WMVOXELIZER_H

#include <string>
#include <utility>

#include <osg/Geode>

#include "core/kernel/WModule.h"

class WBresenhamDBL;
class WDataSetScalar;
class WFiberCluster;
class WRasterAlgorithm;
class WGEManagedGroupNode;
template< class T > class WModuleInputData;

/**
 * Traces a given set of deterministic tracts as given by a dataset of deterministic tracts (optionally only a subset may be
 * processed) in a voxel-wise manner.
 * Every voxel which is hit by one or more tracts or tract-segments is marked with a scalar and stored in a dataset scalar.
 * \ingroup modules
 */
class WMVoxelizer : public WModule
{
/**
* Only UnitTests may be friends.
*/
friend class WMVoxelizerTest;
public:
    /**
     * Default Constructor.
     */
    WMVoxelizer();

    /**
     * Default Destructor.
     */
    virtual ~WMVoxelizer();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return Description to module.
     */
    virtual const std::string getDescription() const;

    /**
     * Due to the prototype design pattern used to build modules, this method
     * returns a new instance of this method. NOTE: it should never be
     * initialized or modified in some other way. A simple new instance is
     * required.
     *
     * \return The prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     * Get the icon for this module in XPM format.
     * \return The icon.
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
     * Generates a intensity dataset where each tract is rasterized into. If additionally the (optional) cluster is given as
     * subselection only tracts with IDs inside this dataset are considered. If additionally a parameterization was selected
     * also a corresponding dataset will be generated and returned.
     *
     * \param tracts Dataset of deterministic tracts.
     * \param cluster Optional selection of a subset of tracts.
     *
     * \return Two references to datasets where the intensity field is always the first and should be always present, but not when
     * an error has occured. Since parameterization is optional, the second dataset will only be generated when a parameterization
     * algo has been selected first.
     */
    boost::array< boost::shared_ptr< WDataSetScalar >, 2 > generateDatasets(
            boost::shared_ptr< const WDataSetFibers > tracts,
            boost::shared_ptr< const WFiberCluster > cluster ) const;

    /**
     * Removes or inserts geode for the center line of the current cluster into this modules group node.
     */
    void updateCenterLine();

   /**
     * Builds an OSG geode where all voxels inside the dataSet which are not
     * zero are drawn as cuboids.
     *
     * \param dataset The dataset which should be drawn
     *
     * \return OSG node with the voxels as cuboids.
     */
    osg::ref_ptr< osg::Node > genDataSetGeode( boost::shared_ptr< WDataSetScalar > dataset ) const;

    /**
     * Performs rasterization with the given algorithm on either all tracts or only a subset if given.
     *
     * \param algo The algorithm which actually rasters every fiber.
     * \param tracts Dataset of tracts.
     * \param cluster A subset of tracts.
     */
    void raster( boost::shared_ptr< WRasterAlgorithm > algo, boost::shared_ptr< const WDataSetFibers > tracts,
        boost::shared_ptr< const WFiberCluster > cluster ) const;

    /**
     * Creates two vertices describing the bounding box of a cluster.
     *
     * \param cluster With its fibers
     *
     * \return The axis aligned bounding box.
     */
    WBoundingBox createBoundingBox( const WFiberCluster& cluster ) const;

    /**
     * Constructs a grid out of the current tract dataset or out of a subset (selection) of this dataset.
     *
     * \param tracts Dataset of deterministic tracts.
     * \param cluster Optional subselection of tracts.
     *
     * \return A WGridRegular3D reference wherein the voxels may be marked.
     */
    boost::shared_ptr< WGridRegular3D > constructGrid( boost::shared_ptr< const WDataSetFibers > tracts,
            boost::shared_ptr< const WFiberCluster > cluster ) const;

    /**
     * Finds and returns a copy of the longest line (in term of \#points) in the dataset, or in a subset of it specified by the
     * given cluster. If the cluster is empty all tracts are considered.
     *
     * \param tracts The dataset of tracts.
     * \param cluster A subset of tracts.
     *
     * \note This involves heavy computational time, including dataset convertion to WDataSetFiberVector, since I didn't had the
     * time to use the crappy index lists of the original WDataSetFibers.
     *
     * \return A reference of a copy of the longest line.
     */
    boost::shared_ptr< WFiber > longestLine( boost::shared_ptr< const WDataSetFibers > tracts,
            boost::shared_ptr< const WFiberCluster > cluster = boost::shared_ptr< const WFiberCluster >() ) const;

    /**
     * Finds and returns a copy of the center line in the dataset, or in a subset of it specified by the given cluster. If the
     * cluster is empty all tracts are considered.
     *
     * \param tracts The dataset of tracts.
     * \param cluster A subset of tracts.
     *
     * \note This involves heavy computational time, including dataset convertion to WDataSetFiberVector, since I didn't had the
     * time to use the crappy index lists of the original WDataSetFibers.
     *
     * \return A reference of a copy of the center line.
     */
    boost::shared_ptr< WFiber > centerLine( boost::shared_ptr< const WDataSetFibers > tracts,
            boost::shared_ptr< const WFiberCluster > cluster = boost::shared_ptr< const WFiberCluster >() ) const;

private:
    /**
     * Input connector for a fiber dataset.
     */
    boost::shared_ptr< WModuleInputData< const WDataSetFibers > > m_tractIC;

    /**
     * Input connector for an optional selection of some fibers in the fiber dataset via a cluster.
     */
    boost::shared_ptr< WModuleInputData< const WFiberCluster > > m_clusterIC;

    /**
     * Output connector for a voxelized cluster.
     */
    boost::shared_ptr< WModuleOutputData< WDataSetScalar > > m_voxelizedOC;

    /**
     * Output providing parameterization to other algorithms. It provides a scalar field which gets filled with the parameterization of the
     * fibers, i.e. current integrated length.
     */
    boost::shared_ptr< WModuleOutputData< WDataSetScalar > > m_paramOC;

    /**
     * Reference to the fiber dataset.
     */
    boost::shared_ptr< const WDataSetFibers > m_tracts;

    /**
     * OSG root node for this module
     */
    osg::ref_ptr< WGEManagedGroupNode > m_rootNode;

    /**
     * module is performing an expensive update
     */
    boost::shared_ptr< WCondition > m_fullUpdate;

    /**
     * Enable/Disable anti-aliased drawing of voxels
     */
    WPropBool m_antialiased;

    /**
     * List for selecting the rasterization method.
     *
     * \note At the moment there is only Bresenham, but this might change.
     */
    WPropSelection m_rasterAlgo;

    /**
     * The number of voxels per unit in the coordinate system
     */
    WPropInt  m_voxelsPerUnit;

    /**
     * The available parameterization algorithms.
     */
    boost::shared_ptr< WItemSelection > m_paramAlgoSelections;

    /**
     * The actually selected parameterization algorithm.
     */
    WPropSelection m_parameterAlgo;

    /**
     * add colormapping
     */
    WPropBool m_colorMapping;

    /**
     * Add lighting to the scene
     */
    WPropBool m_phongShading;
};

inline const std::string WMVoxelizer::getName() const
{
    return std::string( "Voxelizer" );
}

inline const std::string WMVoxelizer::getDescription() const
{
    return std::string( "Rasterize or voxelize a dataset with the given tracts." );
}

#endif  // WMVOXELIZER_H
