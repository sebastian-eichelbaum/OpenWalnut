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

#ifndef WMWRITEMESH_H
#define WMWRITEMESH_H

#include <string>

#include <osg/Geode>

#include "core/graphicsEngine/WTriangleMesh.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

/**
 * Someone should add some documentation here.
 * Probably the best person would be the module's
 * creator, i.e. "wiebel".
 *
 * This is only an empty template for a new module. For
 * an example module containing many interesting concepts
 * and extensive documentation have a look at "src/modules/template"
 *
 * \ingroup modules
 */
class WMWriteMesh: public WModule
{
/**
 * Only UnitTests may be friends.
 */
friend class WMWriteMeshTest;

public:
    /**
     *
     */
    WMWriteMesh();

    /**
     *
     */
    virtual ~WMWriteMesh();

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


private:
    /**
     * Store the mesh in legacy vtk file format.
     *
     * \return true if successful.
     */
    bool saveVTKASCII() const;

    /**
     * Store the mesh in a json (javascript object notation) file.
     *
     * \return true if successful.
     */
    bool saveJson() const;

    boost::shared_ptr< WModuleInputData< WTriangleMesh > > m_meshInput; //!< Input connector for a mesh
    boost::shared_ptr< WTriangleMesh > m_triMesh; //!< A pointer to the currently processed tri mesh

    boost::shared_ptr< WCondition > m_propCondition;  //!< A condition used to notify about changes in several properties.
    WPropGroup    m_savePropGroup; //!< Property group containing properties needed for saving the mesh.
    WPropTrigger  m_saveTriggerProp; //!< This property triggers the actual writing,
    WPropFilename m_meshFile; //!< The mesh will be written to this file.

    /**
     * A list of file type selection types
     */
    boost::shared_ptr< WItemSelection > m_fileTypeSelectionsList;

    /**
     * Selection property for file types
     */
    WPropSelection m_fileTypeSelection;

    /**
     * If true, colors get exported too.
     */
    WPropBool m_writeColors;
};

#endif  // WMWRITEMESH_H
