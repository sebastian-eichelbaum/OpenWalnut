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
#include <cmath>
#include <string>

#include "../common/WCondition.h"

#include "WProgress.h"

WProgress::WProgress( std::string name, unsigned int count )
    : m_name( name ),
      m_max( count - 1 ),
      m_count( 0 ),
      m_pending( true ),
     m_determined( true )
{
    if( count == 0 )
    {
        m_max = 0;
        m_determined = false;
    }
}

WProgress::~WProgress()
{
    // clean up
}

void WProgress::update()
{
    // This updates the internal state. But as this class updates its state directly -> do nothing here
}

void WProgress::finish()
{
    m_pending = false;
    m_count = m_max;
}

WProgress& WProgress::operator++()
{
    return *this + 1;
}

float WProgress::getProgress()
{
    return isDetermined() ? 100.0 * ( static_cast< float >( m_count ) / static_cast< float >( m_max ) ) : 0.0;
}

bool WProgress::isPending()
{
    return m_pending;
}

std::string WProgress::getName() const
{
    return m_name;
}

bool WProgress::isDetermined()
{
    return m_determined;
}

WProgress& WProgress::operator+( unsigned int steps )
{
    if( isDetermined() )
    {
        m_count = std::min( m_max, m_count + steps );
    }

    return *this;
}
