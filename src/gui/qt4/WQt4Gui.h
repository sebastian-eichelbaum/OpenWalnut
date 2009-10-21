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

#ifndef WQT4GUI_H
#define WQT4GUI_H

#include <string>
#include <vector>

#include "../WGUI.h"

class WMainWindow;

/**
 * Starts up the QT GUI.
 * \ingroup gui
 */
class WQt4Gui : public WGUI
{
public:
    /**
     * Default Constructor.
     */
    WQt4Gui();

    /**
     * Default destructor.
     */
    virtual ~WQt4Gui();

    /**
     * return trues when initialization is finished, otherwise false
     */
    bool isInitalized();

    void addDatasetToBrowser( boost::shared_ptr< WModule > module, int subjectId );

    /**
     * returns a vector of pointers to the loaded datasets for a given subject
     */
    virtual std::vector< boost::shared_ptr< WModule > >getDataSetList( int subjectId );

    /**
     * getter functions for the signales proved by the gui
     */
    boost::signal1< void, std::vector< std::string > >* getLoadButtonSignal();

    /**
     * this function allows modules to register their property object with the gui
     */
    virtual void connectProperties( WProperties* properties );

protected:

    /**
     * Execution loop.
     */
    virtual void threadMain();


private:
    WMainWindow* m_gui;
};

#endif  // WQT4GUI_H

