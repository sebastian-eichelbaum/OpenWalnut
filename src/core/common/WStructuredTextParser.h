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

#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include <map>

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

namespace WStructuredTextParser
{
    /**
     * The grammar describing the structured format. It uses the boost::spirit features to parse the input.
     *
     * \tparam Iterator the iterator, used to get the input stream
     */
    template <typename Iterator>
    struct Grammar: qi::grammar<Iterator, ObjectType(), ascii::space_type >
    {
        /**
         * Constructor and grammar description. It contains the EBNF (Extended Backus Naur Form) of the format we can parse.
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
     * This simplifies working with a tree in a WStructuredTextParser::ObjectType instance.
     */
    class StructuredValueTree
    {
    public:
        /**
         * Construct a tree by giving a name.
         *
         * \param name the name
         */
        explicit StructuredValueTree( const std::string& name );

        /**
         * Cleanup.
         */
        virtual ~StructuredValueTree();

        /**
         * Returns the name of this object.
         *
         * \return the name
         */
        std::string getName() const;

        /**
         * Gives the object with the give name. Is not recursive. If the given entity is not found, a exception is thrown.
         *
         * \param name the key name.
         *
         * \return the object.
         */
        const StructuredValueTree& operator[]( std::string name ) const;

        /**
         * Gives the object with the give name. Is not recursive. If the given entity is not found, a exception is thrown.
         *
         * \param name the key name.
         *
         * \return the object.
         */
        StructuredValueTree& operator[]( std::string name );

        /**
         * Checks whether the given value or object exists.
         *
         * \param name name of the value/object to search
         *
         * \return true if existing.
         */
        bool exists( std::string name ) const;

        /**
         * Queries the value with the given name. If it is not found, the default value will be returned.
         *
         * \param name name of the value
         * \param defaultValue the default if no value was found
         *
         * \return the value
         *
         * \note this does not return a reference because. It returns a copy of the value.
         */
        std::string operator()( std::string name, std::string defaultValue ) const;

        /**
         * Queries the value with the given name. If it is not found, an exception is thrown.
         *
         * \param name name of the value
         *
         * \return the value
         */
        std::string& operator()( std::string name );

        /**
         * Queries the value with the given name. If it is not found, an exception is thrown.
         *
         * \param name name of the value
         *
         * \return the value
         */
        const std::string& operator()( std::string name ) const;

    protected:
    private:
        /**
         * The name of this object
         */
        std::string m_name;

        /**
         * The type used for storing the list of children
         */
        typedef std::vector< StructuredValueTree > ChildrenType;

        /**
         * The of an value entry.
         */
        typedef std::map< std::string, std::string > EntryMapType;

        /**
         * The named values.
         */
        EntryMapType m_entries;

        /**
         * Child trees
         */
        ChildrenType m_children;

        StructuredValueTree* queryTree( const std::string& name );

        const StructuredValueTree* queryTree( const std::string& name ) const;
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
}

#endif  // WSTRUCTUREDTEXTPARSER_H

