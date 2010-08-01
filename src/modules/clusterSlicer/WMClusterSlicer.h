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

#include "../../dataHandler/datastructures/WJoinContourTree.h"
#include "../../dataHandler/datastructures/WFiberCluster.h"
#include "../../graphicsEngine/WGEGroupNode.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../kernel/WModuleOutputData.h"

/**
 * Creates slices of a fiber cluster along its center line.
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
     * Generates new geode for marking the volume voxels
     *
     * \return Newly constructed geode
     */
    osg::ref_ptr< osg::Geode > generateISOVoxelGeode() const;

    osg::ref_ptr< WGEGroupNode > m_rootNode; //!< The root node used for this modules graphics.

    osg::ref_ptr< osg::Geode > m_isoVoxelGeode; //!< Separate geode for voxels of the cluster volume

    typedef WModuleInputData< WFiberCluster > InputClusterType; //!< Internal alias for m_inputCluster's type
    boost::shared_ptr< InputClusterType >  m_inputCluster; //!< InputConnector for a fiber cluster with its CenterLine
    typedef WModuleInputData< WDataSetSingle > InputDataSetType; //!< Internal alias for m_inputDataSet's type
    boost::shared_ptr< InputDataSetType > m_inputDataSet; //!< InputConnector for the dataset derived from a voxelized cluster

    boost::shared_ptr< WFiberCluster >  m_cluster; //!< A cluster with its CenterLine
    boost::shared_ptr< WDataSetSingle > m_dataSet; //!< Dataset derived from a voxelized cluster

    boost::shared_ptr< WJoinContourTree > m_joinTree; //!< Stores the JoinTree
    boost::shared_ptr< std::set< size_t > > m_isoVoxels; //!< Stores the voxels belonging to the cluster volume of a certain iso value

    WPropBool   m_drawISOVoxels; //!< En/Disable the display of cluster volume voxels
    WPropDouble m_isoValue; //!< The ISO value selecting the size of the cluster volume
private:
};

#endif  // WMCLUSTERSLICER_H

