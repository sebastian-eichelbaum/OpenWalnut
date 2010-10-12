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

#ifndef WMARBITRARYPLANE_H
#define WMARBITRARYPLANE_H

#include <string>
#include <vector>

#include <osg/Geode>

#include "../../graphicsEngine/WGEManagedGroupNode.h"
#include "../../graphicsEngine/WROISphere.h"

#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../kernel/WModuleOutputData.h"

/**
 * Someone should add some documentation here.
 * Probably the best person would be the module's
 * creator, i.e. "schurade".
 *
 * This is only an empty template for a new module. For
 * an example module containing many interesting concepts
 * and extensive documentation have a look at "src/modules/template"
 *
 * \ingroup modules
 */
class WMArbitraryPlane: public WModule
{
public:

    /**
     * standard constructor
     */
    WMArbitraryPlane();

    /**
     * destructor
     */
    virtual ~WMArbitraryPlane();

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

    /**
     * called once at the start of the module main loop
     */
    void initPlane();

    /**
     * updates the plane with the current settings
     */
    void updatePlane();

    /**
     * setter for dirty flag
     */
    void setDirty();

    /**
     * getter
     * \return dirty flag
     */
    bool isDirty();

    /**
     *  updates textures and shader parameters
     */
    void updateTextures();

    /**
     * creates and initializes the uniform parameters for the shader
     * \param rootState The uniforms will be applied to this state.
     */
    void initUniforms( osg::StateSet* rootState );

    /**
     * Used as callback which simply sets m_textureChanged to true. Called by WSubject whenever the datasets change.
     */
    void notifyTextureChange();

    /**
     * getter for the position of the center manipulator
     *
     * \return center position
     */
    wmath::WPosition getCenterPosition();

    /**
     * update function, called with each update pass of the osg render loop
     */
    void updateCallback();


private:
    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;


    /**
     * True if the shader shouldn't discard a fragment when the value is zero
     */
    WPropBool m_showComplete;

    /**
     * True if the manipulator spheres should be shown
     */
    WPropBool m_showManipulators;

    /**
     * If true the center position will move with the nav slice selection
     */
    WPropBool m_attach2Crosshair;


    /**
     * When triggered the center manipulator is set to the nav slice position and the plane is aligned along
     * the axial slice
     */
    WPropTrigger m_buttonReset2Axial;

    /**
     * When triggered the center manipulator is set to the nav slice position and the plane is aligned along
     * the coronal slice
     */
    WPropTrigger m_buttonReset2Coronal;

    /**
     * When triggered the center manipulator is set to the nav slice position and the plane is aligned along
     * the axial slice
     */
    WPropTrigger m_buttonReset2Sagittal;


    /**
     * The root node used for this modules graphics.
     */
    osg::ref_ptr< WGEManagedGroupNode > m_rootNode;

    /**
     * The geometry rendered by this module.
     */
    osg::ref_ptr< osg::Geode > m_geode;

    wmath::WPosition m_p0; //!< stores the last position of the center manipulator
    wmath::WPosition m_p1; //!< stores the last position of  manipulator 1
    wmath::WPosition m_p2; //!< stores the last position of  manipulator 2

    boost::shared_ptr<WROISphere> m_s0; //!< stores pointer to the center manipulator
    boost::shared_ptr<WROISphere> m_s1; //!< stores pointer to manipulator 1
    boost::shared_ptr<WROISphere> m_s2; //!< stores pointer to manipulator 2

    bool m_dirty; //!< dirty flag, used when manipulator positions change

    /**
     * True when textures have changed.
     */
    bool m_textureChanged;

    /**
     * the shader object for this module
     */
    osg::ref_ptr< WShader > m_shader;

    /**
     * vector of uniforms for type of texture
     */
    std::vector< osg::ref_ptr<osg::Uniform> > m_typeUniforms;

    /**
     * vector of alpha values per texture
     */
    std::vector< osg::ref_ptr<osg::Uniform> > m_alphaUniforms;

    /**
     * vector of thresholds per texture
     */
    std::vector< osg::ref_ptr<osg::Uniform> > m_thresholdUniforms;

    /**
     * vector of color maps per texture
     */
    std::vector< osg::ref_ptr<osg::Uniform> > m_cmapUniforms;

    /**
     * vector of samplers
     */
    std::vector< osg::ref_ptr<osg::Uniform> > m_samplerUniforms;

    osg::ref_ptr<osg::Uniform> m_showCompleteUniform; //!< Determines whether the slice should be drawn completely
};

#endif  // WMARBITRARYPLANE_H
