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

#include <iostream>

#include "../kernel/WKernel.h"

#include "WFiberSelector.h"

WFiberSelector::WFiberSelector( boost::shared_ptr< const WDataSetFibers > fibers ) :
    m_fibers( fibers ),
    m_size( fibers->size() ),
    m_dirty( true )
{
    boost::shared_ptr< std::vector< float > > verts = m_fibers->getVertices();
    m_kdTree = boost::shared_ptr< WKdTree >( new WKdTree( verts->size() / 3, &( ( *verts )[0] ) ) );

    m_outputBitfield = boost::shared_ptr< std::vector< bool > >( new std::vector< bool >( m_size, true ) );

    std::vector< osg::ref_ptr< WROI > >rois = WKernel::getRunningKernel()->getRoiManager()->getRois();

    for ( size_t i = 0; i < rois.size(); ++i )
    {
        slotAddRoi( rois[i] );
        rois[i].get()->getProperties()->getProperty( "Dirty" )->toPropBool()->set( true );
    }

    boost::function< void( osg::ref_ptr< WROI > ) > assocRoiSignal =
            boost::bind( &WFiberSelector::slotAddRoi, this, _1 );
    WKernel::getRunningKernel()->getRoiManager()->addAddNotifier( assocRoiSignal );
    boost::function< void( osg::ref_ptr< WROI > ) > removeRoiSignal =
            boost::bind( &WFiberSelector::slotRemoveRoi, this, _1 );
    WKernel::getRunningKernel()->getRoiManager()->addRemoveNotifier( removeRoiSignal );

    boost::function< void( boost::shared_ptr< WRMBranch > ) > removeBranchSignal =
            boost::bind( &WFiberSelector::slotRemoveBranch, this, _1 );
    WKernel::getRunningKernel()->getRoiManager()->addRemoveBranchNotifier( removeBranchSignal );
}

WFiberSelector::~WFiberSelector()
{
}

void WFiberSelector::slotAddRoi( osg::ref_ptr< WROI > roi )
{
    boost::shared_ptr< WSelectorBranch > branch;

    for ( std::list< boost::shared_ptr< WSelectorBranch > >::iterator iter = m_branches.begin(); iter != m_branches.end(); ++iter )
    {
        if ( ( *iter )->getBranch() == WKernel::getRunningKernel()->getRoiManager()->getBranch( roi ) )
        {
            branch = ( *iter );
        }
    }
    if ( !( branch.get() ) )
    {
        branch = boost::shared_ptr<WSelectorBranch>(
                new WSelectorBranch( m_fibers, WKernel::getRunningKernel()->getRoiManager()->getBranch( roi ) ) );
        m_branches.push_back( branch );
    }

    boost::shared_ptr< WSelectorRoi> sroi = boost::shared_ptr< WSelectorRoi>( new WSelectorRoi( roi, m_fibers, m_kdTree ) );

    branch->addRoi( sroi );

    boost::function< void() > changeRoiSignal = boost::bind( &WFiberSelector::setDirty, this );
    sroi->getRoi()->addChangeNotifier( changeRoiSignal );

    setDirty();
}

void WFiberSelector::slotRemoveRoi( osg::ref_ptr< WROI > roi )
{
    for ( std::list< boost::shared_ptr< WSelectorBranch > >::iterator iter = m_branches.begin(); iter != m_branches.end(); ++iter )
    {
        ( *iter )->removeRoi( roi );

        if ( (*iter )->empty() )
        {
            m_branches.erase( iter );
            break;
        }
    }
    setDirty();
}

void WFiberSelector::slotRemoveBranch( boost::shared_ptr< WRMBranch > branch )
{
    for ( std::list< boost::shared_ptr< WSelectorBranch > >::iterator iter = m_branches.begin(); iter != m_branches.end(); ++iter )
    {
        if ( branch == ( *iter )->getBranch() )
        {
            m_branches.erase( iter );
            break;
        }
    }
    setDirty();
}

boost::shared_ptr< std::vector< bool > > WFiberSelector::getBitfield()
{
    for ( std::list< boost::shared_ptr< WSelectorBranch > >::iterator iter = m_branches.begin(); iter != m_branches.end(); ++iter )
    {
        m_dirty = std::max( m_dirty, ( *iter )->dirty() );
    }

    if ( m_dirty )
    {
        recalculate();
    }
    return m_outputBitfield;
}

void WFiberSelector::recalculate()
{
    if ( m_branches.empty() )
    {
        m_workerBitfield = boost::shared_ptr< std::vector< bool > >( new std::vector< bool >( m_size, true ) );
    }
    else
    {
        m_workerBitfield = boost::shared_ptr< std::vector< bool > >( new std::vector< bool >( m_size, false ) );

        for ( std::list< boost::shared_ptr< WSelectorBranch > >::iterator iter = m_branches.begin(); iter != m_branches.end(); ++iter )
        {
            boost::shared_ptr< std::vector< bool > > bf = ( *iter )->getBitField();

            for ( size_t i = 0; i < m_size; ++i )
            {
                ( *m_workerBitfield )[i] = ( *m_workerBitfield )[i] | ( *bf )[i];
            }
        }
    }

    for ( size_t i = 0; i < m_size; ++i )
    {
      ( *m_outputBitfield )[i] = ( *m_workerBitfield )[i];
    }
    m_dirty = false;
    //m_outputBitfield = m_workerBitfield;
}

void WFiberSelector::setDirty()
{
    recalculate();
    m_dirty = true;
}
