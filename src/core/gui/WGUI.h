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

#ifndef WGUI_H
#define WGUI_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "../common/WFlag.h"
#include "../graphicsEngine/WGECamera.h"
#include "WCustomWidget.h"

class WDataSet;

/**
 * This library implements the graphical user interface for OpenWalnut.
 *
 * \defgroup gui GUI
 */

/**
 * This class prescribes the interface to the GUI. It basically is an abstract class defining the interface common to all possible
 * GUI implementations.
 *
 * \ingroup gui
 */
class WGUI : public boost::enable_shared_from_this< WGUI >
{
public:
    /**
     * Constructor.
     *
     * \param argc number of arguments given on command line.
     * \param argv arguments given on command line.
     */
    WGUI( int argc, char** argv );

    /**
     * Destructor.
     */
    virtual ~WGUI();

    /**
     * Returns the init flag.
     *
     * \return Reference to the flag.
     */
    virtual const WFlag< bool >& isInitialized() const;

    /**
     * Runs the GUI. All initialization should be done here.
     *
     * \return the return code.
     */
    virtual int run() = 0;

    /**
     * Instruct to open a new custom widget. The specified condition should be the shutdown condition of the module, as the function returns only
     * if the widget was created. To ensure that the creation is aborted properly if the module shuts down in the meantime, this condition is
     * used.
     *
     * \note this function blocks until the widget was created. Check the resulting pointer for NULL.
     *
     * \param title the title of the widget
     * \param projectionMode the kind of projection which should be used
     * \param shutdownCondition a condition enforcing abort of widget creation.
     *
     * \return the created widget
     */
    virtual WCustomWidget::SPtr openCustomWidget(
            std::string title,
            WGECamera::ProjectionMode projectionMode,
            boost::shared_ptr< WCondition > shutdownCondition ) = 0;

    /**
     * Instruct to close a custom widget.
     *
     * \param title The title of the widget
     */
    virtual void closeCustomWidget( std::string title ) = 0;

    /**
     * Instruct to close the custom widget.
     *
     * \param widget the widget to close again.
     */
    virtual void closeCustomWidget( WCustomWidget::SPtr widget ) = 0;

protected:
    /**
     * Flag determining whether the GUI is properly initialized.
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

#endif  // WGUI_H

