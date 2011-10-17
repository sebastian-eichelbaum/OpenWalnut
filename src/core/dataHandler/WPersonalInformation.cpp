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

#include <string>

#include "../common/WAssert.h"
#include "WSubject.h"
#include "WPersonalInformation.h"

WPersonalInformation WPersonalInformation::createDummyInformation()
{
    return WPersonalInformation();
}

WPersonalInformation::WPersonalInformation()
    : m_subjectID( WSubject::SUBJECT_UNKNOWN ),
      m_subjectCode( "" ),
      m_lastName( "UNKNOWN" ),
      m_middleName( "" ),
      m_firstName( "" ),
      m_dateOfBirth( boost::date_time::not_a_date_time ),
      m_streetAndNumber( "" ),
      m_zipCode( "" ),
      m_city( "" ),
      m_state( "" ),
      m_country( "" ),
      m_phone( "" ),
      m_eMail( "" ),
      m_handicaps( "" ),
      m_sex( unknown ),
      m_categoryId( 0 ),
      m_handedness( "" ),
      m_notes( "" ),
      m_diagnostic( "" ),
      m_medication( "" ),
      m_referringDoctor( "" )
{
}

uint64_t WPersonalInformation::getSubjectID() const
{
    return m_subjectID;
}

void WPersonalInformation::setSubjectID( uint64_t subjectID )
{
    WAssert( subjectID != WSubject::SUBJECT_UNKNOWN, "ID zero is reserved for dummies." );
    m_subjectID = subjectID;
}

std::string WPersonalInformation::getLastName() const
{
    return m_lastName;
}

std::string WPersonalInformation::getCompleteName() const
{
    return getLastName() + ", " + getFirstName() + " " + getMiddleName();
}

void WPersonalInformation::setLastName( std::string lastName )
{
    WAssert( m_subjectID != WSubject::SUBJECT_UNKNOWN, "SubjectID is still zero. This is reserved for empty dummies. Set it first." );
    m_lastName = lastName;
}

std::string WPersonalInformation::getMiddleName() const
{
    return m_middleName;
}

std::string WPersonalInformation::getFirstName() const
{
    return m_firstName;
}

bool WPersonalInformation::operator==( WPersonalInformation info ) const
{
    return m_subjectID  == info.m_subjectID
        && m_subjectCode == info.m_subjectCode
        && m_lastName == info.m_lastName
        && m_middleName == info.m_middleName
        && m_firstName == info.m_firstName
        && m_dateOfBirth == info.m_dateOfBirth
        && m_streetAndNumber == info.m_streetAndNumber
        && m_zipCode == info.m_zipCode
        && m_city == info.m_city
        && m_state == info.m_state
        && m_country == info.m_country
        && m_phone == info.m_phone
        && m_eMail == info.m_eMail
        && m_handicaps == info.m_handicaps
        && m_sex == info.m_sex
        && m_categoryId == info.m_categoryId
        && m_handedness == info.m_handedness
        && m_notes == info.m_notes
        && m_diagnostic == info.m_diagnostic
        && m_medication == info.m_medication
        && m_referringDoctor == info.m_referringDoctor;
}

bool WPersonalInformation::operator!=( WPersonalInformation info ) const
{
    return !( (*this) == info );
}
