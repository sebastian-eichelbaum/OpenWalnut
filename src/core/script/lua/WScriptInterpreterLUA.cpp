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

#include "../../common/WLogger.h"

#include "../../kernel/WKernel.h"

#include "../wrappers/WLoggerWrapper.h"
#include "../wrappers/WModuleWrapper.h"
#include "../wrappers/WPropertyGroupWrapper.h"
#include "../wrappers/WPropertyWrapper.h"

#include "WScriptInterpreterLUA.h"

WScriptInterpreterLUA::WScriptInterpreterLUA()
{
    m_lua = lua_open();

    luaL_openlibs( m_lua );
    luabind::open( m_lua );
}

WScriptInterpreterLUA::~WScriptInterpreterLUA()
{
    lua_close( m_lua );
}

void WScriptInterpreterLUA::initBindings()
{
    luabind::module( m_lua )[ luabind::class_< WPropertyWrapper >( "WProperty" )
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
                              .def( "waitForUpdate", &WPropertyWrapper::waitForUpdate ) ];

    luabind::module( m_lua )[ luabind::class_< WPropertyGroupWrapper >( "WPropertyGroup" )
                              .def( "getProperty", &WPropertyGroupWrapper::getProperty )
                              .def( "getGroup", &WPropertyGroupWrapper::getGroup )
                              .def( "getName", &WPropertyGroupWrapper::getName )
                              .def( "getDescription", &WPropertyGroupWrapper::getDescription ) ];


    luabind::module( m_lua )[ luabind::class_< WModuleContainerWrapper >( "WModuleContainer" )
                              .def( "create", &WModuleContainerWrapper::create )
                              .def( "remove", &WModuleContainerWrapper::remove )
                              .def( "createDataModule", &WModuleContainerWrapper::createDataModule ) ];

    luabind::module( m_lua )[ luabind::class_< WOutputConnectorWrapper >( "WOutputConnector" )
                              .def( "disconnect", &WOutputConnectorWrapper::disconnect ) ];

    luabind::module( m_lua )[ luabind::class_< WInputConnectorWrapper >( "WInputConnector" )
                             .def( "connect", &WInputConnectorWrapper::connect )
                             .def( "disconnect", &WInputConnectorWrapper::disconnect )
                             .def( "waitForInput", &WInputConnectorWrapper::waitForInput ) ];

    luabind::module( m_lua )[ luabind::class_< WModuleWrapper >( "WModuleWrapper" )
                              .def( "getName", &WModuleWrapper::getName )
                              .def( "getDescription", &WModuleWrapper::getDescription )
                              .def( "getProperties", &WModuleWrapper::getProperties )
                              .def( "getInformationProperties", &WModuleWrapper::getInformationProperties )
                              .def( "getInputConnector", &WModuleWrapper::getInputConnector )
                              .def( "getOutputConnector", &WModuleWrapper::getOutputConnector ) ];

    // bind the kernel's root container to the "rootContainer" global in lua
    // this allows access to the modules via this global
    m_rootContainer = WModuleContainerWrapper( WKernel::getRunningKernel()->getRootContainer() );
    luabind::globals( m_lua )[ "rootContainer" ] = &m_rootContainer;

    luabind::module( m_lua )[ luabind::class_< WLoggerWrapper >( "WLogger" )
                              .def( "addFileStream", &WLoggerWrapper::addFileStream )
                              .def( "removeFileStream", &WLoggerWrapper::removeFileStream )
                              .def( "removeAllFileStreams", &WLoggerWrapper::removeAllFileStreams ) ];

    m_logger = WLoggerWrapper( WLogger::getLogger() );
    luabind::globals( m_lua )[ "logger" ] = &m_logger;
}

void WScriptInterpreterLUA::execute( std::string const& p_code )
{
    std::string code( p_code );
    // look for shebang
    size_t shebangIndex = code.rfind( "#!" );
    if ( shebangIndex == 0 && code.size() != 0 )
    {
        // comment out shebang line
        code[0] = '-';
        code[1] = '-';
    }
    if( luaL_dostring( m_lua, code.c_str() ) != 0 )
    {
        WLogger::getLogger()->addLogMessage( lua_tostring( m_lua, -1 ), "WScriptInterpreterLUA", LL_ERROR );
    }
}
