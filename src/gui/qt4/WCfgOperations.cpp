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

#include <Qt/qstring.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "../../common/WStringUtils.h"

#include "WCfgOperations.h"

std::vector< std::string > WCfgOperations::readCfg( const std::string fileName )
{
    std::ifstream ifs( fileName.c_str(), std::ifstream::in );

    std::vector< std::string > lines;

    std::string line;

    while ( !ifs.eof() )
    {
        getline( ifs, line );

        lines.push_back( std::string( line ) );
    }

    ifs.close();

    return lines;
}

void WCfgOperations::writeCfg( const std::string fileName, const std::vector< std::string > lines )
{
    std::ofstream ofs( fileName.c_str(), std::ofstream::out );

    std::stringstream mendl;
    mendl << std::endl;

    for ( size_t i = 0; i < lines.size(); ++i )
    {
        ofs << lines[i];

        // if there is no newline character add it
        if ( (lines[i]).find( mendl.str() ) == std::string::npos )
        {
            ofs << std::endl;
        }
    }

    ofs.close();
}

bool WCfgOperations::isComment( const std::string line )
{
    if ( line.length() == 0 )
    {
        return false;
    }

    return string_utils::lTrim( line )[0] == '#';
}

bool WCfgOperations::isAssignment( const std::string line )
{
    if ( line.length() == 0 )
    {
        return false;
    }
    // a comment is never an Assignment
    if ( isComment( line ) )
    {
        return false;
    }
    // no equals sign means no assignment
    if ( line.find( '=' ) == std::string::npos )
    {
        return false;
    }
    // left and right from equals there has to be at least one none whitespace char
    bool hasLeftChar = false;
    bool hasRightChar = false;

    std::string toTest = removeComment( line );
    size_t p = toTest.find( '=' );
    size_t i;
    for ( i = 0; i < p - 1; ++i )
    {
        if ( toTest[i] != ' ' )
        {
            hasLeftChar = true;
        }
    }
    for ( i = p + 1; i < toTest.length(); ++i )
    {
        if ( toTest[i] != ' ' )
        {
            hasRightChar = true;
        }
    }

    return hasLeftChar && hasRightChar;
}

std::string WCfgOperations::removeComment( const std::string line )
{
    std::string lineWithoutComment = line;

    size_t p = lineWithoutComment.find( '#' );

    if ( p == std::string::npos )
    {
        return lineWithoutComment;
    }
    lineWithoutComment.erase( p, lineWithoutComment.length() - p );

    return lineWithoutComment;
}

void WCfgOperations::getAssignementComponents( const std::string line, std::string *left, std::string *right )
{
    if ( !isAssignment( line ) )
    {
        return;
    }
    size_t p = line.find( '=' );
    ( *left ) = removeComment( line );
    left->erase( p, left->length() - p );
    ( *left ) = string_utils::trim( *left );

    ( *right ) = removeComment( line );
    right->erase( 0, p + 1 );
    ( *right ) = string_utils::trim( *right );
}

bool WCfgOperations::isCommentedAssignment( const std::string line )
{
    std::string toTest = line;

    if ( !isComment( line ) )
    {
        return false;
    }

    while ( isComment( toTest ) )
    {
        toTest = string_utils::lTrim( toTest, "#" );
    }

    return isAssignment( toTest );
}

bool WCfgOperations::isSection( const std::string line )
{
    bool res = false;
    size_t leftB = line.find( '[' );
    size_t rightB = line.find( ']' );
    size_t comment = line.find( '#' );
    if ( leftB != std::string::npos && rightB != std::string::npos && leftB < rightB && ( comment == std::string::npos || rightB < comment ) )
    {
        res = true;
    }
    return res;
}

std::string WCfgOperations::getSectionName( const std::string line )
{
    std::string res = line;
    if ( isSection(res) )
    {
        size_t leftB = res.find( '[' );
        res.erase( 0, leftB + 1 );
        size_t rightB = res.find( ']' );
        res.erase( rightB, res.length() - rightB );
    }
    else
    {
        res = std::string( "" );
    }
    return res;
}

