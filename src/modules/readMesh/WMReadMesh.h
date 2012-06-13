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

#ifndef WMREADMESH_H
#define WMREADMESH_H

#include <fstream>
#include <string>

#include <osg/Geode>

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"
#include "core/graphicsEngine/WTriangleMesh.h"

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
    /**
     * Handles chage of selected mesh type, e.g. show/hide other properties.
     */
    void meshTypeSelected();

    /**
     * Reads a mesh file and creates a WTriangleMesh out of it.
     *
     * \return Reference to the dataset.
     */
    virtual boost::shared_ptr< WTriangleMesh > readMesh();

    /**
     * Reads a mesh file and creates a WTriangleMesh out of it.
     *
     * \return Reference to the dataset.
     */
    virtual boost::shared_ptr< WTriangleMesh > readMeshFnav();


    /**
     * Reads a dip file and creates a WTriangleMesh out of it.
     *
     * \return Reference to the dataset.
     */
    virtual boost::shared_ptr< WTriangleMesh > readDip();

    /**
     * Reads a BrainVISA (.mesh) file and creates a WTriangleMesh out of it.
     *
     * \return Reference to the dataset.
     */
    virtual boost::shared_ptr< WTriangleMesh > readBrainVISA();

    /**
     * Reads a Freesurfer (.surf) file and creates a WTriangleMesh out of it.
     *
     * \return Reference to the dataset.
     */
    virtual boost::shared_ptr< WTriangleMesh > readFreesurfer();

    /**
     * creates a color map for loaded dip file
     * \param value a value between 0.0 and 1.0
     * \return the color
     */
    osg::Vec4 blueGreenPurpleColorMap( float value );

    /**
     * Reads the next line from current position in stream of the fiber VTK file.
     *
     * \param ifs file stream of the mesh file to be loaded
     * \param desc In case of trouble while reading, this gives information in
     * the error message about what was tried to read
     * \throws WDHIOFailure, WDHParseError
     * \return Next line as string.
     */
    std::string getLine( boost::shared_ptr< std::ifstream > ifs, const std::string& desc );

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

    WPropInt m_propDatasetSizeX; //!< Size of the dataset (X)
    WPropInt m_propDatasetSizeY; //!< Size of the dataset (Y)
    WPropInt m_propDatasetSizeZ; //!< Size of the dataset (Z)
    WPropBool m_propVectorAsColor; //!< Use vectors from file as color of surface.

    WPropInt m_nbTriangles; //!< Info-property showing the number of triangles in the mesh.
    WPropInt m_nbVertices; //!< Info-property showing the number of vertices in the mesh.
};

#endif  // WMREADMESH_H
