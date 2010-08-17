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

#include "../../../common/WAssert.h"
#include "WUpdateThread.h"
#include "WROIManagerFibers.h"
#include "../../../graphicsEngine/WROIBox.h"

WROIManagerFibers::WROIManagerFibers() :
    m_recalcLock( false )
{
}

WROIManagerFibers::~WROIManagerFibers()
{
}

boost::shared_ptr< WRMROIRepresentation > WROIManagerFibers::addRoi( osg::ref_ptr< WROI > newRoi )
{
    // create new branch
    boost::shared_ptr< WRMBranch > newBranch = boost::shared_ptr< WRMBranch >( new WRMBranch( shared_from_this() ) );
    // add branch to list
    m_branches.push_back( newBranch );
    // create roi
    boost::shared_ptr< WRMROIRepresentation > rroi = boost::shared_ptr< WRMROIRepresentation >( new WRMROIRepresentation( newRoi, newBranch ) );
    // add roi to branch
    newBranch->addRoi( rroi );

    for ( std::list< boost::function< void( boost::shared_ptr< WRMROIRepresentation > ) > >::iterator iter = m_addNotifiers.begin();
            iter != m_addNotifiers.end(); ++iter )
    {
        ( *iter )( rroi );
    }
    return rroi;
}

boost::shared_ptr< WRMROIRepresentation > WROIManagerFibers::addRoi( osg::ref_ptr< WROI > newRoi, osg::ref_ptr< WROI > parentRoi )
{
    // find branch
    boost::shared_ptr< WRMBranch > branch;
    for ( std::list< boost::shared_ptr< WRMBranch > >::iterator iter = m_branches.begin(); iter != m_branches.end(); ++iter )
    {
        if ( ( *iter ).get()->getFirstRoi()->getROI() == parentRoi )
        {
            branch = ( *iter );
        }
    }
    // create roi
    boost::shared_ptr< WRMROIRepresentation > rroi = boost::shared_ptr< WRMROIRepresentation >( new WRMROIRepresentation( newRoi, branch ) );
    // add roi to branch
    branch->addRoi( rroi );

    for ( std::list< boost::function< void( boost::shared_ptr< WRMROIRepresentation > ) > >::iterator iter = m_addNotifiers.begin();
            iter != m_addNotifiers.end(); ++iter )
    {
        ( *iter )( rroi );
    }

    return rroi;
}

void WROIManagerFibers::removeRoi( boost::shared_ptr< WRMROIRepresentation > roi )
{
    while ( m_recalcLock )
    {
        boost::this_thread::sleep( boost::posix_time::seconds( 10 ) );
    }
    m_recalcLock = true;

    roi->removeFromGE();

    for ( std::list< boost::shared_ptr< WRMBranch > >::iterator iter = m_branches.begin(); iter != m_branches.end(); ++iter )
    {
        ( *iter )->removeRoi( roi );

        if ( (*iter )->empty() )
        {
            m_branches.erase( iter );
            break;
        }
    }

    m_recalcLock = false;
    setDirty();

    for ( std::list< boost::function< void( boost::shared_ptr< WRMROIRepresentation > ) > >::iterator iter = m_removeNotifiers.begin();
            iter != m_removeNotifiers.end(); ++iter )
    {
        ( *iter )( roi );
    }
}

void WROIManagerFibers::removeBranch( boost::shared_ptr< WRMROIRepresentation > roi )
{
    while ( m_recalcLock )
    {
        boost::this_thread::sleep( boost::posix_time::seconds( 10 ) );
    }
    m_recalcLock = true;

    for ( std::list< boost::shared_ptr< WRMBranch > >::iterator iter = m_branches.begin(); iter != m_branches.end(); ++iter )
    {
        if ( roi == ( *iter )->getFirstRoi() )
        {
            ( *iter )->removeAllRois();
        }

        if ( (*iter )->empty() )
        {
            m_branches.erase( iter );
            break;
        }
    }

    m_recalcLock = false;
    setDirty();
}

void WROIManagerFibers::addFiberDataset( boost::shared_ptr< const WDataSetFibers > fibers )
{
    m_fibers = fibers;

    m_size = fibers->size();

    boost::shared_ptr< std::vector< float > > verts = fibers->getVertices();
    m_kdTree = boost::shared_ptr< WKdTree >( new WKdTree( verts->size() / 3, &( ( *verts )[0] ) ) );

    m_outputBitfield = boost::shared_ptr< std::vector< bool > >( new std::vector< bool >( m_size, false ) );
    createCustomColorArray();

    setDirty();
}

void WROIManagerFibers::removeFiberDataset( boost::shared_ptr< const WDataSetFibers > /*fibers*/ )
{
    WAssert( false, "removeFiberDataset( --- ) not implemented yet." );
}

