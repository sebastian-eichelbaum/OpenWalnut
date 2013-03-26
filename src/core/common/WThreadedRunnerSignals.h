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

#ifndef WTHREADEDRUNNERSIGNALS_H
#define WTHREADEDRUNNERSIGNALS_H

#include <boost/signals2/signal.hpp>
#include <boost/function.hpp>

class WException;

/**
 * Enum of all possible signals WThreadedRunner instances can emit.
 */
enum THREAD_SIGNAL
{
    WTHREAD_ERROR   // error during execution
};

// **************************************************************************************************************************
// Types
// **************************************************************************************************************************

/**
 * Signal for error events like thrown and un-catched exceptions.
 */
typedef boost::function< void ( const WException& ) > t_ThreadErrorSignalHandlerType;

/**
 * Signal type used in the most signals involving exceptions.
 */
typedef boost::signals2::signal< void ( const WException& ) > t_ThreadErrorSignalType;

#endif  // WTHREADEDRUNNERSIGNALS_H

