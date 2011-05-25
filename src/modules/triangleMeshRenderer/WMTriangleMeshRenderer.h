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

#include <osg/Geode>

#include "core/common/datastructures/WColoredVertices.h"
#include "core/graphicsEngine/WGEGroupNode.h"
#include "core/graphicsEngine/shaders/WGEShader.h"
#include "core/graphicsEngine/WTriangleMesh.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

class WTriangleMesh;

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
     *
     */
    WMTriangleMeshRenderer();

    /**
     *
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

    /**
     *  updates shader parameters
     */
    void update();

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
     * An input connector used to get meshes from other modules. The connection management between connectors must not be handled by the module.
     */
    boost::shared_ptr< WModuleInputData< WTriangleMesh > > m_meshInput;
    boost::shared_ptr< WModuleInputData< WColoredVertices > > m_colorMapInput; //!< for each vertex ID in that container a special color is given.

    WPropColor m_meshColor; //!< The color of the mesh
    WPropInt m_opacityProp; //!< Property holding the opacity value assigned to the surface
    WPropBool m_mainComponentOnly; //!< En/Disable display of only the main component (biggest vertices number)

    WPropBool m_usePerVertexColor; //!< En/Disable display of only the main component (biggest vertices number)

    osg::ref_ptr< WGEGroupNode > m_moduleNode; //!< Pointer to the modules group node.

    osg::ref_ptr< osg::Geode > m_surfaceGeode; //!< Pointer to geode containing the surface.

    osg::ref_ptr< WGEShader > m_shader; //!< The shader used for the iso surface in m_geode


    /**
     * This function generates the osg geometry from the WTriangleMesh.
     * \param mesh The triangle mesh that will be rendered.
     */
    void renderMesh( boost::shared_ptr< WTriangleMesh > mesh );
};

/**
 * Adapter object for realizing callbacks of the node representing the surface in the osg
 */
class TriangleMeshRendererCallback : public osg::NodeCallback
{
public:
    /**
     * Constructor of the callback adapter.
     * \param module A function of this module will be called
     */
    explicit TriangleMeshRendererCallback( boost::shared_ptr< WMTriangleMeshRenderer > module );

    /**
     * Function that is called by the osg and that call the function in the module.
     * \param node The node we are called.
     * \param nv the visitor calling us.
     */
    virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );

private:
    boost::shared_ptr< WMTriangleMeshRenderer > m_module; //!< Pointer to the module to which the function that is called belongs to.
};

inline TriangleMeshRendererCallback::TriangleMeshRendererCallback( boost::shared_ptr< WMTriangleMeshRenderer > module )
    : m_module( module )
{
}

inline void TriangleMeshRendererCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    if( m_module )
    {
        m_module->update();
    }
    traverse( node, nv );
}


#endif  // WMTRIANGLEMESHRENDERER_H