boost::shared_ptr< std::vector< bool > > WROIManagerFibers::getBitField()
{
    m_dirty = false;
    return m_outputBitfield;
}

void WROIManagerFibers::recalculate()
{
    while ( m_recalcLock )
    {
        boost::this_thread::sleep( boost::posix_time::seconds( 10 ) );
    }
    m_recalcLock = true;

    if ( m_branches.empty() )
    {
        m_outputBitfield = boost::shared_ptr< std::vector< bool > >( new std::vector< bool >( m_size, true ) );
        m_recalcLock = false;
        return;
    }
    else
    {
        m_workerBitfield = boost::shared_ptr< std::vector< bool > >( new std::vector< bool >( m_size, false ) );
    }

    for ( std::list< boost::shared_ptr< WRMBranch > >::iterator iter = m_branches.begin(); iter != m_branches.end(); ++iter )
    {
        boost::shared_ptr< std::vector< bool > > bf = ( *iter )->getBitField();

        for ( size_t i = 0; i < m_size; ++i )
        {
            ( *m_workerBitfield )[i] = ( *m_workerBitfield )[i] | ( *bf )[i];
        }
    }

    m_outputBitfield = m_workerBitfield;
    m_dirty = true;
    m_recalcLock = false;
}

void WROIManagerFibers::setDirty()
{
    boost::unique_lock< boost::shared_mutex > lock;
    {
        boost::shared_ptr< WUpdateThread > t = boost::shared_ptr< WUpdateThread >( new WUpdateThread( shared_from_this() ) );
        t->run();
        lock = boost::unique_lock< boost::shared_mutex >( m_updateListLock );
        m_updateThreads.push_back( t );
        lock.unlock();
    }

    {
        lock = boost::unique_lock< boost::shared_mutex >( m_updateListLock );

        std::list< boost::shared_ptr< WUpdateThread > >::iterator itr;

        // remove finished threads
        for( itr = m_updateThreads.begin(); itr != m_updateThreads.end(); )
        {
            if( ( *itr )->isFinished() )
            {
                itr = m_updateThreads.erase( itr );
            }
            else
            {
                ++itr;
            }
        }

        lock.unlock();
    }
}

boost::shared_ptr< const WDataSetFibers > WROIManagerFibers::getDataSet()
{
    return m_fibers;
}

boost::shared_ptr< WKdTree > WROIManagerFibers::getKdTree()
{
    return m_kdTree;
}

void WROIManagerFibers::addAddNotifier( boost::function< void( boost::shared_ptr< WRMROIRepresentation > ) > notifier )
{
    boost::unique_lock< boost::shared_mutex > lock;
    lock = boost::unique_lock< boost::shared_mutex >( m_associatedNotifiersLock );
    m_addNotifiers.push_back( notifier );
    lock.unlock();
}

void WROIManagerFibers::addRemoveNotifier( boost::function< void( boost::shared_ptr< WRMROIRepresentation > ) > notifier )
{
    boost::unique_lock< boost::shared_mutex > lock;
    lock = boost::unique_lock< boost::shared_mutex >( m_associatedNotifiersLock );
    m_removeNotifiers.push_back( notifier );
    lock.unlock();
}

void WROIManagerFibers::createCustomColorArray()
{
    boost::shared_ptr< std::vector< float > > colors = m_fibers->getGlobalColors();
    m_customColors = boost::shared_ptr< std::vector< float > >( new std::vector< float >( ( *colors ).size() ) );

    for ( size_t i = 0; i < ( *colors ).size(); ++i )
    {
        ( *m_customColors )[i] = ( *colors )[i];
    }
}

boost::shared_ptr< std::vector< float > >WROIManagerFibers::getCustomColors()
{
    return m_customColors;
}

void WROIManagerFibers::updateBundleColor( boost::shared_ptr<WRMBranch> branch, WColor color )
{
    boost::shared_ptr< std::vector< bool > > bf = branch->getBitField();
    boost::shared_ptr< std::vector< size_t > > starts = m_fibers->getLineStartIndexes();
    boost::shared_ptr< std::vector< size_t > > lengths = m_fibers->getLineLengths();

    for ( size_t i = 0; i < bf->size(); ++i )
    {
        if ( (*bf)[i] )
        {
            for ( size_t k = (*starts)[i]; k < (*starts)[i] + (*lengths)[i]; ++k)
            {
                (*m_customColors)[k*3] = color.getRed();
                (*m_customColors)[k*3+1] = color.getGreen();
                (*m_customColors)[k*3+2] = color.getBlue();
            }
        }
    }
}

void WROIManagerFibers::setSelectedRoi( boost::shared_ptr< WRMROIRepresentation > roi )
{
    m_selectedRoi = roi;
}

boost::shared_ptr< WRMROIRepresentation > WROIManagerFibers::getSelectedRoi()
{
    return m_selectedRoi;
}
