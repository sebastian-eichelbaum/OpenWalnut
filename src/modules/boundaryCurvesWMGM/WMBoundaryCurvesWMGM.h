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

#ifndef WMBOUNDARYCURVESWMGM_H
#define WMBOUNDARYCURVESWMGM_H

#include <string>

#include "../../dataHandler/WDataSetScalar.h"
#include "../../dataHandler/WSubject.h"
#include "../../graphicsEngine/WGEManagedGroupNode.h"
#include "../../graphicsEngine/WGEShader.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"

/**
 * This module generates out of an image two isolines, adjustable via two sliders on a axial, coronal and sagittal slice. They each may represent a
 * gray matter or white matter boundary curve.
 *
 * \ingroup modules
 */
class WMBoundaryCurvesWMGM: public WModule
{
public:

    /**
     * Default constructor.
     */
    WMBoundaryCurvesWMGM();

    /**
     * Destructor.
     */
    virtual ~WMBoundaryCurvesWMGM();

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
     * Initializes the needed geodes, transformations and vertex arrays. This needs to be done once for each new dataset.
     *
     * \param grid the grid to places the slices in.
     * \param dataset the image to generate the boundary curves.
     */
    void initOSG( boost::shared_ptr< WGridRegular3D > grid, boost::shared_ptr< WDataSetScalar > dataset );

    /**
     * Input connector for t1 or t2 images, where the gray and white matter curves are extracted from.
     */
    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_textureIC;

    /**
     * The Geode containing all the slices and the mesh
     */
    osg::ref_ptr< WGEManagedGroupNode > m_output;

    /**
     * The transformation node moving the X slice through the dataset space if the sliders are used
     */
    osg::ref_ptr< WGEManagedGroupNode > m_xSlice;

    /**
     * The transformation node moving the Y slice through the dataset space if the sliders are used
     */
    osg::ref_ptr< WGEManagedGroupNode > m_ySlice;

    /**
     * The transformation node moving the Z slice through the dataset space if the sliders are used
     */
    osg::ref_ptr< WGEManagedGroupNode > m_zSlice;

    WPropGroup    m_sliceGroup; //!< the group contains several slice properties

    WPropInt      m_xPos; //!< x position of the slice

    WPropInt      m_yPos; //!< y position of the slice

    WPropInt      m_zPos; //!< z position of the slice

    WPropBool     m_showonX; //!< indicates whether it should be shown slice X

    WPropBool     m_showonY; //!< indicates whether it should be shown slice Y

    WPropBool     m_showonZ; //!< indicates whether it should be shown slice Z

    /**
     * Shader generating the curves of gray and white matter.
     */
    osg::ref_ptr< WGEShader > m_shader;

    /**
     * Slider to adjust the gray matter curve.
     */
    WPropDouble   m_grayMatter;

    /**
     * Slider to adjust the white matter curve.
     */
    WPropDouble   m_whiteMatter;
};

#endif  // WMBOUNDARYCURVESWMGM_H
