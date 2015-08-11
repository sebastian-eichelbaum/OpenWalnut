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

#include "core/kernel/WKernel.h"

#include "../wrappers/WColorWrapper.h"
#include "../wrappers/WLoggerWrapper.h"
#include "../wrappers/WModuleWrapper.h"
#include "../wrappers/WPropertyGroupWrapper.h"
#include "../wrappers/WPropertyWrapper.h"
#include "../wrappers/WUtilityFunctions.h"

#include "WScriptInterpreterPython.h"

#ifdef PYTHON_FOUND

WScriptInterpreterPython::WScriptInterpreterPython( boost::shared_ptr< WModuleContainer > const& rootContainer )
    : WScriptInterpreter(),
      m_rootContainer( rootContainer ),
      m_argc( 0 ),
      m_argv( 0 ),
      m_scriptThread( *this )
{
    try
    {
        Py_Initialize();
        m_pyModule = pb::import( "__main__" );
        m_pyMainNamespace = m_pyModule.attr( "__dict__" );
    }
    catch( pb::error_already_set const& )
    {
        PyErr_Print();
    }
    // Make ctrl+c key available for killing interpeter
    execute( "import signal" );
    execute( "signal.signal( signal.SIGINT, signal.SIG_DFL )" );

    m_scriptThread.run();
}

WScriptInterpreterPython::~WScriptInterpreterPython()
{
    m_scriptThread.requestStop();
    m_scriptThread.wait();

    Py_Finalize();

    if( m_argv )
    {
        for( int k = 0; k < m_argc; ++k )
        {
            delete[] m_argv[ k ];
        }
        delete[] m_argv;
    }
}

void WScriptInterpreterPython::initBindings()
{
    boost::unique_lock< boost::mutex > lock( m_mutex );

    m_pyMainNamespace[ "WColor" ] = pb::class_< WColorWrapper >( "WColor", pb::init< float, float, float, float >() )
                                    .add_property( "r", &WColorWrapper::getR, &WColorWrapper::setR )
                                    .add_property( "g", &WColorWrapper::getG, &WColorWrapper::setG )
                                    .add_property( "b", &WColorWrapper::getB, &WColorWrapper::setB )
                                    .add_property( "a", &WColorWrapper::getA, &WColorWrapper::setA );

    // bind WPropertyWrapper class to "WProperty" in the python namespace
    // no constructor in python for now
    m_pyMainNamespace[ "WProperty" ] = pb::class_< WPropertyWrapper >( "WProperty", pb::no_init )
                                       .def( "getBool", &WPropertyWrapper::getBool )
                                       .def( "getInt", &WPropertyWrapper::getInt )
                                       .def( "getString", &WPropertyWrapper::getString )
                                       .def( "getDouble", &WPropertyWrapper::getDouble )
                                       .def( "getFilename", &WPropertyWrapper::getFilename )
                                       .def( "getSelection", &WPropertyWrapper::getSelection )
                                       .def( "getColor", &WPropertyWrapper::getColor )
                                       .def( "setBool", &WPropertyWrapper::setBool )
                                       .def( "setInt", &WPropertyWrapper::setInt )
                                       .def( "setString", &WPropertyWrapper::setString )
                                       .def( "setDouble", &WPropertyWrapper::setDouble )
                                       .def( "setFilename", &WPropertyWrapper::setFilename )
                                       .def( "setSelection", &WPropertyWrapper::setSelection )
                                       .def( "setColor", &WPropertyWrapper::setColor )
                                       .def( "click", &WPropertyWrapper::click )
                                       .def( "getName", &WPropertyWrapper::getName )
                                       .def( "getDescription", &WPropertyWrapper::getDescription )
                                       .def( "waitForUpdate", &WPropertyWrapper::waitForUpdate );

    m_pyMainNamespace[ "WPropertyGroup" ] = pb::class_< WPropertyGroupWrapper >( "WPropertyGroup", pb::no_init )
                                            .def( "getProperty", &WPropertyGroupWrapper::getProperty )
                                            .def( "getGroup", &WPropertyGroupWrapper::getGroup )
                                            .def( "getName", &WPropertyGroupWrapper::getName )
                                            .def( "getDescription", &WPropertyGroupWrapper::getDescription );

    m_pyMainNamespace[ "WModuleContainer" ] = pb::class_< WModuleContainerWrapper >( "WModuleContainer", pb::no_init )
                                              .def( "create", &WModuleContainerWrapper::create )
                                              .def( "remove", &WModuleContainerWrapper::remove )
                                              .def( "createDataModule", &WModuleContainerWrapper::createDataModule );

    m_pyMainNamespace[ "WOutputConnector" ] = pb::class_< WOutputConnectorWrapper >( "WOutputConnectorWrapper", pb::no_init )
                                             .def( "disconnect", &WOutputConnectorWrapper::disconnect );

    m_pyMainNamespace[ "WInputConnector" ] = pb::class_< WInputConnectorWrapper >( "WInputConnectorWrapper", pb::no_init )
                                             .def( "connect", &WInputConnectorWrapper::connect )
                                             .def( "disconnect", &WInputConnectorWrapper::disconnect )
                                             .def( "waitForInput", &WInputConnectorWrapper::waitForInput );

    m_pyMainNamespace[ "WModule" ] = pb::class_< WModuleWrapper >( "WModule", pb::no_init )
                                     .def( "getName", &WModuleWrapper::getName )
                                     .def( "getDescription", &WModuleWrapper::getDescription )
                                     .def( "getProperties", &WModuleWrapper::getProperties )
                                     .def( "getInformationProperties", &WModuleWrapper::getInformationProperties )
                                     .def( "getInputConnector", &WModuleWrapper::getInputConnector )
                                     .def( "getOutputConnector", &WModuleWrapper::getOutputConnector );

    // bind the kernel's root container to the "rootContainer" variable in the python namespace
    // this allows access to the modules via this variable
    m_pyMainNamespace[ "rootContainer" ] = &m_rootContainer;

    m_pyMainNamespace[ "WLogger" ]  = pb::class_< WLoggerWrapper >( "WLogger", pb::no_init )
                                     .def( "addFileStream", &WLoggerWrapper::addFileStream )
                                     .def( "removeFileStream", &WLoggerWrapper::removeFileStream )
                                     .def( "removeAllFileStreams", &WLoggerWrapper::removeAllFileStreams )
                                     .def( "error", &WLoggerWrapper::error )
                                     .def( "warning", &WLoggerWrapper::warning )
                                     .def( "info", &WLoggerWrapper::info )
                                     .def( "debug", &WLoggerWrapper::debug );


    m_pyMainNamespace[ "screenshot" ] = pb::make_function( &screenshot );
    m_pyMainNamespace[ "initCamera" ] = pb::make_function( &initCamera );

    m_logger = WLoggerWrapper( WLogger::getLogger() );
    m_pyMainNamespace[ "logger" ] = &m_logger;
}

