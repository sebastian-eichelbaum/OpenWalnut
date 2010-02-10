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

#ifndef WSTLCONTAINER_H
#define WSTLCONTAINER_H

#include <boost/thread.hpp>

/**
 * Wrapper around the STL sequence container providing thread safe iterator access.
 */
template < typename T >
class WSTLContainer
{
public:

    /**
     * Default constructor.
     */
    WSTLContainer();

    /**
     * Destructor.
     */
    virtual ~WSTLContainer();

protected:

    /**
     * The lock to ensure thread safe access.
     */
    boost::shared_mutex m_lock;

    /**
     * The write lock used for write access.
     */
    boost::unique_lock< boost::shared_mutex > m_wLock;

    /**
     * The read lock used for write access.
     */
    boost::shared_lock< boost::shared_mutex > m_rLock;

private:
};

template < typename T >
WSTLContainer< T >::WSTLContainer()
{
    // init members
}

template < typename T >
WSTLContainer< T >::~WSTLContainer()
{
    // clean up
}


#endif  // WSTLCONTAINER_H

