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

#ifndef WREALTIMETIMER_H
#define WREALTIMETIMER_H

#include <boost/shared_ptr.hpp>

#include <osg/Timer>

#include "WTimer.h"

#include "WExportCommon.h"

/**
 * Realtime timing. Internally, this uses an osg::Timer.
 */
class OWCOMMON_EXPORT WRealtimeTimer: public WTimer
{
public:
    /**
     * Convenience typedef for a shared_ptr
     */
    typedef boost::shared_ptr< WRealtimeTimer > SPtr;

    /**
     * Convenience typedef for a const shared_ptr.
     */
    typedef boost::shared_ptr< const WRealtimeTimer > ConstSPtr;

    /**
     * Constructs a realtime animation timer.
     */
    WRealtimeTimer();

    /**
     * Destructor.
     */
    virtual ~WRealtimeTimer();

    /**
     * Resets the start-tick.
     */
    virtual void reset();

    /**
     * Returns the elapsed time since the last reset in seconds with milliseconds precision.
     *
     * \return elapsed time in seconds with millisecond precision.
     */
    virtual double elapsed() const;

private:
    /**
     * The osg timer is used here.
     */
    osg::Timer m_timer;
};

#endif  // WREALTIMETIMER_H
