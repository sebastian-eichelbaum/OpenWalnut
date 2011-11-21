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

#ifndef WGESIGNALS_H
#define WGESIGNALS_H

#include <boost/signals2/signal.hpp>
#include <boost/function.hpp>

/**
 * Enum of all possible signals WGraphicsEngine instances can emit.
 */
typedef enum
{
    GE_RELOADSHADERS,   // when a shader reload is requested
    GE_STARTUPCOMPLETE  // when the first view was correctly initialized
}
GE_SIGNAL;

// **************************************************************************************************************************
// Types
// **************************************************************************************************************************

/**
 * Signal for generic events like "GE_RELOADSHADERS".
 *
 */
typedef boost::function< void ( void ) > t_GEGenericSignalHandlerType;

/**
 * Generic signal type used in the most signals.
 */
typedef boost::signals2::signal< void ( void ) >  t_GEGenericSignalType;


#endif  // WGESIGNALS_H



