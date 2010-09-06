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
#include <vector>

#include <osg/Node>
#include <osg/Uniform>

#include "../../graphicsEngine/WShader.h"
#include "../../kernel/WModuleContainer.h"
#include "../../kernel/WModuleInputData.h"
#include "../../kernel/WModuleOutputData.h"

/**
 * This module build the base for DirectVolumeRendering in OpenWalnut. It uses shader based raytracing and will be able to have multi dimensional
 * transfer functions.
 * \ingroup modules
 */
class WMProbTractDisplay: public WModuleContainer
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

    /**
     * In order to use sub modules we need to create, initialize (properties) and wire them.
     */
    void initSubModules();

    /**
     * Update the submodules incase there has been additionally or less modules selected for isosurface generation.
     */
    void updateSubmoduleInstances();

    /**
     * Update the properties for each isosurface incase the number of isosurfaces has changed.
     */
    void updateProperties();

private:
    /**
     * Scalar dataset representing the probability field either in real numbers in [0,1] or gray values or just simply the connectivity
     * score (\#visits per voxel).
     */
    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_input;

    /**
     * This is a pointer to the dataset the module is currently working on.
     */
    boost::shared_ptr< WDataSetScalar > m_dataSet;

    /**
     * The number of ISO-Surfaces.
     */
    WPropInt m_numSurfaces;

    /**
     * For each surface its corresponding isovalue.
     */
    std::vector< WPropDouble > m_isoValues;

    /**
     * The color for each isosurface.
     */
    std::vector< WPropColor > m_colors;

    /**
     * The alpha values for each isosurface.
     */
    std::vector< WPropDouble > m_alphaValues;

    /**
     * Submodules for the iso surface generation.
     */
    std::vector< boost::shared_ptr< WModule > > m_isoSurfaces;
};

#endif  // WMPROBTRACTDISPLAY_H
