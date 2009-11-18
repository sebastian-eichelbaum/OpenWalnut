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
#include <vector>

#include <boost/shared_ptr.hpp>

#include "../common/WThreadedRunner.h"
#include "../common/WFlag.hpp"
#include "../kernel/WModule.h"
#include "qt4/signalslib.hpp"

/**
 * \defgroup gui GUI
 *
 * \brief
 * This module implements the graphical user interface for OpenWalnut.
 *
 */

/**
 * This class prescribes the interface to the GUI. It basically is an abstract class defining the interface common to all possible
 * GUI implementations.
 *
 * \ingroup gui
 */
class WGUI:
    public boost::enable_shared_from_this< WGUI >
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
     * Returns the init flag..
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
     * Slot gets called whenever a new module is added.
     * 
     * \param module
     */
    virtual void slotAddDatasetToBrowser( boost::shared_ptr< WModule > module );

    /**
     *  adds a dataset to the dataset browser for a give subject
     */
    virtual void addDatasetToBrowser( boost::shared_ptr< WModule > module, int subjectId ) = 0;

    /**
     * returns a vector of pointers to the loaded datasets for a given subject
     */
    virtual std::vector< boost::shared_ptr< WModule > >getDataSetList( int subjectId ) = 0;

    /**
     * getter functions for the signales proved by the gui
     */
    virtual boost::signal1< void, std::vector< std::string > >* getLoadButtonSignal() = 0;

    /**
     * this function allows modules to register their property object with the gui
     */
    virtual void connectProperties( boost::shared_ptr<WProperties> properties ) = 0;

protected:

    /**
     * Flag determining whether the GUI is properly initialized.
     */
    WFlag< bool > m_isInitialized;

    /**
     * Number of command line arguments given.
     */
    int argc;

    /**
     * Command line arguments given.
     */
    char** argv;
};

#endif  // WGUI_H

