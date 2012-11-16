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

#include <string>
#include <vector>

#include "core/kernel/WKernel.h"

#include "../wrappers/WLoggerWrapper.h"
#include "../wrappers/WModuleWrapper.h"
#include "../wrappers/WPropertyGroupWrapper.h"
#include "../wrappers/WPropertyWrapper.h"

#include "WScriptInterpreterPython.h"

#ifdef PYTHON_FOUND

WScriptInterpreterPython::WScriptInterpreterPython( boost::shared_ptr< WModuleContainer > const& rootContainer )
    : m_rootContainer( rootContainer ),
      m_argc( 0 ),
      m_argv( 0 )
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
}

WScriptInterpreterPython::~WScriptInterpreterPython()
{
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
    // bind WPropertyWrapper class to "WProperty" in the python namespace
    // no constructor in python for now
    m_pyMainNamespace[ "WProperty" ] = pb::class_< WPropertyWrapper >( "WProperty", pb::no_init )
                                       .def( "getBool", &WPropertyWrapper::getBool )
                                       .def( "getInt", &WPropertyWrapper::getInt )
                                       .def( "getString", &WPropertyWrapper::getString )
                                       .def( "getDouble", &WPropertyWrapper::getDouble )
                                       .def( "getFilename", &WPropertyWrapper::getFilename )
                                       .def( "getSelection", &WPropertyWrapper::getSelection )
                                       .def( "setBool", &WPropertyWrapper::setBool )
                                       .def( "setInt", &WPropertyWrapper::setInt )
                                       .def( "setString", &WPropertyWrapper::setString )
                                       .def( "setDouble", &WPropertyWrapper::setDouble )
                                       .def( "setFilename", &WPropertyWrapper::setFilename )
                                       .def( "setSelection", &WPropertyWrapper::setSelection )
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
                                     .def( "removeAllFileStreams", &WLoggerWrapper::removeAllFileStreams );

    m_logger = WLoggerWrapper( WLogger::getLogger() );
    m_pyMainNamespace[ "logger" ] = &m_logger;
}

void WScriptInterpreterPython::setParameters( std::vector< std::string > const& params )
{
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
    try
    {
        pb::exec( line.c_str(), m_pyMainNamespace );
    }
    catch( pb::error_already_set const& )
    {
        PyErr_Print();
    }
}

std::string const WScriptInterpreterPython::getName() const
{
    return "python";
}

std::string const WScriptInterpreterPython::getExtension() const
{
    return ".py";
}

#endif  // PYTHON_FOUND
