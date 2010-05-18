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

#ifndef WMPROBTRACTDISPLAY_H
#define WMPROBTRACTDISPLAY_H

#include <string>

#include <osg/Node>
#include <osg/Uniform>

#include "../../graphicsEngine/WShader.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../kernel/WModuleOutputData.h"

/**
 * This module build the base for DirectVolumeRendering in OpenWalnut. It uses shader based raytracing and will be able to have multi dimensional
 * transfer functions.
 * \ingroup modules
 */
class WMProbTractDisplay: public WModule
{
public:

    /**
     * Default constructor.
     */
    WMProbTractDisplay();

    /**
     * Destructor.
     */
    virtual ~WMProbTractDisplay();

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
     * The root node used for this modules graphics. For OSG nodes, always use osg::ref_ptr to ensure proper resource management.
     */
    osg::ref_ptr< osg::Node > m_rootNode;

    /**
     * Callback for m_active. Overwrite this in your modules to handle m_active changes separately.
     */
    virtual void activate();

private:

    /**
     * An input connector used to get datasets from other modules. The connection management between connectors must not be handled by the module.
     */
    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_input;

    /**
     * This is a pointer to the dataset the module is currently working on.
     */
    boost::shared_ptr< WDataSetScalar > m_dataSet;

    /**
     * If this property is true, as special shader is used which emulates isosurfaces using the m_isoValue property.
     */
    WPropBool m_isoSurface;

    WPropInt m_isoValue0; //!< The Isovalue used in the case m_isoSurface is true for the 0'th isosurface.
    WPropInt m_isoValue1; //!< The Isovalue used in the case m_isoSurface is true for the 1'th isosurface.
    WPropInt m_isoValue2; //!< The Isovalue used in the case m_isoSurface is true for the 2'th isosurface.
    WPropInt m_isoValue3; //!< The Isovalue used in the case m_isoSurface is true for the 3'th isosurface.

    WPropColor m_isoColor0; //!< The color used when in isosurface mode for blending for the 0'th isosurface.
    WPropColor m_isoColor1; //!< The color used when in isosurface mode for blending for the 1'th isosurface.
    WPropColor m_isoColor2; //!< The color used when in isosurface mode for blending for the 2'th isosurface.
    WPropColor m_isoColor3; //!< The color used when in isosurface mode for blending for the 3'th isosurface.

    /**
     * The number of steps to walk along the ray.
     */
    WPropInt m_stepCount;

    WPropInt m_alpha0; //!< The alpha transparency used for the rendering
    WPropInt m_alpha1; //!< The alpha transparency used for the rendering
    WPropInt m_alpha2; //!< The alpha transparency used for the rendering
    WPropInt m_alpha3; //!< The alpha transparency used for the rendering

    /**
     * If true, the shader will only color using the depth of the point on the surface.
     */
    WPropBool m_useSimpleDepthColoring;

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * the PTD shader.
     */
    osg::ref_ptr< WShader > m_shader;

    /**
     * Node callback to change the color of the shapes inside the root node. For more details on this class, refer to the documentation in
     * moduleMain().
     */
    class SafeUpdateCallback : public osg::NodeCallback
    {
    public: // NOLINT

        /**
         * Constructor.
         *
         * \param module just set the creating module as pointer for later reference.
         */
        explicit SafeUpdateCallback( WMProbTractDisplay* module ): m_module( module ), m_initialUpdate( true )
        {
        };

        /**
         * operator () - called during the update traversal.
         *
         * \param node the osg node
         * \param nv the node visitor
         */
        virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );

        /**
         * Pointer used to access members of the module to modify the node.
         */
        WMProbTractDisplay* m_module;

        /**
         * Denotes whether the update callback is called the first time.
         */
        bool m_initialUpdate;
    };

    /**
     * Class handling uniform update during render traversal
     */
    class SafeUniformCallback: public osg::Uniform::Callback
    {
    public:

        /**
         * Constructor.
         *
         * \param module just set the creating module as pointer for later reference.
         */
        explicit SafeUniformCallback( WMProbTractDisplay* module ): m_module( module )
        {
        };

        /**
         * The callback. Called every render traversal for the uniform.
         *
         * \param uniform the uniform for which this callback is.
         * \param nv the visitor.
         */
        virtual void operator() ( osg::Uniform* uniform, osg::NodeVisitor* nv );

        /**
         * Pointer used to access members of the module to modify the node.
         */
        WMProbTractDisplay* m_module;
    };
};

#endif  // WMPROBTRACTDISPLAY_H

