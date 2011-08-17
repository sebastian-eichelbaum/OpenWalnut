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

#include "core/kernel/WModule.h"

// forward declarations
class WBoundaryBuilder;
class WDataSetScalar;
class WGEManagedGroupNode;
template< class T > class WModuleInputData;

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
     *
     * \return xpm pointer
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
     * This is a callback for the builder, inorder to re-run if certain properties have changed.
     * It applys only if the selected strategy is "Iso-Lines".
     *
     * \note We need this, since the Iso-Fragment builder does not need to be informed about those changes since the properties
     * are directly coupled to some uniforms inside shaders.  The Iso-Lines strategy instead needs to rebuild its geodes every
     * e.g. slice change. Of course we could have accomplish this with some logic inside moduleMain's while loop but this way the
     * logic is simple and clear for every strategy.
     *
     * \param prop This is a standard parameter for the callback and not used since each builder has already access to all
     * properties.
     */
    void rerunBuilder( boost::shared_ptr< WPropertyBase > prop );

    /**
     * Input connector for t1 or t2 images, where the gray and white matter curves are extracted from.
     */
    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_textureIC;

    /**
     * The Geode containing all the slices and the mesh
     */
    osg::ref_ptr< WGEManagedGroupNode > m_output;

    /**
     * Axial, coronal and sagittal slices as array.
     * 0 : xSlice, 1 : ySlice, 2 : zSlice
     */
    boost::array< osg::ref_ptr< WGEManagedGroupNode >, 3 > m_slices;

    /**
     * The group contains several slice properties
     */
    WPropGroup m_sliceGroup;

    /**
     * Position of the axial, sagittal and coronal slices.
     * 0 : xSlice, 1 : ySlice, 2 : zSlice
     */
    boost::array< WPropDouble, 3 > m_slicePos;

    /**
     * Indicates if the corresponding slice is shown or not.
     * 0 : xSlice, 1 : ySlice, 2 : zSlice
     */
    boost::array< WPropBool, 3 > m_showSlice;

    /**
     * List for selecting the strategy.
     */
    WPropSelection m_strategySelector;

    /**
     * Builder for graphic generation with the selected strategy.
     */
    boost::shared_ptr< WBoundaryBuilder > m_builder;

    /**
     * Isovalue specifying the Gray-Matter-CSF border.
     */
    WPropDouble m_grayMatter;

    /**
     * Isovalue specifying the White-Matter-Gray-Matter border.
     */
    WPropDouble m_whiteMatter;

    /**
     * Size of the quads, used for sampling the Iso-Lines.
     */
    WPropDouble m_resolution;

    /**
     * Used to enforce a unique access on slice updates!
     */
    boost::mutex m_updateMutex;

    /**
     * The color used for the gray matter.
     * \note: ATM only the IsoLines strategy makes use of this.
     */
    WPropColor m_grayMatterColor;

    /**
     * The color used for the white matter.
     * \note: ATM only the IsoLines strategy makes use of this.
     */
    WPropColor m_whiteMatterColor;
};

#endif  // WMBOUNDARYCURVESWMGM_H
