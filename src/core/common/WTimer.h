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

#ifndef WTIMER_H
#define WTIMER_H

#include <boost/shared_ptr.hpp>



/**
 * Base class for timing. Derive from it to write several timers like a frame-timer or realtime-timer.
 */
class WTimer      // NOLINT - no does not need an virtual destructor.
{
public:
    /**
     * Convenience typedef for a shared_ptr
     */
    typedef boost::shared_ptr< WTimer > SPtr;

    /**
     * Convenience typedef for a const shared_ptr.
     */
    typedef boost::shared_ptr< const WTimer > ConstSPtr;

    /**
     * Constructs a animation timer.
     */
    WTimer();

    /**
     * Destructor.
     */
    virtual ~WTimer();

    /**
     * Resets the start-tick.
     */
    virtual void reset() = 0;

    /**
     * Returns the elapsed time since the last reset in seconds with milliseconds precision.
     *
     * \return elapsed time in seconds with millisecond precision.
     */
    virtual double elapsed() const = 0;

private:
};

#endif  // WTIMER_H
