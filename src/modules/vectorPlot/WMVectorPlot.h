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

#ifndef WMVECTORPLOT_H
#define WMVECTORPLOT_H

#include <string>
#include <vector>

#include <osg/Geode>

#include "../../dataHandler/WDataSetVector.h"
#include "../../graphicsEngine/WShader.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../kernel/WModuleOutputData.h"

#include "../../common/math/WVector3D.h"

/**
 * Show an arrow plot of a vector data set.
 * \warning Selectable slices are limited to [0,160]x[0,200]x[0,160] so far.
 * \ingroup modules
 */
class WMVectorPlot: public WModule
{
public:

    /**
     * Default constructor.
     */
    WMVectorPlot();

    /**
     * Destructor.
     */
    virtual ~WMVectorPlot();

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
    osg::ref_ptr<osg::Geode> m_rootNode;

    /**
     * Callback for m_active. Overwrite this in your modules to handle m_active changes separately.
     */
    virtual void activate();

private:
    /**
     * creates a vector plot on the currently selected slices
     */
    osg::ref_ptr<osg::Geometry> buildPlotSlices();


    /**
     * An input connector used to get datasets from other modules. The connection management between connectors must not be handled by the module.
     */
    boost::shared_ptr< WModuleInputData< WDataSetVector > > m_input;

    /**
     * This is a pointer to the dataset the module is currently working on.
     */
    boost::shared_ptr< WDataSetVector > m_dataSet;

    /**
     * list of positions to plot vectors on, this will come from a selection tool class
     */
    boost::shared_ptr< std::vector< wmath::WPosition > > m_positions;

    WPropBool     m_projectOnSlice; //!< projects the vectors on the slice.

    WPropBool     m_coloringMode; //!< indicates a set color or direction color coding.

    WPropInt      m_xPos; //!< x posistion of the slice

    WPropInt      m_yPos; //!< y posistion of the slice

    WPropInt      m_zPos; //!< z posistion of the slice

    WPropBool     m_showonX; //!< in dicates whether the vector should be shown on slice X

    WPropBool     m_showonY; //!< in dicates whether the vector should be shown on slice Y

    WPropBool     m_showonZ; //!< in dicates whether the vector should be shown on slice Z


    WPropColor    m_aColor; //!< color

    osg::ref_ptr< WShader > m_shader; //!< the shader object for this module

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
        explicit SafeUpdateCallback( WMVectorPlot* module ): m_module( module )
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
        WMVectorPlot* m_module;
    };
};

#endif  // WMVECTORPLOT_H

