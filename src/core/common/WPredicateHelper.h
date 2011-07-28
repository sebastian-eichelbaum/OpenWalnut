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
#include <boost/function.hpp>

/**
 * This namespace contains some useful helper classes which use some common class methods as predicate. This is especially useful and handy if
 * std containers are used with OpenWalnut's classes. The predicate helper classes allow easy use of std::count_if, std::find_if and so on.
 */
namespace WPredicateHelper
{
    /**
     * Predicate which is always true. Useful if you want to ignore something all the time.
     *
     * @tparam T the value type to check
     *
     * \return always true.
     */
    template< typename T >
    bool alwaysTrue( const T& /* obj */ )
    {
        return true;
    }

    /**
     * Predicate which is always false. Useful if you want to ignore something all the time.
     *
     * @tparam T the value type to check
     *
     * \return always false.
     */
    template< typename T >
    bool alwaysFalse( const T& /* obj */ )
    {
        return false;
    }

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

    /**
     * This class builds the base for wrapping around nearly every possible predicates like functors, classes with operator() and so on. It is
     * especially useful to have an base class allowing predicate evaluation without knowing the exact predicate type. In multi-threaded
     * environments, command queues are a common way to add/remove/replace items in a list. With this base class it is possible to provide
     * predicates in such queues. The direct use of this class for std algorithms (find_if, remove_if, count_if, ... ) is not recommended as it
     * simply is not needed.
     *
     * \tparam the type to evaluate the predicate for. Usually, this is the type of list elements.
     */
    template < typename T >
    class ArbitraryPredicateBase
    {
    public:
        /**
         * Creates instance.
         */
        ArbitraryPredicateBase()
        {
        };

        /**
         * Destructor.
         */
        virtual ~ArbitraryPredicateBase()
        {
        };

        /**
         * Checks the instance of T against an arbitrary predicate.
         *
         * \param inst the value to check against a predicate
         *
         * \return true if predicate evaluates to true
         */
        virtual bool operator()( T const& inst ) const = 0;
    };

    /**
     * The actual class implementing the predicate evaluation. The default predicate is a functor evaluating to true or false. For more details
     * see \ref ArbitraryPredicateBase.
     *
     * \tparam T the type to check. This usually is the type of the elements in a list or similar.
     * \tparam Predicate this is the predicate type. By default, it is a functor.
     */
    template < typename T, typename Predicate = boost::function1< bool, T > >
    class ArbitraryPredicate: public ArbitraryPredicateBase< T >
    {
    public:
        /**
         * Creates instance.
         *
         * \param predicate the predicate used for checking
         */
        explicit ArbitraryPredicate( Predicate predicate ):
            ArbitraryPredicateBase< T >(),
            m_predicate( predicate )
        {
        };

        /**
         * Destructor.
         */
        virtual ~ArbitraryPredicate()
        {
        };

        /**
         * Checks the instance of T against an arbitrary predicate.
         *
         * \param inst the value to check against a predicate
         *
         * \return true if predicate evaluates to true
         */
        virtual bool operator()( T const& inst ) const
        {
            return m_predicate( inst );
        };

    private:

        /**
         * The predicate to use for checking
         */
        Predicate m_predicate;
    };
}

#endif  // WPREDICATEHELPER_H

