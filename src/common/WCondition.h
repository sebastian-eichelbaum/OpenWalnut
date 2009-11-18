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

#ifndef WCONDITION_H
#define WCONDITION_H

#include <boost/thread.hpp> 

/**
 * Class to encapsulate boost::condition_variable_any. You may use it to efficiently wait for events (a condition comes true). It
 * is a very simple implementation. It might be extended easily. Timed wait functions and so on.
 */
class WCondition
{
    friend class WCondition_test;
public:

    /**
     * Default constructor.
     */
    WCondition();

    /**
     * Destructor.
     */
    virtual ~WCondition();

    /**
     * Wait for the condition. Sets the calling thread asleep.
     */
    virtual void wait();

    /** 
     * Notifies all waiting threads.
     */
    virtual void notify();

protected:

    /**
     * The condition.
     */
    boost::condition_variable_any m_condition;

    /**
     * The mutex used for the condition.
     */
    boost::shared_mutex m_mutex;

private:
};

#endif  // WCONDITION_H

