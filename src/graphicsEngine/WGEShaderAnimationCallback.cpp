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

#include "boost/date_time/posix_time/posix_time.hpp"

#include "WGEShaderAnimationCallback.h"

WGEShaderAnimationCallback::WGEShaderAnimationCallback( int ticksPerSecond ):
    osg::Uniform::Callback(),
    m_ticksPerSec( ticksPerSecond )
{
    // TODO(ebaum): make this stuff compatible to windows
    timeval tv;
    gettimeofday( &tv, 0L );

    m_startUsec = tv.tv_sec * 1000000 + tv.tv_usec;
}

WGEShaderAnimationCallback::~WGEShaderAnimationCallback()
{
    // cleanup
}

void WGEShaderAnimationCallback::operator() ( osg::Uniform* uniform, osg::NodeVisitor* /*nv*/ )
{
    // TODO(ebaum): make this stuff compatible to windows
    timeval tv;
    gettimeofday( &tv, 0L );

    int64_t currentUSecs = tv.tv_sec * 1000000 + tv.tv_usec;
    int milli = static_cast< int >( ( currentUSecs - m_startUsec ) / ( 1000000 / m_ticksPerSec ) );
    uniform->set( milli );
}

