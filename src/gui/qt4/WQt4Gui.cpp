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

#include <iostream>
#include <string>
#include <vector>

#include <QtGui/QApplication>
#include <QtGui/QFileDialog>

#include "WMainWindow.h"

#include "WQt4Gui.h"

WQt4Gui::WQt4Gui():
    WGUI()
{
    m_isInitialized = false;
}

WQt4Gui::~WQt4Gui()
{
}

void WQt4Gui::threadMain()
{
    // TODO(ebaum): currently removed argument stuff. will be done later in conjunction with a better
    // option handler.+
#ifdef __APPLE__
    char * dummy = "";
    int dummyInt = 0;
    QApplication appl( dummyInt, &dummy, 0 );
#else
    QApplication appl( 0, NULL );
#endif
    QMainWindow* mainWindow = new QMainWindow;
    m_gui = new WMainWindow;
    m_gui->setupGUI( mainWindow );

    mainWindow->show();

    m_isInitialized = true;

    int qtExecResult;
    qtExecResult = appl.exec();

    // TODO(ebaum): how to handle return codes?
}

void WQt4Gui::addDatasetToBrowser( boost::shared_ptr< WModule > module, int subjectId )
{
    m_gui->getDatasetBrowser()->addDataset( module, subjectId );
}

std::vector< boost::shared_ptr< WModule > >WQt4Gui::getDataSetList( int subjectId )
{
    return m_gui->getDatasetBrowser()->getDataSetList( subjectId );
}

bool WQt4Gui::isInitalized()
{
    return m_isInitialized;
}

boost::signal1< void, std::vector< std::string > >* WQt4Gui::getLoadButtonSignal()
{
    return m_gui->getLoaderSignal();
}

boost::signal1< void, int >* WQt4Gui::getAxialSliderSignal()
{
    return m_gui->getNavAxial()->getboostSignal();
}

boost::signal1< void, int >* WQt4Gui::getCoronalSliderSignal()
{
    return m_gui->getNavCoronal()->getboostSignal();
}

boost::signal1< void, int >* WQt4Gui::getSagittalSliderSignal()
{
    return m_gui->getNavSagittal()->getboostSignal();
}

boost::signal1< void, bool >* WQt4Gui::getAxialButtonSignal()
{
    return m_gui->getAxiSignal();
}

boost::signal1< void, bool >* WQt4Gui::getCoronalButtonSignal()
{
    return m_gui->getCorSignal();
}

boost::signal1< void, bool >* WQt4Gui::getSagittalButtonSignal()
{
    return m_gui->getSagSignal();
}
