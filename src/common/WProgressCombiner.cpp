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

#include "WProgressCombiner.h"

WProgressCombiner::WProgressCombiner( std::string name ):
    WProgress( name, 1 ),
    m_name( name ),
    m_progress( 0.0 )
{
    // initialize members
}

WProgressCombiner::~WProgressCombiner()
{
    // cleanup
}


void WProgressCombiner::update()
{
    // This updates the internal state. Here, all states from child progress' get combined.

    // get read lock
    boost::shared_lock< boost::shared_mutex > rlock;
    rlock = boost::shared_lock< boost::shared_mutex >( m_updateLock );

    m_pending = false;
    m_determined = true;
    m_progress = 0.0;
    unsigned int numPendingChilds = 0;

    // as the childs define this progress' state -> iterate childs
    for ( std::set< boost::shared_ptr< WProgress > >::iterator i = m_childs.begin(); i != m_childs.end(); ++i )
    {
        // enforce child to update
        ( *i )->update();

        // update own state basing on child states.
        if ( ( *i )->isPending() )
        {
            // TODO(ebaum): this actually builds the mean value. This might cause backstepping in progress, which is not wanted.
            m_pending = true;
            m_determined |= ( *i )->isDetermined();
            m_progress += ( *i )->getProgress();
            numPendingChilds++;
        }
    }

    rlock.unlock();
}

void WProgressCombiner::finish()
{
    // in progress combiners, this can be ignored, as it is marked "finished" when all childs are finished.
}

WProgressCombiner& WProgressCombiner::operator++()
{
    // in progress combiners, this can be ignored. The progress is defined by the childs.
    return *this;
}

float WProgressCombiner::getProgress()
{
    return m_progress;
}

