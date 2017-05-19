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

#ifndef Q_MOC_RUN
    // We exclude inclusion of boost fusion, since the combination of Qt 5.3 moc and Boost 1.56 causes MOC error. Moc complains
    // about a macro in the boost headers. Moc does not need all symbols to be defined, so we leave these headers out.
    #include <boost/config/warning_disable.hpp>
    #include <boost/spirit/include/qi.hpp>
    #include <boost/spirit/include/phoenix_core.hpp>
    #include <boost/spirit/include/phoenix_operator.hpp>
    #include <boost/spirit/include/phoenix_fusion.hpp>
    #include <boost/spirit/include/phoenix_stl.hpp>
    #include <boost/spirit/include/phoenix_object.hpp>
    #include <boost/fusion/include/adapt_struct.hpp>
    #include <boost/fusion/include/io.hpp>
    #include <boost/variant/recursive_variant.hpp>
#endif

#include <boost/filesystem/path.hpp>

#include "WStringUtils.h"
#include "exceptions/WTypeMismatch.h"
#include "exceptions/WNotFound.h"

/**
 * This namespace contains the WStructuredTextParser data types and the parser. It builds up the abstract syntax tree (AST)
 * for the given input which later can be traversed.
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
     * The type used for comments
     */
    typedef std::string CommentType;

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
            KeyValueType,
            CommentType
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

    /**
     * An object representing all objects and comments on file level.
     */
    typedef std::vector< MemberType > FileType;
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
     * The grammar describing the structured format. It uses the boost::spirit features to parse the input. There are several rules to comply to
     * successfully parse a file:
     * <ul>
     *  <li>Key: identifier, needs to be a-z,A-Z,0-9,_
     *  <li>Object: defined as key + { ... }
     *  <li> ";" is optional after objects
     *  <li>Key-Value Pair: is a member of an object and is defines as key="value".
     *  <li>Comments begin with //
     * </ul>
     * For more details please see the test fixture file in core/common/test/fixtures/WStrutcuredTextParser_test.txt.
     *
     * \tparam Iterator the iterator, used to get the input stream
     */
    template <typename Iterator>
    struct Grammar: qi::grammar<Iterator, FileType(), ascii::space_type >
    {
        /**
         * Constructor and grammar description. It contains the EBNF (Extended Backus Naur Form) of the format we can parse.
         *
         * \param error Will contain error message if any occurs during functions execution
         */
        explicit Grammar( std::ostream& error ): Grammar::base_type( file, "WStructuredTextParser::Grammar" ) // NOLINT - non-const ref
        {
            // a key begins with a letter
            key    %= qi::char_( "a-zA-Z_" ) >> *qi::char_( "a-zA-Z_0-9" );
            // a value is a quoted string. Multi-line strings possible
            value  %= '"' >> *( ~qi::char_( "\"" ) | qi::char_( " " ) ) >> '"';

            // a pair is: key = value
            kvpair %= key >> '=' >> value >> ';';
            // a comment is // + arbitrary symbols
            comment %= qi::lexeme[ qi::char_( "/" ) >> qi::char_( "/" ) >> *qi::char_( "a-zA-Z_0-9!\"#$%&'()*,:;<>?@\\^`{|}~/ .@=[]§!+-" ) ];
            // a object is a name, and a set of nested objects or key-value pairs
            object %= ( key | value ) >> '{' >> *( object | kvpair | comment ) >> '}' >> *qi::char_( ";" );
            // a file is basically an object without name.
            file %= *( object | kvpair | comment );

            // provide names for these objects for better readability of parse errors
            object.name( "object" );
            kvpair.name( "key-value pair" );
            key.name( "key" );
            value.name( "value" );
            file.name( "file" );
            comment.name( "comment" );

            // provide error handlers
            // XXX: can someone tell me how to get them work? According to the boost::spirit doc, this is everything needed but it doesn't work.
            qi::on_error< qi::fail >( object, error << phoenix::val( "Error: " ) << qi::_4 );
            qi::on_error< qi::fail >( kvpair, error << phoenix::val( "Error: " ) << qi::_4 );
            qi::on_error< qi::fail >( key,    error << phoenix::val( "Error: " ) << qi::_4 );
            qi::on_error< qi::fail >( value,  error << phoenix::val( "Error: " ) << qi::_4 );
            qi::on_error< qi::fail >( comment,  error << phoenix::val( "Error: " ) << qi::_4 );
            qi::on_error< qi::fail >( file,  error << phoenix::val( "Error: " ) << qi::_4 );
       }

        // Rules we use

        /**
         * Rule for objects. Attribute is ObjectType and is the start rule of the grammar. See constructor for exact definition.
         */
        qi::rule< Iterator, ObjectType(), ascii::space_type > object;

        /**
         * Rule for files. Basically the same as an object but without name
         */
        qi::rule< Iterator, FileType(), ascii::space_type > file;

        /**
         * Rule for comments. Ignored.
         */
        qi::rule< Iterator, CommentType(), ascii::space_type > comment;

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
        qi::rule< Iterator, ValueType() > value;
    };

    /**
     * This simplifies working with a tree in a \ref WStructuredTextParser::FileType instance. It provides easy query and check methods. It does not
     * provide any semantic options. So check validity of the contents and structure of the tree is the job of the using class/derived class. As
     * the tree does not know anything about the semantics of your structure, it is also untyped. For every key you query, you need to specify
     * the type.
     *
     * This tree uses the types in the WStructuredTextParser namespace. To avoid unnecessary copy operations, this class is not recursive
     * itself. When querying, you always need to specify the full path. This class can be seen as accessor to the
     * \ref WStructuredTextParser::ObjectType tree.
     *
     * \note The syntax of the parsed files is defined by the parser itself. See WStructuredTextParser::Grammar for details.
     * \note This also stores the comments of the parsed file. This allows them to be written again if OW loads a file, modifies it and re-writes
     * it.
     */
    class StructuredValueTree
    {
        friend class WStructuredTextParserTest; //!< Access for test class.
    public:
        /**
         * This char is used as separator for identifying values in the tree. NEVER change this value.
         */
        static const std::string Separator;

        /**
         * Construct the instance given the original parsing structure.
         *
         * \param file the parsing result structure (the root node).
         */
        explicit StructuredValueTree( const FileType& file );

        /**
         * Construct the instance given a text as string.
         *
         * \param toParse the text to parse
         */
        explicit StructuredValueTree( const std::string& toParse );

        /**
         * Construct the instance given a path to a file to load.
         *
         * \param file the path to a file to load.
         */
        explicit StructuredValueTree( const boost::filesystem::path& file );

        /**
         * Creates an empty tree. It will contain no information at all.
         */
        StructuredValueTree();

        /**
         * Cleanup.
         */
        virtual ~StructuredValueTree();

        /**
         * Checks whether the given value or object exists. If you want to know only if a value with the given name exists, set valuesOnly to
         * true.
         *
         * \param key path to the value
         * \param valuesOnly if true, it checks only if a value with the name exists. If false, also objects with this name cause this function
         * to return true.
         *
         * \return true if existing.
         */
        bool exists( std::string key, bool valuesOnly = false ) const;

        /**
         * It is possible that there are multiple values matching a key. This method counts them.
         *
         * \param key path to the values to count
         * \param valuesOnly if true, it only counts values matching the given name.
         *
         * \return the number of found values.
         */
        size_t count( std::string key, bool valuesOnly = false ) const;

        /**
         * Queries the value with the given name. If it is not found, the default value will be returned.
         *
         * \param key path to the value. Paths to whole objects are invalid.
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
         * \param key path to the value. Paths to whole objects are invalid.
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
         * \param key path to the value. Paths to whole objects are invalid.
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
         * \param key path to the value. Paths to whole objects are invalid.
         * \tparam T the return type. This method tries to cast to this type. If it fails, an exception is thrown. Type std::string is always valid.
         * \throw WTypeMismatch if the value cannot be cast to the specified target type
         * \throw WNotFound if the key:value pair does not exist
         *
         * \return the value as copy to avoid any const_cast which would allow modification.
         */
        template< typename T >
        T operator[]( std::string key ) const;

        /**
         * Gets a subtree. The ValueTree returned contains the node you have searched. It only contains the first match. If all matches are
         * needed, use \ref getSubTrees instead. If the key is not valid/nothing matches the key, an empty value tree is returned. If they key
         * matches a key-value pair, nothing is returned. This means, this method is only useful for objects.
         *
         * \param key key to search.
         *
         * \return the structured value tree.
         */
        StructuredValueTree getSubTree( std::string key ) const;

        /**
         * Gets all matching subtrees. The subtrees returned contains the node you have searched. If multiple objects match the key, a list of
         * subtrees is returned. If nothing matches, the returned list is empty. If they key
         * matches a key-value pair, nothing is returned. This means, this method is only useful for objects.
         *
         * \param key key to search.
         *
         * \return the structured value trees.
         */
        std::vector< StructuredValueTree > getSubTrees( std::string key ) const;

    protected:
    private:
        /**
         * The named values.
         */
        FileType m_file;

        /**
         * Recursively fills a result vector using a given path iterator. It checks whether the current element matches the current key. If yes,
         * it traverses or adds the value to the result vector. This uses depth-first search and allows multiple matches for one key.
         *
         * \param current current element to check and recursively traverse
         * \param keyIter the current path element
         * \param keyEnd the end iter. Just used to stop iteration if the key as not further elements
         * \param resultObjects all matching instances of type \ref WStructuredTextParser::ObjectType
         * \param resultValues all matching instances of type \ref WStructuredTextParser::KeyValueType
         */
        void traverse( MemberType current, std::vector< std::string >::const_iterator keyIter,
                                           std::vector< std::string >::const_iterator keyEnd,
                                           std::vector< ObjectType >& resultObjects,
                                           std::vector< KeyValueType >& resultValues ) const;

        /**
         * Recursively fills a result vector using a given path iterator. It checks whether the current element matches the current key. If yes,
         * it traverses or adds the value to the result vector. This uses depth-first search and allows multiple matches for one key.
         *
         * \param current current element to check and recursively traverse
         * \param key the path
         * \param resultObjects all matching instances of type \ref WStructuredTextParser::ObjectType
         * \param resultValues all matching instances of type \ref WStructuredTextParser::KeyValueType
         */
        void traverse( FileType current, std::string key,
                                         std::vector< ObjectType >& resultObjects,
                                         std::vector< KeyValueType >& resultValues ) const;
    };

    /**
     * Parse the given input and return the syntax tree. Throws an exception WParseError on error.
     *
     * \param input the input to parse.
     *
     * \return the syntax tree in plain format. You should use WStructuredValueTree to use this.
     *
     * \throw WParseError on parse error
     */
    FileType parseFromString( std::string input );

    /**
     * Parse the given input and return the syntax tree. Throws an exception WParseError on error.
     *
     * \param path the file to parse
     *
     * \return the syntax tree in plain format. You should use WStructuredValueTree to use this.
     *
     * \throw WParseError on parse error
     * \throw WFileNotFOund in case the specified file could not be opened
     */
    FileType parseFromFile( boost::filesystem::path path );

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
     * Visitor to identify whether the given variant of type \ref WStructuredTextParser::MemberType is a object or key-value pair.
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
         * \tparam T the type. Should be \ref WStructuredTextParser::ObjectType or \ref WStructuredTextParser::CommentType
         * \return always false since it identified an Object/comment
         */
        template< typename T >
        bool operator()( const T& /* element */ ) const
        {
            return false;
        }
    };

    /**
     * Visitor to identify whether the given variant of type \ref WStructuredTextParser::MemberType is a comment.
     */
    class IsCommentVisitor: public boost::static_visitor< bool >
    {
    public:
        /**
         * Returns always true as it is only called for comments.
         *
         * \return always true
         */
        bool operator()( const CommentType& /* element */ ) const
        {
            return true;
        }

        /**
         * Returns always false as it is only called for objects and key-value pairs.
         *
         * \tparam T the type. Should be \ref WStructuredTextParser::ObjectType or \ref WStructuredTextParser::KeyValueType
         * \return always false since it identified an Object/KeyValueType
         */
        template< typename T >
        bool operator()( const T& /* element */ ) const
        {
            return false;
        }
    };

    /**
     * Visitor to query the m_name member of \ref WStructuredTextParser::ObjectType and \ref WStructuredTextParser::KeyValueType.
     */
    class NameQueryVisitor: public boost::static_visitor< std::string >
    {
    public:
        /**
         * Comments have no name.
         *
         * \return empty string.
         */
        std::string operator()( const CommentType& /* element */ ) const
        {
            return "";
        }

        /**
         * Returns the m_name member of the specified object or key-valuev pair.
         *
         * \param element Specified object.
         *
         * \tparam T one of the types of the \ref WStructuredTextParser::MemberType variant
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
        // traverse the tree
        std::vector< ObjectType > rObj;
        std::vector< KeyValueType > rKV;

        // traverse
        traverse( m_file, key, rObj, rKV );

        // copy to result vector and cast
        std::vector< T > r;
        for( std::vector< KeyValueType >::const_iterator i = rKV.begin(); i != rKV.end(); ++i )
        {
            try
            {
                r.push_back( string_utils::fromString< T >( ( *i ).m_value ) );
            }
            catch( ... )
            {
                // convert the standard exception (if cannot convert) to a WTypeMismnatch.
                throw WTypeMismatch( "Cannot convert element \"" + key + "\" to desired type." );
            }
        }

        // done
        return r;
    }
}

#endif  // WSTRUCTUREDTEXTPARSER_H

