//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

#ifndef WMARBITRARYROIS_H
#define WMARBITRARYROIS_H

#include <string>
#include <vector>

#include <osg/Geode>

#include "core/graphicsEngine/WROI.h"
#include "core/graphicsEngine/WGEGroupNode.h"
#include "core/graphicsEngine/WTriangleMesh.h"

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

class WROIArbitrary;
class WROIBox;
class WDataSetScalar;

/**
 * Create non uniformly shaped ROIs for fiber selection.
 *
 * \ingroup modules
 */
class WMArbitraryROIs: public WModule
{
public:
    /**
     *
     */
    WMArbitraryROIs();

    /**
     *
     */
    virtual ~WMArbitraryROIs();

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
     * Callback for m_active. Overwrite this in your modules to handle m_active changes separately.
     */
    virtual void activate();

private:
    /**
     * create a selection box to mark the area in a dataset which will provide the data for a new ROI
     */
    void initSelectionROI();

    /**
     * creates a new dataset
     */
    void createCutDataset();

    /**
     * renders the temporary ROI
     */
    void renderMesh();

    /**
     * creates a ROI and adds it to the ROI manager
     */
    void finalizeROI();

    /**
     * Removes the ROI and cleans up the scene.
     */
    void cleanup();

    /**
     * copies the data from the input dataset which is marked by the selection box, otherwise data is zero
     *
     * \param inGrid The grid from which the area will be cut
     * \param vals The values on the original grid
     *
     * \return Data in the selected box
     */
    template< typename T > boost::shared_ptr< std::vector< float > > cutArea( boost::shared_ptr< WGrid > inGrid,
                                                                              boost::shared_ptr< WValueSet< T > > vals );

    /**
     * This condition denotes whether we need to recompute the surface
     */
    boost::shared_ptr< WCondition > m_recompute;

    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_input;  //!< Input connector required by this module.

    boost::shared_ptr< const WDataSetScalar > m_dataSet; //!< pointer to dataSet to be able to access it throughout the whole module.

    boost::shared_ptr< WValueSet< float > > m_newValueSet; //!< pointer to the created cut valueSet

    osg::ref_ptr< WROIBox > m_selectionROI; //!< stores a pointer to the cutting tool ROI

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * A trigger which can be used to trigger some kind of operation.
     */
    WPropTrigger  m_finalizeTrigger;

    WPropDouble   m_threshold; //!< the threshold for the ROI

    WPropColor m_surfaceColor; //!< Property determining the color for the surface

    osg::ref_ptr< WGEGroupNode > m_moduleNode; //!< Pointer to the modules group node. We need it to be able to update it when callback is invoked.

    osg::ref_ptr< osg::Geode > m_outputGeode; //!< Pointer to geode containing the glpyhs

    boost::shared_ptr< WTriangleMesh > m_triMesh; //!< This triangle mesh is provided as output through the connector.

    bool m_showSelector; //!< flag indication if the temporary ROI should be shown;

    WPosition m_lastMinPos; //!< the last know min position of the ROI if there was any
    WPosition m_lastMaxPos; //!< the last know max position of the ROI if there was any
};

#endif  // WMARBITRARYROIS_H
