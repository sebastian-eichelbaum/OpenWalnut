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

#ifndef WSTRUCTUREDTEXTPARSER_H
#define WSTRUCTUREDTEXTPARSER_H

#include <algorithm>
#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <vector>

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/filesystem/path.hpp>

#include "WStringUtils.h"
#include "exceptions/WTypeMismatch.h"
#include "exceptions/WNotFound.h"

/**
 * This namespace contains the WStructuredTextParser data types and the parser. It builds up the AST for the given input
 * which later can be traversed.
 */
namespace WStructuredTextParser
{
    //! we use these quite often, so define some short alias for them:
    namespace qi = boost::spirit::qi;

    //! we use these quite often, so define some short alias for them:
    namespace fusion = boost::fusion;

    //! we use these quite often, so define some short alias for them:
    namespace ascii = boost::spirit::ascii;

    //! we use these quite often, so define some short alias for them:
    namespace phoenix = boost::phoenix;

    //! we use these quite often, so define some short alias for them:
    namespace spirit = boost::spirit;

    /**
     * The type used for keys
     */
    typedef std::string KeyType;

    /**
     * The type used for values
     */
    typedef std::string ValueType;

    /**
     * Forward declare the object type.
     */
    struct ObjectType;

    /**
     * KeyValueType - a tuple containing name and value
     */
    struct KeyValueType
    {
        /**
         * Name string.
         */
        std::string m_name;
        /**
         * Value string.
         */
        std::string m_value;
    };

    /**
     * A node inside the AST is either another object or a key-value pair.
     */
    typedef
        boost::variant<
            boost::recursive_wrapper< ObjectType >,
            KeyValueType
        >
    MemberType;

    /**
     * An object is always a name and contains several further nodes
     */
    struct ObjectType
    {
        /**
         * Name of the object
         */
        std::string m_name;

        /**
         * Object's members
         */
        std::vector< MemberType > m_nodes;
    };
}


// Doxygen has problems with the following
// \cond Suppress_Doxygen
/**
 * Tell boost::fusion about our types.
 */
BOOST_FUSION_ADAPT_STRUCT(
    WStructuredTextParser::ObjectType,
    ( std::string, m_name )
    ( std::vector< WStructuredTextParser::MemberType >, m_nodes )
    )

/**
 * Tell boost::fusion about our types.
 */
BOOST_FUSION_ADAPT_STRUCT(
    WStructuredTextParser::KeyValueType,
    ( std::string, m_name )
    ( std::string, m_value )
    )
// \endcond

namespace WStructuredTextParser
{
    /**
     * The grammar describing the structured format. It uses the boost::spirit features to parse the input. There are several rules to comply to
     * successfully parse a file:
     *  Key: identifier, needs to be a-z,A-Z,0-9,_
     *  Object: defined as key + { ... }
     *  Key-Value Pair: is a member of an object and is defines as key="value".
     *
     * \tparam Iterator the iterator, used to get the input stream
     */
    template <typename Iterator>
    struct Grammar: qi::grammar<Iterator, ObjectType(), ascii::space_type >
    {
        /**
         * Constructor and grammar description. It contains the EBNF (Extended Backus Naur Form) of the format we can parse.
         *
         * \param error Will contain error message if any occurs during functions execution
         */
        explicit Grammar( std::ostream& error ): Grammar::base_type( object, "WStructuredTextParser::Grammar" ) // NOLINT - non-const ref
        {
            // a key begins with a letter
            key    %=  qi::char_( "a-zA-Z_" ) >> *qi::char_( "a-zA-Z_0-9" );
            // a value is a quoted string. Multi-line strings possible
            value  %=  qi::lexeme[ '"' >> +( qi::char_ - '"' ) >> '"' ];
            // a pair is: key = value
            kvpair %=  key >> '=' >> value >> ';';
            // a object is a name, and a set of nested objects or key-value pairs
            object %=  key >> '{' >> *( object | kvpair ) >> '}';

            // provide names for these objects for better readability of parse errors
            object.name( "object" );
            kvpair.name( "key-value pair" );
            key.name( "key" );
            value.name( "value" );

            // provide error handlers
            // XXX: can someone tell me how to get them work? According to the boost::spirit doc, this is everything needed but it doesn't work.
            qi::on_error< qi::fail >( object, error << phoenix::val( "Error: " ) << qi::_4 );
            qi::on_error< qi::fail >( kvpair, error << phoenix::val( "Error: " ) << qi::_4 );
            qi::on_error< qi::fail >( key,    error << phoenix::val( "Error: " ) << qi::_4 );
            qi::on_error< qi::fail >( value,  error << phoenix::val( "Error: " ) << qi::_4 );
       }

        // Rules we use

        /**
         * Rule for objects. Attribute is ObjectType and is the start rule of the grammar. See constructor for exact definition.
         */
        qi::rule< Iterator, ObjectType(), ascii::space_type > object;

