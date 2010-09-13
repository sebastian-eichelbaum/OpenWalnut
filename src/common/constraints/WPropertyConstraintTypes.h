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

#ifndef WPROPERTYCONSTRAINTTYPES_H
#define WPROPERTYCONSTRAINTTYPES_H

/**
 * IDs for simple identification of the real constraint type.
 * \note If you modify this list, also adopt WPropertyVariable::WPropertyConstraint::create() in an appropriate way.
 */
typedef enum
{
    PC_UNKNOWN,          // type ID for arbitrary WPropertyConstraint
    PC_MIN,              // type ID for WPropertyConstraintMin
    PC_MAX,              // type ID for WPropertyConstraintMax
    PC_NOTEMPTY,         // type ID for WPropertyConstraintNotEmpty
    PC_PATHEXISTS,       // type ID for WPropertyConstraintPathExists
    PC_ISDIRECTORY,      // type ID for WPropertyConstraintIsDirectory
    PC_SELECTONLYONE,    // type ID for WPropertyConstraintSelectOnlyOne
    PC_SELECTATLEASTONE, // type ID for WPropertyConstraintSelectAtLeastOne
    PC_ISVALID           // type ID for WPropertyConstraintIsValid
}
PROPERTYCONSTRAINT_TYPE;

#endif  // WPROPERTYCONSTRAINTTYPES_H
