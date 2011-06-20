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

#ifndef WMWRITETRACTS_H
#define WMWRITETRACTS_H

#include <string>

#include <osg/Geode>

#include "core/dataHandler/datastructures/WFiberCluster.h"
#include "core/dataHandler/WDataSetFibers.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

/**
 * This module writes the tracts of either a fiber cluster or directly out of a
 * WDataSetFibers at its connector to a file.
 *
 * \ingroup modules
 */
class WMWriteTracts: public WModule
{
public:
    /**
     * Constructs an instance to write tracts to a file.
     */
    WMWriteTracts();

    /**
     * Destructs this instance.
     */
    virtual ~WMWriteTracts();

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
    /**
     * Store the mesh in a json (javascript object notation) file
     */
    bool saveJson() const;

    /**
     * Store the mesh in a json (javascript object notation) file
     * using only every other vertex
     */
    bool saveJson2() const;

    /**
     * Store the mesh in a json (javascript object notation) file
     */
    bool saveJsonTriangles() const;

    /**
     * Input connector for writing the tracts out of a WFiberCluster to a file.
     */
    boost::shared_ptr< WModuleInputData< const WFiberCluster > > m_clusterIC;

    /**
     * Input connector for writing directly tracts to a file.
     */
    boost::shared_ptr< WModuleInputData< const WDataSetFibers > > m_tractIC;

    WPropTrigger m_run; //!< Button to start saving
    WPropFilename m_savePath; //!< Path where tracts should be stored

    /**
     * A list of file type selection types
     */
    boost::shared_ptr< WItemSelection > m_fileTypeSelectionsList;

    /**
     * Selection property for file types
     */
    WPropSelection m_fileTypeSelection;
};

#endif  // WMWRITETRACTS_H
