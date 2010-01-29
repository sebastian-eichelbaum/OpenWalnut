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

#ifndef WEVENTTYPES_H
#define WEVENTTYPES_H

#include <QtCore/QEvent>

/**
 * This header contains every custom event ID used in QT's event mechanism. Please note, that since Qt4.4 there is
 * QEvent::registerEventType which can handle this job better than this header. But as we use an older Qt Version we need to do it
 * this way.
 */

// when a module got associated
#define WQT_ASSOC_EVENT QEvent::User + 1

// when a module signals its ready state
#define WQT_READY_EVENT QEvent::User + 2

// when a roi got associated
#define WQT_ROI_ASSOC_EVENT QEvent::User + 3

#endif  // WEVENTTYPES_H

