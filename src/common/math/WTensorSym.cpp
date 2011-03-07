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

#include "../exceptions/WPreconditionNotMet.h"
#include "WMath.h"
#include "WTensorSym.h"

std::size_t calcSupersymmetricTensorMultiplicity( std::size_t order, std::size_t numZeros, std::size_t numOnes, std::size_t numTwos )
{
    WPrecondEquals( order, numZeros + numOnes + numTwos );
    return factorial( order )
       / ( factorial( numZeros )
         * factorial( numOnes )
         * factorial( numTwos ) );
}