void WScriptInterpreterPython::setParameters( std::vector< std::string > const& params )
{
    boost::unique_lock< boost::mutex > lock( m_mutex );

    if( params.size() == 0 )
    {
        return;
    }

    m_argc = params.size();
    m_argv = new char*[ params.size() ];

    for( std::size_t k = 0; k < params.size(); ++k )
    {
        m_argv[ k ] = new char[ params[ k ].length() + 1 ];
        std::snprintf( m_argv[ k ], params[ k ].length() + 1, "%s", params[ k ].c_str() );
        m_argv[ k ][ params[ k ].length() ] = '\0';
    }

    PySys_SetArgv( m_argc, m_argv );
}

void WScriptInterpreterPython::execute( std::string const& line )
{
    boost::unique_lock< boost::mutex > lock( m_mutex );

    try
    {
        pb::exec( line.c_str(), m_pyMainNamespace );
    }
    catch( pb::error_already_set const& )
    {
        PyErr_Print();
    }
}

void WScriptInterpreterPython::executeAsync( std::string const& script )
{
    m_scriptThread.addToExecuteQueue( script );
}

void WScriptInterpreterPython::executeFile( std::string const& filename )
{
    // load file content into string
    std::ifstream in( filename.c_str() );
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
        execute( script );
    }
    catch( WException const& e )
    {
        wlog::error( "Walnut" ) << "Error while executing script: " << e.what();
    }
}

void WScriptInterpreterPython::executeFileAsync( std::string const& filename )
{
    // load file content into string
    std::ifstream in( filename.c_str() );
    std::string script;
    std::string line;
    while( std::getline( in, line ) )
    {
        script += line + "\n";
    }
    in.close();

    // execute
    executeAsync( script );
}

std::string const WScriptInterpreterPython::getName() const
{
    return "python";
}

std::string const WScriptInterpreterPython::getExtension() const
{
    return ".py";
}

WScriptInterpreterPython::ScriptThread::ScriptThread( WScriptInterpreterPython& interpreter ) // NOLINT reference
    : WThreadedRunner(),
      m_scriptQueue(),
      m_queueMutex(),
      m_condition( new WCondition() ),
      m_conditionSet(),
      m_interpreter( interpreter )
{
    m_conditionSet.setResetable( true, true );
    m_conditionSet.add( m_condition );
}

WScriptInterpreterPython::ScriptThread::~ScriptThread()
{
}

void WScriptInterpreterPython::ScriptThread::requestStop()
{
    WThreadedRunner::requestStop();
    m_condition->notify();
}

void WScriptInterpreterPython::ScriptThread::threadMain()
{
    while( !m_shutdownFlag )
    {
        m_conditionSet.wait();

        if( m_shutdownFlag )
            break;

        std::size_t numScripts = 0;
        {
            boost::unique_lock< boost::mutex > lock( m_queueMutex );
            numScripts = m_scriptQueue.size();
        }

        while( numScripts > 0 )
        {
            std::string script;

            // only getting the script content must be locked
            {
                boost::unique_lock< boost::mutex > lock( m_queueMutex );
                script = m_scriptQueue.front();
                m_scriptQueue.pop();
            }

            if( script.length() != 0 )
            {
                wlog::info( "WScriptInterpreterPython::ScriptThread" ) << "Executing script asyncronously.";
                // note that this may block if the interpreter is currently executing another script
                m_interpreter.execute( script );
                wlog::info( "WScriptInterpreterPython::ScriptThread" ) << "Done executing script.";
            }
            {
                boost::unique_lock< boost::mutex > lock( m_queueMutex );
                numScripts = m_scriptQueue.size();
            }
        }
    }
}

void WScriptInterpreterPython::ScriptThread::addToExecuteQueue( std::string const& script )
{
    wlog::info( "WScriptInterpreterPython::ScriptThread" ) << "Queueing script for asyncronous execution.";

    boost::unique_lock< boost::mutex > lock( m_queueMutex );
    m_scriptQueue.push( script );
    m_condition->notify();
}

#endif  // PYTHON_FOUND
