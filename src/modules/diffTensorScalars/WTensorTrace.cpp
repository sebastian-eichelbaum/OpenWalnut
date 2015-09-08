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

#include "WTensorTrace.h"

WTensorTrace::WTensorTrace()
    : WObjectNDIP< WDataSetDTIToScalar_I >( "Trace", "Computes the trace of the tensor" )
{
}

double WTensorTrace::tensorToScalar( const WVector3d& /* evals */, const WTensorSym< 2, 3, float >& tensor )
{
    return tensor( 0, 0 ) + tensor( 1, 1 ) + tensor( 2, 2 );
}
