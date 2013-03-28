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

#include <list>
#include <vector>

#include "../common/WAssert.h"

#include "../graphicsEngine/WGraphicsEngine.h"

#include "WROIManager.h"

WROIManager::WROIManager()
{
    m_properties = boost::shared_ptr< WProperties >( new WProperties( "Properties", "Module's properties" ) );
    m_dirty = m_properties->addProperty( "dirty", "dirty flag", true );
}

WROIManager::~WROIManager()
{
}

void WROIManager::addRoi( osg::ref_ptr< WROI > newRoi )
{
    // create new branch
    boost::shared_ptr< WRMBranch > newBranch = boost::shared_ptr< WRMBranch >( new WRMBranch( shared_from_this() ) );
    // add branch to list
    m_branches.push_back( newBranch );
    // add roi to branch
    newBranch->addRoi( newRoi );

    for( std::list< boost::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > >::iterator iter = m_addNotifiers.begin();
            iter != m_addNotifiers.end(); ++iter )
    {
        ( **iter )( newRoi );
    }
}

void WROIManager::addRoi( osg::ref_ptr< WROI > newRoi, osg::ref_ptr< WROI > parentRoi )
{
    // find branch
    boost::shared_ptr< WRMBranch > branch;
    for( std::list< boost::shared_ptr< WRMBranch > >::iterator iter = m_branches.begin(); iter != m_branches.end(); ++iter )
    {
        if( ( *iter ).get()->contains( parentRoi ) )
        {
            branch = ( *iter );
        }
    }
    // add roi to branch
    branch->addRoi( newRoi );

    for( std::list< boost::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > >::iterator iter = m_addNotifiers.begin();
            iter != m_addNotifiers.end(); ++iter )
    {
        ( **iter )( newRoi );
    }
}

void WROIManager::removeRoi( osg::ref_ptr< WROI > roi )
{
    WGraphicsEngine::getGraphicsEngine()->getScene()->remove( roi );

    for( std::list< boost::shared_ptr< WRMBranch > >::iterator iter = m_branches.begin(); iter != m_branches.end(); ++iter )
    {
        ( *iter )->removeRoi( roi );

        if( ( *iter )->empty() )
        {
            for( std::list< boost::shared_ptr< boost::function< void( boost::shared_ptr< WRMBranch > ) > > >::iterator iter2
                      = m_removeBranchNotifiers.begin();
                  iter2 != m_removeBranchNotifiers.end();
                  ++iter2 )
            {
                ( **iter2 )( *iter );
            }
            m_branches.erase( iter );
            break;
        }
    }
    setDirty();

    for( std::list< boost::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > >::iterator iter
              = m_removeNotifiers.begin();
          iter != m_removeNotifiers.end();
          ++iter )
    {
        ( **iter )( roi );
    }
}

void WROIManager::removeBranch( osg::ref_ptr< WROI > roi )
{
    for( std::list< boost::shared_ptr< WRMBranch > >::iterator iter = m_branches.begin(); iter != m_branches.end(); ++iter )
    {
        if( roi == ( *iter )->getFirstRoi() )
        {
            ( *iter )->removeAllRois();
        }

        if( ( *iter )->empty() )
        {
            for( std::list< boost::shared_ptr< boost::function< void( boost::shared_ptr< WRMBranch > ) > > >::iterator iter2
                      = m_removeBranchNotifiers.begin();
                  iter2 != m_removeBranchNotifiers.end();
                  ++iter2 )
            {
                ( **iter2 )( *iter );
            }
            m_branches.erase( iter );
            break;
        }
    }
    setDirty();
}

boost::shared_ptr< WRMBranch> WROIManager::getBranch( osg::ref_ptr< WROI > roi )
{
    boost::shared_ptr< WRMBranch> branch;

    for( std::list< boost::shared_ptr< WRMBranch > >::iterator iter = m_branches.begin(); iter != m_branches.end(); ++iter )
    {
        if( ( *iter )->contains( roi ) )
        {
            branch = ( *iter );
        }
    }
    return branch;
}

