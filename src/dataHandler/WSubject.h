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

class WDataSet;

/**
 * Container for all WDataSets belonging to one subject.
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
     * Empty standard constructor.
     */
    WSubject();

    /**
     * Allows to give the subject a name during construction
     */
    explicit WSubject( std::string name );

    /**
     * Returns the name of the subject. See WSubject::m_name for details on the name.
     */
    std::string getName() const;

    /**
     * Get the pointer to the i'th WDataSet. The return type is const since we
     * want to ensure that each DataSet cannot modified after retrival.
     */
    boost::shared_ptr< WDataSet > getDataSet( const unsigned int dataSetId ) const;

    /**
     * Returns a shared_ptr to the i'th WSubject. The return type is const since we
     * want to ensure that a subject cannot be modified after retrieval.
     */
    boost::shared_ptr< const WDataSet > operator[]( const unsigned int dataSetId ) const;

    /**
     * Insert a new DataSet referenced by a pointer.
     */
    void addDataSet( boost::shared_ptr< WDataSet > newDataSet );

    /**
     * Get the number of DataSets which are actually handled by our subject.
     */
    unsigned int getNumberOfDataSets() const;
protected:
private:
    /**
     * Name of subject. As not all data formats contain real names
     * this may also be some kind of id (e.g. initials).
     */
    std::string m_name;

    /**
     * A container for all WDataSets belonging to the subject.
     */
    std::vector< boost::shared_ptr< WDataSet > > m_dataSets;
};

#endif  // WSUBJECT_H
