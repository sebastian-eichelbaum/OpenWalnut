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

#ifndef WUI_H
#define WUI_H

#include <boost/shared_ptr.hpp>

#include "../common/WFlag.h"

#include "WUIWidgetFactory.h"
#include "WUIViewEventHandler.h"
#include "WUIRequirement.h"

class WDataSet;

/**
 * This library implements the user interface for OpenWalnut.
 *
 * \defgroup ui UI
 */

/**
 * This class prescribes the interface to the UI. It basically is an abstract class defining the interface common to all possible
 * UI implementations.
 *
 * \ingroup ui
 */
class WUI: public boost::enable_shared_from_this< WUI >
{
public:
    /**
     * Constructor.
     *
     * \param argc number of arguments given on command line.
     * \param argv arguments given on command line.
     */
    WUI( int argc, char** argv );

    /**
     * Destructor.
     */
    virtual ~WUI();

    /**
     * Returns the init flag.
     *
     * \return Reference to the flag.
     */
    virtual const WFlag< bool >& isInitialized() const;

    /**
     * Runs the UI. All initialization should be done here.
     *
     * \return the return code.
     */
    virtual int run() = 0;

    /**
     * Returns the widget factory of the UI. Use it to create custom widgets.
     *
     * \return the factory. Use this to create your widget instances.
     */
    virtual WUIWidgetFactory::SPtr getWidgetFactory() const = 0;
protected:
    /**
     * Flag determining whether the UI is properly initialized.
     */
    WFlag< bool > m_isInitialized;

    /**
     * Number of command line arguments given.
     */
    int m_argc;

    /**
     * Command line arguments given.
     */
    char** m_argv;
};

#endif  // WUI_H

