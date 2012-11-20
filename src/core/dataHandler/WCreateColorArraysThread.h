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

#ifndef WCREATECOLORARRAYSTHREAD_H
#define WCREATECOLORARRAYSTHREAD_H

#include <vector>

#include "../common/WThreadedRunner.h"

/**
 * Thread for computing directional color coding of fibers.
 */
class WCreateColorArraysThread: public WThreadedRunner // NOLINT
{
public:
    /**
     * default constructor
     *
     * \param left start position of the first line to comput colors for
     * \param right last line for which the color is computed
     * \param vertices vertices of all lines
     * \param lineStartIndexes line start indices in vertex array
     * \param lineLengths line length in vertex array
     * \param globalColors where to write global coloring
     * \param localColors where to write local coloring
     * \param tangents
     */
    WCreateColorArraysThread( int left, int right, boost::shared_ptr< std::vector< float > >vertices,
                                                   boost::shared_ptr< std::vector< size_t > > lineLengths,
                                                   boost::shared_ptr< std::vector< float > > globalColors,
                                                   boost::shared_ptr< std::vector< float > > localColors,
                                                   boost::shared_ptr< std::vector< float > > tangents );

    /**
     * destructor
     */
    virtual ~WCreateColorArraysThread();

    /**
     * entry for the run command
     */
    virtual void threadMain();

    /**
     * Return the value of the finished flag.
     *
     * \return true if finished
     */
    inline bool isFinished();

protected:
private:
    bool m_myThreadFinished; //!< Has the thread finished?

    int m_left; //!< left boundary

    int m_right; //!< right boundary

    /**
     * Point vector for all fibers
     */
    boost::shared_ptr< const std::vector< float > > m_vertices;

    /**
     * Point vector for tangents at each vertex, used for fake tubes
     */
    boost::shared_ptr< std::vector< float > > m_tangents;

    /**
     * Storing the global color value of the fibers for each point.
     */
    boost::shared_ptr< std::vector< float > > m_globalColors;

    /**
     * Storing the local color value of the fibers for each point.
     * \note it is mutable to allow getLocalColors creating it on demand.
     */
    boost::shared_ptr< std::vector< float > > m_localColors;

    /**
     * Line vector that contains the number of vertices for each line
     */
    boost::shared_ptr< const std::vector< size_t > > m_lineLengths;
};

#endif  // WCREATECOLORARRAYSTHREAD_H
