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

#ifndef WGEANIMATIONFRAMETIMER_H
#define WGEANIMATIONFRAMETIMER_H

#include <boost/shared_ptr.hpp>

#include "../../common/WTimer.h"



/**
 * Frame-based animation timing.
 */
class WGEAnimationFrameTimer: public WTimer
{
public:
    /**
     * Convenience typedef for a shared_ptr
     */
    typedef boost::shared_ptr< WGEAnimationFrameTimer > SPtr;

    /**
     * Convenience typedef for a const shared_ptr.
     */
    typedef boost::shared_ptr< const WGEAnimationFrameTimer > ConstSPtr;

    /**
     * Constructs a frame based animation timer. The specified number of frames is used to convert the number of frames to a time.
     *
     * \param framesPerSecond frames per second.
     */
    explicit WGEAnimationFrameTimer( float framesPerSecond = 24.0 );

    /**
     * Destructor.
     */
    virtual ~WGEAnimationFrameTimer();

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

    /**
     * Increments frame count.
     */
    virtual void tick();

private:
    /**
     * The current tick value.
     */
    size_t m_tick;

    /**
     * The number of frames per second.
     */
    double m_framesPerSecond;
};

#endif  // WGEANIMATIONFRAMETIMER_H
