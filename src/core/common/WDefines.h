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

#ifndef WDEFINES_H
#define WDEFINES_H

/**
 * \defgroup macros Macros
 *
 * This are macros used in OpenWalnut. Generally speaking we want to use as less macros as possible, so introduce new macros only if you have read:
 * http://www.parashift.com/c++-faq-lite/inline-functions.html#faq-9.5 esp. all four "why are macros - evil" pages.
 */

/**
 * \ingroup macros
 * \def OW_API_DEPRECATED
 * In order to mark functions for the compiler as deprecated we need to put this before each deprecated function to get compiler warnings whenever this function is used.
 * \note This macro is defined in here, since almost every header of the dataHandler includes this header.
 */
#if ( __GNUC__ > 3 || ( __GNUC__ == 3 && __GNUC_MINOR__ >= 1 ) )
    #define OW_API_DEPRECATED  __attribute__( ( __deprecated__ ) )
#else
    #define OW_API_DEPRECATED
#endif /* __GNUC__ */

#endif  // WDEFINES_H
