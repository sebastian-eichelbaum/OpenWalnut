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

#ifndef WEVENT_H
#define WEVENT_H

#include <osg/Node>


/**
 * This class marks a special time position in an EEG or MEG recording.
 */
class WEvent
{
public:
    /**
     * Constructor
     *
     * \param time sets the time position
     */
    explicit WEvent( double time );

    /**
     * Set the time position
     *
     * \param time time position
     */
    void setTime( double time );

    /**
     * Get the time position
     *
     * \return time position
     */
    double getTime() const;

    /**
     * Set the OSG-Node representing the event
     *
     * \param node OSG-Node as ref_ptr
     */
    void setNode( osg::ref_ptr< osg::Node > node );

    /**
     * Get the OSG-Node representing the event
     *
     * \return OSG-Node as ref_ptr
     */
    osg::ref_ptr< osg::Node > getNode() const;

protected:
private:
    /**
     * time position
     */
    double m_time;

    /**
     * OSG-Node representing the event
     */
    osg::ref_ptr< osg::Node > m_node;
};

#endif  // WEVENT_H
