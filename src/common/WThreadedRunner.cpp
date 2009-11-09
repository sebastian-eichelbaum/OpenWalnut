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

#include <iostream>

#include "WThreadedRunner.h"

WThreadedRunner::WThreadedRunner()
{
    // initialize members
    m_FinishRequested = false;
    m_Thread = NULL;
}

WThreadedRunner::~WThreadedRunner()
{
    // cleanup
    // XXX is this working if thread already has finished?
    // wait( true ); <-- no
}

void WThreadedRunner::run()
{
    m_Thread = new boost::thread( boost::bind( &WThreadedRunner::threadMain, this ) );
}

void WThreadedRunner::wait( bool requestFinish )
{
    if( requestFinish )
    {
        m_FinishRequested = requestFinish;
        notifyStop();
    }
    m_Thread->join();
}

void WThreadedRunner::threadMain()
{
}

void WThreadedRunner::notifyStop()
{
}

void WThreadedRunner::yield() const
{
    m_Thread->yield();
}

void WThreadedRunner::sleep( const int t ) const
{
    boost::this_thread::sleep( boost::posix_time::seconds( t ) );
}

