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

#ifndef WASSERT_H
#define WASSERT_H

#include <string>

#include "WException.h"
#include "WExportCommon.h"

/**
 * Implements assertion logic.
 *
 * If the flag WASSERT_AS_CASSERT is set, an error message is output to stderr,
 * then abort() is called. If the flag is not set, a WException is thrown.
 *
 * \param expression The expression that failed.
 * \param file The name of the file where the assertion statement can be found.
 * \param line The line in the file.
 * \param msg An optional message.
 */
void OWCOMMON_EXPORT wAssertFailed( std::string const& expression, std::string const& file, std::size_t line, std::string const& msg );

//! the actual assertion macro
#define WAssert( e, msg )  ( ( e ) ? ( ( void )0 ) : ( wAssertFailed( #e, __FILE__, __LINE__, msg ) ) )
//! the actual assertion macro, this one will be tested only in debug-mode.
#ifdef DEBUG
    #define WAssertDebug( e, msg ) WAssert( e, msg )
#else
    #define WAssertDebug( e, msg ) ( void )0
#endif

#endif  // WASSERT_H
