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

#include "WDataSetGP.h"

WDataSetGP::WDataSetGP( boost::shared_ptr< const WDataSetFibers > tracts,
                        boost::shared_ptr< const WDataSetDTI > tensors,
                        const WBoolFlag& shutdownFlag,
                        boost::shared_ptr< WProgress > progress )
    : WMixinVector< WGaussProcess >(),
      WDataSet()
{
    reserve( tracts->size() );
    for( size_t i = 0; i < tracts->size(); ++i )
    {
        if( shutdownFlag() )
        {
            clear();
            break;
        }
        ++*progress;
        push_back( WGaussProcess( i, tracts, tensors ) );
    }
}

WDataSetGP::~WDataSetGP()
{
}

double WDataSetGP::mean( const WPosition& p ) const
{
    double avg = 0.0;
    for( const_iterator cit = begin(); cit != end(); ++cit )
    {
        avg += cit->mean( p );
    }
    return ( avg < 1.0 ? avg : 1.0 ); // real averaging would be bad when to many processes comes into account
}
