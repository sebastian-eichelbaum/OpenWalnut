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
#include "../../graphicsEngine/shaders/WGEShader.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../kernel/WModuleOutputData.h"

#include "../../common/math/WVector3D.h"

/**
 * Show an glyph plot (in this case the glyphs are arrows) of a vector data set.
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
     * The update callback that is called for the osg node of this module.
     */
    void updateCallback();

    /**
     * Transforms the given vertices according to m_matrix
     * \param verts These vertices will be transformed.
     */
    void transformVerts( osg::ref_ptr< osg::Vec3Array > verts );


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

    WPropDouble      m_xSlice; //!< x position of the slice

    WPropDouble      m_ySlice; //!< y position of the slice

    WPropDouble      m_zSlice; //!< z position of the slice

    WPropBool     m_showOnSagittal; //!< indicates whether the vector should be shown on sagittal slice

    WPropBool     m_showOnCoronal; //!< indicates whether the vector should be shown on coronal slice

    WPropBool     m_showOnAxial; //!< indicates whether the vector should be shown on axial slice


    WPropColor    m_aColor; //!< color

    osg::ref_ptr< WGEShader > m_shader; //!< the shader object for this module

    wmath::WPosition m_oldPos; //!< The previous position of the slides.
};

#endif  // WMVECTORPLOT_H
