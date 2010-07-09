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

#ifndef WMSPLINESURFACE_H
#define WMSPLINESURFACE_H

#include <map>
#include <string>
#include <vector>

#include <osg/Node>
#include <osg/Geode>
#include <osg/Uniform>

#include "../../graphicsEngine/WTriangleMesh2.h"
#include "../../dataHandler/WDataSetScalar.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../kernel/WModuleOutputData.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../../graphicsEngine/WShader.h"
#include "../../graphicsEngine/WGEGroupNode.h"

/**
 * Module implementing the marching cubes algorithm with consistent triangulation for data
 * given on regular grids with axis-aligned cells.
 * \ingroup modules
 */
class WMSplineSurface: public WModule
{
public:
    /**
     * Standard constructor.
     */
    WMSplineSurface();

    /**
     * Destructor.
     */
    ~WMSplineSurface();

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
     */
    virtual const char** getXPMIcon() const;

    /**
     *  updates textures and shader parameters
     */
    void updateGraphics();

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
     * Used as callback which simply sets m_textureChanged to true. Called by WSubject whenever the datasets change.
     */
    void notifyTextureChange();

    /**
     * Prepares and commits everything for rendering with the OSG
     */
    void renderMesh();

    /**
     * Store the mesh in legacy vtk file format.
     */
    bool save() const;

    WPropInt m_opacityProp; //!< Property holding the opacity valueassigned to the surface
    WPropBool m_useTextureProp; //!< Property indicating whether to use texturing with scalar data sets.
    WPropColor m_surfaceColor; //!< Property determining the color for the surface if no textures are displayed

    WPropGroup m_savePropGroup; //!< Property group containing properties needed for saving the mesh.
    WPropTrigger m_saveTriggerProp; //!< This property triggers the actual writing,
    WPropFilename m_meshFile; //!< The mesh will be written to this file.

    /**
     * True when textures have changed.
     */
    bool m_textureChanged;

    /**
     * This condition denotes whether we need to recompute the surface
     */
    boost::shared_ptr< WCondition > m_recompute;

    boost::shared_ptr< WModuleOutputData< WTriangleMesh2 > > m_output; //!< Input connector required by this module.

    boost::shared_ptr< WTriangleMesh2 > m_triMesh; //!< This triangle mesh is provided as output through the connector.

    bool m_shaderUseLighting; //!< shall the shader use lighting?
    bool m_shaderUseTransparency; //!< shall the shader use transparency?

    osg::ref_ptr< WGEGroupNode > m_moduleNode; //!< Pointer to the modules group node. We need it to be able to update it when callback is invoked.

    osg::ref_ptr< osg::Geode > m_surfaceGeode; //!< Pointer to geode containing the surface.

    /**
     * The shader used for the iso surface in m_geode
     */
    osg::ref_ptr< WShader > m_shader;

    std::vector< osg::ref_ptr< osg::Uniform > > m_typeUniforms; //!< uniforms for ...... ? for shader
    std::vector< osg::ref_ptr< osg::Uniform > > m_alphaUniforms; //!< uniforms for opacities of textures in shader
    std::vector< osg::ref_ptr< osg::Uniform > > m_thresholdUniforms; //!< uniforms for thresholds of textures in shader
    std::vector< osg::ref_ptr< osg::Uniform > > m_samplerUniforms; //!< uniforms for ids of textures in shader
    std::vector< osg::ref_ptr< osg::Uniform > > m_cmapUniforms; //!< uniforms for color maps per texture in shader

    static const int m_maxNumberOfTextures = 8; //!< We support only 8 textures because some known hardware does not support more texture coordinates.
};

/**
 * Adapter object for realizing callbacks of the node representing the isosurface in the osg
 */
class SplineSurfaceNodeCallback: public osg::NodeCallback
{
public:
    /**
     * Constructor of the callback adapter.
     * \param module A function of this module will be called
     */
    explicit SplineSurfaceNodeCallback( WMSplineSurface* module );

    /**
     * Function that is called by the osg and that call the function in the module.
     * \param node The node we are called.
     * \param nv the visitor calling us.
     */
    virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );

private:
    WMSplineSurface* m_module; //!< Pointer to the module to which the function that is called belongs to.
};

inline SplineSurfaceNodeCallback::SplineSurfaceNodeCallback( WMSplineSurface* module ) :
    m_module( module )
{
}

inline void SplineSurfaceNodeCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    if ( m_module )
    {
        m_module->updateGraphics();
    }
    traverse( node, nv );
}

#endif  // WMSPLINESURFACE_H
