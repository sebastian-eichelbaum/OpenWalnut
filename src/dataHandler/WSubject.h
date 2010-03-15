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
#include <set>
#include <boost/shared_ptr.hpp>

#include "../common/WConditionSet.h"
#include "../common/WSharedObject.h"
#include "../common/WSharedSequenceContainer.h"

#include "WPersonalInformation.h"

class WDataSet;
class WDataTexture3D;

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
     * List of some standard subjects. This is currently used for the default subject as we do not have any others.
     */
    enum
    {
        SUBJECT_UNKNOWN = 0
    };

    /**
     * For shortening: a type defining a shared vector of WSubject pointers.
     */
    typedef std::vector< boost::shared_ptr< WDataSet > > DatasetContainerType;

    /**
     * The alias for a shared container.
     */
    typedef WSharedSequenceContainer< boost::shared_ptr< WDataSet >, DatasetContainerType > DatasetSharedContainerType;

    /**
     * Alias for the proper access object
     */
    typedef DatasetSharedContainerType::WSharedAccess DatasetAccess;

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
     * Destructs the subject. Removes all datasets from the list.
     */
    virtual ~WSubject();

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
     * Insert a new dataset referenced by a pointer.
     *
     * \param dataset a pointer to the dataset that will be added
     */
    void addDataSet( boost::shared_ptr< WDataSet > dataset );

    /**
     * Removes the specified dataset if it is in the set.
     *
     * \param dataset the dataset to remove.
     */
    void removeDataSet( boost::shared_ptr< WDataSet > dataset );

    /**
     * Remove all datasets from the subjects.
     */
    void clear();

// TODO(all): rethink this
//  wiebel: I deactivated this as we want to resort thes list ... so we have to rethinks this.
//     /**
//      * Returns the dataset which corresponds to the specified ID. It throws an exception, if the dataset does not exists anymore.
//      *
//      * \ param datasetID the ID to search the dataset for
//      *
//      * \return the dataset.
//      *
//      * \throw WNoSuchDataSet in case the dataset can't be found.
//      *
//      * \note you should avoid this function. Do NOT store ID's. They may change.
//      */
//     boost::shared_ptr< WDataSet > getDataSetByID( size_t datasetID );

    /**
     * This gives a list of data textures from all supporting datasets in this subject.
     *
     * \param onlyActive true whenever only textures should be returned where isGloballyActive() == true.
     * \return the list of textures.
     */
    std::vector< boost::shared_ptr< WDataTexture3D > > getDataTextures( bool onlyActive = false );

    /**
     * Gets an access object which allows thread save iteration over the datasets.
     *
     * \return the access object.
     */
    DatasetAccess getAccessObject();

    /**
     * This condition fires whenever the list of datasets changes, or one dataset got marked as "dirty" (threshold, opacity, ...).
     *
     * \return the condition
     */
    boost::shared_ptr< WCondition > getChangeCondition() const;

    /**
     * This condition fires whenever the list of datasets changes.
     *
     * \return the condition
     */
    boost::shared_ptr< WCondition > getListChangeCondition() const;

protected:

    /**
     * A container for all WDataSet.
     */
    DatasetSharedContainerType m_datasets;

    /**
     * The access object used for thread safe access.
     */
    DatasetSharedContainerType::WSharedAccess m_datasetAccess;

    /**
     * This condition set fires whenever one dataset gets dirty or the list of datasets changes.
     */
    boost::shared_ptr< WConditionSet > m_changeCondition;

    /**
     * This condition set fires whenever the list of datasets changes.
     */
    boost::shared_ptr< WConditionSet > m_listChangeCondition;

private:

    WPersonalInformation m_personalInfo; //!< Information on the person represented by this WSubject.
};

#endif  // WSUBJECT_H

