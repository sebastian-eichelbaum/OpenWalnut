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

#include "WUpdateThread.h"
#include "WROIManagerFibers.h"

WROIManagerFibers::WROIManagerFibers()
{
    m_activeBitField = 0;
    m_recalcLock = false;
}

WROIManagerFibers::~WROIManagerFibers()
{
}

void WROIManagerFibers::addRoi( boost::shared_ptr< WROI > newRoi )
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

    for ( std::list< boost::function< void( boost::shared_ptr< WRMROIRepresentation > ) > >::iterator iter = m_notifiers.begin(); iter
            != m_notifiers.end(); ++iter )
    {
        ( *iter )( rroi );
    }
}

void WROIManagerFibers::addRoi( boost::shared_ptr< WROI > newRoi, boost::shared_ptr< WROI > parentRoi )
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

    for ( std::list< boost::function< void( boost::shared_ptr< WRMROIRepresentation > ) > >::iterator iter = m_notifiers.begin(); iter
            != m_notifiers.end(); ++iter )
    {
        ( *iter )( rroi );
    }
}

void WROIManagerFibers::removeRoi( boost::shared_ptr< WROI > /*roi */ )
{
}

void WROIManagerFibers::addFiberDataset( boost::shared_ptr< const WDataSetFibers > fibers )
{
    m_fibers = fibers;

    boost::shared_ptr< std::vector< float > > verts = fibers->getVertices();
    m_kdTree = boost::shared_ptr< WKdTree >( new WKdTree( verts->size() / 3, &( ( *verts )[0] ) ) );

    addBitField( fibers->size() );

    setDirty();
}

void WROIManagerFibers::removeFiberDataset( boost::shared_ptr< const WDataSetFibers > /*fibers*/ )
{
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
    WUpdateThread* t = new WUpdateThread( shared_from_this() );
    t->run();
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

void WROIManagerFibers::addDefaultNotifier( boost::function< void( boost::shared_ptr< WRMROIRepresentation > ) > notifier )
{
    boost::unique_lock< boost::shared_mutex > lock;
    lock = boost::unique_lock< boost::shared_mutex >( m_associatedNotifiersLock );
    m_notifiers.push_back( notifier );
    lock.unlock();
}
