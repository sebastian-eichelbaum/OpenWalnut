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
#include "../../kernel/WKernel.h"
#include "../../graphicsEngine/WGraphicsEngine.h"

#include "WQt4Gui.h"

WQt4Gui::WQt4Gui( int argc, char** argv ):
    WGUI( argc, argv )
{
}

WQt4Gui::~WQt4Gui()
{
}

int WQt4Gui::run()
{
    WLogger::getLogger()->addLogMessage( "Bringing up GUI", "GUI", LL_DEBUG );

#ifdef __APPLE__
    // TODO(hlawitschka): what does the third parameter mean?
    QApplication appl( argc, argv, 0 );
#else
    QApplication appl( argc, argv );
#endif

    // startup graphics engine
    m_ge = boost::shared_ptr< WGraphicsEngine >( new WGraphicsEngine() );

    // and startup kernel
    m_kernel = boost::shared_ptr< WKernel >( new WKernel( m_ge, shared_from_this() ) );
    m_kernel->run();

    // create the window
    m_gui = new WMainWindow;
    m_gui->show();

    // connect out loader signal with krnel
    getLoadButtonSignal()->connect( boost::bind( &WKernel::doLoadDataSets, m_kernel, _1 ) );

    // bind the GUI's slot with the ready signal
    t_ModuleGenericSignalHandlerType f = boost::bind( &WGUI::slotAddDatasetToBrowser, this, _1 );
    m_kernel->getRootContainer()->addDefaultNotifier( READY, f );

    // now we are initialized
    m_isInitialized = true;
    m_isInitializedCondition.notify();

    // run
    // NOTE: kernel shutdown is implemented in WMainWindow
    return appl.exec();
}

void WQt4Gui::addDatasetToBrowser( boost::shared_ptr< WModule > module, int subjectId )
{
    m_gui->getDatasetBrowser()->addDataset( module, subjectId );
}

std::vector< boost::shared_ptr< WModule > >WQt4Gui::getDataSetList( int subjectId )
{
    return m_gui->getDatasetBrowser()->getDataSetList( subjectId );
}

boost::signal1< void, std::vector< std::string > >* WQt4Gui::getLoadButtonSignal()
{
    return m_gui->getLoaderSignal();
}

void WQt4Gui::connectProperties( boost::shared_ptr<WProperties> properties )
{
    m_gui->getPropertyManager()->connectProperties( properties );
}
