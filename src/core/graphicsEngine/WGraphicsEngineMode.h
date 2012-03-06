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

#ifndef WGRAPHICSENGINEMODE_H
#define WGRAPHICSENGINEMODE_H

#if ( defined( WGEMODE_SINGLETHREADED ) || defined( WGEMODE_MULTITHREADED ))
    // should we do something here? No! We assume the user defined it in cmake
#else
    #ifdef __APPLE__
        #define WGEMODE_SINGLETHREADED
    #else
        #define WGEMODE_MULTITHREADED
    #endif

#endif

#endif  // WGRAPHICSENGINEMODE_H

