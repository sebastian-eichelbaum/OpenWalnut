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

#include <ctime>
#include <iostream>

#include "boost/date_time/posix_time/posix_time.hpp"

#include "WGEShaderAnimationCallback.h"

WGEShaderAnimationCallback::WGEShaderAnimationCallback( int ticksPerSecond ):
    osg::Uniform::Callback(),
    m_start( boost::posix_time::microsec_clock::local_time() ),
    m_ticksPerSec( ticksPerSecond ),
    m_tickMillisecRatio( static_cast< double >( ticksPerSecond ) / 1000.0 )
{
}

WGEShaderAnimationCallback::~WGEShaderAnimationCallback()
{
    // cleanup
}

void WGEShaderAnimationCallback::operator() ( osg::Uniform* uniform, osg::NodeVisitor* /*nv*/ )
{
    // according to boost doc, this is available on windows too! From boost doc "Get the local time using a sub second resolution clock. On Unix
    // systems this is implemented using GetTimeOfDay. On most Win32 platforms it is implemented using ftime. Win32 systems often do not achieve
    // microsecond resolution via this API. If higher resolution is critical to your application test your platform to see the achieved resolution.
    // -> fortunately, millisecond resolution is enough here.
    boost::posix_time::ptime t = boost::posix_time::microsec_clock::local_time();

    boost::posix_time::time_duration td = t - m_start;
    int ticks = static_cast< int >( static_cast< double >( td.total_milliseconds() ) * m_tickMillisecRatio );

    uniform->set( ticks );
}

