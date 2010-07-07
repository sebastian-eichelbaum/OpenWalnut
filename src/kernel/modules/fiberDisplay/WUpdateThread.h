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

#ifndef WUPDATETHREAD_H
#define WUPDATETHREAD_H

#include "../../../common/WThreadedRunner.h"

class WROIManagerFibers;
/**
 * implements a thread that updates the fiber selection bit field
 */
class WUpdateThread: public WThreadedRunner
{
public:
    /**
     * default constructor
     *
     * \param roiManager
     */
    explicit WUpdateThread( boost::shared_ptr< WROIManagerFibers >roiManager );

    /**
     * destructor
     */
    virtual ~WUpdateThread();

    /**
     * entry for the run command
     */
    virtual void threadMain();

    /**
     * Return the value of the finished flag.
     */
    inline bool isFinished();

protected:
private:
    boost::shared_ptr< WROIManagerFibers > m_roiManager; //!< stores pointer to the roi manager
    bool m_myThreadFinished; //!< Has the thread finished?
};

bool WUpdateThread::isFinished()
{
    return m_myThreadFinished;
}

#endif  // WUPDATETHREAD_H
