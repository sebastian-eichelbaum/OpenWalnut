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

#ifndef WEEGEVENT_H
#define WEEGEVENT_H

#include <cstddef>

#include <vector>

#include <boost/shared_ptr.hpp>

#include <osg/Node>
#include <osg/ref_ptr>

#include "core/common/exceptions/WOutOfBounds.h"
#include "core/dataHandler/WDataSetDipoles.h"
#include "core/dataHandler/WEEG2.h"
#include "core/graphicsEngine/WGEGroupNode.h"


/**
 * A special time position in an EEG recording with corresponding data.
 */
class WEEGEvent
{
public:
    /**
     * Constructor
     *
     * \param time           time position in seconds
     * \param yPos           y position in pixels
     * \param eeg            pointer to the loaded EEG dataset
     * \param segmentID      number of the segment
     * \param parentNode     node where m_node is inserted and removed from
     * \param snapToDipole   whether the selected time position should be snapped to an active dipole
     * \param proofOfConcept whether we only show the proof of concept or the real dipoles
     * \param dipoles        pointer to the loaded dipoles dataset
     */
    WEEGEvent( double time,
               double yPos,
               boost::shared_ptr< WEEG2 > eeg,
               std::size_t segmentID,
               osg::ref_ptr< WGEGroupNode > parentNode,
               bool snapToDipole,
               bool proofOfConcept,
               boost::shared_ptr< WDataSetDipoles > dipoles );

    /**
     * Constructor for an empty event
     */
    WEEGEvent();

    /**
     * Destructor, removing m_node from m_parentNode
     */
    ~WEEGEvent();

    /**
     * Get the time position
     *
     * \return time position in seconds
     */
    double getTime() const;

    /**
     * Get the value of each channel at the time position
     *
     * \return values as reference to a vector
     */
    const std::vector< double >& getValues() const;

protected:
private:
    double m_time; //!< time position in seconds

    std::vector< double > m_values; //!< the value of each channel at the given time position

    osg::ref_ptr< osg::Node > m_node; //!< OSG node representing the event

    osg::ref_ptr< WGEGroupNode > m_parentNode; //!< parent node, where m_node is inserted and removed from
};

#endif  // WEEGEVENT_H
