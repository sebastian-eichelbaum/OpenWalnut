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

#ifndef WQTNETWORKCOLORS_H
#define WQTNETWORKCOLORS_H

#include <QtGui/QColor>

// This file defines several commonly used colors
namespace WQtNetworkColors
{
    /**
     * Color for source modules
     */
    const QColor SourceModule = QColor( 25, 100, 156 );

    /**
     * Color for sink modules
     */
    const QColor SinkModule = QColor( 25, 156, 25 );

    /**
     * Color for modules without in and out
     */
    const QColor StandaloneModule = QColor( 235, 147, 62 );

    /**
     * Color for all other kinds of modules
     */
    const QColor Module = QColor( 128, 128, 128 );

    /**
     * Color used for output connectors
     */
    const QColor OutputConnector = QColor( 255, 255, 255 );

    /**
     * Color used for input connectors
     */
    const QColor InputConnector = QColor( 255, 255, 255 );

    /**
     * Color for an active module's activator
     */
    const QColor ActivatorActive = QColor( 255, 255, 255 );

    /**
     * Color for an inactive module's activator
     */
    const QColor ActivatorInactive = QColor( 0, 0, 0 );

    /**
     * Color for crashed modules
     */
    const QColor ModuleCrashed = QColor( 215, 37, 46 );

    /**
     * Color of the busy indicator.
     */
    const QColor BusyIndicator = QColor( 255, 45, 45 );

    /**
     * Color of the busy indicator background.
     */
    const QColor BusyIndicatorBackground = QColor( 255, 255, 255 );
}

#endif  // WQTNETWORKCOLORS_H

