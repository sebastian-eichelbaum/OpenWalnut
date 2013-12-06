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
#include <iostream>
#include <list>
#include <vector>

#include "../kernel/WKernel.h"
#include "WFiberSelector.h"
#include "WROIManager.h"

WFiberSelector::WFiberSelector( boost::shared_ptr< const WDataSetFibers > fibers ) :
    m_fibers( fibers ),
    m_size( fibers->size() ),
    m_dirty( true ),
    m_dirtyCondition( boost::shared_ptr< WCondition >( new WCondition() ) )
{
    boost::shared_ptr< std::vector< float > > verts = m_fibers->getVertices();
    m_kdTree = boost::shared_ptr< WKdTree >( new WKdTree( verts->size() / 3, &( ( *verts )[0] ) ) );

    m_outputBitfield = boost::shared_ptr< std::vector< bool > >( new std::vector< bool >( m_size, true ) );
    m_outputColorMap = boost::shared_ptr< std::vector< float > >( new std::vector< float >( m_size * 4, 1.0 ) );

    std::vector< osg::ref_ptr< WROI > >rois = WKernel::getRunningKernel()->getRoiManager()->getRois();

    m_changeRoiSignal
        = boost::shared_ptr< boost::function< void() > >( new boost::function< void() >( boost::bind( &WFiberSelector::setDirty, this ) ) );

    m_assocRoiSignal =
        boost::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > >(
            new boost::function< void( osg::ref_ptr< WROI > ) > ( boost::bind( &WFiberSelector::slotAddRoi, this, _1 ) ) );
    WKernel::getRunningKernel()->getRoiManager()->addAddNotifier( m_assocRoiSignal );

    m_removeRoiSignal =
             boost::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > >(
                 new boost::function< void( osg::ref_ptr< WROI > ) > ( boost::bind( &WFiberSelector::slotRemoveRoi, this, _1 ) ) );
    WKernel::getRunningKernel()->getRoiManager()->addRemoveNotifier( m_removeRoiSignal );

    m_removeBranchSignal =
             boost::shared_ptr< boost::function< void( boost::shared_ptr< WRMBranch >  ) > >(
                 new boost::function< void( boost::shared_ptr< WRMBranch > ) > (
                     boost::bind( &WFiberSelector::slotRemoveBranch, this, _1 ) ) );
    WKernel::getRunningKernel()->getRoiManager()->addRemoveBranchNotifier( m_removeBranchSignal );

    for( size_t i = 0; i < rois.size(); ++i )
    {
        slotAddRoi( rois[i] );
        ( rois[i] )->getProperties()->getProperty( "Dirty" )->toPropBool()->set( true );
    }
}

WFiberSelector::~WFiberSelector()
{
    WKernel::getRunningKernel()->getRoiManager()->removeAddNotifier( m_assocRoiSignal );
    WKernel::getRunningKernel()->getRoiManager()->removeRemoveNotifier( m_removeRoiSignal );
    WKernel::getRunningKernel()->getRoiManager()->removeRemoveBranchNotifier( m_removeBranchSignal );

    // We need the following because not all ROIs are removed per slot below
    {
        for( std::list< boost::shared_ptr< WSelectorBranch > >::iterator iter = m_branches.begin(); iter != m_branches.end(); ++iter )
        {
            std::list< boost::shared_ptr< WSelectorRoi > > rois = ( *iter )->getROIs();
            for( std::list< boost::shared_ptr< WSelectorRoi > >::iterator roiIter = rois.begin(); roiIter != rois.end(); ++roiIter )
            {
                ( *roiIter )->getRoi()->removeROIChangeNotifier( m_changeRoiSignal );
            }
            ( *iter )->getBranch()->removeChangeNotifier( m_changeRoiSignal );
        }
    }
    m_branches.clear();
}

void WFiberSelector::slotAddRoi( osg::ref_ptr< WROI > roi )
{
    boost::shared_ptr< WSelectorBranch > branch;

    for( std::list< boost::shared_ptr< WSelectorBranch > >::iterator iter = m_branches.begin(); iter != m_branches.end(); ++iter )
    {
        if( ( *iter )->getBranch() == WKernel::getRunningKernel()->getRoiManager()->getBranch( roi ) )
        {
            branch = ( *iter );
        }
    }
    if( !branch )
    {
        branch = boost::shared_ptr<WSelectorBranch>(
                new WSelectorBranch( m_fibers, WKernel::getRunningKernel()->getRoiManager()->getBranch( roi ) ) );
        branch->getBranch()->addChangeNotifier( m_changeRoiSignal );
        m_branches.push_back( branch );
    }

    boost::shared_ptr< WSelectorRoi> sroi( new WSelectorRoi( roi, m_fibers, m_kdTree ) );

    branch->addRoi( sroi );
    sroi->getRoi()->addROIChangeNotifier( m_changeRoiSignal );

    setDirty();
}

