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

#ifndef WMEEGVIEW_H
#define WMEEGVIEW_H

#include <string>

#include "../../kernel/WModule.h"

/**
 * Test module to open a new widget and display EEG data
 */
class WMEEGView: public WModule
{
public:
    /**
     * default constructor
     */
    WMEEGView();

    /**
     * destructor
     */
    virtual ~WMEEGView();

    /**
     * Due to the prototype design pattern used to build modules, this method
     * returns a new instance of this method.
     * NOTE: it should never be initialized or modified in some other way. A
     * simple new instance is required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     * Gets the name of this module.
     *
     * \return the name.
     */
    virtual const std::string getName() const;

    /**
     * Gets the description for this module.
     *
     * \return the description
     */
    virtual const std::string getDescription() const;

protected:
    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void moduleMain();

private:
    /**
     * Sample HUD-Text. Copied from an OSG-Example
     *
     * \return OSG-Graph
     */
    osg::Node* createText();
};

#endif  // WMEEGVIEW_H
