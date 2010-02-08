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

#include <string>
#include <algorithm>
#include <vector>

#include "WDataSet.h"

#include "WDataSetFiberVector.h"

// prototype instance as singleton
boost::shared_ptr< WPrototyped > WDataSetFiberVector::m_prototype = boost::shared_ptr< WPrototyped >();

void WDataSetFiberVector::sortDescLength()
{
    std::sort( m_fibers->begin(), m_fibers->end(), wmath::hasGreaterLengthThen );
}

void WDataSetFiberVector::erase( const std::vector< bool > &unused )
{
    assert( unused.size() == m_fibers->size() );
    std::vector< wmath::WFiber >::iterator useable = m_fibers->begin();
    for( size_t i = 0 ; i < unused.size(); ++i )
    {
        if( !unused[i] )
        {
            *useable = ( *m_fibers )[i];
            useable++;
        }
    }
    m_fibers->erase( useable, m_fibers->end() );
}

bool WDataSetFiberVector::isTexture() const
{
    return false;
}

WDataSetFiberVector::WDataSetFiberVector( boost::shared_ptr< std::vector< wmath::WFiber > > fibs )
    : WDataSet(),
      m_fibers( fibs )
{
}

WDataSetFiberVector::WDataSetFiberVector()
    : WDataSet(),
      m_fibers()
{
    // default constructor used by the prototype mechanism
}

size_t WDataSetFiberVector::size() const
{
    return m_fibers->size();
}

const wmath::WFiber& WDataSetFiberVector::operator[]( const size_t index ) const
{
    assert( index < m_fibers->size() );
    return (*m_fibers)[index];
}

const std::string WDataSetFiberVector::getName() const
{
    return "WDataSetFiberVector";
}

const std::string WDataSetFiberVector::getDescription() const
{
    return "Contains tracked fiber data.";
}

boost::shared_ptr< WPrototyped > WDataSetFiberVector::getPrototype()
{
    if ( !m_prototype )
    {
        m_prototype = boost::shared_ptr< WPrototyped >( new WDataSetFiberVector() );
    }

    return m_prototype;
}

