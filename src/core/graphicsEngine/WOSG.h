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

#ifndef WOSG_H
#define WOSG_H

#include "osg/Version"

// Since OSG 3.2, OpenSceneGraph has changed several things in their Geometry class. For compilation compatibility, they provide
// deprecated_osg::Geometry. It was introduced somewhere in between 3.0 and 3.2. As I do not know where exactly, we only differentiate between
// 3.2 and earlier.
#if OSG_VERSION_GREATER_OR_EQUAL( 3, 2, 0 )
    #define wosg deprecated_osg
#else
    #define wosg osg
#endif

#endif  // WOSG_H
