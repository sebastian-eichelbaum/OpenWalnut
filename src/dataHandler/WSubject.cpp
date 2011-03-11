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
#include <vector>

#include <boost/lexical_cast.hpp>

#include "../common/WLogger.h"

#include "WDataSet.h"
#include "WDataTexture3D.h"
#include "exceptions/WDHNoSuchDataSet.h"

#include "WSubject.h"

WSubject::WSubject():
    m_datasets(),
    m_changeCondition( boost::shared_ptr< WConditionSet >( new WConditionSet() ) ),
    m_listChangeCondition( boost::shared_ptr< WConditionSet >( new WConditionSet() ) ),
    m_personalInfo( WPersonalInformation::createDummyInformation() )
{
}

WSubject::WSubject( WPersonalInformation personInfo ):
    m_datasets(),
    m_changeCondition( boost::shared_ptr< WConditionSet >( new WConditionSet() ) ),
    m_listChangeCondition( boost::shared_ptr< WConditionSet >( new WConditionSet() ) ),
    m_personalInfo( personInfo )
{
}

WSubject::~WSubject()
{
    clear();
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

    // also register condition
    boost::shared_ptr< WCondition > c = dataset->getChangeCondition();
    if ( c.get() )
    {
        m_changeCondition->add( c );
    }
    m_changeCondition->notify();
    m_listChangeCondition->notify();
}

void WSubject::removeDataSet( boost::shared_ptr< WDataSet > dataset )
{
    DatasetSharedContainerType::WriteTicket l = m_datasets.getWriteTicket();

    // iterate and find, remove
    DatasetIterator fIt = std::find( l->get().begin(), l->get().end(), dataset );
    l->get().erase( fIt );

    // also de-register condition
    boost::shared_ptr< WCondition > c = dataset->getChangeCondition();
    if ( c.get() )
    {
        m_changeCondition->remove( c );
    }

    // unlock if some callback notified below wants to access the list
    l.reset();

    m_changeCondition->notify();
    m_listChangeCondition->notify();
}

void WSubject::clear()
{
    DatasetSharedContainerType::WriteTicket l = m_datasets.getWriteTicket();

    // iterate and find, remove
    for ( DatasetIterator iter = l->get().begin(); iter != l->get().end(); ++iter )
    {
        // also de-register condition
        boost::shared_ptr< WCondition > c = ( *iter )->getChangeCondition();
        if ( c.get() )
        {
            m_changeCondition->remove( c );
        }
    }
    l->get().clear();

    // unlock if some callback notified below wants to access the list
    l.reset();

    m_listChangeCondition->notify();
}

WSubject::DatasetSharedContainerType::ReadTicket WSubject::getDatasets() const
{
    return m_datasets.getReadTicket();
}

WSubject::DatasetSharedContainerType::WriteTicket WSubject::getDatasetsForWriting() const
{
    return m_datasets.getWriteTicket();
}

std::vector< boost::shared_ptr< WDataTexture3D > > WSubject::getDataTextures( bool onlyActive )
{
    std::vector< boost::shared_ptr< WDataTexture3D > > tex;

    // Read lock the list, the lock is freed upon destruction of the ticket (if it goes out of scope for example).
    DatasetSharedContainerType::ReadTicket l = m_datasets.getReadTicket();

    // iterate the list and find all textures
    for ( DatasetConstIterator iter = l->get().begin(); iter != l->get().end(); ++iter )
    {
        // is it a texture?
        if ( ( *iter )->isTexture() && ( !onlyActive || ( *iter )->getTexture()->isGloballyActive() ) )
        {
            tex.push_back( ( *iter )->getTexture() );
        }
    }

    return tex;
}

boost::shared_ptr< WCondition > WSubject::getChangeCondition() const
{
    return m_changeCondition;
}

boost::shared_ptr< WCondition > WSubject::getListChangeCondition() const
{
    return m_listChangeCondition;
}
