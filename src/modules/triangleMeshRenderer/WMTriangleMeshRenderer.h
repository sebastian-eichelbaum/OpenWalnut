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

#ifndef WMTRIANGLEMESHRENDERER_H
#define WMTRIANGLEMESHRENDERER_H

#include <string>

#include <osg/Uniform>

#include "core/common/datastructures/WColoredVertices.h"
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
     * A function which gives you the median of 3 ints
     */
    void updateMinMix( double& minX, double& maxX, double& minY, double& maxY, double& minZ, double& maxZ, const osg::Vec3d& vector ) const;

    /**
     * A function which gives you the median of 3 ints
     */
    double getMedian( double x, double y, double z ) const;

    /**
     * The middle point of the intervall center
     */
     double getIntervallCenterMiddle( double min, double max ) const;

     /**
      * Middle points of the intervall center
      */
      double middleX;
      double middleY;
      double middleZ;

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
     * Set the transformation tool to default
    */
    void setToDefault();

    /**
     * OSG Uniform
    */
    osg::ref_ptr< osg::Uniform > m_colorMapTransformation;

    /**
     * The mesh's opacity value.
     */
    WPropDouble m_opacity;

    /**
     * The mesh's scale value.
     */
    WPropBool m_scale;
    WPropDouble m_scaleX;
    WPropDouble m_scaleY;
    WPropDouble m_scaleZ;

    /**
     * The mesh's rotate value.
     */
    WPropDouble m_rotateX;
    WPropDouble m_rotateY;
    WPropDouble m_rotateZ;

    /**
     * The mesh's translate value.
     */
     WPropDouble m_translateX;
     WPropDouble m_translateY;
     WPropDouble m_translateZ;

    /**
     * En/Disable display of only the main component (biggest vertices number)
     */
    WPropBool m_mainComponentOnly;

    /**
     * The color of the mesh to be rendered.
     */
    WPropColor m_color;

    /**
     * Which colormode should be used?
     */
    WPropSelection m_colorMode;

    /**
     * Updates the transformation matrix of the main node. Called every frame.
     */
    void updateTransformation();

    /**
     * The node containing all geometry nodes.
     */
    WGEManagedGroupNode::SPtr m_moduleNode;
};

#endif  // WMTRIANGLEMESHRENDERER_H
