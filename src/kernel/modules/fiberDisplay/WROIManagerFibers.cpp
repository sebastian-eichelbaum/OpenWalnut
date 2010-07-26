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

WROIManagerFibers::WROIManagerFibers()
{
    m_activeBitField = 0;
    m_recalcLock = false;
}

WROIManagerFibers::~WROIManagerFibers()
{
}

boost::shared_ptr< WRMROIRepresentation > WROIManagerFibers::addRoi( osg::ref_ptr< WROI > newRoi )
{
    // create new branch
    boost::shared_ptr< WRMBranch > newBranch = boost::shared_ptr< WRMBranch >( new WRMBranch( shared_from_this() ) );
    // create roi
    boost::shared_ptr< WRMROIRepresentation > rroi = boost::shared_ptr< WRMROIRepresentation >( new WRMROIRepresentation( newRoi, newBranch ) );
    // add roi to branch
    newBranch->addRoi( rroi );
    // add branch to list
    m_branches.push_back( newBranch );
    // add bit fields
    newBranch->addBitField( m_fibers.get()->size() );

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
    // add bit fields
    rroi->addBitField( m_fibers.get()->size() );
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
    if ( m_recalcLock )
        return;
    m_recalcLock = true;

    roi->removeFromGE();

    for ( std::list< boost::shared_ptr< WRMBranch > >::iterator iter = m_branches.begin(); iter != m_branches.end(); ++iter )
    {
        ( *iter )->removeRoi( roi );

        if ( (*iter )->isEmpty() )
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
    if ( m_recalcLock )
        return;
    m_recalcLock = true;

    for ( std::list< boost::shared_ptr< WRMBranch > >::iterator iter = m_branches.begin(); iter != m_branches.end(); ++iter )
    {
        if ( roi == ( *iter )->getFirstRoi() )
        {
            ( *iter )->removeAllRois();
        }

        if ( (*iter )->isEmpty() )
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

    boost::shared_ptr< std::vector< float > > verts = fibers->getVertices();
    m_kdTree = boost::shared_ptr< WKdTree >( new WKdTree( verts->size() / 3, &( ( *verts )[0] ) ) );

    addBitField( fibers->size() );
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
    if ( m_activeBitField == 0 )
    {
        return m_bitField;
    }
    else
    {
        return m_bitField2;
    }
}

void WROIManagerFibers::addBitField( size_t size )
{
    boost::shared_ptr< std::vector< bool > > bf = boost::shared_ptr< std::vector< bool > >( new std::vector< bool >( size, false ) );
    m_bitField = bf;
    boost::shared_ptr< std::vector< bool > > bf2 = boost::shared_ptr< std::vector< bool > >( new std::vector< bool >( size, false ) );
    m_bitField2 = bf2;

    for ( std::list< boost::shared_ptr< WRMBranch > >::iterator iter = m_branches.begin(); iter != m_branches.end(); ++iter )
    {
        ( *iter ).get()->addBitField( size );
    }
    setDirty();
}

void WROIManagerFibers::recalculate()
{
//    boost::unique_lock<boost::shared_mutex> slock;
//    slock = boost::unique_lock<boost::shared_mutex>( m_updateLock );
    if ( m_recalcLock )
        return;
    m_recalcLock = true;

    boost::shared_ptr< std::vector< bool > > mbf;
    if ( m_activeBitField == 0 )
    {
        mbf = m_bitField2;
    }
    else
    {
        mbf = m_bitField;
    }

    int size = mbf->size();
    mbf->clear();

    if ( m_branches.empty() )
    {
        mbf->resize( size, true );
        if ( m_activeBitField == 0 )
        {
            m_activeBitField = 1;
        }
        else
        {
            m_activeBitField = 0;
        }
        m_recalcLock = false;
        //slock.unlock();
        return;
    }
    else
    {
        mbf->resize( size, false );
    }

    for ( std::list< boost::shared_ptr< WRMBranch > >::iterator iter = m_branches.begin(); iter != m_branches.end(); ++iter )
    {
        boost::shared_ptr< std::vector< bool > > bf = ( *iter )->getBitField();

        for ( size_t i = 0; i < mbf->size(); ++i )
        {
            mbf->at( i ) = mbf->at( i ) | bf->at( i );
        }
    }
    if ( m_activeBitField == 0 )
    {
        m_activeBitField = 1;
    }
    else
    {
        m_activeBitField = 0;
    }
    m_dirty = true;

    //slock.unlock();
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

bool WROIManagerFibers::isDirty()
{
    return m_dirty;
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
