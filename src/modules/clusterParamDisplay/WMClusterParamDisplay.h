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

#ifndef WMCLUSTERPARAMDISPLAY_H
#define WMCLUSTERPARAMDISPLAY_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "../../dataHandler/WDataSetFibers.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleContainer.h"
#include "../../kernel/WModuleInputForwardData.h"
#include "../../kernel/WModuleOutputForwardData.h"

/**
 * Displays certains parameters of a cluster or bundle.
 */
class WMClusterParamDisplay: public WModuleContainer
{
public:
    /**
     * Default constructor.
     */
    WMClusterParamDisplay();

    /**
     * Destructor.
     */
    virtual ~WMClusterParamDisplay();

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
     * The root node used for this modules graphics.
     */
    osg::ref_ptr<osg::Geode> m_rootNode;

    /**
     * En/Disables display of the root node.
     */
    virtual void activate();

    void initSubModules(); //!< Create and initialize submodule instances

private:
    boost::shared_ptr< WModuleInputForwardData< WDataSetFibers > > m_input;  //!< Input connector required by this module.

    boost::shared_ptr< WModule > m_fiberClustering; //!< Submodule doing clustering of the fibers and center line generation
    boost::shared_ptr< WModule > m_voxelizer; //!< Submodule doing voxelization of a cluster
    boost::shared_ptr< WModule > m_gaussFiltering; //!< Submodule blurring the generated voxelized dataset
    boost::shared_ptr< WModule > m_isoSurface; //!< Selects the appropriate ISO value

//    WPropInt    m_clusterOutputID; //!< Specifies which cluster should be connected to the Output
};

#endif  // WMCLUSTERPARAMDISPLAY_H
