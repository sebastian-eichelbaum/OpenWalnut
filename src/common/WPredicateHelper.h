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

#ifndef WPREDICATEHELPER_H
#define WPREDICATEHELPER_H

#include <string>

#include <boost/shared_ptr.hpp>

/**
 * This namespace contains some useful helper classes which use some common class methods as predicate. This is especially useful and handy if
 * std containers are used with OpenWalnut's classes. The predicate helper classes allow easy use of std::count_if, std::find_if and so on.
 */
namespace WPredicateHelper
{
    /**
     * This class tests against the getName() method of the instances of type T. Many, many, many many many classes in OpenWalnut provide a getName()
     * method. This predicate can check against a defined name. Useful for searching.
     */
    template< typename T >
    class Name
    {
    public:
        /**
         * Creates instance. The specified string is used for checking.
         *
         * \param check the string to check against.
         */
        explicit Name( std::string check ):
            m_check( check )
        {
        };

        /**
         * Checks the instance of T against the string specified during construction.
         *
         * \param inst use getName of this instance of T
         *
         * \return true if m_checked == inst.getName()
         */
        bool operator()( const T& inst )
        {
            return inst.getName() == m_check;
        };

    private:

        /**
         * The string to check against.
         */
        std::string m_check;
    };

    /**
     * This class tests against the getName() method of the instances of type T. Many, many, many many many classes in OpenWalnut provide a getName()
     * method. This predicate can check against a defined name. Useful for searching. This partial specialization is for shared_ptr, which are a
     * very common tool in OpenWalnut.
     */
    template< typename T >
    class Name< boost::shared_ptr< T > >
    {
    public:
        /**
         * Creates instance. The specified string is used for checking.
         *
         * \param check the string to check against.
         */
        explicit Name( std::string check ):
            m_check( check )
        {
        };

        /**
         * Checks the instance of T against the string specified during construction.
         *
         * \param inst use getName of this instance of T
         *
         * \return true if m_checked == inst.getName()
         */
        bool operator()( const boost::shared_ptr< T >& inst )
        {
            return inst->getName() == m_check;
        };

    private:

        /**
         * The string to check against.
         */
        std::string m_check;
    };
}

#endif  // WPREDICATEHELPER_H

