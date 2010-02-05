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

////////////////////////////////////////////////////////////////////////////////////////////////////////
// NOTE: If you add new types here, please also add corresponding addProperty methods to WProperties
////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////
// NOTE: Always use the WPVBaseTypes in all your declarations to allow easy type modifications later on
////////////////////////////////////////////////////////////////////////////////////////////////////////

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
 * Namespace containing all base types of the WPropertyVariables. Use these types instead of issuing int32_t, double, bool, ...
 * directly.
 */
namespace WPVBaseTypes
{
    typedef int32_t                     PV_INT;     //!< base type used for every WPVInt
    typedef double                      PV_DOUBLE;  //!< base type used for every WPVDouble
    typedef bool                        PV_BOOL;    //!< base type used for every WPVBool
    typedef std::string                 PV_STRING;  //!< base type used for every WPVString
    typedef boost::filesystem::path     PV_PATH;    //!< base type used for every WPVFilename
    typedef std::list< std::string >    PV_LIST;    //!< base type used for every WPVList
}

/**
 * Some convenience type alias for a even more easy usage of WPropertyVariable.
 * These typedefs are useful for casts, as they alias the PropertyVariable types. Please use these types instead of directly
 * int32_t, double, bool, ... so we are able to change the type later on without modifications of thousands of modules.
 */

/**
 * Int properties.
 */
typedef WPropertyVariable< WPVBaseTypes::PV_INT > WPVInt;

/**
 * Floating point properties.
 */
typedef WPropertyVariable< WPVBaseTypes::PV_DOUBLE > WPVDouble;

/**
 * Boolean properties.
 */
typedef WPropertyVariable< WPVBaseTypes::PV_BOOL > WPVBool;

/**
 * String properties.
 */
typedef WPropertyVariable< WPVBaseTypes::PV_STRING > WPVString;

/**
 * Filename properties.
 */
typedef WPropertyVariable< WPVBaseTypes::PV_PATH > WPVFilename;

/**
 * List properties
 */
typedef WPropertyVariable< WPVBaseTypes::PV_LIST > WPVList;

/**
 * Some convenience type alias for a even more easy usage of WPropertyVariable.
 * These typdefs define some pointer alias.
 */

/**
 * Alias for int32_t property variables.
 */
typedef boost::shared_ptr< WPVInt > WPropInt;

/**
 * Alias for int32_t property variables.
 */
typedef boost::shared_ptr< WPVDouble > WPropDouble;

/**
 * Alias for bool property variables.
 */
typedef boost::shared_ptr< WPVBool > WPropBool;

/**
 * Alias for string property variables.
 */
typedef boost::shared_ptr< WPVString > WPropString;

/**
 * Alias for filename property variables.
 */
typedef boost::shared_ptr< WPVFilename > WPropFilename;

/**
 * Alias for string list property variables.
 */
typedef boost::shared_ptr< WPVList > WPropList;

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
    class WTypeIdentifier< WPVBaseTypes::PV_BOOL >
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
    class WTypeIdentifier< WPVBaseTypes::PV_INT >
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
    class WTypeIdentifier< WPVBaseTypes::PV_DOUBLE >
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
    class WTypeIdentifier< WPVBaseTypes::PV_STRING >
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
    class WTypeIdentifier< WPVBaseTypes::PV_PATH >
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
    class WTypeIdentifier< WPVBaseTypes::PV_LIST >
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
