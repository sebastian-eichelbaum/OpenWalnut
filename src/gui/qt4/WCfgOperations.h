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

#ifndef WCFGOPERATIONS_H
#define WCFGOPERATIONS_H

#include <string>
#include <vector>

#include "../../common/WProperties.h"

class WCfgOperations
{
public:
    static std::vector< std::string > readCfg( const std::string fileName );

    static void writeCfg( const std::string fileName, const std::vector< std::string > lines );

    // determines if a line is a comment
    static bool isComment( const std::string line );

    static bool isAssignment( const std::string line );

    static std::string removeComment( const std::string line );

    static void getAssignementComponents( const std::string line, std::string *left, std::string *right );

    static bool isCommentedAssignment( const std::string line );

    static bool isSection( const std::string line );

    static std::string getSectionName( const std::string line );

    static std::string uncommentLine( const std::string line );

    static bool isBool( const std::string line );

    static bool getAsBool( const std::string line );

    static bool isInt( const std::string line );

    static int getAsInt( const std::string line );

    static bool isDouble( const std::string line );

    static double getAsDouble( const std::string line );

    static bool isString( const std::string line );

    static std::string getAsString( const std::string line );

    static std::string getPropValAsString( boost::shared_ptr< WProperties > prop );
private:
};

#endif  // WCFGOPERATIONS_H
