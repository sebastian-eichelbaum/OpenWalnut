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

#include <algorithm>
#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>

#include "WSubject.h"
#include "exceptions/WDHNoSuchSubject.h"

#include "../common/WLogger.h"

#include "WDataHandler.h"

// instance as singleton
boost::shared_ptr< WDataHandler > WDataHandler::m_instance = boost::shared_ptr< WDataHandler >();

WDataHandler::WDataHandler():
    m_subjects(),
    m_subjectAccess( m_subjects.getAccessObject() )
{
    WLogger::getLogger()->addLogMessage( "Initializing Data Handler", "Data Handler", LL_INFO );
    addSubject( boost::shared_ptr< WSubject >( new WSubject( WPersonalInformation::createDummyInformation() ) ) );
}

WDataHandler::~WDataHandler()
{
}

void WDataHandler::addSubject( boost::shared_ptr< WSubject > subject )
{
    WLogger::getLogger()->addLogMessage( "Adding subject with ID \"" +
                                         boost::lexical_cast< std::string >( subject->getPersonalInformation().getSubjectID() ) + "\" and Name \""
                                         + subject->getName() + "\".",
                                         "Data Handler", LL_DEBUG );

    // simply add the new subject
    m_subjects.push_back( subject );
}

void WDataHandler::removeSubject( boost::shared_ptr< WSubject > subject )
{
    m_subjectAccess->beginWrite();

    WLogger::getLogger()->addLogMessage( "Removing subject with ID \"" +
                                         boost::lexical_cast< std::string >( subject->getPersonalInformation().getSubjectID() ) + "\" and Name \""
                                         + subject->getName() + "\".",
                                         "Data Handler", LL_DEBUG );

    // iterate and find, remove
    for ( SubjectContainerType::iterator iter = m_subjectAccess->get().begin(); iter != m_subjectAccess->get().end();
            ++iter )
    {
        if ( ( *iter ) ==  subject )
        {
            m_subjectAccess->get().erase( iter );
            break;
        }
    }

    m_subjectAccess->endWrite();
}

void WDataHandler::clear()
{
    m_subjectAccess->beginWrite();

    WLogger::getLogger()->addLogMessage( "Removing all subjects.", "Data Handler", LL_INFO );

    for ( SubjectContainerType::const_iterator iter = m_subjectAccess->get().begin(); iter != m_subjectAccess->get().end();
            ++iter )
    {
        WLogger::getLogger()->addLogMessage( "Removing subject \"" +
                boost::lexical_cast< std::string >( ( *iter )->getPersonalInformation().getSubjectID() ) + "\".",
                "Data Handler", LL_DEBUG );

        ( *iter )->clear();
    }

    m_subjectAccess->get().clear();
    m_subjectAccess->endWrite();
}

WDataHandler::SubjectAccess WDataHandler::getAccessObject()
{
    return m_subjects.getAccessObject();
}

boost::shared_ptr< WSubject > WDataHandler::getSubjectByID( size_t subjectID )
{
    m_subjectAccess->beginRead();

    // search it
    boost::shared_ptr< WSubject > result;
    try
    {
        if ( subjectID < m_subjectAccess->get().size() )
        {
            result = m_subjectAccess->get().at( subjectID );
        }
        else
        {
            result = boost::shared_ptr< WSubject >();
        }
    }
    catch( const std::out_of_range& e )
    {
        throw WDHNoSuchSubject();
    }

    m_subjectAccess->endRead();

    return result;
}

boost::shared_ptr< WDataHandler > WDataHandler::getDataHandler()
{
    if ( !m_instance )
    {
        m_instance = boost::shared_ptr< WDataHandler >( new WDataHandler() );
    }

    return m_instance;
}

boost::shared_ptr< WSubject > WDataHandler::getDefaultSubject()
{
    return getDataHandler()->getSubjectByID( WSubject::SUBJECT_UNKNOWN );
}

void WDataHandler::registerDataSet( boost::shared_ptr< WDataSet > dataset )
{
    getDefaultSubject()->addDataSet( dataset );
}

void WDataHandler::deregisterDataSet( boost::shared_ptr< WDataSet > dataset )
{
    getDefaultSubject()->removeDataSet( dataset );
}

