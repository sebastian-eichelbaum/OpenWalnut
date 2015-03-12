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

#ifndef WQTNETWORKEDITORGLOBALS_H
#define WQTNETWORKEDITORGLOBALS_H

// The size of in/out ports
#define WNETWORKPORT_SIZEX 10
#define WNETWORKPORT_SIZEY 10

// Distance to element border of sub elements
#define WNETWORKITEM_MARGINX 3
#define WNETWORKITEM_MARGINY 3

// The minimum width and heigh of items inside the scene
#define WNETWORKITEM_MINIMUM_WIDTH  150
#define WNETWORKITEM_MINIMUM_HEIGHT 50
#define WNETWORKITEM_MAXIMUM_WIDTH  150
#define WNETWORKITEM_MAXIMUM_HEIGHT 50

// Animation times in ms

// Mode birth/dead
#define WNETWORKITEM_BIRTH_DURATION 500
#define WNETWORKITEM_DEATH_DURATION 250

// Module movement (re-layout and snap-back)
#define WNETWORKITEM_MOVE_DURATION 500

// Item grid line blending
#define WNETWORKITEM_GRIDBLENDIN_DURATION 250
#define WNETWORKITEM_GRIDBLENDOUT_DURATION 250

// View Scroll times
#define WNETWORKITEM_VIEWPAN_DURATION 500

// Item grid
#define WNETWORKITEM_XSPACE 16  // distance between the grid boxes
#define WNETWORKITEM_YSPACE 50  // distance between the grid boxes


#endif  // WQTNETWORKEDITORGLOBALS_H

