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

#include <boost/program_options.hpp>

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

/**
 * This function defines and parses the valid command line options.
 * This might once be put in a separate class like WOptionHandler.
 * At the moment it seems reasonable that differnet GUIs might have
 * different command line options, thus we implement their parsing
 * in the GUI implemntation itself, i.e. here.
 */
boost::program_options::variables_map parseOptions( int argc, char** argv )
{
    // since the namespace is far to big we use a shortcut here
    namespace po = boost::program_options;

    po::variables_map optionsMap;
    po::options_description desc( "Allowed options" );
    desc.add_options()
        ( "help,h", "Prints this help message" )
        ( "input,i", po::value< std::vector< std::string > >(), "Input data files that should be loaded automatically" );

    po::positional_options_description p;
    p.add( "input", -1 );

    po::store( po::command_line_parser( argc, argv ).options( desc ).positional( p ).run(), optionsMap );
    po::notify( optionsMap );

    // print usage information if command line asks for help.
    if( optionsMap.count( "help" ) )
    {
        std::cout << desc << std::endl;
    }
    return optionsMap;
}

int WQt4Gui::run()
{
    try
    {
        m_optionsMap = parseOptions( argc, argv );
    }
    catch( boost::program_options::unknown_option e )
    {
        std::cout << e.what() << std::endl;
        return 1;
    }
    catch( boost::program_options::invalid_command_line_syntax e )
    {
        std::cout << e.what() << std::endl;
        return 1;
    }

    // exit as fast as possible if command line asked for help. The ,essage has been printed by parseOptions().
    if( m_optionsMap.count( "help" ) )
    {
        return 1;
    }

    WLogger::getLogger()->run();
    WLogger::getLogger()->addLogMessage( "Bringing up GUI", "GUI", LL_INFO );

    QApplication appl( argc, argv, true );

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
    m_isInitialized( true );

    // check if we want to load data due to command line and call the respective function
    if( m_optionsMap.count("input") )
    {
        m_kernel->doLoadDataSets( m_optionsMap["input"].as< std::vector< std::string > >() );
    }

    // run
    int qtRetCode = appl.exec();

    // signal everybody to shut down properly.
    WKernel::getRunningKernel()->wait( true );
    WKernel::getRunningKernel()->getGraphicsEngine()->wait( true );

    return qtRetCode;
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