        /**
         * Key-value pair rule. See constructor for exact definition.
         */
        qi::rule< Iterator, KeyValueType(), ascii::space_type > kvpair;

        /**
         * Key rule. See constructor for exact definition.
         */
        qi::rule< Iterator, KeyType() > key;

        /**
         * Value rule. See constructor for exact definition.
         */
        qi::rule< Iterator, ValueType(), ascii::space_type > value;
    };

    /**
     * This simplifies working with a tree in a WStructuredTextParser::ObjectType instance. It provides easy query and check methods. It does not
     * provide any semantic options. So check validity of the contents and structure of the tree is the job of the using class/derived class. As
     * the tree does not know anything about the semantics of your structure, it is also untyped. For every key you query, you need to specify
     * the type.
     *
     * This tree uses the types in the \ref WStructuredTextParser namespace. To avoid unnecessary copy operations, this class is not recursive
     * itself. When querying, you always need to specify the full path. This class can be seen as accessor to the \ref ObjectType tree.
     *
     * \note The syntax of the parsed files is defined by the parser itself. See \ref Grammar for details.
     */
    class StructuredValueTree
    {
    public:
        /**
         * This char is used as separator for identifying values in the tree.
         */
        static const std::string Separator;

        /**
         * Construct the instance given the original parsing structure.
         *
         * \param tree the parsing result structure (the root node).
         */
        explicit StructuredValueTree( const ObjectType& tree );

        /**
         * Cleanup.
         */
        virtual ~StructuredValueTree();

        /**
         * Checks whether the given value or object exists.
         *
         * \param key path to the value
         *
         * \return true if existing.
         */
        bool exists( std::string key ) const;

        /**
         * It is possible that there are multiple values matching a key. This method counts them.
         *
         * \param key path to the values to count
         *
         * \return the number of found values.
         */
        size_t count( std::string key ) const;

        /**
         * Queries the value with the given name. If it is not found, the default value will be returned.
         *
         * \param key path to the value
         * \param defaultValue the default if no value was found
         * \tparam T the return type. This method tries to cast to this type. If it fails, an exception is thrown. Type std::string is always valid.
         *
         * \throw WTypeMismatch if the value cannot be cast to the specified target type
         *
         * \return the value
         *
         * \note this does not return a reference as the default value might be returned. It returns a copy of the value.
         */
        template< typename T >
        T getValue( std::string key, const T& defaultValue ) const;

        /**
         * Queries the list of values matching the given path. If it is not found, the default value will be returned.
         *
         * \param key path to the value
         * \param defaults the defaults if no value was found
         * \tparam T the return type. This method tries to cast to this type. If it fails, an exception is thrown. Type std::string is always valid.
         *
         * \throw WTypeMismatch if the value cannot be cast to the specified target type
         *
         * \return the value
         *
         * \note this does not return a reference as the default value might be returned. It returns a copy of the value.
         */
        template< typename T >
        std::vector< T > getValues( std::string key, const std::vector< T >& defaults ) const;

        /**
         * Queries the list of values matching the given path. If it is not found, an empty results vector is returned.
         *
         * \param key path to the value
         * \tparam T the return type. This method tries to cast to this type. If it fails, an exception is thrown. Type std::string is always valid.
         *
         * \throw WTypeMismatch if the value cannot be cast to the specified target type
         *
         * \return the value vector. Might be empty if no elements where found.
         *
         * \note this does not return a reference as the default value might be returned. It returns a copy of the value.
         */
        template< typename T >
        std::vector< T > getValues( std::string key ) const;

        /**
         * Queries the value with the given name. If it is not found, an exception is thrown. If multiple entries with this path exist, the first
         * one is returned. Use \ref getValues in this case. Query the count of a key:value pair using \ref count
         *
         * \param key path to the value
         * \tparam T the return type. This method tries to cast to this type. If it fails, an exception is thrown. Type std::string is always valid.
         * \throw WTypeMismatch if the value cannot be cast to the specified target type
         * \throw WNotFound if the key:value pair does not exist
         *
         * \return the value as copy to avoid any const_cast which would allow modification.
         */
        template< typename T >
        T operator[]( std::string key ) const;

    protected:
    private:
        /**
         * The named values.
         */
        MemberType m_tree;

        /**
         * Recursively fills a result vector using a given path iterator. It checks whether the current element matches the current key. If yes,
         * it traverses or adds the value to the result vector. This uses depth-first search and allows multiple matches for one key.
         *
         * \tparam T the return type. This method tries to cast to this type. If it fails, an exception is thrown. Type std::string is always valid.
         * \throw WTypeMismatch if the value cannot be cast to the specified target type
         *
         * \param current current element to check and recursively traverse
         * \param keyIter the current path element
         * \param keyEnd the end iter. Just used to stop iteration if the key as not further elements
         * \param result the result vector gets filled with the result values in correct type.
         */
        template< typename T >
        void traverse( MemberType current, std::vector< std::string >::const_iterator keyIter,
                                           std::vector< std::string >::const_iterator keyEnd,
                                           std::vector< T >& result ) const;
    };

