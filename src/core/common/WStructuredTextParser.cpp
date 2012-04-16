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
#include <sstream>
#include <algorithm>

#include "exceptions/WParseError.h"
#include "exceptions/WNotFound.h"
#include "WIOTools.h"
#include "WPredicateHelper.h"

#include "WStructuredTextParser.h"

namespace WStructuredTextParser
{
    const std::string StructuredValueTree::Separator = "/";

    StructuredValueTree::StructuredValueTree( const ObjectType& tree )
    {
        // initialize member
        m_tree = tree;
    }

    StructuredValueTree::~StructuredValueTree()
    {
        // cleanup
    }

    bool StructuredValueTree::exists( std::string key ) const
    {
        return count( key );
    }

    size_t StructuredValueTree::count( std::string key ) const
    {
        return getValues< std::string >( key ).size();
    }

    StructuredValueTree parseFromString( std::string input )
    {
        std::ostringstream error;
        WStructuredTextParser::Grammar< std::string::const_iterator > parser( error );

        // parse
        ObjectType ast;
        std::string::const_iterator iter = input.begin();
        std::string::const_iterator end = input.end();
        bool r = phrase_parse( iter, end, parser, boost::spirit::ascii::space, ast );

        // error?
        if( !( r && iter == end ) )
        {
            throw WParseError( "Parse error. Parser message: " + error.str() );
        }

        // done. return
        return StructuredValueTree( ast );
    }

    StructuredValueTree parseFromFile( boost::filesystem::path path )
    {
        // NOTE: do not catch the io exception here.
        std::string input= readFileIntoString( path );

        // instantiate parser
        std::ostringstream error;
        WStructuredTextParser::Grammar< std::string::const_iterator > parser( error );

        // parse
        ObjectType ast;
        std::string::const_iterator iter = input.begin();
        std::string::const_iterator end = input.end();
        bool r = phrase_parse( iter, end, parser, boost::spirit::ascii::space, ast );

        // error?
        if( !( r && iter == end ) )
        {
            throw WParseError( "Parse error. Parser message: " + error.str() );
        }

        // done. return
        return StructuredValueTree( ast );
    }
}

