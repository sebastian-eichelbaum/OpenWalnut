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

#ifndef WMSLICES_H
#define WMSLICES_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "core/kernel/WModuleContainer.h"

// forward declarations
class WDataSetScalar;
class WDataSetVector;
class WModule;
class WCondition;
template< class T > class WModuleInputForwardData;

/**
 * Combines two submodules in order to display probabilistic tract with a vector field of principal diffusion directed line
 * stipples as well as gray matter and white matter boundary curves as context. Since both submodules are slice based so this
 * container module is also slice based, and the reason for putting them together is just to move both slices at easy navigation
 *
 * \ingroup modules
 */
class WMSlices: public WModuleContainer
{
public:
    /**
     * Constructs this module.
     */
    WMSlices();

    /**
     * Destroy this module.
     */
    virtual ~WMSlices();

    /**
     * Gives back the name of this module.
     * \return The module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return Description to module.
     */
    virtual const std::string getDescription() const;

    /**
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     *
     * \return The prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     * Get the icon for this module in XPM format.
     *
     * \return icon pointer
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

    /**
     * Short cut to add a new submodule by its name via the WModuleFactory.
     *
     * \param prototypeName The name of the module which should be generated.
     *
     * \return The just generated module.
     */
    boost::shared_ptr< WModule > addNewSubmodule( const std::string &prototypeName );

    /**
     * Initializes and wires submodules. Also some properties might be forwarded from submodules so call this method after
     * \ref properties() was called.
     */
    void initSubModules();

private:
    /**
     * Since NavSlices may change during slice computation we have to check each redraw if we are outdated because
     * the change may already have been eaten by the NavSlices!
     *
     * \param sliceNum The slice number to check an redraw.
     * \param propName The property name of the slice position property
     */
    void redrawUntilSlicePosChangingAnymore( unsigned char sliceNum, const std::string &propName );

    /**
     * The probabilisitc tract which should be rendered.
     */
    boost::array< boost::shared_ptr< WModuleInputForwardData< WDataSetScalar > >, 9 > m_probICs;

    /**
     * Dataset used for the boundary surface generation of WM and GM.
     */
    boost::shared_ptr< WModuleInputForwardData< WDataSetScalar > > m_t1ImageIC;

    /**
     * Vector field of principal diffusion directions used for probabilistic tract rendering.
     */
    boost::shared_ptr< WModuleInputForwardData< WDataSetVector > > m_evecsIC;

    /**
     * Submodule displaying the t1 based GM WM boundary curves.
     */
    boost::shared_ptr< WModule > m_boundaryCurvesWMGM;

    /**
     * Submodule displaying the line stipples with the vector field of the given probabilistic tract.
     */
    boost::shared_ptr< WModule > m_probTractDisplaySP;

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
};

#endif  // WMSLICES_H
