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

#include "WSelectorBranch.h"

WSelectorBranch::WSelectorBranch( size_t size, boost::shared_ptr<WRMBranch> branch ) :
    m_size( size ),
    m_dirty( true ),
    m_branch( branch )
{
    m_bitField = boost::shared_ptr< std::vector<bool> >( new std::vector<bool>( m_size, false ) );
}

WSelectorBranch::~WSelectorBranch()
{
}

void WSelectorBranch::addRoi( boost::shared_ptr< WSelectorRoi> roi )
{
    m_rois.push_back( roi );

    boost::function< void() > changeRoiSignal = boost::bind( &WSelectorBranch::setDirty, this );
    roi->getRoi()->addChangeNotifier( changeRoiSignal );
}

void WSelectorBranch::setDirty()
{
    m_dirty = true;
}

void WSelectorBranch::removeRoi( osg::ref_ptr< WROI > roi )
{
    for( std::list< boost::shared_ptr< WSelectorRoi > >::iterator iter = m_rois.begin(); iter != m_rois.end(); ++iter )
    {
        if ( ( *iter )->getRoi() == roi )
        {
            m_rois.erase( iter );
            break;
        }
    }
}

void WSelectorBranch::recalculate()
{
    bool atLeastOneActive = false;

    for( std::list< boost::shared_ptr< WSelectorRoi > >::iterator iter = m_rois.begin(); iter != m_rois.end(); ++iter )
    {
        if ( ( *iter )->getRoi()->active() )
        {
            atLeastOneActive = true;
        }
    }

    if ( atLeastOneActive )
    {
        m_workerBitfield = boost::shared_ptr< std::vector< bool > >( new std::vector< bool >( m_size, true ) );

        for( std::list< boost::shared_ptr< WSelectorRoi > >::iterator iter = m_rois.begin(); iter != m_rois.end(); ++iter )
        {
            if ( ( *iter )->getRoi()->active() )
            {
                boost::shared_ptr< std::vector<bool> > bf = ( *iter )->getBitField();
                bool isnot = ( *iter )->getRoi()->isNot();
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

        if ( m_branch->isNot() )
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

    m_bitField = m_workerBitfield;
}
