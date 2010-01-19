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

#include "../../common/datastructures/WFiberCluster.h"
#include "../../dataHandler/WDataSetSingle.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "WBresenhamDBL.h"
#include "WRasterAlgorithm.h"

/**
 * TODO(math): document this
 * \ingroup modules
 */
class WMVoxelizer : public WModule
{
friend class WMVoxelizerTest;
public:
    /**
     * Constructs new FiberTestModule
     */
    WMVoxelizer();

    /**
     * Destructs this FiberTestModule
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
     * Generates an OSG geode for the fibers in the given cluster to display
     * only this cluster.
     *
     * \return OSG geode containing the fibers of the cluster.
     */
    osg::ref_ptr< osg::Geode > genFiberGeode() const;

    /**
     * Every parameter change this function is doing:
     *  - bounding box and grid generation
     *  - executing a rasterization algo for the fibers
     *  - generate dataset out of the grid and a value set
     *  - display the rastered voxels
     */
    void update();

    /**
     * Builds an OSG geode where all voxels inside the dataSet which are not
     * zero are drawn as cuboids.
     *
     * \param dataset The dataset which should be drawn
     *
     * \return OSG Geode with the voxels as cuboids.
     */
    osg::ref_ptr< osg::Geode > genDataSetGeode( boost::shared_ptr< WDataSetSingle > dataset ) const;

    /**
     * Performs rasterization with the given algorithm.
     *
     * \param algo The algorithm which actualy rasters every fiber.
     */
    void raster( boost::shared_ptr< WBresenhamDBL > algo ) const;

    /**
     * Creates two vertices describing the bounding box of a cluster.
     *
     * \param cluster With its fibers
     *
     * \return Pair of WPositions: first == front lower left, second == back upper right
     */
    std::pair< wmath::WPosition, wmath::WPosition > createBoundingBox( const WFiberCluster& cluster ) const;

    /**
     * Generates an OSG geode for the bounding box.
     *
     * \param fll Front lower left corner of the bounding box.
     * \param bur Back upper right corner of the bounding box.
     *
     * \return The OSG geode containing the 12 edges of the box.
     */
    osg::ref_ptr< osg::Geode > genBBGeode( const wmath::WPosition& fll,
                                           const wmath::WPosition& bur ) const;


private:
    /**
     * Input connector for a fiber cluster dataset.
     */
    boost::shared_ptr< WModuleInputData< const WFiberCluster > > m_input;

    /**
     * Output connector for a voxelized fiber cluster dataset.
     */
    boost::shared_ptr< WModuleOutputData< WDataSetSingle > > m_output;

    /**
     * Reference to the fiber clusters
     */
    boost::shared_ptr< const WFiberCluster > m_clusters;

    /**
     * OSG node for this module. All other OSG nodes of this module should be
     * placed as child to this node.
     */
    osg::ref_ptr< osg::Group > m_osgNode;
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
