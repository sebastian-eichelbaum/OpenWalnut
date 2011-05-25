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

#include <sstream>
#include <string>
#include <set>

#include <boost/utility.hpp>

#include "WProgressCombiner.h"

WProgressCombiner::WProgressCombiner( std::string name ):
    WProgress( name, 1 ),
    m_name( name ),
    m_progress( 0.0 )
{
    // initialize members
    m_pending = false;
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
    unsigned int numPendingChildren = 0;

    // as the children define this progress' state -> iterate children
    for( std::set< boost::shared_ptr< WProgress > >::iterator i = m_children.begin(); i != m_children.end(); ++i )
    {
        // enforce child to update
        ( *i )->update();

        // update own state basing on child states.
        if( ( *i )->isPending() )
        {
            // This actually builds the mean value. This might cause backstepping in progress, which is not wanted.
            m_pending = true;
            m_determined &= ( *i )->isDetermined();
            m_progress += ( *i )->getProgress();
            numPendingChildren++;
        }
    }
    if( numPendingChildren )
    {
        m_progress /= static_cast< float >( numPendingChildren );
    }

    rlock.unlock();
}

std::string WProgressCombiner::getCombinedNames() const
{
    // read lock combiner
    boost::shared_lock< boost::shared_mutex > rlock = boost::shared_lock< boost::shared_mutex >( m_updateLock );

    std::stringstream ss;
    ss << "[";
    for( std::set< boost::shared_ptr< WProgress > >::const_iterator i = m_children.begin(); i != m_children.end(); ++i )
    {
        // enforce child to update
        ss << ( *i )->getName();
        if( boost::next( i ) != m_children.end() )
        {
            ss << ", ";
        }
    }
    ss << "]";

    // Done. Free lock.
    rlock.unlock();
    return ss.str();
}

void WProgressCombiner::addSubProgress( boost::shared_ptr< WProgress > progress )
{
    boost::unique_lock<boost::shared_mutex> lock = boost::unique_lock<boost::shared_mutex>( m_updateLock );
    // add the progress to the children list
    m_children.insert( progress );
    lock.unlock();
}

void WProgressCombiner::removeSubProgress( boost::shared_ptr< WProgress > progress )
{
    boost::unique_lock<boost::shared_mutex> lock = boost::unique_lock<boost::shared_mutex>( m_updateLock );
    // add the progress to the children list
    m_children.erase( progress );
    lock.unlock();
}

void WProgressCombiner::finish()
{
    // combiner just propagate the finish request down to all children
    boost::unique_lock<boost::shared_mutex> lock = boost::unique_lock<boost::shared_mutex>( m_updateLock );

    // as the children define this progress' state -> iterate children
    for( std::set< boost::shared_ptr< WProgress > >::iterator i = m_children.begin(); i != m_children.end(); ++i )
    {
        // enforce child to update
        ( *i )->finish();
        ( *i )->update();
    }

    // remove the children
    m_children.clear();

    // set the defaults
    WProgress::finish();
    m_progress = 0.0;

    lock.unlock();
}

WProgressCombiner& WProgressCombiner::operator++()
{
    // in progress combiners, this can be ignored. The progress is defined by the children.
    return *this;
}

float WProgressCombiner::getProgress()
{
    return m_progress;
}

