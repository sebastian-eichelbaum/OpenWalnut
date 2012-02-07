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

#ifndef WPROPERTYHELPER_H
#define WPROPERTYHELPER_H

#include <boost/shared_ptr.hpp>

#include "WPropertyVariable.h"

#include "constraints/WPropertyConstraintNotEmpty.h"
#include "constraints/WPropertyConstraintPathExists.h"
#include "constraints/WPropertyConstraintIsDirectory.h"
#include "constraints/WPropertyConstraintSelectOnlyOne.h"
#include "constraints/WPropertyConstraintIsValid.h"


/**
 * This namespace contains several utility functions to handle properties. Especially constraint helper to easily create constraints.
 */
namespace WPropertyHelper
{
    /**
     * Contains functions for easily adding constraints of type PC_SELECTONLYONE to properties compatible with this constraint.
     */
    namespace PC_SELECTONLYONE
    {
        /**
         * Add the PC_SELECTONLYONE constraint to the property.
         *
         * \param prop the property where to add the constraint.
         */
        void addTo( WPropSelection prop );
    }

    /**
     * Contains functions for easily adding constraints of type PC_NOTEMPTY to properties compatible with this constraint.
     */
    namespace PC_NOTEMPTY
    {
        /**
         * Add the PC_NOTEMPTY constraint to the property.
         *
         * \param prop the property where to add the constraint.
         */
        void addTo( WPropSelection prop );

        /**
         * Add the PC_NOTEMPTY constraint to the property.
         *
         * \param prop the property where to add the constraint.
         */
        void addTo( WPropString prop );

        /**
         * Add the PC_NOTEMPTY constraint to the property.
         *
         * \param prop the property where to add the constraint.
         */
        void addTo( WPropFilename prop );
    }

    /**
     * Contains functions for easily adding constraints of type PC_PATHEXISTS to properties compatible with this constraint.
     */
    namespace PC_PATHEXISTS
    {
        /**
         * Add the PC_PATHEXISTS constraint to the property.
         *
         * \param prop the property where to add the constraint.
         */
        void addTo( WPropFilename prop );
    }

    /**
     * Contains functions for easily adding constraints of type PC_ISDIRECTORY to properties compatible with this constraint.
     */
    namespace PC_ISDIRECTORY
    {
        /**
         * Add the PC_PATHEXISTS constraint to the property.
         *
         * \param prop the property where to add the constraint.
         */
        void addTo( WPropFilename prop );
    }

    /**
     * Contains functions for easily adding constraints of type PC_ISVALID  to properties compatible with this constraint.
     */
    namespace PC_ISVALID
    {
        /**
         * Add the PC_ISVALID constraint to the property.
         *
         * \param prop the property where to add the constraint.
         */
        void addTo( WPropSelection prop );
    }
}

#endif  // WPROPERTYHELPER_H

