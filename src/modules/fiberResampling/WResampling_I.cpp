//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2014 OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS
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

#include <core/dataHandler/WDataSetFiberVector.h>

#include "WResampling_I.h"

WResampling_I::~WResampling_I()
{
}

WDataSetFibers::SPtr WResampling_I::operator()( WProgress::SPtr progress, WBoolFlag const &shutdown, WDataSetFibers::SPtr fibers )
{
    wlog::debug( "WResampling_I" ) << "Start resampling: " << fibers->getLineStartIndexes()->size() << " fibers";

    WDataSetFiberVector::SPtr newDS( new WDataSetFiberVector() );

    for( size_t fidx = 0; fidx < fibers->getLineStartIndexes()->size() && !shutdown; ++fidx )
    {
        newDS->push_back( resample( (*fibers)[ fidx ] ) );
        ++*progress;
    }

    return newDS->toWDataSetFibers();
}