void WROIManager::setDirty()
{
    m_dirty->set( true );
}

void WROIManager::addAddNotifier( boost::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > notifier )
{
    boost::unique_lock< boost::shared_mutex > lock;
    lock = boost::unique_lock< boost::shared_mutex >( m_associatedNotifiersLock );
    m_addNotifiers.push_back( notifier );
    lock.unlock();
}

void WROIManager::removeAddNotifier( boost::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > notifier )
{
    boost::unique_lock< boost::shared_mutex > lock;
    lock = boost::unique_lock< boost::shared_mutex >( m_associatedNotifiersLock );
    std::list<  boost::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > >::iterator it;
    it = std::find( m_addNotifiers.begin(), m_addNotifiers.end(), notifier );
    if( it != m_addNotifiers.end() )
    {
        m_addNotifiers.erase( it );
    }
    lock.unlock();
}

void WROIManager::addRemoveNotifier( boost::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > notifier )
{
    boost::unique_lock< boost::shared_mutex > lock;
    lock = boost::unique_lock< boost::shared_mutex >( m_associatedNotifiersLock );
    m_removeNotifiers.push_back( notifier );
    lock.unlock();
}

void WROIManager::removeRemoveNotifier( boost::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > notifier )
{
    boost::unique_lock< boost::shared_mutex > lock;
    lock = boost::unique_lock< boost::shared_mutex >( m_associatedNotifiersLock );
    std::list<  boost::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > >::iterator it;
    it = std::find( m_removeNotifiers.begin(), m_removeNotifiers.end(), notifier );
    if( it != m_removeNotifiers.end() )
    {
        m_removeNotifiers.erase( it );
    }
    lock.unlock();
}

void WROIManager::addRemoveBranchNotifier(  boost::shared_ptr< boost::function< void( boost::shared_ptr< WRMBranch > ) > > notifier )
{
    boost::unique_lock< boost::shared_mutex > lock;
    lock = boost::unique_lock< boost::shared_mutex >( m_associatedNotifiersLock );
    m_removeBranchNotifiers.push_back( notifier );
    lock.unlock();
}

void WROIManager::removeRemoveBranchNotifier(  boost::shared_ptr< boost::function< void( boost::shared_ptr< WRMBranch > ) > > notifier )
{
    boost::unique_lock< boost::shared_mutex > lock;
    lock = boost::unique_lock< boost::shared_mutex >( m_associatedNotifiersLock );
    std::list<  boost::shared_ptr< boost::function< void( boost::shared_ptr< WRMBranch > ) > > >::iterator it;
    it = std::find( m_removeBranchNotifiers.begin(), m_removeBranchNotifiers.end(), notifier );
    if( it != m_removeBranchNotifiers.end() )
    {
        m_removeBranchNotifiers.erase( it );
    }
    lock.unlock();
}

void WROIManager::setSelectedRoi( osg::ref_ptr< WROI > roi )
{
    m_selectedRoi = roi;
}

osg::ref_ptr< WROI > WROIManager::getSelectedRoi()
{
    return m_selectedRoi;
}

WROIManager::ROIs WROIManager::getRois() const
{
    ROIs returnVec;

    for( std::list< boost::shared_ptr< WRMBranch > >::const_iterator iter = m_branches.begin(); iter != m_branches.end(); ++iter )
    {
        ( *iter )->getRois( returnVec );
    }
    return returnVec;
}

WROIManager::Branches WROIManager::getBranches() const
{
    // copy to this vec
    Branches returnVec;

    // copy
    for( std::list< boost::shared_ptr< WRMBranch > >::const_iterator iter = m_branches.begin(); iter != m_branches.end(); ++iter )
    {
        returnVec.push_back( *iter );
    }
    return returnVec;
}

