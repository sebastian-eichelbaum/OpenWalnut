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

#ifndef WMREADMESH_H
#define WMREADMESH_H

#include <fstream>
#include <string>

#include <osg/Geode>

#include "core/common/WStrategyHelper.h"
#include "core/common/WObjectNDIP.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"
#include "core/graphicsEngine/WTriangleMesh.h"

#include "WMeshReaderInterface.h"

/**
 * This module reads a file containing mesh data (several formats supported) and creates a mesh
 * (or triangle mesh) object delivered through the output connector
 *
 * \ingroup modules
 */
class WMReadMesh: public WModule
{
public:
    /**
     *
     */
    WMReadMesh();

    /**
     *
     */
    virtual ~WMReadMesh();

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
    boost::shared_ptr< WTriangleMesh > m_triMesh; //!< This triangle mesh is provided as output through the connector.
    boost::shared_ptr< WModuleOutputData< WTriangleMesh > > m_output;  //!< Output connector provided by this module.
    boost::shared_ptr< WCondition > m_propCondition;  //!< A condition used to notify about changes in several properties.

    WPropTrigger  m_readTriggerProp; //!< This property triggers the actual reading,
    WPropFilename m_meshFile; //!< The mesh will be read from this file.

    /**
     * A list of file type selection types
     */
    boost::shared_ptr< WItemSelection > m_fileTypeSelectionsList;

    /**
     * Selection property for file types
     */
    WPropSelection m_fileTypeSelection;

    WPropInt m_nbTriangles; //!< Info-property showing the number of triangles in the mesh.
    WPropInt m_nbVertices; //!< Info-property showing the number of vertices in the mesh.

    /**
     * Handle each loader as strategy.
     */
    WStrategyHelper< WObjectNDIP< WMeshReaderInterface > > m_strategy;
};

#endif  // WMREADMESH_H
