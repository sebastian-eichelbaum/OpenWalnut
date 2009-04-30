//---------------------------------------------------------------------------
//
// Project: OpenWalnut
//
// Copyright 2009 SomeCopyrightowner
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

#include "BOptionHandler.h"

BOptionHandler::BOptionHandler( int argc, char* argv[] )
    : m_argc( argc ),
      m_argv( argv ),
      m_errorOccured( false ),
      m_desc( "Allowed options" ),
      m_map()
{
    createOptions();
    try
    {
        parseOptions();
    }
    catch( po::multiple_occurrences error )
    {
        m_errorOccured = true;
        std::cout << "Command-line option error: each option is allowed only once" << std::endl;
    }
    catch( po::unknown_option error )
    {
        m_errorOccured = true;
        std::cout << "Command-line option error: " << error.what() << std::endl;
        std::cout << m_desc << std::endl;
    }
    catch( po::error error )
    {
        m_errorOccured = true;
        std::cout << "Unknown command-line option error: " << error.what() << std::endl;
        std::cout << m_desc << std::endl;
    }
}

void BOptionHandler::createOptions()
{
    // This is not our CodingStandard, but this is a special overloaded operator()
    m_desc.add_options()
        ( "help,h", "prints this help message" );
}

void BOptionHandler::parseOptions()
{
    po::store( po::parse_command_line( m_argc, m_argv, m_desc ), m_map );
}

int BOptionHandler::takeActions() const
{
    if( m_errorOccured )
    {
        return 1;
    }

    if( m_map.count( "help" ) )
    {
        std::cout << m_desc << std::endl;
        return 0;
    }

    BMainApplication app;
    return app.runQT( m_argc, m_argv );
}

