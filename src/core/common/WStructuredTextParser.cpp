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

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

#include "exceptions/WParseError.h"
#include "exceptions/WNotFound.h"
#include "WIOTools.h"
#include "WPredicateHelper.h"

#include "WStructuredTextParser.h"

namespace WStructuredTextParser
{
    const std::string StructuredValueTree::Separator = "/";

    StructuredValueTree::StructuredValueTree( const FileType& file ):
        m_file( file )
    {
        // initialize member
    }

    StructuredValueTree::StructuredValueTree()
    {
        // do nothing.
    }

    StructuredValueTree::StructuredValueTree( const std::string& toParse ):
        m_file( parseFromString( toParse ) )
    {
        // initialize
    }

    StructuredValueTree::StructuredValueTree( const boost::filesystem::path& file ):
        m_file( parseFromFile( file ) )
    {
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
        traverse( m_file, key, rObj, rKV );

        if( valuesOnly )
        {
            return rKV.size();
        }
        else
        {
            return rKV.size() + rObj.size();
        }
    }

    StructuredValueTree StructuredValueTree::getSubTree( std::string key ) const
    {
        std::vector< StructuredValueTree > r = getSubTrees( key );

        // return first match if any
        if( r.size() )
        {
            return *r.begin();
        }
        else
        {
            return StructuredValueTree();
        }
    }

    std::vector< StructuredValueTree > StructuredValueTree::getSubTrees( std::string key ) const
    {
        std::vector< ObjectType > rObj;
        std::vector< KeyValueType > rKV;
        std::vector< StructuredValueTree > r;

        // traverse
        traverse( m_file, key, rObj, rKV );

        // now we transform each found object int a MemberType (boost variant).
        for( std::vector< ObjectType >::const_iterator objects = rObj.begin(); objects != rObj.end(); ++objects )
        {
            // create a new StructuredValueTree instance
            r.push_back( StructuredValueTree( ( *objects ).m_nodes ) );
        }

        return r;
    }

    void StructuredValueTree::traverse( FileType current, std::string key,
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
        for( FileType::const_iterator i = current.begin(); i != current.end(); ++i )
        {
            traverse( *i, keySplit.begin(), keySplit.end(), resultObjects, resultValues );
        }
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

    FileType parseFromString( std::string input )
    {
        std::ostringstream error;
        WStructuredTextParser::Grammar< std::string::const_iterator > parser( error );

        // parse
        FileType ast;
        std::string::const_iterator iter = input.begin();
        std::string::const_iterator end = input.end();
        bool r = phrase_parse( iter, end, parser, boost::spirit::ascii::space, ast );

        // error?
        if( !( r && iter == end ) )
        {
            throw WParseError( "Parse error. Parser message: " + error.str() );
        }

        // done. return
        return ast;
    }

    FileType parseFromFile( boost::filesystem::path path )
    {
        // NOTE: do not catch the io exception here.
        std::string input= readFileIntoString( path );

        // instantiate parser
        std::ostringstream error;
        WStructuredTextParser::Grammar< std::string::const_iterator > parser( error );

        // parse
        FileType ast;
        std::string::const_iterator iter = input.begin();
        std::string::const_iterator end = input.end();
        bool r = phrase_parse( iter, end, parser, boost::spirit::ascii::space, ast );

        // error?
        if( !( r && iter == end ) )
        {
            throw WParseError( "Parse error. Parser message: " + error.str() );
        }

        // done. return
        return ast;
    }
}
