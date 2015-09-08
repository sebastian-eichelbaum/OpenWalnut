//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2015 OpenWalnut Community, Nemtics, BSV@Uni-Leipzig
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

#include <cmath>
#include "WRA.h"

WRA::WRA()
    : WObjectNDIP< WDataSetDTIToScalar_I >( "RA (Fractional Anisotropy)", "Computes the RA of the tensor" )
{
}

double WRA::tensorToScalar( const WVector3d& evals, const WTensorSym< 2, 3, float >& /* tensor */ )
{
    const WVector3d& l = evals; // shorthand (name) for lambda_1,...
    double mean = ( l[0] + l[1] + l[2] ) / 3.0;
    double nom = ( l[0] - mean ) * ( l[0] - mean ) + ( l[1] - mean ) * ( l[1] - mean ) + ( l[2] - mean ) * ( l[2] - mean );

    return std::sqrt( nom / 3.0 ) / mean;
}
