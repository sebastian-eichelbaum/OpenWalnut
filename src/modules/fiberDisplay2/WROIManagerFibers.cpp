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


WROIManagerFibers::WROIManagerFibers()
{
}

WROIManagerFibers::~WROIManagerFibers()
{
}

void WROIManagerFibers::addRoi( boost::shared_ptr<WROI> newRoi )
{
    // create new branch
    boost::shared_ptr< WRMBranch > newBranch = boost::shared_ptr< WRMBranch >( new WRMBranch( shared_from_this() ) );
    // create roi
    boost::shared_ptr< WRMROIRepresentation >rroi = boost::shared_ptr< WRMROIRepresentation >( new WRMROIRepresentation( newRoi, newBranch ) );
    // add roi to branch
    newBranch->addRoi( rroi );
    // add branch to list
    m_branches.push_back( newBranch );
    // add bit fields
    for( std::list< boost::shared_ptr< const WDataSetFibers2> >::iterator iter = m_fiberList.begin(); iter != m_fiberList.end(); ++iter )
    {
        newBranch->addBitField( ( *iter ).get()->size() );
    }
}

void WROIManagerFibers::addRoi( boost::shared_ptr<WROI> newRoi, boost::shared_ptr<WROI> parentRoi )
{
    // find branch
    boost::shared_ptr< WRMBranch> branch;

    for( std::list< boost::shared_ptr< WRMBranch > >::iterator iter = m_branches.begin(); iter != m_branches.end(); ++iter )
    {
        if ( ( *iter).get()->getFirstRoi()->getROI() == parentRoi )
        {
            branch = ( *iter );
        }
    }
    // create roi
    boost::shared_ptr< WRMROIRepresentation >rroi = boost::shared_ptr< WRMROIRepresentation >( new WRMROIRepresentation( newRoi, branch ) );
    // add bit fields
    for( std::list< boost::shared_ptr< const WDataSetFibers2> >::iterator iter = m_fiberList.begin(); iter != m_fiberList.end(); ++iter )
    {
        rroi->addBitField( ( *iter ).get()->size() );
    }
    // add roi to branch
    branch->addRoi( rroi );
}

void WROIManagerFibers::removeRoi( boost::shared_ptr<WROI> /*roi */)
{

}

void WROIManagerFibers::addFiberDataset( boost::shared_ptr<const WDataSetFibers2> fibers )
{
    m_fiberList.push_back( fibers );

    boost::shared_ptr< std::vector< float > > verts = fibers->getVertices();
    m_kdTreeList.push_back( boost::shared_ptr< WKdTree>( new WKdTree( fibers->getVertices()->size() / 3, &((*verts)[0]) , true ) ) );
    //m_kdTreeList.push_back( boost::shared_ptr< WKdTree>( new WKdTree( fibers->getVertices()->size() / 3, &((*verts)[0]) ) ) );

    addBitField( fibers->size() );
}

void WROIManagerFibers::removeFiberDataset( boost::shared_ptr<const WDataSetFibers2> /*fibers*/ )
{

}

boost::shared_ptr<std::vector<bool> > WROIManagerFibers::getBitField( boost::shared_ptr<const WDataSetFibers2> fibers )
{
    if ( m_dirty )
    {
        recalculate();
    }

    unsigned int c = 0;
    for( std::list< boost::shared_ptr< const WDataSetFibers2 > >::iterator iter = m_fiberList.begin(); iter != m_fiberList.end(); ++iter )
    {
        if ( *iter == fibers )
        {
            break;
        }
        ++c;
    }

    unsigned int index = 0;
    for( std::list< boost::shared_ptr<std::vector<bool> > >::iterator iter = m_bitFields.begin(); iter != m_bitFields.end(); ++iter )
    {
        if ( c == index)
        {
            return (*iter);
        }
        ++index;
    }
}

void WROIManagerFibers::addBitField( size_t size )
{
    boost::shared_ptr<std::vector< bool > >bf = boost::shared_ptr<std::vector< bool > >( new std::vector< bool >( size, false ) );
    m_bitFields.push_back( bf );
    for( std::list< boost::shared_ptr<WRMBranch > >::iterator iter = m_branches.begin(); iter != m_branches.end(); ++iter )
    {
        (*iter).get()->addBitField( size );
    }
    setDirty();
}

void WROIManagerFibers::recalculate()
{
//    std::cout << "roi manager recalc" << std::endl;
    boost::shared_ptr< std::vector<bool> > mbf = m_bitFields.front();
    int size = mbf->size();
    mbf->clear();
    mbf->resize( size, false );

    for( std::list< boost::shared_ptr< WRMBranch > >::iterator iter = m_branches.begin(); iter != m_branches.end(); ++iter )
    {
        boost::shared_ptr< std::vector<bool> > bf = ( *iter )->getBitField( 0 );

        for ( int i = 0 ; i < mbf->size() ; ++i )
        {
            mbf->at( i ) = mbf->at( i ) | bf->at( i );
        }
    }

//    int counter = 0;
//    for (int i = 0 ; i < mbf->size() ; ++i)
//    {
//        if ( mbf->at(i) )
//            ++counter;
//    }
//    std::cout << "active fibers total :" << counter << std::endl;

    m_dirty = false;
}

void WROIManagerFibers::setDirty()
{
    m_dirty = true;
}

boost::shared_ptr<const WDataSetFibers2> WROIManagerFibers::getDataSet( unsigned int index )
{
    unsigned int c = 0;
    for( std::list< boost::shared_ptr< const WDataSetFibers2 > >::iterator iter = m_fiberList.begin(); iter != m_fiberList.end(); ++iter )
    {
        if ( index == c )
        {
            return *iter;
        }
        ++c;
    }
}

boost::shared_ptr< WKdTree > WROIManagerFibers::getKdTree( unsigned int index )
{
    unsigned int c = 0;
    for( std::list< boost::shared_ptr< WKdTree > >::iterator iter = m_kdTreeList.begin(); iter != m_kdTreeList.end(); ++iter )
    {
        if ( index == c )
        {
            return *iter;
        }
        ++c;
    }
}

bool WROIManagerFibers::isDirty()
{
    return m_dirty;
}
