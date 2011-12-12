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

#ifndef WPROPERTIES_FWD_H
#define WPROPERTIES_FWD_H

#include <boost/shared_ptr.hpp>

// This class contains forward declarations for every thing which is needed to use WPropertyGroup in all their variations. Simply include this file
// in your headers if your somehow need properties or related classes.

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Property base classes and types
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// WPropertyTypes.h does not include any other property header. So we can include it directly here.
// Provides:
//  * class WPropertyVariable< T >
//  * class WPropertyGroup
//  * enum PROPERTY_TYPE
//  * enum PROPERTY_PURPOSE
//  * typedefs WPVPropXYZ
//  * typedefs WPropXYZ
//  * namespace WPVBaseTypes
//  * namespace PROPERTY_TYPE_HELPER
#include "WPropertyTypes.h"

// From WPropertyBase.h
class WPropertyBase;

// From WPropertyGroupBase.h
class WPropertyGroupBase;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Utilities
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// From WPropertyObserver
class WPropertyObserver;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constraints
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Provides: enum PROPERTYCONSTRAINT_TYPE
#include "constraints/WPropertyConstraintTypes.h"

// From constraints/*.h
template < typename T >
class WPropertyConstraintIsDirectory;
template < typename T >
class WPropertyConstraintSelectOnlyOne;
template< typename T >
class WPropertyConstraintIsValid;
template< typename T >
class WPropertyConstraintMax;
template< typename T >
class WPropertyConstraintMin;
template< typename T >
class WPropertyConstraintNotEmpty;
template < typename T >
class WPropertyConstraintPathExists;

#endif  // WPROPERTIES_FWD_H

