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

#ifndef WLOGEVENT_H
#define WLOGEVENT_H

#include <QtCore/QEvent>

#include "core/common/WLogEntry.h"

/**
 * This event is triggered when a new event is to be added to the log.
 **/
class WLogEvent : public QEvent
{
public:
    /**
     * constructor
     *
     * \param entry WLogEvent which is associated with the event
     **/
    explicit WLogEvent( const WLogEntry& entry );

    /**
     * destructor
     **/
    ~WLogEvent();

    /**
     * To access the WLogEntry of associated with the event.
     *
     * \return the log entry
     **/
    const WLogEntry& getEntry() const;

protected:

private:
    /**
     * The associated WLogEntry to the event.
     **/
    const WLogEntry m_entry;
};

#endif  // WLOGEVENT_H

