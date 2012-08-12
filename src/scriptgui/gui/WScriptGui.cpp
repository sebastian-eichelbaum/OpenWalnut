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

#include <fstream>
#include <string>
#include <vector>

#include "core/common/WLogger.h"
#include "core/common/WIOTools.h"
#include "core/common/WLogStream.h"
#include "core/common/WThreadedRunner.h"
#include "core/common/WSegmentationFault.h"
#include "core/common/WPathHelper.h"
#include "scripting/WScriptInterpreterFactory.h"

#include "core/kernel/WKernel.h"
#include "core/kernel/WModuleFactory.h"

#include "WScriptGui.h"

WScriptGui::WScriptGui( int argc, char** argv, boost::program_options::variables_map const& options )
    : WGUI( argc, argv ),
      m_programOptions( options )
{
}

WScriptGui::~WScriptGui()
{
}

int WScriptGui::run()
{
    // the call path of the application
    boost::filesystem::path walnutBin = boost::filesystem::path( m_argv[ 0 ] ).parent_path();

    // TODO(reichenbach): this might not work under windows
    boost::filesystem::path homePath = boost::filesystem::path( getenv( "HOME" ) );

    // setup path helper which provides several paths to others
#ifdef Q_WS_MAC
    // apple has a special file hierarchy in so-called bundles
    // this code determines whether we are started from a bundle context
    // and sets the paths according to Apple's guidelines inside the bundle
    // if( QApplication::applicationDirPath().endsWith( "/MacOS" ) )
    {
        // we are in a bundle
        // TODO(mario): apply default OSX behavior of using $HOME/Library/OpenWalnut ?
        WPathHelper::getPathHelper()->setBasePathsOSXBundle( walnutBin, homePath / ".OpenWalnut" );
    }
    // else
    {
        // assume standard behavior
        WPathHelper::getPathHelper()->setBasePaths( walnutBin, homePath / ".OpenWalnut" );
    }
#else
    // on all other platforms, get the home directory form Qt and the path from the application binary location
    WPathHelper::getPathHelper()->setBasePaths( walnutBin, homePath / ".OpenWalnut" );
#endif

    //--------------------------------
    // choose interpreter to use
    //--------------------------------
    boost::shared_ptr< WScriptInterpreter > scriptInterpreter;
    bool executeScriptFile = false;

    // first check if a script file is to be executed
    if( m_programOptions.count( "file" ) )
    {
        boost::filesystem::path scriptFile( m_programOptions[ "file" ].as< std::vector< std::string > >()[ 0 ] );

        if( !boost::filesystem::exists( scriptFile ) )
        {
            wlog::error( "Walnut" ) << std::string( "Could not find script file: " ) + scriptFile.string();
            wlog::error( "Walnut" ) << "Aborting!";
            return 1;
        }

        std::string ext = scriptFile.extension().string();

        scriptInterpreter = WScriptInterpreterFactory::constructByFileExtension( ext );
        executeScriptFile = ( scriptInterpreter != NULL );

        if( executeScriptFile )
        {
            // set command line parameters
            scriptInterpreter->setParameters( m_programOptions[ "file" ].as< std::vector< std::string > >() );
        }
    }
    // then check for interp parameter
    else if( m_programOptions.count( "interp" ) )
    {
        scriptInterpreter = WScriptInterpreterFactory::constructByName( m_programOptions[ "interp" ].as< std::string >() );
    }

    if( !scriptInterpreter )
    {
        wlog::error( "Walnut" ) << "Could not create a script interpreter.";
        wlog::error( "Walnut" ) << "Only the following interpreters are supported in this build:";
        wlog::error( "Walnut" ) << WScriptInterpreterFactory::getSupportedInterpreterList();
        wlog::error( "Walnut" ) << "If the interpreter you want to use is not listed, it is either not implemented yet"
                                << " or dependencies are missing.";
        return 1;
    }

    //----------------------------
    // startup
    //----------------------------

    // now we successfully chose a script interpreter to use
    // start the kernel
    boost::shared_ptr< WKernel > kernel( WKernel::instance( WGraphicsEngine::getGraphicsEngine(), shared_from_this() ) );
    kernel->run();

    // initialize walnut bindings for the interpreter
    scriptInterpreter->initBindings();

    // now we are initialized
    m_isInitialized( true );

    // execute
    if( executeScriptFile )
    {
        // execute provided script file

        // load file content into string
        std::ifstream in( m_programOptions[ "file" ].as< std::vector< std::string > >()[ 0 ].c_str() );
        std::string script;
        std::string line;
        while( std::getline( in, line ) )
        {
            script += line + "\n";
        }
        in.close();

        // execute
        try
        {
            scriptInterpreter->execute( script );
        }
        catch( WException const& e )
        {
            wlog::error( "Walnut" ) << "Error while executing script: " << e.what();
        }
    }
    else
    {
        // get lines to interpret from the user via console
        while( true )
        {
            std::string in;
            std::cout << ">>";
            std::getline( std::cin, in );

            if( in == "quit" )
            {
                break;
            }
            else if( in == "help" )
            {
                in += "()";
            }

            try
            {
                scriptInterpreter->execute( in );
            }
            catch( WException const& e )
            {
                wlog::error( "Walnut" ) << "Error while executing script: " << e.what();
                break;
            }
        }
    }

    // delete interpreter
    scriptInterpreter.reset();

    // signal everybody to shut down properly.
    WKernel::getRunningKernel()->wait( true );

    // TODO(reichenbach): waiting for the graphics engine to stop will result in a deadlock; why?
    // WKernel::getRunningKernel()->getGraphicsEngine()->wait( true );

    return 0;
}

WCustomWidget::SPtr WScriptGui::openCustomWidget( std::string, WGECamera::ProjectionMode, boost::shared_ptr< WCondition > )
{
    return WCustomWidget::SPtr();
}

void WScriptGui::closeCustomWidget( std::string )
{
}

void WScriptGui::closeCustomWidget( WCustomWidget::SPtr )
{
}


