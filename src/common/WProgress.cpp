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

#include <cmath>

#include "../common/WCondition.h"

#include "WProgress.h"

WProgress::WProgress( std::string name, unsigned int count )
    : m_name( name ),
      m_max ( count ),
      m_count ( 0 ),
      m_pending ( true )
{
    m_determined = !( m_max == 0 );
}

WProgress::~WProgress()
{
    // clean up
}

void WProgress::update()
{
    // This updates the internal state. But as this class updates its state directly -> do nothing here

    /*
    //return 100.0 * static_cast< float >( m_max ) / static_cast< float >( m_count );
    // get read lock
    boost::shared_lock< boost::shared_mutex > rlock;
    rlock = boost::shared_lock< boost::shared_mutex >( m_childLock );

    // if there are no childs, the state is defined by m_count and m_started
    if ( !m_childs.empty() )
    {
        m_started = false;
        m_count = 0;
        m_max = 0;
        // as the childs define this progress' state -> iterate childs
        for ( std::set< boost::shared_ptr< WProgress > >::iterator i = m_childs.begin(); i != m_childs.end(); ++i )
        {
            ( *i )->update();
            m_started |= ( *i )->isStarted();
            if ( ( *i )->isStarted() )
            {
                m_count += ( *i )->getCurrentCount();
            }
        }
    }

    rlock.unlock();*/
}

void WProgress::finish()
{
    m_pending = false;
    m_count = m_max - 1;
}

WProgress& WProgress::operator++()
{
    if ( isDetermined() )
    {
        m_count = std::max ( m_max - 1 , m_count + 1 );
    }

    return *this;
}

float WProgress::getProgress()
{
    return 100.0 * static_cast< float >( m_max ) / static_cast< float >( m_count );
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


