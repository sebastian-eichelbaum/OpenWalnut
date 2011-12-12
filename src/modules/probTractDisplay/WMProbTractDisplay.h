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

#include "core/kernel/WModuleContainer.h"

// forward declarations
class WModule;
template< class T > class WModuleInputForwardData;

/**
 * This module generates a visualization of probabilistic tracts using iso surfaces.
 * Therefor three iso surfaces are generated and the user may choose isovalue, color and
 * alpha value for each surface.
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
     * \return The icon.
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
     * In order to use sub modules we need to create, initialize (properties) and wire them.
     */
    void initSubModules();

private:
    /**
     * Scalar dataset representing the probability field either in real numbers in [0,1] or gray values or just simply the connectivity
     * score (\#visits per voxel).
     */
    boost::shared_ptr< WModuleInputForwardData< WDataSetScalar > > m_input;

    /**
     * Submodules for the iso surface generation.
     */
    std::vector< boost::shared_ptr< WModule > > m_isoSurfaces;
};

#endif  // WMPROBTRACTDISPLAY_H
