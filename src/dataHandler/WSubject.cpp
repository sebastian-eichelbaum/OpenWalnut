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
    m_datasetAccess( m_datasets.getAccessObject() ),
    m_changeCondition( boost::shared_ptr< WConditionSet >( new WConditionSet() ) ),
    m_listChangeCondition( boost::shared_ptr< WConditionSet >( new WConditionSet() ) ),
    m_personalInfo( WPersonalInformation::createDummyInformation() )
{
}

WSubject::WSubject( WPersonalInformation personInfo ):
    m_datasets(),
    m_datasetAccess( m_datasets.getAccessObject() ),
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
    m_datasetAccess->beginWrite();

    // iterate and find, remove
    DatasetContainerType::iterator fIt = std::find( m_datasetAccess->get().begin(), m_datasetAccess->get().end(), dataset );
    m_datasetAccess->get().erase( fIt );

    // also deregister condition
    boost::shared_ptr< WCondition > c = dataset->getChangeCondition();
    if ( c.get() )
    {
        m_changeCondition->remove( c );
    }
    m_datasetAccess->endWrite();

    m_changeCondition->notify();
    m_listChangeCondition->notify();
}

void WSubject::clear()
{
    m_datasetAccess->beginWrite();

    // iterate and find, remove
    for ( DatasetContainerType::iterator iter = m_datasetAccess->get().begin(); iter != m_datasetAccess->get().end(); ++iter )
    {
        // also deregister condition
        boost::shared_ptr< WCondition > c = ( *iter )->getChangeCondition();
        if ( c.get() )
        {
            m_changeCondition->remove( c );
        }
    }
    m_datasetAccess->get().clear();

    m_datasetAccess->endWrite();

    m_listChangeCondition->notify();
}

// TODO(all): rethink this
//  wiebel: I deactivated this as we want to resort thes list ... so we have to rethinks this.
// boost::shared_ptr< WDataSet > WSubject::getDataSetByID( size_t datasetID )
// {
//     m_datasetAccess->beginRead();

//     // search it
//     boost::shared_ptr< WDataSet > result;
//     try
//     {
//         result = m_datasetAccess->get().at( datasetID );
//     }
//     catch( const std::out_of_range& e )
//     {
//         throw WDHNoSuchDataSet();
//     }

//     m_datasetAccess->endRead();

//     return result;
// }

std::vector< boost::shared_ptr< WDataTexture3D > > WSubject::getDataTextures( bool onlyActive )
{
    std::vector< boost::shared_ptr< WDataTexture3D > > tex;

    // iterate the list and find all textures
    m_datasetAccess->beginRead();

    for ( DatasetContainerType::iterator iter = m_datasetAccess->get().begin(); iter != m_datasetAccess->get().end(); ++iter )
    {
        // is it a texture?
        if ( ( *iter )->isTexture() && ( !onlyActive || ( *iter )->getTexture()->isGloballyActive() ) )
        {
            tex.push_back( ( *iter )->getTexture() );
        }
    }

    m_datasetAccess->endRead();
    return tex;
}

WSubject::DatasetAccess WSubject::getAccessObject()
{
    return m_datasets.getAccessObject();
}

boost::shared_ptr< WCondition > WSubject::getChangeCondition() const
{
    return m_changeCondition;
}

boost::shared_ptr< WCondition > WSubject::getListChangeCondition() const
{
    return m_listChangeCondition;
}

