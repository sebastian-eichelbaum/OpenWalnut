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
#include <vector>

#include <boost/shared_ptr.hpp>
#include "../gui/qt4/signalslib.hpp"
#include <boost/enable_shared_from_this.hpp>

#include "WDataSet.h"

class WSubject;

/**
 * Provides the environment for storing and accessing different subjects.
 * As all measured data belongs to one subject, this is the main access point
 * to our data.
 * \ingroup dataHandler
 */
class WDataHandler : public boost::enable_shared_from_this< WDataHandler >
{
/**
 * Only UnitTests may be friends.
 */
friend class WDataHandlerTest;

public:
    /**
     * Empty standard constructor.
     */
    WDataHandler();

    /**
     * Get the pointer to the i'th WSubject. The return type is const since we
     * want to ensure that each subject cannot modified after retrival.
     * \param subjectId the number of the subject to be retrieved.
     */
    boost::shared_ptr< WSubject > getSubject( const unsigned int subjectId ) const;

    /**
     * Returns a  to the i'th WSubject. The return type is const since we
     * want to ensure that each subject cannot modified after retrival.
     * \param subjectId the number of the subject to be retrieved.
     */
    boost::shared_ptr< WSubject > operator[]( size_t subjectId ) const;

    /**
     * Insert a new WSubject referenced by a pointer.
     * \param newSubject a pointer to the subject that will be added
     */
    void addSubject( boost::shared_ptr< WSubject > newSubject );

    /**
     * Get the number of subjects which are actually handled by our WDataHandler.
     */
    unsigned int getNumberOfSubjects() const;

protected:

private:
    /**
     * A container for all WSubjects.
     */
    std::vector< boost::shared_ptr< WSubject > > m_subjects;
};

/**
 * \defgroup dataHandler Data Handler
 *
 * \brief
 * This module implements the data storage facility of OpenWalnut.
 */

#endif  // WDATAHANDLER_H
