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

#ifndef WSHAREDOBJECT_H
#define WSHAREDOBJECT_H

#include <boost/thread.hpp>

#include "WCondition.h"

/**
 * Wrapper around an object/type for thread safe sharing of objects among multiple threads. The advantage of this class over WFlag
 * is, that WFlag just protects simple get/set operations, while this class can protect a whole bunch of operations on the
 * encapsulated object.
 */
template < typename T >
class WSharedObject
{
public:

    /**
     * Default constructor.
     */
    WSharedObject();

    /**
     * Destructor.
     */
    virtual ~WSharedObject();

    /**
     * Class allowing thread-safe access to an object. It provides some convenience methods to read and write lock the access.
     */
    class WSharedObjectAccess
    {
    public:

        /**
         * Constructor. It uses the specified mutex which is shared among all access objects of the same WSharedObject.
         *
         * \param mutex the mutex used to lock the access.
         * \param object the object to be shared.
         * \param condition the condition that should be used for notifying changes.
         */
        WSharedObjectAccess( T& object, boost::shared_ptr< boost::shared_mutex > mutex,  boost::shared_ptr< WCondition > condition );

        /**
         * Constructor. It uses the specified mutex which is shared among all access objects of the same WSharedObject.
         *
         * \param mutex the mutex used to lock the access.
         * \param object the object to be shared.
         */
        WSharedObjectAccess( T& object, boost::shared_ptr< boost::shared_mutex > mutex );

        /**
         * Desctructor.
         */
        ~WSharedObjectAccess();

        /**
         * Gets the contained, and protected object.
         *
         * \return the contained object
         */
        T& get();

        /**
         * Acquires a read lock to the protected object for easy access. Use this EVERYTIME you want to read from the object. Use
         * endRead() to free the lock.
         */
        void beginRead();

        /**
         * Frees the lock to the object. If you do not free the lock, no write access will be granted in the future. To nobody!
         * So always free the lock.
         */
        void endRead();

        /**
         * Acquires a write lock to the object
         */
        void beginWrite();

        /**
         * Frees the lock to the object. If you do not free the lock, no read or write access will be granted in the future. To nobody!
         * So always free the lock.
         *
         * \param suppressNotify if true, the change condition won't fire.
         */
        void endWrite( bool suppressNotify = false );

    protected:

        /**
         * The write lock. Used by beginWrite and endWrite.
         */
        boost::unique_lock< boost::shared_mutex > m_writeLock;

        /**
         * The write lock. Used by beginWrite and endWrite.
         */
        boost::shared_lock< boost::shared_mutex > m_readLock;

        /**
         * The lock to ensure thread safe access. It is the lock provided by WSharedObject.
         */
        boost::shared_ptr< boost::shared_mutex > m_lock;

        /**
         * the object protected.
         */
        T& m_object;

        /**
         * The pointer to the global change condition. Fired whenever endWrite() got called.
         */
        boost::shared_ptr< WCondition > m_objectChangeCondition;
    };

    /**
     * Use a shared_ptr since the shared and unique locks from boost are non-copyable.
     */
    typedef boost::shared_ptr< WSharedObjectAccess > WSharedAccess;

    /**
     * This method distributes access objects. These objects are able to read/write lock the object and grant access to it, in
     * a thread-safe manner.
     *
     * \return the access object which allows thread safe access to the object.
     */
    WSharedAccess getAccessObject();

    /**
     * This condition fires whenever the encapsulated object changed. This is fired automatically by endWrite().
     *
     * \return the condition
     */
    boost::shared_ptr< WCondition > getChangeCondition();

protected:

    /**
     * The object wrapped by this class.
     */
    T m_object;

    /**
     * The lock to ensure thread safe access.
     */
    boost::shared_ptr< boost::shared_mutex > m_lock;

    /**
     * This condition set fires whenever the contained object changes. This corresponds to the Observable pattern.
     */
    boost::shared_ptr< WCondition > m_changeCondition;

private:
};

template < typename T >
WSharedObject< T >::WSharedObject():
    m_lock( new boost::shared_mutex ),
    m_changeCondition( new WCondition() )
{
    // init members
}

template < typename T >
WSharedObject< T >::~WSharedObject()
{
    // clean up
}

template < typename T >
typename WSharedObject< T >::WSharedAccess WSharedObject< T >::getAccessObject()
{
    return WSharedObject< T >::WSharedAccess( new WSharedObject< T>::WSharedObjectAccess( m_object, m_lock, m_changeCondition ) );
}


template < typename T >
WSharedObject< T >::WSharedObjectAccess::WSharedObjectAccess( T& object, boost::shared_ptr< boost::shared_mutex > mutex ):
    m_lock( mutex ),
    m_object( object ),
    m_objectChangeCondition()
{
}

template < typename T >
WSharedObject< T >::WSharedObjectAccess::WSharedObjectAccess( T& object, boost::shared_ptr< boost::shared_mutex > mutex,
                                                              boost::shared_ptr< WCondition > condition ):
    m_lock( mutex ),
    m_object( object ),
    m_objectChangeCondition( condition )
{
}

template < typename T >
WSharedObject< T >::WSharedObjectAccess::~WSharedObjectAccess()
{
    // this shouldn't be necessary as the locks automatically unlock if the get destroyed
    // m_readLock.unlock();
    // m_writeLock.unlock();
}

template < typename T >
void WSharedObject< T >::WSharedObjectAccess::beginRead()
{
    m_readLock = boost::shared_lock< boost::shared_mutex >( *m_lock );
}

template < typename T >
void WSharedObject< T >::WSharedObjectAccess::endRead()
{
    m_readLock.unlock();
}

template < typename T >
void WSharedObject< T >::WSharedObjectAccess::beginWrite()
{
    m_writeLock = boost::unique_lock< boost::shared_mutex >( *m_lock );
}

template < typename T >
void WSharedObject< T >::WSharedObjectAccess::endWrite( bool suppressNotify )
{
    m_writeLock.unlock();

    if ( !suppressNotify && m_objectChangeCondition.get() )
    {
        m_objectChangeCondition->notify();
    }
}

template < typename T >
T& WSharedObject< T >::WSharedObjectAccess::get()
{
    return m_object;
}

template < typename T >
boost::shared_ptr< WCondition > WSharedObject< T >::getChangeCondition()
{
    return m_changeCondition;
}

#endif  // WSHAREDOBJECT_H

