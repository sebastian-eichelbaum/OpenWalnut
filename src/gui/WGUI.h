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

#include "../common/WThreadedRunner.h"
#include "../kernel/WModule.h"
#include "qt4/signalslib.hpp"

/**
 * This class prescribes the interface to the GUI.
 * \ingroup gui
 */
class WGUI: public WThreadedRunner
{
public:

    /**
     * Default destructor.
     */
    virtual ~WGUI();

    virtual bool isInitalized() = 0;

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
    virtual void connectProperties( WProperties* properties ) = 0;

protected:
    bool m_isInitialized;
};


/**
 * \defgroup gui GUI
 *
 * \brief
 * This module implements the graphical user interface for OpenWalnut.
 *
 */

#endif  // WGUI_H
