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

#ifndef WMTEMPLATEUI_H
#define WMTEMPLATEUI_H

#include <string>
#include <vector>

#include <boost/thread.hpp>

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

/**
 * \class WMTemplateUI
 *
 * A module that explains the usage of the abstract UI interface in OpenWalnut. Please read the C++ code.
 *
 * \ingroup modules
 */
class WMTemplateUI : public WModule
{
public:
    /**
     * Constuctor.
     */
    WMTemplateUI();

    /**
     * Destructor.
     */
    virtual ~WMTemplateUI();

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
    /**
     * Called on every mouse drag-move event from the custom widget.
     *
     * \note this runs in OSG thread.
     * \param pos New mouse position.
     * \param button the button.
     */
    void handleMouseDrag( WVector2f pos, int button );

    /**
     * Called on every mouse move event from the custom widget.
     *
     * \note this runs in OSG thread.
     * \param pos New mouse position.
     */
    void handleMouseMove( WVector2f pos );

    /**
     * Called on every resize event from the custom widget.
     *
     * \note this runs in OSG thread.
     * \param x X pos
     * \param y Y pos
     * \param width Widht
     * \param height Height
     */
    void handleResize( int x, int y, int width, int height );

    /**
     * Handle mouse clicks
     *
     * \param coords where
     * \param button the button
     */
    void handleButtonRelease( WVector2f coords , int button );

    //! A condition for property updates.
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * A boolean property used in this example.
     */
    WPropBool m_boolProp;

    /**
     * A trigger property used in this example.
     */
    WPropTrigger m_triggerProp;
};

#endif  // WMTEMPLATEUI_H
