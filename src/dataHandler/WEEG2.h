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

#ifndef WEEG2_H
#define WEEG2_H

#include <cstddef>

#include <vector>

#include <boost/shared_ptr.hpp>

#include "io/WPagerEEG.h"
#include "WEEG2Segment.h"
#include "WRecording.h"


/**
 * Class which contains EEG recording data, read from a WPagerEEG.
 * \ingroup dataHandler
 */
class WEEG2 : public WRecording
{
public:
    /**
     * Constructor
     *
     * \param pager pager class which contains the data, read from a file on
     *              demand
     */
    explicit WEEG2( boost::shared_ptr< WPagerEEG > pager );

    /**
     * Get the number of segments this EEG consists of.
     *
     * \return number of segments
     */
    std::size_t getNumberOfSegments() const;

    /**
     * Get one segment.
     *
     * \param segmentID number of segment
     * \return segment
     */
    boost::shared_ptr< WEEG2Segment > getSegment( std::size_t segmentID ) const;

protected:
private:
    std::vector< boost::shared_ptr< WEEG2Segment > > m_segments; //!< list of all segments this EEG consists of
};

#endif  // WEEG2_H