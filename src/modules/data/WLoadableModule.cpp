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

#include "core/kernel/WModule.h"

#include "WMData.h"
#include "WMReadSphericalHarmonics.h"
#include "WMWriteNIfTI.h"

/**
 * The following macro is used by modules so the factory can acquire a prototype instance from a shared library using the symbol.
 * You can write this symbol for your own if you need to add multiple modules to the list. This one is for convenience.
 *
 * \note we need the module instance to be created using a shared_ptr as WModule is derived from enable_shared_from_this. Removing the shared
 *       pointer causes segmentation faults during load.
 */
#ifdef _MSC_VER
    #define DECLSPECSTUFF __declspec( dllexport )
#else
    #define DECLSPECSTUFF
#endif
extern "C" DECLSPECSTUFF void WLoadModule( WModuleList& m ) // NOLINT - yes this is a non-const reference
{
    m.push_back( boost::shared_ptr< WModule >( new WMData ) );
    m.push_back( boost::shared_ptr< WModule >( new WMReadSphericalHarmonics ) );
    m.push_back( boost::shared_ptr< WModule >( new WMWriteNIfTI ) );
}

