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
template< class T > class WModuleInputData;

/**
 * Traces a given set of deterministic tracts as given by a WFiberCluster in a voxel-wise manner.
 * Every voxel which is hit by one or more tracts or tract-segments is marked with a scalar.
 * \ingroup modules
 */
class WMVoxelizer : public WModule
{
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
     * Generates an OSG geode for the fibers in the given cluster to display
     * only this cluster.
     *
     * \return OSG geode containing the fibers of the cluster.
     */
    osg::ref_ptr< osg::Geode > genFiberGeode() const;

    /**
     * Every parameter change this function is doing:
     *  - redraw fibers
     *  - bounding box and grid generation
     *  - executing a rasterization algo for the fibers
     *  - generate dataset out of the grid and a value set
     *  - display the rastered voxels
     */
    void update();

    /**
     * Removes or inserts new fiber Geode into this modules group node.
     */
    void updateFibers();

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
     * \return OSG Geode with the voxels as cuboids.
     */
    osg::ref_ptr< osg::Geode > genDataSetGeode( boost::shared_ptr< WDataSetScalar > dataset ) const;

    /**
     * Performs rasterization with the given algorithm.
     *
     * \param algo The algorithm which actually rasters every fiber.
     */
    void raster( boost::shared_ptr< WRasterAlgorithm > algo ) const;

    /**
     * Creates two vertices describing the bounding box of a cluster.
     *
     * \param cluster With its fibers
     *
     * \return The axis aligned bounding box.
     */
    WBoundingBox createBoundingBox( const WFiberCluster& cluster ) const;

    /**
     * Constructs a grid out of the given bounding box.
     *
     * \param bb The bounding box
     *
     * \return A WGridRegular3D reference wherein the voxels may be marked.
     */
    boost::shared_ptr< WGridRegular3D > constructGrid( const WBoundingBox& bb ) const;

    /**
     * Callback for m_active. Overwrite this in your modules to handle m_active changes separately.
     */
    virtual void activate();

private:
    boost::shared_ptr< WModuleInputData< const WFiberCluster > > m_input; //!< Input connector for a fiber cluster
    boost::shared_ptr< WModuleOutputData< WDataSetScalar > > m_output; //!< Output connector for a voxelized cluster
    boost::shared_ptr< WModuleOutputData< WDataSetScalar > > m_dirOutput; //!< Output connector for a voxelized cluster (the fiber directions)

    /**
     * Output providing parameterization to other algorithms. It provides a scalar field which gets filled with the parameterization of the
     * fibers, i.e. current integrated length.
     */
    boost::shared_ptr< WModuleOutputData< WDataSetScalar > > m_parameterizationOutput;

    boost::shared_ptr< const WFiberCluster > m_clusters; //!< Reference to the fiber cluster

    osg::ref_ptr< WGEGroupNode > m_osgNode; //!< OSG root node for this module
    osg::ref_ptr< osg::Geode > m_fiberGeode; //!< OSG fiber geode
    osg::ref_ptr< osg::Geode > m_centerLineGeode; //!< OSG center line of the current cluster geode
    osg::ref_ptr< osg::Geode > m_boundingBoxGeode; //!< OSG bounding box geode
    osg::ref_ptr< osg::Geode > m_voxelGeode; //!< OSG voxel geode

    boost::shared_ptr< WCondition > m_fullUpdate; //!< module is performing an expensive update

    WPropBool m_antialiased; //!< Enable/Disable anti-aliased drawing of voxels
    WPropBool m_drawfibers; //!< Enable/Disable drawing of the fibers of a cluster
    WPropBool m_drawBoundingBox; //!< Enable/Disable drawing of a clusters BoundingBox
    WPropBool m_drawCenterLine; //!< Enable/Disable drawing of the current clusters CenterLine
    WPropBool m_lighting; //!< Enable/Disable lighting
    WPropBool m_drawVoxels; //!< Enable/Disable drawing of marked voxels (this is not hide/unhide since its expensive computation time too!)
    WPropString m_rasterAlgo; //!< Specifies the algorithm you may want to use for voxelization
    WPropInt  m_voxelsPerUnit;  //!< The number of voxels per unit in the coordinate system

    WPropDouble m_fiberTransparency; //!< Transparency of the fibers
    WPropColor m_explicitFiberColor; //!< If set not to 0.2 0.2 0.2 all fiber having this color

    /**
     * The available parameterization algorithms.
     */
    boost::shared_ptr< WItemSelection > m_paramAlgoSelections;

    /**
     * The actually selected parameterization algorithm.
     */
    WPropSelection m_parameterAlgo;

    /**
     * Node callback to hide unhide bounding box
     */
    class OSGCB_HideUnhideBB : public osg::NodeCallback
    {
    public: // NOLINT
        /**
         * Constructor.
         *
         * \param module just set the creating module as pointer for later reference.
         */
        explicit OSGCB_HideUnhideBB( WMVoxelizer* module )
            : m_module( module )
        {
        }

        /**
         * operator () - called during the update traversal.
         *
         * \param node the osg node
         * \param nv the node visitor
         */
        virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );
    private: // NOLINT

        /**
         * Pointer used to access members of the module to modify the node.
         */
        WMVoxelizer* m_module;
    };

    /**
     * Node callback to change lighting of the voxels
     */
    class OSGCB_ChangeLighting : public osg::NodeCallback
    {
    public: // NOLINT

        /**
         * Constructor.
         *
         * \param module just set the creating module as pointer for later reference.
         */
        explicit OSGCB_ChangeLighting( WMVoxelizer* module )
            : m_module( module )
        {
        }

        /**
         * operator () - called during the update traversal.
         *
         * \param node the osg node
         * \param nv the node visitor
         */
        virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );
    private: // NOLINT

        /**
         * Pointer used to access members of the module to modify the node.
         */
        WMVoxelizer* m_module;
    };
};

inline const std::string WMVoxelizer::getName() const
{
    return std::string( "Voxelizer" );
}

inline const std::string WMVoxelizer::getDescription() const
{
    return std::string( "Detects voxel of a given grid via a WGeometry." );
}

#endif  // WMVOXELIZER_H
