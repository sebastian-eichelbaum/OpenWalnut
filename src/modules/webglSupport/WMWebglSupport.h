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

#ifndef WMWEBGLSUPPORT_H
#define WMWEBGLSUPPORT_H

#include <string>

#include <osg/Geode>

#include "core/dataHandler/WDataSetScalar.h"
#include "core/dataHandler/WDataSetVector.h"
#include "core/graphicsEngine/WTriangleMesh.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

/** 
 * Someone should add some documentation here.
 * Probably the best person would be the module's
 * creator, i.e. "ralph".
 *
 * This is only an empty template for a new module. For
 * an example module containing many interesting concepts
 * and extensive documentation have a look at "src/modules/template"
 *
 * \ingroup modules
 */
class WMWebglSupport: public WModule
{
public:
    /**
     * constructor
     */
    WMWebglSupport();

    /**
     * destructor
     */
    virtual ~WMWebglSupport();

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
     * Initialize requirements for this module.
     */
    virtual void requirements();


private:
    /**
     * colors a connected triangle mesh with the connected gray scale texture
     */
    void colorTriMeshGray();

    /**
	 * colors a connected triangle mesh with the connected rgb texture
	 */
    void colorTriMeshRGB();

    /**
     * saves a connected gray scale 3D texture as set of 2D textures
     */
    void saveSlicesGray();

    /**
	 * saves a connected rgb 3D texture as set of 2D textures
	 */
    void saveSlicesRGB();

    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_datasetInputScalar;  //!< Input connector required by this module.
    boost::shared_ptr< WModuleInputData< WDataSetVector > > m_datasetInputVector;  //!< Input connector required by this module.

    boost::shared_ptr< WModuleInputData< WTriangleMesh > > m_meshInput; //!< Input connector for a mesh
    boost::shared_ptr< WModuleOutputData< WTriangleMesh > > m_meshOutput;  //!< Input connector required by this module.

    boost::shared_ptr< WTriangleMesh > m_triMesh; //!< A pointer to the currently processed tri mesh
    boost::shared_ptr< WDataSetScalar > m_datasetScalar; //!< A pointer to the currently processed dataset
    boost::shared_ptr< WDataSetVector > m_datasetVector; //!< A pointer to the currently processed dataset

    boost::shared_ptr< WCondition > m_propCondition;  //!< A condition used to notify about changes in several properties.

    WPropTrigger  m_propTriggerSaveGray; //!< This property triggers the actual writing,
    WPropTrigger  m_propTriggerSaveRGB; //!< This property triggers the actual writing,
    WPropFilename m_fileName; //!< The mesh will be written to this file.
    WPropFilename m_meshFile; //!< The mesh will be written to this file.

    WPropTrigger  m_propTriggerUpdateOutputGray; //!< This property triggers the actual writing,
    WPropTrigger  m_propTriggerUpdateOutputRGB; //!< This property triggers the actual writing,

    /**
     * A list of file type selection types
     */
    boost::shared_ptr< WItemSelection > m_fileTypeSelectionsList;

    /**
     * Selection property for file types
     */
    WPropSelection m_fileTypeSelection;
};

#endif  // WMWEBGLSUPPORT_H
