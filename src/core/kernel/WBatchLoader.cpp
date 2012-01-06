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

#include "WModule.h"
#include "WModuleContainer.h"
#include "WModuleFactory.h"

#include "WBatchLoader.h"

WBatchLoader::WBatchLoader( std::vector< std::string > fileNames, boost::shared_ptr< WModuleContainer > targetContainer ):
    WThreadedRunner(),
    boost::enable_shared_from_this< WBatchLoader >(),
    m_fileNamesToLoad( fileNames ),
    m_targetContainer( targetContainer ),
    m_suppressColormaps( false )
{
    // initialize members
}

WBatchLoader::~WBatchLoader()
{
    // cleanup
}

void WBatchLoader::run()
{
    // the module needs to be added here, as it else could be freed before the thread finishes ( remember: it is a shared_ptr).
    m_targetContainer->addPendingThread( shared_from_this() );

    // actually run
    WThreadedRunner::run();
}

void WBatchLoader::threadMain()
{
    // add a new data module for each file to load
    for( std::vector< std::string >::iterator iter = m_fileNamesToLoad.begin(); iter != m_fileNamesToLoad.end(); ++iter )
    {
        boost::shared_ptr< WModule > mod = WModuleFactory::getModuleFactory()->create(
                WModuleFactory::getModuleFactory()->getPrototypeByName( "Data Module" )
        );
        WDataModule::SPtr dmod = boost::shared_static_cast< WDataModule >( mod );
        dmod->setSuppressColormaps( m_suppressColormaps );

        // set the filename
        dmod->setFilename( *iter );

        m_targetContainer->add( mod );
        // serialize loading of a couple of data sets
        // ignore the case where isCrashed is true
        mod->isReadyOrCrashed().wait();

        // add module to the list
        m_dataModules.push_back( dmod );
    }

    m_targetContainer->finishedPendingThread( shared_from_this() );
}

WBatchLoader::DataModuleList::ReadTicket WBatchLoader::getDataModuleList() const
{
    return m_dataModules.getReadTicket();
}

void WBatchLoader::setSuppressColormaps( bool suppress )
{
    m_suppressColormaps = suppress;
}

bool WBatchLoader::getSuppressColormaps() const
{
    return m_suppressColormaps;
}

