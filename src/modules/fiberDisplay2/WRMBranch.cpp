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
    m_roiManager( roiManager )
{
    setDirty();
}

WRMBranch::~WRMBranch()
{
}

void WRMBranch::addRoi( boost::shared_ptr< WRMROIRepresentation > roi )
{
    m_rois.push_back( roi );
}

boost::shared_ptr< std::vector< bool > > WRMBranch::getBitField( unsigned int index )
{
    if ( m_dirty )
    {
        recalculate();
    }
    unsigned int c = 0;
    for( std::list< boost::shared_ptr< std::vector<bool> > >::iterator iter = m_bitFields.begin(); iter != m_bitFields.end(); ++iter )
    {
        if ( c == index)
        {
            return *iter;
        }
        ++c;
    }
}

void WRMBranch::addBitField( size_t size )
{
    boost::shared_ptr< std::vector< bool > >bf = boost::shared_ptr< std::vector< bool > >( new std::vector< bool >( size, false ) );
    m_bitFields.push_back( bf );
    for( std::list< boost::shared_ptr< WRMROIRepresentation> >::iterator iter = m_rois.begin(); iter != m_rois.end(); ++iter )
    {
        (*iter)->addBitField( size );
    }
    setDirty();
}

void WRMBranch::recalculate()
{
//    std::cout << "branch recalc" << std::endl;
    boost::shared_ptr< std::vector<bool> > mbf = m_bitFields.front();
    int size = mbf->size();
    mbf->clear();
    mbf->resize( size, true );

    for( std::list< boost::shared_ptr< WRMROIRepresentation > >::iterator iter = m_rois.begin(); iter != m_rois.end(); ++iter )
    {
        boost::shared_ptr< std::vector<bool> > bf = ( *iter )->getBitField( 0 );
        bool isnot = ( *iter )->getROI()->isNot();
        if ( !isnot )
        {
            for ( int i = 0 ; i < mbf->size() ; ++i )
            {
                mbf->at( i ) = mbf->at( i ) & bf->at( i );
            }
        }
        else
        {
            for ( int i = 0 ; i < mbf->size() ; ++i )
            {
                mbf->at( i ) = mbf->at( i ) & !bf->at( i );
            }
        }
    }

//    int counter = 0;
//    for (int i = 0 ; i < mbf->size() ; ++i)
//    {
//        if ( mbf->at(i) )
//            ++counter;
//    }
//    std::cout << "active fibers in branch :" << counter << std::endl;

    m_dirty = false;
}

bool WRMBranch::isDirty()
{
    return m_dirty;
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
