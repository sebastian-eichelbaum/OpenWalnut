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
        //return SyntaxTree( ast );
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
        //return SyntaxTree( ast );
    }


    StructuredValueTree::StructuredValueTree( const std::string& name ):
        m_name( name )
    {
        // initialize member
    }

    StructuredValueTree::~StructuredValueTree()
    {
        // cleanup
    }

    std::string StructuredValueTree::getName() const
    {
        return m_name;
    }

    std::string StructuredValueTree::operator()( std::string name, std::string defaultValue ) const
    {
        if( !exists( name ) )
        {
            return defaultValue;
        }
        return operator()( name );
    }

    const std::string& StructuredValueTree::operator()( std::string name ) const
    {
        return const_cast< StructuredValueTree& >( *this ).operator()( name );
    }

    const StructuredValueTree& StructuredValueTree::operator[]( std::string name ) const
    {
        return const_cast< StructuredValueTree& >( *this ).operator[]( name );
    }

    const StructuredValueTree* StructuredValueTree::queryTree( const std::string& name ) const
    {
        return const_cast< StructuredValueTree* >( this )->queryTree( name );
    }

    StructuredValueTree& StructuredValueTree::operator[]( std::string name )
    {
        // find
        /*ChildrenType::iterator elem = std::find_if( m_children.begin(), m_children.end(),
                                                    WPredicateHelper::Name< StructuredValueTree >( name ) );
        if( elem == m_children.end() )
        {
            throw WNotFound( "The object with the name \"" + name + "\" was not found." );
        }*/
    }

    bool StructuredValueTree::exists( std::string name ) const
    {
        /*ChildrenType::const_iterator elem = std::find_if( m_children.begin(), m_children.end(),
                                                          WPredicateHelper::Name< StructuredValueTree >( name ) );
        bool entryExists = ( m_entries.count( name ) != 0 );

        return !entryExists && ( elem == m_children.end() );*/
        return false;
    }

    std::string& StructuredValueTree::operator()( std::string name )
    {
        /*bool entryExists = ( m_entries.count( name ) != 0 );
        if( !entryExists )
        {
            throw WNotFound( "The object with the name \"" + name + "\" was not found." );
        }
        return m_entries[ name ];*/
    }

    StructuredValueTree* StructuredValueTree::queryTree( const std::string& name )
    {
        return NULL;
    }
}