std::string WCfgOperations::uncommentLine( const std::string line )
{
    std::string res = line;

    if ( !isComment( res ) )
    {
        return res;
    }

    while ( res.length() > 0 && ( res[0] == '#' || res[0] == '\t' || res[0] == ' ' ) )
    {
        res.erase( 0, 1 );
    }
    return res;
}

bool WCfgOperations::isBool( const std::string line )
{
    bool res = false;
    if ( line == std::string( "yes" ) || line == std::string( "no" ) )
    {
        res = true;
    }
    return res;
}

bool WCfgOperations::getAsBool( const std::string line )
{
    bool res = false;
    if ( line == std::string( "yes" ) )
    {
        res = true;
    }
    return res;
}

bool WCfgOperations::isInt( const std::string line )
{
    bool res = false;
    QString l2 = QString::fromStdString( line );
    l2.toInt( &res );
    return res;
}

int WCfgOperations::getAsInt( const std::string line )
{
    bool ok = false;
    QString l2 = QString::fromStdString( line );
    int res = l2.toInt( &ok );
    if ( !ok )
    {
        res = 0;
    }
    return res;
}

bool WCfgOperations::isDouble( const std::string line )
{
    bool res = false;
    QString l2 = QString::fromStdString( line );
    l2.toDouble( &res );
    return res;
}

double WCfgOperations::getAsDouble( const std::string line )
{
    bool ok = false;
    QString l2 = QString::fromStdString( line );
    double res = l2.toDouble( &ok );
    if ( !ok )
    {
        res = 0.0;
    }
    return res;
}

bool WCfgOperations::isString( const std::string line )
{
    bool res = false;
    if ( line.length() > 1 && line[0] == '\"' && line[line.length() - 1] == '\"' )
    {
        res = true;
    }
    return res;
}

std::string WCfgOperations::getAsString( const std::string line )
{
    std::string res = line;
    if ( !isString( line ) )
    {
        return std::string( "" );
    }
    res.erase( 0, 1 );
    res.erase( res.length() - 1, 1 );
    return res;
}

std::string WCfgOperations::getPropValAsString( boost::shared_ptr< WProperties > prop )
{
    std::string result = "";

    switch ( prop->getType() )
    {
    case PV_STRING:
        {
            result = "\"" + prop->toPropString()->get() + "\"";
            break;
        }
    case PV_BOOL:
        {
            if ( prop->toPropBool()->get() == true )
            {
                result = "yes";
            }
            else
            {
                result = "no";
            }
            break;
        }
    case PV_DOUBLE:
        {
            result = boost::lexical_cast< std::string >( prop->toPropDouble()->get() );
            // make sure to only have a max of 2 digits after the dot, and if there is only a 0 before the
            // dot to delete that
            if ( result[0] == '0' && result.find( '.' ) == 1 )
            {
                result.erase( 0, 1 );
            }
            size_t digitsAfterDot;
            size_t dotPos = result.find( '.' );
            if ( dotPos != std::string::npos )
            {
                digitsAfterDot = result.length() - dotPos + 1;
                if ( digitsAfterDot > 2 )
                {
                    result.erase( dotPos + 3, result.length() - dotPos - 2 );
                }
            }
            // also remove the last zeros if they are not directly after the dot
            if ( result.length() > 2 ) // at least a dot and and digit
            {
                while ( result[ result.length() - 1 ] == '0' )
                {
                    dotPos = result.find( '.' );
                    if ( dotPos + 2 < result.length() )
                    {
                        result.erase( result.length() - 1, 1 );
                    }
                    else
                    {
                        break;
                    }
                }
            }
            break;
        }
    case PV_INT:
        {
            result = boost::lexical_cast< std::string >( prop->toPropInt()->get() );
            break;
        }
    default:
        {
            result = "";
            break;
        }
    }

    return result;
}
