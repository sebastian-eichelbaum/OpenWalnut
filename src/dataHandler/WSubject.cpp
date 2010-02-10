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

#include <boost/lexical_cast.hpp>

#include "../common/WLogger.h"

#include "WSubject.h"
#include "exceptions/WDHNoSuchDataSet.h"


WSubject::WSubject():
    m_datasets(),
    m_datasetAccess( m_datasets.getAccessObject() ),
    m_personalInfo( WPersonalInformation::createDummyInformation() )
{
}

WSubject::WSubject( WPersonalInformation personInfo ):
    m_datasets(),
    m_datasetAccess( m_datasets.getAccessObject() ),
    m_personalInfo( personInfo )
{
}

std::string WSubject::getName() const
{
    return m_personalInfo.getCompleteName();
}

WPersonalInformation WSubject::getPersonalInformation() const
{
    return m_personalInfo;
}

void WSubject::addDataSet( boost::shared_ptr< WDataSet > dataset )
{
    // simply add the new dataset
    m_datasets.push_back( dataset );
}

void WSubject::removeDataSet( boost::shared_ptr< WDataSet > dataset )
{
    m_datasetAccess->beginWrite();

    // iterate and find, remove
    remove( m_datasetAccess->get().begin(), m_datasetAccess->get().end(), dataset );

    m_datasetAccess->endWrite();
}

void WSubject::clear()
{
    m_datasetAccess->beginWrite();

    // iterate and find, remove
    m_datasetAccess->get().clear();

    m_datasetAccess->endWrite();
}

boost::shared_ptr< WDataSet > WSubject::getDataSetByID( size_t datasetID )
{
    m_datasetAccess->beginRead();

    // search it
    boost::shared_ptr< WDataSet > result;
    try
    {
        result = m_datasetAccess->get().at( datasetID );
    }
    catch( std::out_of_range& e )
    {
        throw WDHNoSuchDataSet();
    }

    m_datasetAccess->endWrite();

    return result;
}

WSubject::DatasetSharedContainerType::WSharedAccess WSubject::getAccessObject()
{
    return m_datasets.getAccessObject();
}

