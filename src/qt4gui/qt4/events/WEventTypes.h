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

// when a ROI got associated
#define WQT_ROI_ASSOC_EVENT QEvent::User + 3

// when a module crashes
#define WQT_CRASH_EVENT QEvent::User + 4

// when a subject signals a newly registered data set
#define WQT_UPDATE_TEXTURE_SORTER_EVENT QEvent::User + 5

// when a property changes
#define WQT_PROPERTY_CHANGED_EVENT QEvent::User + 6

// when a ROI got removed
#define WQT_ROI_REMOVE_EVENT QEvent::User + 7

// when a module got removed from the root container
#define WQT_MODULE_REMOVE_EVENT QEvent::User + 8

// when a module should be deleted finally.
#define WQT_MODULE_DELETE_EVENT QEvent::User + 9

// when a module connection got established.
#define WQT_MODULE_CONNECT_EVENT QEvent::User + 10

// when a module connection got closed.
#define WQT_MODULE_DISCONNECT_EVENT QEvent::User + 11

// a frame got rendered
#define WQT_RENDERED_FRAME_EVENT QEvent::User + 12

// a connector of a module changed state or value
#define WQT_MODULE_CONNECTOR_EVENT QEvent::User + 13

// when the screencapture update.
#define WQT_SCREENCAPTURE_EVENT QEvent::User + 14

// open a new custom dock widget
#define WQT_OPENCUSTOMDOCKWIDGET QEvent::User + 15

// close an existing custom dock widget
#define WQT_CLOSECUSTOMDOCKWIDGET QEvent::User + 16

// A async load operation has finished
#define WQT_LOADFINISHED QEvent::User + 17

// Log entry added
#define WQT_LOG_EVENT QEvent::User + 18
#endif  // WEVENTTYPES_H
