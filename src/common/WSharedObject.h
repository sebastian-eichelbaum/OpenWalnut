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

/**
 * Wrapper around an object/type for thread safe sharing of objects among multiple threads.
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
         */
        explicit WSharedObjectAccess( T& object, boost::shared_ptr< boost::shared_mutex > mutex );

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
         */
        void endWrite();

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

protected:

    /**
     * The object wrapped by this class.
     */
    T m_object;

    /**
     * The lock to ensure thread safe access.
     */
    boost::shared_ptr< boost::shared_mutex > m_lock;

private:
};

template < typename T >
WSharedObject< T >::WSharedObject():
    m_lock( new boost::shared_mutex )
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
    return WSharedObject< T >::WSharedAccess( new WSharedObject< T>::WSharedObjectAccess( m_object, m_lock ) );
}


template < typename T >
WSharedObject< T >::WSharedObjectAccess::WSharedObjectAccess( T& object, boost::shared_ptr< boost::shared_mutex > mutex ):
    m_lock( mutex ),
    m_object( object )
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
void WSharedObject< T >::WSharedObjectAccess::endWrite()
{
    m_writeLock.unlock();
}

template < typename T >
T& WSharedObject< T >::WSharedObjectAccess::get()
{
    return m_object;
}

#endif  // WSHAREDOBJECT_H

