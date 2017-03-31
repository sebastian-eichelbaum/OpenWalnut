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

#ifndef WMGRIDRENDERER_H
#define WMGRIDRENDERER_H

#include <map>
#include <string>
#include <vector>

#include <osg/Node>
#include <osg/Geode>
#include <osg/Uniform>

#include "core/graphicsEngine/WGEGroupNode.h"
#include "core/graphicsEngine/geodes/WGEGridNode.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/dataHandler/WDataSet.h"

/**
 * Show the bounding box and grid of a WDataSetSingle
 * \ingroup modules
 */
class WMGridRenderer : public WModule
{
public:
    /**
     * Standard constructor.
     */
    WMGridRenderer();

    /**
     * Destructor.
     */
    ~WMGridRenderer();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description of module.
     */
    virtual const std::string getDescription() const;

    /**
     * Determine what to do if a property was changed.
     * \param propertyName Name of the property.
     */
    void slotPropertyChanged( std::string propertyName );

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

private:
    boost::shared_ptr< WModuleInputData< WDataSet > > m_input;  //!< Input connector required by this module.

    WItemSelection::SPtr m_possibleModes; //!< the modes available.

    WPropSelection m_mode; //!< the mode (bbox, grid, both)

    WPropColor m_bboxColor; //!< the color of the bounding box

    WPropColor m_gridColor; //!< the color of the grid

    WPropInt m_boxLineWidth; //!< the width of the boundary lines

    WPropInt m_gridLineWidth; //!< the width of the grid lines

    /**
     * This condition denotes whether we need to recompute the surface
     */
    boost::shared_ptr< WCondition > m_recompute;

    /**
     * The node actually drawing the grid.
     */
    WGEGridNode::SPtr m_gridNode;

    /**
     * The currently handled data.
     */
    WDataSet::SPtr m_dataSetOld;

    /**
     * Handles updates in properties.
     *
     * \param property the property that has updated.
     */
    void updateNode( WPropertyBase::SPtr property );
};
#endif  // WMGRIDRENDERER_H
