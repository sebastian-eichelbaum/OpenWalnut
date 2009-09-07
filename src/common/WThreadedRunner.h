//---------------------------------------------------------------------------
//
// Project: OpenWalnut
//
// Copyright 2009 SomeCopyrightowner
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

#ifndef WTHREADEDRUNNER_H
#define WTHREADEDRUNNER_H

#include <boost/thread/thread.hpp>

/**
 * Base class for all classes needing to be executed in a separate thread.
 */
class WThreadedRunner
{
public:

    /**
     * Default constructor.
     */
    WThreadedRunner();

    /**
     * Destructor.
     */
    virtual ~WThreadedRunner();

    /**
     * Run thread.
     */
    void run();

    /**
     * Wait for the thread to be finished.
     *
     * \param requestFinish true if the thread should be notified.
     */
    void wait( bool requestFinish = false );

protected:

    /**
     * Function that has to be overwritten for execution. It gets executed in a separate thread after run()
     * has been called.
     */
    virtual void threadMain();

    /**
     * Thread instance.
     */
    boost::thread* m_Thread;

    /**
     * True if thread should end execution.
     */
    bool m_FinishRequested;

    /**
     * Give remaining execution timeslice to another thread.
     */
    void yield() const;

    /**
     * Sets thread asleep.
     *
     * \param t time to sleep in seconds.
     */
    void sleep( const int t ) const;

private:
};

#endif  // WTHREADEDRUNNER_H

