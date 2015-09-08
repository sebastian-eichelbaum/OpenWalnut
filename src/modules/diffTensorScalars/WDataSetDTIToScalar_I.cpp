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

#include <vector>

#include "core/common/WFlag.h"
#include "core/common/WLogger.h"
#include "core/common/WProgress.h"
#include "core/dataHandler/WDataSetDTI.h"
#include "core/dataHandler/WDataSetScalar.h"
#include "core/dataHandler/WDataSetVector.h"
#include "WDataSetDTIToScalar_I.h"

WDataSetDTIToScalar_I::~WDataSetDTIToScalar_I()
{
}

WDataSetScalar::SPtr WDataSetDTIToScalar_I::operator()( WProgress::SPtr progress, WBoolFlag const &shutdown,
    WDataSetDTI::SPtr tensors, WDataSetVector::SPtr evals )
{
    wlog::debug( "WDataSetDTIToScalar_I" ) << "Start computation";
    typedef double ValueType;
    typedef WValueSet< ValueType > ValueSetType;
    boost::shared_ptr< WGrid > grid( evals->getGrid() );
    boost::shared_ptr< std::vector< ValueType > > data( new std::vector< ValueType >( grid->size() ) );

    boost::shared_ptr< WValueSet< float > > t = boost::dynamic_pointer_cast< WValueSet< float > >( tensors->getValueSet() );
    WAssert( t, "Error: Only tensor data sets with doubles are allowed currently." );

    for( unsigned int i = 0; i < grid->size() && !shutdown; ++i )
    {
        WTensorSym< 2, 3, float > tensor( t->getWValue( i ) );
        WVector3d ev = evals->getVectorAt( i );
        (*data)[i] = tensorToScalar( ev, tensor );
        ++*progress;
    }

    if( shutdown )
    {
        return WDataSetScalar::SPtr( new WDataSetScalar() ); // incase we had to abort due to shutdown, empty result
    }

    wlog::debug( "WDataSetDTIToScalar_I" ) << "Computation done.";
    boost::shared_ptr< ValueSetType > vals( new ValueSetType( 0, 1, data ) );
    return WDataSetScalar::SPtr( new WDataSetScalar( vals, grid ) );
}
