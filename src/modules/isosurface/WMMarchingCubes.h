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

#ifndef WMMARCHINGCUBES_H
#define WMMARCHINGCUBES_H

#include <map>
#include <string>
#include <vector>

#include <osg/Node>
#include <osg/Geode>
#include <osg/Uniform>

#include "core/graphicsEngine/WGEManagedGroupNode.h"
#include "core/graphicsEngine/WTriangleMesh.h"
#include "core/dataHandler/WDataSetScalar.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"
#include "core/dataHandler/WGridRegular3D.h"
#include "core/graphicsEngine/shaders/WGEShader.h"

/**
 * Module implementing the marching cubes algorithm with consistent triangulation for data
 * given on regular grids with axis-aligned cells.
 * \ingroup modules
 */
class WMMarchingCubes : public WModule
{
/**
 * Only UnitTests may be friends.
 */
friend class WMMarchingCubesTest;

public:
    /**
     * Standard constructor.
     */
    WMMarchingCubes();

    /**
     * Destructor.
     */
    ~WMMarchingCubes();

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
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     * Get the icon for this module in XPM format.
     * \return the icon.
     */
    virtual const char** getXPMIcon() const;

    /**
     *  updates textures and shader parameters when called (usually from the callback)
     */
    void updateGraphicsCallback();

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
     * Prepares and commits everything for rendering with the OSG
     */
    void renderMesh();

    /**
     * Kind of a convenience function for generate surface.
     * It performs the conversions of the value sets of different data types.
     * \param isoValue The surface will represent this value.
     */
    void generateSurfacePre( double isoValue );

    boost::shared_mutex m_updateLock; //!< Lock to prevent concurrent threads trying to update the osg node

    WPropInt m_nbTriangles; //!< Info-property showing the number of triangles in the mesh.
    WPropInt m_nbVertices; //!< Info-property showing the number of vertices in the mesh.

    WPropDouble m_isoValueProp; //!< Property holding the iso value
    WPropInt m_opacityProp; //!< Property holding the opacity valueassigned to the surface
    WPropBool m_useTextureProp; //!< Property indicating whether to use texturing with scalar data sets.
    WPropColor m_surfaceColor; //!< Property determining the color for the surface if no textures are displayed

    WPropBool m_useMarchingLego; //!< Property indicating whether to use interpolated or non interpolated triangulation

    /**
     * This condition denotes whether we need to recompute the surface
     */
    boost::shared_ptr< WCondition > m_recompute;


    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_input;  //!< Input connector required by this module.
    boost::shared_ptr< WModuleOutputData< WTriangleMesh > > m_output;  //!< Input connector required by this module.

    boost::shared_ptr< WTriangleMesh > m_triMesh; //!< This triangle mesh is provided as output through the connector.

    static const unsigned int m_edgeTable[256];  //!< Lookup table for edges used in the construction of the isosurface.
    static const int m_triTable[256][16];  //!< Lookup table for triangles used in the construction of the isosurface.

    boost::shared_ptr< const WDataSetScalar > m_dataSet; //!< pointer to dataSet to be able to access it throughout the whole module.
    boost::shared_ptr< WGridRegular3D > m_grid; //!< pointer to grid, because we need to access the grid for the dimensions of the texture.

    bool m_firstDataProcessed; //!< Indicates if we already processed the first arrived data. This helps us to reset the isovalue only the first time.

    osg::ref_ptr< WGEManagedGroupNode > m_moduleNode; //!< Pointer to the module's group node. We need it to be able to update it for callback.
    bool m_moduleNodeInserted; //!< ensures that the above module node gets inserted once the first triangle mesh has been calculated.

    osg::ref_ptr< osg::Geode > m_surfaceGeode; //!< Pointer to geode containing the surface.
};

#endif  // WMMARCHINGCUBES_H
