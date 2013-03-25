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

#ifndef WMODULESIGNALS_H
#define WMODULESIGNALS_H

#include <boost/signals2/signal.hpp>
#include <boost/function.hpp>

class WModule;
class WException;

/**
 * Enum of all possible signals WModule instances can emit.
 */
enum MODULE_SIGNAL
{
    WM_ASSOCIATED,  // module got associated to a container
    WM_READY,       // module ready
    WM_ERROR,       // error during execution
    WM_REMOVED      // removed from the container
};

// **************************************************************************************************************************
// Types
// **************************************************************************************************************************

/**
 * Signal for generic events like "WM_READY".
 *
 * \param module The module emitting the signal.
 *
 */
typedef boost::function< void ( boost::shared_ptr< WModule > ) > t_ModuleGenericSignalHandlerType;

/**
 * Generic signal type used in the most signals involving just the emitting signal.
 *
 * \param module The module emitting the signal.
 */
typedef boost::signals2::signal< void ( boost::shared_ptr< WModule > ) >  t_ModuleGenericSignalType;

/**
 * Signal for error events like "WM_ERROR".
 *
 * \param module The module emitting the signal.
 *
 */
typedef boost::function< void ( boost::shared_ptr< WModule >, const WException& ) > t_ModuleErrorSignalHandlerType;

/**
 * Signal type used in the most signals involving exceptions.
 *
 * \param module The module emitting the signal.
 */
typedef boost::signals2::signal< void ( boost::shared_ptr< WModule >, const WException& ) >  t_ModuleErrorSignalType;

#endif  // WMODULESIGNALS_H

