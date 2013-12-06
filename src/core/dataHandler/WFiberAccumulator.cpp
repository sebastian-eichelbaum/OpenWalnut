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

#include <fstream>
#include <string>
#include <vector>

#include "../common/WAssert.h"

#include "WFiberAccumulator.h"

WFiberAccumulator::WFiberAccumulator()
    : m_fiberMutex(),
      m_points( new std::vector< float >() ),
      m_fiberIndices( new std::vector< size_t >() ),
      m_fiberLengths( new std::vector< size_t >() ),
      m_pointToFiber( new std::vector< size_t >() )
{
}

WFiberAccumulator::~WFiberAccumulator()
{
}

void WFiberAccumulator::add( std::vector< WVector3d > const& in )
{
    boost::unique_lock< boost::mutex > lock( m_fiberMutex );

    if( in.size() > 0 )
    {
        m_fiberIndices->push_back( m_points->size() / 3 );
        m_fiberLengths->push_back( in.size() );

        for( size_t k = 0; k < in.size(); ++k )
        {
            m_points->push_back( in[ k ][ 0 ] );
            m_points->push_back( in[ k ][ 1 ] );
            m_points->push_back( in[ k ][ 2 ] );

            m_pointToFiber->push_back( m_fiberIndices->size() - 1 );
        }
    }
}

boost::shared_ptr< WDataSetFibers > WFiberAccumulator::buildDataSet()
{
    boost::unique_lock< boost::mutex > lock( m_fiberMutex );

    boost::shared_ptr< WDataSetFibers > res( new WDataSetFibers( m_points, m_fiberIndices, m_fiberLengths, m_pointToFiber ) );

    m_points = boost::shared_ptr< std::vector< float > >( new std::vector< float >() );
    m_fiberIndices = boost::shared_ptr< std::vector< size_t > >( new std::vector< size_t >() );
    m_fiberLengths = boost::shared_ptr< std::vector< size_t > >( new std::vector< size_t >() );
    m_pointToFiber = boost::shared_ptr< std::vector< size_t > >( new std::vector< size_t >() );

    return res;
}

void WFiberAccumulator::clear()
{
    m_points->clear();
    m_fiberIndices->clear();
    m_fiberLengths->clear();
    m_pointToFiber->clear();
}
