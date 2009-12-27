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
#include <fstream>
#include <string>
#include <vector>

#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

#include <QtGui/QApplication>
#include <QtGui/QFileDialog>

#include "WMainWindow.h" // this has to be included before any other includes
#include "../../graphicsEngine/WGraphicsEngine.h"
#include "../../kernel/WKernel.h"
#include "../../modules/data/WMData.h"
#include "../../utils/WIOTools.h"
#include "WCreateCustomDockWidgetEvent.h"
#include "WQt4Gui.h"

WQt4Gui::WQt4Gui( int argc, char** argv )
    : WGUI( argc, argv )
{
}

WQt4Gui::~WQt4Gui()
{
}

#ifdef _WIN32
// TODO(wiebel): Hi, this is math. I assume the line below is due to linker errors with boost. I just marked it with this todo
// since we don't forget this. When using Linux or Mac I _think_ we don't need this.
const unsigned int boost::program_options::options_description::m_default_line_length = 2048;
#endif

bool WQt4Gui::parseOptions()
{
    namespace po = boost::program_options; // since the namespace is far to big we use a shortcut here
    po::options_description desc( "Allowed options" );

#ifndef _WIN32
// TODO(wiebel): this does not link on windows at the moment. But it should!
    desc.add_options()
        ( "help,h", "Prints this help message" )
        ( "input,i", po::value< std::vector< std::string > >(), "Input data files that should be loaded automatically" );
#endif

    po::positional_options_description p;
    p.add( "input", -1 );

    try
    {
        po::store( po::command_line_parser( m_argc, m_argv ).options( desc ).positional( p ).run(), m_optionsMap );
    }
    catch( const po::error &e )
    {
        std::cerr << e.what() << std::endl;
        return false;
    }

    po::notify( m_optionsMap );

    //=====================
    // CONFIGURATION FILE
    po::options_description guiConfigurationDescription( "GUI configuration" );

#ifndef _WIN32
// TODO(wiebel): this does not link on windows at the moment. But it should!
    guiConfigurationDescription.add_options()
        ( "ge.bgColor.r", po::value< float >() )
        ( "ge.bgColor.g", po::value< float >() )
        ( "ge.bgColor.b", po::value< float >() );
#endif

    std::string cfgFileName( "walnut.cfg" );

    if( wiotools::fileExists( cfgFileName ) )
    {
        wlog::info( "GUI" ) << "Reading config file: " << cfgFileName;
        std::ifstream ifs( cfgFileName.c_str(), std::ifstream::in );

        try
        {
            po::store( po::parse_config_file( ifs, guiConfigurationDescription ), m_guiConfiguration );
        }
        catch( const po::error &e )
        {
            std::cerr << "Error in configuration file \"" << cfgFileName << "\": " << e.what() << std::endl;
            return false;
        }
    }
    else
    {
        wlog::info( "GUI" ) << "No Config file: " << cfgFileName << " found";
    }

    po::notify( m_guiConfiguration );

    // print usage information if command line asks for help.
    if( m_optionsMap.count( "help" ) )
    {
        std::cout << desc << std::endl;
    }
    return true;
}

int WQt4Gui::run()
{
    bool parsingSuccessful = parseOptions();

    if( !parsingSuccessful )
    {
        return 1;
    }

    // exit as fast as possible if command line asked for help. The ,essage has been printed by parseOptions().
    if( m_optionsMap.count( "help" ) )
    {
        return 1;
    }

    WLogger::getLogger()->run();
    WLogger::getLogger()->addLogMessage( "Bringing up GUI", "GUI", LL_INFO );

    QApplication appl( m_argc, m_argv, true );

    // startup graphics engine
    m_ge = WGraphicsEngine::getGraphicsEngine();

    // and startup kernel
    m_kernel = boost::shared_ptr< WKernel >( new WKernel( m_ge, shared_from_this() ) );
    m_kernel->run();
    // create the window
    m_gui = new WMainWindow( m_guiConfiguration );
    m_gui->show();

    // connect out loader signal with krnel
    getLoadButtonSignal()->connect( boost::bind( &WKernel::loadDataSets, m_kernel, _1 ) );

    m_gui->getModuleButtonSignal()->connect( boost::bind( &WKernel::applyModule, m_kernel, _1, _2 ) );

    // bind the GUI's slot with the ready signal
    t_ModuleGenericSignalHandlerType f = boost::bind( &WQt4Gui::slotAddDatasetOrModuleToBrowser, this, _1 );
    m_kernel->getRootContainer()->addDefaultNotifier( WM_READY, f );

    // now we are initialized
    m_isInitialized( true );

    // check if we want to load data due to command line and call the respective function
    if( m_optionsMap.count("input") )
    {
        m_kernel->loadDataSets( m_optionsMap["input"].as< std::vector< std::string > >() );
    }

    // run
    int qtRetCode = appl.exec();

    // signal everybody to shut down properly.
    WKernel::getRunningKernel()->wait( true );
    WKernel::getRunningKernel()->getGraphicsEngine()->wait( true );

    return qtRetCode;
}

void WQt4Gui::slotAddDatasetOrModuleToBrowser( boost::shared_ptr< WModule > module )
{
    // get properties from the module and register them
    m_gui->getPropertyManager()->connectProperties( module->getProperties() );

    // TODO(schurade): is this differentiation between data and "normal" modules really needed?
    if ( boost::shared_dynamic_cast< WMData >( module ).get() )
    {
        m_gui->getDatasetBrowser()->addDataset( module, 0 );
    }
    else
    {
        m_gui->getDatasetBrowser()->addModule( module );
    }
}

std::vector< boost::shared_ptr< WDataSet > > WQt4Gui::getDataSetList( int subjectId, bool onlyTextures )
{
    return m_gui->getDatasetBrowser()->getDataSetList( subjectId, onlyTextures );
}

boost::shared_ptr< WModule > WQt4Gui::getSelectedModule()
{
    return m_gui->getDatasetBrowser()->getSelectedModule();
}

boost::signals2::signal1< void, std::vector< std::string > >* WQt4Gui::getLoadButtonSignal()
{
    return m_gui->getLoaderSignal();
}

boost::signals2::signal1< void, std::string >* WQt4Gui::getPickSignal()
{
    return m_gui->getPickSignal();
}

void WQt4Gui::createCustomWidget( std::string title )
{
    QCoreApplication::postEvent( m_gui, new WCreateCustomDockWidgetEvent( title ) );
}

void WQt4Gui::closeCustomWidget( std::string title )
{
    m_gui->closeCustomDockWidget( title );
}
