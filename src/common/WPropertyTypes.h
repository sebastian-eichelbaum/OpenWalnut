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

#ifndef WPROPERTYTYPES_H
#define WPROPERTYTYPES_H

#include <stdint.h>

#include <string>
#include <list>

#include <boost/filesystem.hpp>

template < typename T >
class WPropertyVariable;

/**
 * Enum of all possible types, that can be used with WProperty.
 */
typedef enum
{
    PV_UNKNOWN,        // type not know
    PV_INT,            // integer value
    PV_DOUBLE,         // floating point value
    PV_BOOL,           // boolean
    PV_STRING,         // a string
    PV_PATH,           // a Boost Path object denoting a filename/path
    PV_LIST            // a list of strings
}
PROPERTY_TYPE;

/**
 * Some convenience type alias for a even more easy usage of WPropertyVariable
 */
/**
 * Alias for int32_t property variables.
 */
typedef boost::shared_ptr< WPropertyVariable< int32_t > > WPropInt;

/**
 * Alias for int32_t property variables.
 */
typedef boost::shared_ptr< WPropertyVariable< double > > WPropDouble;

/**
 * Alias for bool property variables.
 */
typedef boost::shared_ptr< WPropertyVariable< bool > > WPropBool;

/**
 * Alias for string property variables.
 */
typedef boost::shared_ptr< WPropertyVariable< std::string > > WPropString;

/**
 * Alias for filename property variables.
 */
typedef boost::shared_ptr< WPropertyVariable< boost::filesystem::path > > WPropFilename;

/**
 * Alias for string list property variables.
 */
typedef boost::shared_ptr< WPropertyVariable< std::list< std::string > > > WPropList;

/**
 * This namespace contains several helper classes which translate their template type to an enum.
 */
namespace PROPERTY_TYPE_HELPER
{
    /**
     * Class helping to adapt types specified as template parameter into an enum.
     */
    template< typename T >
    class WTypeIdentifier
    {
    public:
        /**
         * Get type identifier of the template type T.
         *
         * \return type identifier-
         */
        PROPERTY_TYPE getType()
        {
            return PV_UNKNOWN;
        }
    };

    /**
     * Class helping to adapt types specified as template parameter into an enum.
     */
    template<>
    class WTypeIdentifier< bool >
    {
    public:
        /**
         * Get type identifier of the template type T.
         *
         * \return type identifier-
         */
        PROPERTY_TYPE getType()
        {
            return PV_BOOL;
        }
    };

    /**
     * Class helping to adapt types specified as template parameter into an enum.
     */
    template<>
    class WTypeIdentifier< int32_t >
    {
    public:
        /**
         * Get type identifier of the template type T.
         *
         * \return type identifier-
         */
        PROPERTY_TYPE getType()
        {
            return PV_INT;
        }
    };

    /**
     * Class helping to adapt types specified as template parameter into an enum.
     */
    template<>
    class WTypeIdentifier< double >
    {
    public:
        /**
         * Get type identifier of the template type T.
         *
         * \return type identifier-
         */
        PROPERTY_TYPE getType()
        {
            return PV_DOUBLE;
        }
    };

    /**
     * Class helping to adapt types specified as template parameter into an enum.
     */
    template<>
    class WTypeIdentifier< std::string >
    {
    public:
        /**
         * Get type identifier of the template type T.
         *
         * \return type identifier-
         */
        PROPERTY_TYPE getType()
        {
            return PV_STRING;
        }
    };

    /**
     * Class helping to adapt types specified as template parameter into an enum.
     */
    template<>
    class WTypeIdentifier< boost::filesystem::path >
    {
    public:
        /**
         * Get type identifier of the template type T.
         *
         * \return type identifier-
         */
        PROPERTY_TYPE getType()
        {
            return PV_PATH;
        }
    };

    /**
     * Class helping to adapt types specified as template parameter into an enum.
     */
    template<>
    class WTypeIdentifier< std::list< std::string > >
    {
    public:
        /**
         * Get type identifier of the template type T.
         *
         * \return type identifier-
         */
        PROPERTY_TYPE getType()
        {
            return PV_LIST;
        }
    };
}

#endif  // WPROPERTYTYPES_H
