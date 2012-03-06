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

#ifndef WGESHADERANIMATIONCALLBACK_H
#define WGESHADERANIMATIONCALLBACK_H

#include "stdint.h"

#include "boost/date_time/posix_time/posix_time.hpp"

#include <osg/Uniform>



/**
 * This is a uniform callback setting the uniform to the current time in milliseconds, hundredth of a second or tenth of a second.
 */
class WGEShaderAnimationCallback: public osg::Uniform::Callback
{
public:
    /**
     * Default constructor. Creates a new instance and sets the precision
     *
     * \param ticksPerSecond the uniform will increase by 1 every hundredth second if =100, every 10th second if =10 and every second if =1.
     */
    explicit WGEShaderAnimationCallback( int ticksPerSecond = 100 );

    /**
     * Destructor.
     */
    virtual ~WGEShaderAnimationCallback();

    /**
     * Operator called on uniform update.
     *
     * \param uniform the uniform to update
     * \param nv the visitor.
     */
    virtual void operator() ( osg::Uniform* uniform, osg::NodeVisitor* nv );

protected:
    /**
     * Timer that stops the time hopefully OS independent
     */
    boost::posix_time::ptime m_start;

    /**
     * Number of ticks to count per second.
     */
    int m_ticksPerSec;

    /**
     * Ratio between milliseconds and m_ticksPerSec
     */
    double m_tickMillisecRatio;

private:
};

#endif  // WGESHADERANIMATIONCALLBACK_H

