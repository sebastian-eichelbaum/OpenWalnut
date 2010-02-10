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

#ifndef WDATAHANDLER_H
#define WDATAHANDLER_H

#include <string>
#include <set>

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "../common/WSharedObject.h"

#include "WDataSet.h"

class WSubject;

/**
 * Provides the environment for storing and accessing different subjects.
 * As all measured data belongs to one subject, this is the main access point
 * to our data.
 *
 * \ingroup dataHandler
 */
class WDataHandler
{
/**
 * Only UnitTests may be friends.
 */
friend class WDataHandlerTest;

public:

    /**
     * For shortening: a type defining a shared vector of WSubject pointers.
     */
    typedef std::set< boost::shared_ptr< WSubject > > SubjectContainerType;

    /**
     * Empty standard constructor.
     */
    WDataHandler();

    /**
     * Insert a new subject referenced by a pointer.
     *
     * \param subject a pointer to the subject that will be added
     */
    void addSubject( boost::shared_ptr< WSubject > subject );

    /**
     * Removes the specified subject if it is in the set.
     *
     * \param subject the subject to remove.
     */
    void removeSubject( boost::shared_ptr< WSubject > subject );

protected:

    /**
     * A container for all WSubjects.
     */
    WSharedObject< SubjectContainerType > m_subjects;

    /**
     * The access object used for thread safe access.
     */
    WSharedObject< SubjectContainerType >::WSharedAccess m_subjectAccess;

    /**
     * The lock used for avoiding mutual write to the subjects list
     */
    boost::shared_mutex m_subjectsLock;

private:
};

/**
 * \defgroup dataHandler Data Handler
 *
 * \brief
 * This module implements the data storage facility of OpenWalnut.
 */

#endif  // WDATAHANDLER_H