void WFiberSelector::slotRemoveRoi( osg::ref_ptr< WROI > roi )
{
    roi->removeROIChangeNotifier( m_changeRoiSignal );
    for( std::list< boost::shared_ptr< WSelectorBranch > >::iterator iter = m_branches.begin(); iter != m_branches.end(); ++iter )
    {
        ( *iter )->removeRoi( roi );

        if( (*iter )->empty() )
        {
            ( *iter )->getBranch()->removeChangeNotifier( m_changeRoiSignal );
            m_branches.erase( iter );
            break;
        }
    }
    setDirty();
}

void WFiberSelector::slotRemoveBranch( boost::shared_ptr< WRMBranch > branch )
{
    for( std::list< boost::shared_ptr< WSelectorBranch > >::iterator iter = m_branches.begin(); iter != m_branches.end(); ++iter )
    {
        if( branch == ( *iter )->getBranch() )
        {
            // remove notifier
            branch->removeChangeNotifier( m_changeRoiSignal );
            m_branches.erase( iter );
            break;
        }
    }
    setDirty();
}

boost::shared_ptr< std::vector< bool > > WFiberSelector::getBitfield()
{
    return m_outputBitfield;
}

void WFiberSelector::recalculate()
{
    boost::shared_ptr< std::vector< bool > > m_workerBitfield( new std::vector< bool >( m_size, false ) );
    std::vector< float > m_workerColorMap( m_size * 4, 1.0 );

    if( !m_branches.empty() )
    {
        for( std::list< boost::shared_ptr< WSelectorBranch > >::iterator iter = m_branches.begin(); iter != m_branches.end(); ++iter )
        {
            boost::shared_ptr< std::vector< bool > > bf = ( *iter )->getBitField();
            WColor color = ( *iter )->getBranchColor();

            for( size_t i = 0; i < m_size; ++i )
            {
                ( *m_workerBitfield )[i] = ( *m_workerBitfield )[i] | ( *bf )[i];

                if( ( *bf )[i] )
                {
                    // set colors, overwrite previously set colors
                    m_workerColorMap[ 4 * i + 0 ] = color.r();
                    m_workerColorMap[ 4 * i + 1 ] = color.g();
                    m_workerColorMap[ 4 * i + 2 ] = color.b();
                    m_workerColorMap[ 4 * i + 3 ] = color.a();
                }
            }
        }
    }

    for( size_t i = 0; i < m_size; ++i )
    {
      ( *m_outputBitfield )[i] = ( *m_workerBitfield )[i];
      ( *m_outputColorMap )[ 4 * i + 0 ] = m_workerColorMap[ 4 * i + 0 ];
      ( *m_outputColorMap )[ 4 * i + 1 ] = m_workerColorMap[ 4 * i + 1 ];
      ( *m_outputColorMap )[ 4 * i + 2 ] = m_workerColorMap[ 4 * i + 2 ];
      ( *m_outputColorMap )[ 4 * i + 3 ] = m_workerColorMap[ 4 * i + 3 ];
    }
    m_dirty = false;
}

void WFiberSelector::setDirty()
{
    m_dirty = true;
    m_dirtyCondition->notify();
    recalculate();
}

bool WFiberSelector::getDirty()
{
    return m_dirty;
}

WCondition::SPtr WFiberSelector::getDirtyCondition()
{
    return m_dirtyCondition;
}

WColor WFiberSelector::getFiberColor( size_t fidx ) const
{
    if( fidx >= m_outputBitfield->size() )
    {
        return WColor( 1.0, 1.0, 1.0, 1.0 );
    }

    return WColor( ( *m_outputColorMap )[ 4 * fidx + 0 ],
                   ( *m_outputColorMap )[ 4 * fidx + 1 ],
                   ( *m_outputColorMap )[ 4 * fidx + 2 ],
                   ( *m_outputColorMap )[ 4 * fidx + 3 ] );
}

bool WFiberSelector::isNothingFiltered() const
{
    return m_branches.empty();
}
