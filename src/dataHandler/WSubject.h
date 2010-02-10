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

#ifndef WSUBJECT_H
#define WSUBJECT_H

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "WPersonalInformation.h"

class WDataSet;

/**
 * Container for all WDataSets belonging to one subject or patient.
 * \ingroup dataHandler
 */
class WSubject
{
    /**
     * Only tests are allowed as friends.
     */
    friend class WSubjectTest;

public:

    /**
     * Constructs a dummy subject.
     */
    WSubject();

    /**
     * Allows to give the subject information the person during construction.
     *
     * \param personInfo personal information object
     */
    explicit WSubject( WPersonalInformation personInfo );

    /**
     * Returns the name of the subject. See WPersonalInformation for details on the name.
     *
     * \return the name of the subject extracted from this subject's WPersonalInformation.
     */
    std::string getName() const;

    /**
     * Gives the personal information of a subject.
     *
     * \return the personal information of the subject.
     */
    WPersonalInformation getPersonalInformation() const;

    /**
     * Get the pointer to the i'th WDataSet. The return type is const since we
     * want to ensure that each DataSet cannot modified after retrival.
     * \param dataSetId the number of the data set to retrieve
     */
    boost::shared_ptr< WDataSet > getDataSet( const unsigned int dataSetId ) const;

    /**
     * Returns a shared_ptr to the i'th WSubject. The return type is const since we
     * want to ensure that a subject cannot be modified after retrieval.
     * \param dataSetId the number of the data set to retrieve
     */
    boost::shared_ptr< const WDataSet > operator[]( const unsigned int dataSetId ) const;

    /**
     * Get the number of DataSets which are actually handled by our subject.
     */
    unsigned int getNumberOfDataSets() const;

protected:

private:

    WPersonalInformation m_personalInfo; //!< Information on the person represented by this WSubject.

    /**
     * A container for all WDataSets belonging to the subject.
     */
    std::vector< boost::shared_ptr< WDataSet > > m_dataSets;
};

#endif  // WSUBJECT_H