    /**
     * Parse the given input and return the syntax tree. Throws an exception WParseError on error.
     *
     * \param input the input to parse.
     *
     * \return the syntax tree.
     *
     * \throw WParseError on parse error
     */
    StructuredValueTree parseFromString( std::string input );

    /**
     * Parse the given input and return the syntax tree. Throws an exception WParseError on error.
     *
     * \param path the file to parse
     *
     * \return the syntax tree.
     *
     * \throw WParseError on parse error
     * \throw WFileNotFOund in case the specified file could not be opened
     */
    StructuredValueTree parseFromFile( boost::filesystem::path path );

    template< typename T >
    T StructuredValueTree::getValue( std::string key, const T& defaultValue ) const
    {
        // NOTE: getValues ensures that always something is returned (the default value). So the returned vector has a valid begin iterator
        return *getValues< T >( key, std::vector< T >( 1, defaultValue ) ).begin();
    }

    template< typename T >
    std::vector< T > StructuredValueTree::getValues( std::string key, const std::vector< T >& defaults ) const
    {
        std::vector< T > r = getValues< T >( key );
        if( r.size() )
        {
            return r;
        }
        else
        {
            return defaults;
        }
    }

    template< typename T >
    T StructuredValueTree::operator[]( std::string key ) const
    {
        std::vector< T > r = getValues< T >( key );
        if( r.size() )
        {
            return *r.begin();
        }
        else
        {
            throw WNotFound( "The key \"" + key + "\" was not found." );
        }
    }

    /**
     * Visitor to identify whether the given variant of type \ref MemberType is a object or key-value pair.
     */
    class IsLeafVisitor: public boost::static_visitor< bool >
    {
    public:
        /**
         * Returns always true as it is only called for key-value pairs.
         *
         * \return always true since it identified an key-value pair
         */
        bool operator()( const KeyValueType& /* element */ ) const
        {
            return true;
        }

        /**
         * Returns always false as it is only called for objects.
         *
         * \return always false since it identified an Object.
         */
        bool operator()( const ObjectType& /* element */ ) const
        {
            return false;
        }
    };

    /**
     * Visitor to query the m_name member of \ref ObjectType and \ref KeyValueType.
     */
    class NameQueryVisitor: public boost::static_visitor< std::string >
    {
    public:
        /**
         * Returns the m_name member of the specified object or key-valuev pair.
         *
         * \param element Specified object.
         *
         * \tparam T one of the types of the \ref MemberType variant
         * \return always true since it identified an key-value pair
         */
        template< typename T >
        std::string operator()( const T& element ) const
        {
            return element.m_name;
        }
    };

    template< typename T >
    std::vector< T > StructuredValueTree::getValues( std::string key ) const
    {
        MemberType current = m_tree;

        // split up the key
        std::vector< std::string > keySplit = string_utils::tokenize( key, Separator, false );
        // empty key -> return empty result list
        if( !keySplit.size() )
        {
            return std::vector< T >();
        }

        // traverse the tree and construct the result vector
        std::vector< T > r;
        traverse( m_tree, keySplit.begin(), keySplit.end(), r );
        return r;
    }

    template< typename T >
    void StructuredValueTree::traverse( MemberType current, std::vector< std::string >::const_iterator keyIter,
                                                            std::vector< std::string >::const_iterator keyEnd,
                                                            std::vector< T >& result ) const
    {
        std::string elementName = boost::apply_visitor( NameQueryVisitor(), current );
        bool elementIsLeaf = boost::apply_visitor( IsLeafVisitor(), current );

        // does the current node match the current name?
        if( elementName == *keyIter )
        {
            // only if the key path continues AND the current element is no lead, traverse
            if( !elementIsLeaf && ( ( keyIter + 1 ) != keyEnd) )
            {
                ObjectType elementAsObj = boost::get< ObjectType >( current );
                for( std::vector< MemberType >::const_iterator nodeIter = elementAsObj.m_nodes.begin();
                    nodeIter != elementAsObj.m_nodes.end();
                    ++nodeIter )
                {
                    traverse( *nodeIter, keyIter + 1, keyEnd, result );
                }
            }
            else if( elementIsLeaf && ( ( keyIter + 1 ) == keyEnd ) )
            {
                // if this element is a leaf and the path also ends here (names match already checked) -> add value to result vector
                KeyValueType elementAsKV = boost::get< KeyValueType >( current );

                // try to cast to the desired type and add to vector
                try
                {
                    result.push_back( string_utils::fromString< T >( elementAsKV.m_value ) );
                }
                catch( const std::exception& e )
                {
                    // convert the standard exception (if cannot convert) to a WTypeMismnatch.
                    throw WTypeMismatch( "Cannot convert element \"" + *keyIter + "\" to desired type." );
                }
            }
            // all the remaining cases are invalid and cause traversion to stop
        }
        // done
    }
}

#endif  // WSTRUCTUREDTEXTPARSER_H

