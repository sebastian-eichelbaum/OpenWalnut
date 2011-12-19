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

#ifndef WMDIRECTVOLUMERENDERING_H
#define WMDIRECTVOLUMERENDERING_H

#include <string>

#include <osg/Node>
#include <osg/Uniform>

#include "core/graphicsEngine/shaders/WGEShader.h"
#include "core/dataHandler/WDataSetScalar.h"
#include "core/dataHandler/WDataSetVector.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

/**
 * This module is a basic volume renderer.
 *
 * \ingroup modules
 */
class WMDirectVolumeRendering: public WModule
{
public:
    /**
     * Default constructor.
     */
    WMDirectVolumeRendering();

    /**
     * Destructor.
     */
    virtual ~WMDirectVolumeRendering();

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
     * \return the icon.
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
     * Initialize requirements for this module.
     */
    virtual void requirements();

private:
    /**
     * The transfer function as an input data set
     */
    boost::shared_ptr< WModuleInputData< WDataSetSingle > > m_transferFunction;

    /**
     * An input connector used to get datasets from other modules. The connection management between connectors must not be handled by the module.
     */
    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_input;

    /**
     * The gradient field input
     */
    boost::shared_ptr< WModuleInputData< WDataSetVector > > m_gradients;

    /**
     * The number of samples to walk along the ray.
     */
    WPropInt m_samples;

    /**
     * Types of local illumination supported.
     */
    enum LOCALILLUMINATION_ALGORITHMS
    {
        None = 0,
        Phong
    };

    /**
     * The available shading algorithms.
     */
    boost::shared_ptr< WItemSelection > m_localIlluminationSelections;

    /**
     * The actually selected shading algorithm.
     */
    WPropSelection m_localIlluminationAlgo;

    /**
     * All properties for those nice improvement methods.
     */
    WPropGroup m_improvementGroup;

    /**
     * If true, stochastic jittering is used for image quality improvement.
     */
    WPropBool m_stochasticJitterEnabled;

    /**
     * If active, the opacity of the classified fragment gets scaled according to sample count to ensure relative opacities even if sampling
     * number changes (m_samples)
     */
    WPropBool m_opacityCorrectionEnabled;

    /**
     * If active, Maximum intensity projection is used based on the alpha value of the points
     */
    WPropBool m_maximumIntensityProjectionEnabled;

    /**
     * If active, color coding is done by depth projection mode where the intensity is taken from the alpha channel and the color from the color channel ( last part not implemented, yet )
     */
    WPropBool m_depthProjectionEnabled;

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * the DVR shader.
     */
    osg::ref_ptr< WGEShader > m_shader;

    /**
     * the main geometry node
     */
    osg::ref_ptr< osg::Node > cube;
};

#endif  // WMDIRECTVOLUMERENDERING_H

