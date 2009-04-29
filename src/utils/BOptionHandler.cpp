//---------------------------------------------------------------------------
//
// Project: BrainCognize
//
// Copyright 2009 SomeCopyrightowner
//
// This file is part of BrainCognize.
//
// BrainCognize is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// BrainCognize is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with BrainCognize. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------

#include <iostream>

#include "BOptionHandler.h"

BOptionHandler::BOptionHandler( int argc, char* argv[] )
    : m_argc( argc ),
      m_argv( argv ),
      m_desc( "Allowed options" ),
      m_map()
{
    createOptions();
    parseOptions();
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

int BOptionHandler::takeActions()
{
    if( m_map.count( "help" ) ) {
        std::cout << m_desc << std::endl;
        return 0;
    }

    BMainApplication app;
    return app.runQT( m_argc, m_argv );
}

