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

#include "WROIManagerFibers.h"

#include "WRMBranch.h"


WRMBranch::WRMBranch( boost::shared_ptr< WROIManagerFibers > roiManager ) :
    m_dirty( true ),
    m_roiManager( roiManager )
{
    m_size = m_roiManager->size();

    properties();
}

WRMBranch::~WRMBranch()
{
}

void WRMBranch::properties()
{
    m_properties = boost::shared_ptr< WProperties >( new WProperties( "Properties", "This branch's properties" ) );
    m_isNot = m_properties->addProperty( "NOT", "description", false, boost::bind( &WRMBranch::propertyChanged, this ) );
    m_bundleColor = m_properties->addProperty( "Bundle Color", "description", WColor( 1.0, 0.0, 0.0, 1.0 ),
            boost::bind( &WRMBranch::propertyChanged, this ) );
}

void WRMBranch::propertyChanged()
{
    if ( m_bundleColor->changed() )
    {
        m_roiManager->updateBundleColor( shared_from_this(), m_bundleColor->get( true ) );
    }
    setDirty();
}


void WRMBranch::addRoi( boost::shared_ptr< WRMROIRepresentation > roi )
{
    m_rois.push_back( roi );
    setDirty();
}

void WRMBranch::removeRoi( boost::shared_ptr< WRMROIRepresentation > roi )
{
    for( std::list< boost::shared_ptr< WRMROIRepresentation > >::iterator iter = m_rois.begin(); iter != m_rois.end(); ++iter )
    {
        if ( ( *iter ) == roi )
        {
            m_rois.erase( iter );
            setDirty();
            break;
        }
    }
}

void WRMBranch::removeAllRois()
{
    m_rois.clear();
}

boost::shared_ptr< std::vector< bool > > WRMBranch::getBitField()
{
    if ( m_dirty )
    {
        recalculate();
    }
    return m_outputBitfield;
}

void WRMBranch::recalculate()
{
    bool atLeastOneActive = false;

    for( std::list< boost::shared_ptr< WRMROIRepresentation > >::iterator iter = m_rois.begin(); iter != m_rois.end(); ++iter )
    {
        if ( ( *iter )->active() )
        {
            atLeastOneActive = true;
        }
    }

    if ( atLeastOneActive )
    {
        m_workerBitfield = boost::shared_ptr< std::vector< bool > >( new std::vector< bool >( m_size, true ) );

        for( std::list< boost::shared_ptr< WRMROIRepresentation > >::iterator iter = m_rois.begin(); iter != m_rois.end(); ++iter )
        {
            if ( ( *iter )->active() )
            {
                boost::shared_ptr< std::vector<bool> > bf = ( *iter )->getBitField();
                bool isnot = ( *iter )->getROI()->isNot();
                if ( !isnot )
                {
                    for ( size_t i = 0 ; i < m_size ; ++i )
                    {
                        ( *m_workerBitfield )[i] = ( *m_workerBitfield )[i] & ( *bf )[i];
                    }
                }
                else
                {
                    for ( size_t i = 0 ; i < m_size ; ++i )
                    {
                        ( *m_workerBitfield )[i] = ( *m_workerBitfield )[i] & !( *bf )[i];
                    }
                }
            }
        }

        if ( m_isNot->get() )
        {
           for ( size_t i = 0 ; i < m_size ; ++i )
            {
                ( *m_workerBitfield )[i] = !( *m_workerBitfield )[i];
            }
        }
    }
    else
    {
        m_workerBitfield = boost::shared_ptr< std::vector< bool > >( new std::vector< bool >( m_size, false ) );
    }

    m_dirty = false;

    m_outputBitfield = m_workerBitfield;
}

void WRMBranch::setDirty()
{
    m_dirty = true;
    m_roiManager->setDirty();
}

boost::shared_ptr< WRMROIRepresentation >WRMBranch::getFirstRoi()
{
    return m_rois.front();
}

boost::shared_ptr< WROIManagerFibers > WRMBranch::getRoiManager()
{
    return m_roiManager;
}

boost::shared_ptr< WProperties > WRMBranch::getProperties()
{
    return m_properties;
}
