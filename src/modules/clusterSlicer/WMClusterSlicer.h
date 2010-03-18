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

#ifndef WMCLUSTERSLICER_H
#define WMCLUSTERSLICER_H

#include <set>
#include <string>
#include <utility>
#include <vector>

#include "../../common/datastructures/WTriangleMesh.h"
#include "../../dataHandler/datastructures/WFiberCluster.h"
#include "../../dataHandler/datastructures/WJoinContourTree.h"
#include "../../graphicsEngine/WGEGroupNode.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../kernel/WModuleOutputData.h"

/**
 * This module is intended to be a simple template and example module. It can be used for fast creation of new modules by copying and refactoring
 * the files. It shows the basic usage of properties, update callbacks and how to wait for data.
 *
 * \warning ATM there are race conditions possible, e.g. a new FiberCluster arrives while the corresponding dataset is still in processing some
 * where. Hence we need an ensurance that the given dataset belongs to the given cluster!
 * \ingroup modules
 */
class WMClusterSlicer: public WModule
{
public:

    /**
     * Default constructor.
     */
    WMClusterSlicer();

    /**
     * Destructor.
     */
    virtual ~WMClusterSlicer();

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
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

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
     * Callback for m_active. Overwrite this in your modules to handle m_active changes separately.
     */
    virtual void activate();

    /**
     * Complete redraw of the scene.
     */
    void updateDisplay();

    /**
     * Complete redraw of the slices.
     */
    void updateSlices();

    /**
     * Renders the given mesh so that all trianges intersecting the slices are drawn in the slice specific color.
     *
     * \param mesh Triangle mesh describing the ISO surface
     */
    void renderMesh( const WTriangleMesh& mesh );

    /**
     * Computes the average of the positions inside the paramDS which are also inside the cluster volume main component.
     * All other positions will be deleted from this set.
     *
     * \param samplePoints Set of position where the parameter should be gained out of the paramDS, be aware
     *
     * \return The average value of those value where its positions are inside the ISO-Surface/Volume/Cluster. If no position is inside, 0.0 is returned.
     */
    double averageParameter( boost::shared_ptr< std::set< wmath::WPosition > > samplePoints ) const;

    /**
     * Generates new geode for marking the volume voxels
     *
     * \return Newly constructed geode
     */
    osg::ref_ptr< osg::Geode > generateISOVoxelGeode() const;

    osg::ref_ptr< WGEGroupNode > m_rootNode; //!< The root node used for this modules graphics.
    osg::ref_ptr< osg::Geode >   m_isoVoxelGeode; //!< Separate geode for voxels of the cluster volume
    osg::ref_ptr< WGEGroupNode > m_sliceGeode; //!< Separate geode for slices
    osg::ref_ptr< osg::Geode >   m_surfaceGeode; //!< Pointer to geode containing the surface.

    typedef WModuleInputData< WFiberCluster >  InputClusterType; //!< Internal alias for m_cluster type
    boost::shared_ptr< InputClusterType >  m_fiberCluster; //!< InputConnector for a fiber cluster with its CenterLine
    typedef WModuleInputData< WDataSetSingle > InputDataSetType; //!< Internal alias for m_*DataSets types
    boost::shared_ptr< InputDataSetType >  m_clusterDataSet; //!< InputConnector for the dataset derived from a voxelized cluster
    boost::shared_ptr< InputDataSetType >  m_paramDataSet; //!< InputConnector for the dataset of parameters like FA etc.
    typedef WModuleInputData< WTriangleMesh > InputMeshType; //!< Internal alias for the m_triangleMesh type
    boost::shared_ptr< InputMeshType >     m_triangleMesh; //!< InputConnector for the triangle mesh

    boost::shared_ptr< WFiberCluster >  m_cluster; //!< A cluster with its CenterLine
    boost::shared_ptr< WDataSetSingle > m_clusterDS; //!< Dataset derived from a voxelized cluster
    boost::shared_ptr< WDataSetSingle > m_paramDS; //!< Dataset derived from a voxelized cluster
    boost::shared_ptr< std::vector< std::pair< double, WPlane > > > m_slices; //!< stores all planes and their average parameters along centerLine
    boost::shared_ptr< WTriangleMesh > m_mesh; //!< Reference to the TriangleMesh to make intersections

    boost::shared_ptr< WJoinContourTree >   m_joinTree; //!< Stores the JoinTree
    boost::shared_ptr< std::set< size_t > > m_isoVoxels; //!< Stores the voxels belonging to the cluster volume of a certain iso value

    WPropBool   m_drawISOVoxels; //!< En/Disable the display of cluster volume voxels
    WPropBool   m_drawSlices; //!< En/Disable the display of slices along center line
    WPropDouble m_isoValue; //!< The ISO value selecting the size of the cluster volume

    double m_maxAVG; //!< maximum average (of sample points of a plane) parameter value over all planes
    double m_minAVG; //!< minimum average (of sample points of a plane) parameter value over all planes
private:
};

#endif  // WMCLUSTERSLICER_H

