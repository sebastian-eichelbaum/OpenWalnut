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

#ifndef WMLINEGUIDEDSLICE_H
#define WMLINEGUIDEDSLICE_H

#include <string>
#include <vector>
#include <utility>

#include <osg/Node>

#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../kernel/WModuleOutputData.h"
#include "../../graphicsEngine/WGEViewer.h"

// forward declarations
class WFiberCluster;

/**
 * Module to compute a plane along a line and color it with a scalar parameter.
 * \ingroup modules
 */
class WMLineGuidedSlice : public WModule, public osg::Referenced
{
public:

    /**
     *
     */
    WMLineGuidedSlice();

    /**
     *
     */
    virtual ~WMLineGuidedSlice();

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
     * Used as callback which simply sets m_textureChanged to true. Called by WSubject whenever the datasets change.
     */
    void notifyTextureChange();

    /**
     * True when textures have changed.
     */
    bool m_textureChanged;

    /**
     * Updates the positions of the slice
     */
    void updateGeometry();

    /**
     * Uppdates textures and shader parameters
     */
    void updateTextures();

    /**
     * Creates the inital geometry.
     */
    void create();

    /**
     * Removes or inserts geode for the center line of the current cluster into this modules group node.
     */
    void updateCenterLine();

    /**
     * Sets slice position from interaction in the main GL widget
     * \param pickInfo The information provided by the pick handler
     */
    void setSlicePosFromPick( WPickInfo pickInfo );

    /**
     * Initial creation function for the slice geometry
     */
    osg::ref_ptr< osg::Geometry > createGeometry();

    /**
     * Creates and initializes the uniform parameters for the shader
     * \param rootState The uniforms will be applied to this state.
     */
    void initUniforms( osg::StateSet* rootState );

    WPropDouble m_pos; //!< Slice position along line.

    std::pair< float, float > m_oldPixelPosition; //!< Caches the old picked position to a allow for cmoparison
    bool m_isPicked; //!< Indicates whether a slice is currently picked or not.
    boost::shared_ptr< WGEViewer > m_viewer; //!< Stores reference to the main viewer
    boost::shared_ptr< WModuleInputData< const WFiberCluster > > m_input; //!< Input connector for a fiber cluster

    osg::ref_ptr< WGEGroupNode > m_rootNode; //!< The root node for this module.
    osg::ref_ptr< osg::Geode > m_sliceNode; //!< OSG node for slice.
    osg::ref_ptr< osg::Geode > m_centerLineGeode; //!< OSG center line of the current cluster geode.
    osg::ref_ptr< WGEShader > m_shader; //!< The shader object for this module.
    boost::shared_mutex m_updateLock; //!< Lock to prevent concurrent threads trying to update the osg node.
    std::vector< osg::ref_ptr< osg::Uniform > > m_typeUniforms; //!< Vector of uniforms for type of texture.
    std::vector< osg::ref_ptr< osg::Uniform > > m_alphaUniforms; //!< Vector of alpha values per texture.
    std::vector< osg::ref_ptr< osg::Uniform > > m_thresholdUniforms; //!< Vector of thresholds per texture.
    std::vector< osg::ref_ptr< osg::Uniform > > m_samplerUniforms; //!< Vector of samplers.
    osg::ref_ptr< osg::Uniform > m_highlightUniform; //!< Determines whether the slice is highlighted.

    boost::shared_ptr< WFiber > m_centerLine; //!< The line that guides the slice.

    /**
     * Node callback to handle updates properly
     */
    class sliceNodeCallback : public osg::NodeCallback
    {
    public: // NOLINT
        /**
         * operator ()
         *
         * \param node the osg node
         * \param nv the node visitor
         */
        virtual void operator()( osg::Node* node, osg::NodeVisitor* nv )
        {
            osg::ref_ptr< WMLineGuidedSlice > module = static_cast< WMLineGuidedSlice* > ( node->getUserData() );

            if( module )
            {
                module->updateGeometry();
                module->updateTextures();
            }
            traverse( node, nv );
        }
    };
};

#endif  // WMLINEGUIDEDSLICE_H
