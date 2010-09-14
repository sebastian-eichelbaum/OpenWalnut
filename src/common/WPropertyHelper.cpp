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

#include "WPropertyHelper.h"

namespace WPropertyHelper
{
    namespace PC_SELECTONLYONE
    {
        void addTo( WPropSelection prop )
        {
            prop->addConstraint( boost::shared_ptr< WPropertyConstraintSelectOnlyOne< WPVBaseTypes::PV_SELECTION > >(
                    new  WPropertyConstraintSelectOnlyOne< WPVBaseTypes::PV_SELECTION >
            ) );
        }
    }

    namespace PC_NOTEMPTY
    {
        void addTo( WPropSelection prop )
        {
            prop->addConstraint( boost::shared_ptr< WPropertyConstraintNotEmpty< WPVBaseTypes::PV_SELECTION > >(
                    new  WPropertyConstraintNotEmpty< WPVBaseTypes::PV_SELECTION >
            ) );
        }

        void addTo( WPropString prop )
        {
            prop->addConstraint( boost::shared_ptr< WPropertyConstraintNotEmpty< WPVBaseTypes::PV_STRING > >(
                    new  WPropertyConstraintNotEmpty< WPVBaseTypes::PV_STRING >
            ) );
        }

        void addTo( WPropFilename prop )
        {
            prop->addConstraint( boost::shared_ptr< WPropertyConstraintNotEmpty< WPVBaseTypes::PV_PATH > >(
                    new  WPropertyConstraintNotEmpty< WPVBaseTypes::PV_PATH >
            ) );
        }
    }

    namespace PC_PATHEXISTS
    {
        void addTo( WPropFilename prop )
        {
            prop->addConstraint( boost::shared_ptr< WPropertyConstraintPathExists< WPVBaseTypes::PV_PATH > >(
                    new  WPropertyConstraintPathExists< WPVBaseTypes::PV_PATH >()
            ) );
        }
    }

    namespace PC_ISDIRECTORY
    {
        void addTo( WPropFilename prop )
        {
            prop->addConstraint( boost::shared_ptr< WPropertyConstraintIsDirectory< WPVBaseTypes::PV_PATH > >(
                    new  WPropertyConstraintIsDirectory< WPVBaseTypes::PV_PATH >()
            ) );
        }
    }

    namespace PC_ISVALID
    {
        void addTo( WPropSelection prop )
        {
            prop->addConstraint( boost::shared_ptr< WPropertyConstraintIsValid< WPVBaseTypes::PV_SELECTION > >(
                    new  WPropertyConstraintIsValid< WPVBaseTypes::PV_SELECTION >()
            ) );
        }
    }
}
