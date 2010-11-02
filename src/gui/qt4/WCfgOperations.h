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

/**
* static class that provides string and file operation functions
* to operate on a config file
*/
class WCfgOperations
{
public:
    /**
    * read the whole plain text from a text file
    *
    * \param fileName filename
    *
    * \result line vector of the text file
    */
    static std::vector< std::string > readCfg( const std::string fileName );

    /**
    * write the plain line text to a file
    *
    * \param fileName filename
    * \param lines line vector
    */
    static void writeCfg( const std::string fileName, const std::vector< std::string > lines );

    /**
    * determine if a line is a comment
    *
    * \param line line
    *
    * \result is a comment
    */
    static bool isComment( const std::string line );

    /**
    * determine if a line is an assignment
    *
    * \param line line
    *
    * \result is an assignment
    */
    static bool isAssignment( const std::string line );

    /**
    * remove a comment from a line
    *
    * \param line line
    *
    * \result line without the comment
    */
    static std::string removeComment( const std::string line );

    /**
    * parses a line with an assignment and returns the components
    *
    * \param line line
    * \param left pointer to left part of the assignment
    * \param right pointer to right part of the assignment
    */
    static void getAssignementComponents( const std::string line, std::string *left, std::string *right );

    /**
    * determine if the line is a comment and in the comment is an assignment
    *
    * \param line line
    *
    * \result is a commented assignment
    */
    static bool isCommentedAssignment( const std::string line );

    /**
    * determine if a line is a section
    *
    * \param line line
    *
    * \result is line a section
    */
    static bool isSection( const std::string line );

    /**
    * get the section name of a line
    *
    * \param line line
    *
    * \result section name
    */
    static std::string getSectionName( const std::string line );

    /**
    * return the line as uncommented
    *
    * \param line line
    *
    * \result line with first comment signs removed
    */
    static std::string uncommentLine( const std::string line );

    /**
    * determine if a string is a bool var ("yes", "no")
    *
    * \param line line
    *
    * \result is bool var
    */
    static bool isBool( const std::string line );

    /**
    * get the bool value of the line
    *
    * \param line line
    *
    * \result bool value
    */
    static bool getAsBool( const std::string line );

    /**
    * determine if a line an int
    *
    * \param line line
    *
    * \result is int
    */
    static bool isInt( const std::string line );

    /**
    * get a line as an int
    *
    * \param line line
    *
    * \result as int
    */
    static int getAsInt( const std::string line );

    /**
    * determine if a line is a double
    *
    * \param line line
    *
    * \result is double
    */
    static bool isDouble( const std::string line );

    /**
    * get a line as a double
    *
    * \param line line
    *
    * \result as double
    */
    static double getAsDouble( const std::string line );

    /**
    * determine if a line is a string ( beginning and ending with a quote )
    *
    * \param line line
    * \param useColon consider a colon at the beginning and end of the string
    *
    * \result is string
    */
    static bool isString( const std::string line, bool useColon = false );

    /**
    * get the line as a string
    *
    * \param line line
    * \param useColon consider a colon at the beginning and end of the string
    *
    * \result as string
    */
    static std::string getAsString( const std::string line, bool useColon = false );

    /**
    * convert a property value to a string
    * \note so far only functional with the types bool, int, double and string
    * others will return an empty string
    * doubles are capped to 2 digits after the dot and all zeros after the dot except the
    * one behind the dot are removed, also if there's a '0' before the dot it's removed
    * to have a consistent writing in the config file
    *
    * \param prop property variable
    * \param useColon consider a colon at the beginning and end of the string
    *
    * \result property variable as string
    */
    static std::string getPropValAsString( boost::shared_ptr< WProperties > prop, bool useColon = false );
private:
};

#endif  // WCFGOPERATIONS_H
