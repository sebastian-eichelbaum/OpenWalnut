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

#ifndef WMTRIANGLEMESHRENDERER_H
#define WMTRIANGLEMESHRENDERER_H

#include <string>

#include <osg/Uniform>

#include "core/common/datastructures/WColoredVertices.h"
#include "core/common/math/linearAlgebra/WVectorFixed.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"


class WTriangleMesh;
class WGEShader;
class WGEManagedGroupNode;

/**
 * This module renders the triangle mesh given at its input connector
 * as a surface.
 *
 * \ingroup modules
 */
class WMTriangleMeshRenderer: public WModule
{
public:
    /**
     * Constructor. Creates the module skeleton.
     */
    WMTriangleMeshRenderer();

    /**
     * Destructor.
     */
    virtual ~WMTriangleMeshRenderer();

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
     * Calculates the bounding box of a vector and increases the specified one if needed.
     *
     * \param minX old maximum X and gets overwritten by new one
     * \param maxX old maximum Y and gets overwritten by new one
     * \param minY old minimum Z and gets overwritten by new one
     * \param maxY old maximum X and gets overwritten by new one
     * \param minZ old maximum Y and gets overwritten by new one
     * \param maxZ old maximum Z and gets overwritten by new one
     * \param vector the new vector to include in bbox calculation.
     */
    void updateMinMax( double& minX, double& maxX, double& minY, double& maxY, double& minZ, double& maxZ, const osg::Vec3d& vector ) const;

    /**
     * Gets the median of three values.
     *
     * \param x first value
     * \param y second value
     * \param z third value
     *
     * \return the median of x,y,z
     */
    double getMedian( double x, double y, double z ) const;

    /**
     * Calculates the center point of a given interval. This simply is \f$x_{min} + \frac{x_{max} - x_{min}}{2}\f$.
     *
     * \param min interval min
     * \param max interval max
     *
     * \return interval center
     */
    double getIntervallCenterMiddle( double min, double max ) const;

    /**
     * Center of the mesh. Needed  for applying transformation with the mesh in (0,0,0).
     */
    WVector3d m_meshCenter;

    /**
     * Enables mesh's coordinate system.
     */
    WPropBool m_showCoordinateSystem;

    /**
     * Group for all color and colormapping options.
     */
    WPropGroup m_coloringGroup;

    /**
     * Turn Colormapping on/off
     */
    WPropBool m_colormap;

    /**
     * Set Colormap Ratio
     */
    WPropDouble m_colormapRatio;

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * An input connector used to get meshes from other modules. The connection management between connectors must not be handled by the module.
     */
    boost::shared_ptr< WModuleInputData< WTriangleMesh > > m_meshInput;

    /**
     * A map for mapping each vertex to a color.
     */
    boost::shared_ptr< WModuleInputData< WColoredVertices > > m_colorMapInput;

    /**
     * A group wich contains all transformation tools.
     */
    WPropGroup m_groupTransformation;

    /**
     * Set the transformation tool to default
     */
    WPropTrigger  m_setDefault;

    /**
     * Render the mesh
     *
     * \param mesh The mesh to be rendered.
     */
    void renderMesh( boost::shared_ptr< WTriangleMesh > mesh );

    /**
     * Set the transformation tool to default
     */
    void setToDefault();

    /**
     * OSG Uniform for the transformation matrix which transforms the mesh. Needed for the colormapper
    */
    osg::ref_ptr< osg::Uniform > m_colorMapTransformation;

    /**
     * The mesh's opacity value.
     */
    WPropDouble m_opacity;

    /**
     * If true, the mesh scale properties are linked.
     */
    WPropBool m_scale;

    /**
     * The mesh's scale value in X direction.
     */
    WPropDouble m_scaleX;

    /**
     * The mesh's scale value in Y direction.
     */
    WPropDouble m_scaleY;

    /**
     * The mesh's scale value in Z direction.
     */
    WPropDouble m_scaleZ;

    /**
     * The mesh's rotate value around X.
     */
    WPropDouble m_rotateX;

    /**
     * The mesh's rotate value around Y.
     */
    WPropDouble m_rotateY;

    /**
     * The mesh's rotate value around Z.
     */
    WPropDouble m_rotateZ;

    /**
     * The mesh's translate value along X.
     */
    WPropDouble m_translateX;

    /**
     * The mesh's translate value along X.
     */
    WPropDouble m_translateY;

    /**
     * The mesh's translate value along X.
     */
    WPropDouble m_translateZ;

    /**
     * Which rendering mode should be used?
     */
    WPropSelection m_renderingMode;

    /**
     * Toggle showing outline instead fo surface.
     */
    WPropBool m_showOutline;

    /**
     * En/Disable display of only the main component (biggest vertices number)
     */
    WPropBool m_mainComponentOnly;

    /**
     * The color of the mesh to be rendered.
     */
    WPropColor m_color;

    /**
     * Which color mode should be used?
     */
    WPropSelection m_colorMode;

    WPropInt m_nbTriangles; //!< Info-property showing the number of triangles in the mesh.
    WPropInt m_nbVertices; //!< Info-property showing the number of vertices in the mesh.

    /**
     * Updates the transformation matrix of the main node. Called every frame.
     */
    void updateTransformation();

    /**
     * The node containing all geometry nodes.
     */
    WGEManagedGroupNode::SPtr m_moduleNode;

    /**
     * The shader for the mesh
     */
    osg::ref_ptr< WGEShader > m_shader;
};

#endif  // WMTRIANGLEMESHRENDERER_H
