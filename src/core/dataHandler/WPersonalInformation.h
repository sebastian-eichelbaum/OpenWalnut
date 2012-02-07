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

#ifndef WPERSONALINFORMATION_H
#define WPERSONALINFORMATION_H

#include <stdint.h>

#include <string>
#include <boost/date_time/posix_time/posix_time_types.hpp>


/**
 * A structure that holds all relevant information about the subject.
 * \ingroup dataHandler
 */
class WPersonalInformation // NOLINT
{
    /**
     * Only tests are allowed as friends.
     */
    friend class WPersonalInformationTest;
public:
    /**
     * Enumeration of possible sex types.
     */
    enum Sex
    {
        male,
        female,
        unknown
    };

    /**
     * Returns an empty dummy WPersonalInformation object.
     *
     * \return the dummy object
     */
    static WPersonalInformation createDummyInformation();

    /**
     * Returns the subjectID of the person. This is zero for dummy information.
     *
     * \return subject id number
     */
    uint64_t getSubjectID() const;

    /**
     * Sets the subjectID of the person. This must be non-zero as changed information is not considered dummy anymore.
     * \param subjectID New globally unique identifier
     */
    void setSubjectID( uint64_t subjectID );

    /**
     * Returns the last or family name of the person.
     *
     * \return family name
     */
    std::string getLastName() const;

    /**
     * Sets the last or family name of the person if the object is no dummy anymore.
     * \param lastName the new last name
     */
    void setLastName( std::string lastName );

    /**
     * Returns the middle name of the person.
     *
     * \return middle name
     */
    std::string getMiddleName() const;

    /**
     * Returns the first or given name of the person.
     *
     * \return first name
     */
    std::string getFirstName() const;

    /**
     * Returns if all members of the current WPersonalInformation are equal to those of info.
     * \param info the WPersonalInformation to compare with
     *
     * \return true if the information are equal
     */
    bool operator==( WPersonalInformation info ) const;

    /**
     * Returns if not all members of the current WPersonalInformation are equal to those of info.
     * \param info the WPersonalInformation to compare with
     *
     * \return true if the personal informations differ
     */
    bool operator!=( WPersonalInformation info ) const;

    /**
     * Returns the name of the subject. This is a concatenation of first, middle and last name.
     *
     * \return the name of the subject.
     */
    std::string getCompleteName() const;

protected:
private:
    /**
     * Private default constructor to force the use of special function for dummy infos.
     */
    WPersonalInformation();

    // TODO(wiebel): need getters and setters for all methods.
    // TODO(wiebel): Should better be something like dotnet's System.Guid
    uint64_t m_subjectID; //!< Represents a globally unique identifier.
    std::string m_subjectCode; //!< Code for person
    std::string m_lastName; //!< Last name or family of the person.
    std::string m_middleName; //!< Middle name of the person, if any.
    std::string m_firstName; //!< First name or given name of the person.
    boost::posix_time::ptime m_dateOfBirth; //!< Birthday of the person.
    std::string m_streetAndNumber; //!< street name and number of house in which person lives
    std::string m_zipCode; //!< ZIP code of the city in which person lives
    std::string m_city; //!< city in which person lives
    std::string m_state; //!< state in which person lives
    std::string m_country; //!< country in which person lives
    std::string m_phone; //!< phone number of person
    std::string m_eMail; //!< e-mail adress of person
    std::string m_handicaps; //!< Description of the handicaps of the person.
    Sex m_sex; //!< The gender of the person.
    // TODO(wiebel):  Should better be something like dotnet's System.Nullable<byte>
    char m_categoryId; //!< not documented.
    std::string m_handedness; //!< preference for using right or left hand
    std::string m_notes; //!< Notes.
    std::string m_diagnostic; //!< The diagnosis for the person.
    std::string m_medication; //!< The medication of the person.
    std::string m_referringDoctor; //!< The doctor who reffered the person.
};

#endif  // WPERSONALINFORMATION_H
