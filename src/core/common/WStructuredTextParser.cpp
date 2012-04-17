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

#include <iostream>
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

    bool StructuredValueTree::exists( std::string key, bool valuesOnly ) const
    {
        return count( key, valuesOnly );
    }

    size_t StructuredValueTree::count( std::string key, bool valuesOnly ) const
    {
        std::vector< ObjectType > rObj;
        std::vector< KeyValueType > rKV;

        // traverse
        traverse( m_tree, key, rObj, rKV );

        if( valuesOnly )
        {
            return rKV.size();
        }
        else
        {
            return rKV.size() + rObj.size();
        }
    }

    void StructuredValueTree::traverse( MemberType current, std::string key,
                                                            std::vector< ObjectType >& resultObjects,
                                                            std::vector< KeyValueType >& resultValues ) const
    {
        // split up the key
        std::vector< std::string > keySplit = string_utils::tokenize( key, Separator, false );
        // empty key -> return empty result list
        if( !keySplit.size() )
        {
            return;
        }

        // traverse
        traverse( current, keySplit.begin(), keySplit.end(), resultObjects, resultValues );
    }

    void StructuredValueTree::traverse( MemberType current, std::vector< std::string >::const_iterator keyIter,
                                                            std::vector< std::string >::const_iterator keyEnd,
                                                            std::vector< ObjectType >& resultObjects,
                                                            std::vector< KeyValueType >& resultValues ) const
    {
        // get some properties of the current entry:
        std::string elementName = boost::apply_visitor( NameQueryVisitor(), current );
        bool elementIsKeyValuePair = boost::apply_visitor( IsLeafVisitor(), current );
        bool elementIsComment = boost::apply_visitor( IsCommentVisitor(), current );

        // comments will be ignored.
        // NOTE: we store comments in the original data structure to allow them to be written again to the file. This can be useful if OW loads a
        // file (edited by the user) and re-writes this file. -> we are able to keep the comments
        if( elementIsComment )
        {
            return;
        }

        // does the current node match the current name?
        if( elementName == *keyIter )
        {
            // only if the key path continues AND the current element is no leaf, traverse
            if( !elementIsKeyValuePair && ( ( keyIter + 1 ) != keyEnd) )
            {
                ObjectType elementAsObj = boost::get< ObjectType >( current );
                for( std::vector< MemberType >::const_iterator nodeIter = elementAsObj.m_nodes.begin();
                    nodeIter != elementAsObj.m_nodes.end();
                    ++nodeIter )
                {
                    traverse( *nodeIter, keyIter + 1, keyEnd, resultObjects, resultValues );
                }
            }
            else if( ( keyIter + 1 ) == keyEnd )
            {
                // we now have reached the end of the path.
                if( elementIsKeyValuePair )
                {
                    // the current element is a key-value pair -> add to result vector
                    resultValues.push_back( boost::get< KeyValueType >( current ) );
                }
                else
                {
                    // the element is a object -> add to result vector
                    resultObjects.push_back( boost::get< ObjectType >( current ) );
                }
            }
            // all the remaining cases are invalid and cause traversion to stop
        }
        // done
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

