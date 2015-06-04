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

#ifndef WMTEMPLATESHADERS_H
#define WMTEMPLATESHADERS_H

#include <string>
#include <vector>

#include <boost/thread.hpp>

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// If you want to learn how to program a module, refer to WMTemplate.cpp. It is an extensive tutorial on all the details.
// In this tutorial, we assume you already know how to write modules. For other examples, refer to the README file.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * \class WMTemplateShaders
 *
 * A module that explains the usage of the WGEShader interface. Please read the C++ code.
 *
 * \ingroup modules
 */
class WMTemplateShaders : public WModule
{
public:
    /**
     * Constuctor.
     */
    WMTemplateShaders();

    /**
     * Destructor.
     */
    virtual ~WMTemplateShaders();

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
    //! A condition for property updates.
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * Allow scaling the spheres.
     */
    WPropDouble m_sphereScaler;

    /**
     * Color of the plane
     */
    WPropColor m_planeColor;

    /**
     * Color of the spheres
     */
    WPropColor m_spheresColor;

    /**
     * Some weight.
     */
    WPropDouble m_aWeight;

    /**
     * Turn on/off animation.
     */
    WPropBool m_flicker;

    /**
     * Switch between several modes.
     */
    WPropSelection m_modeSelection;
};

#endif  // WMTEMPLATESHADERS_H
