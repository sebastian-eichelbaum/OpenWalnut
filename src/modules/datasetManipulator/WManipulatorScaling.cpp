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

#include "WManipulatorScaling.h"

WManipulatorScaling::WManipulatorScaling()
    : WObjectNDIP< WManipulatorInterface >( "Scaling", "Scales the dataset." )
{
    m_scaling = m_properties->addProperty( "Scale", "Scale of the data.", WPosition( 1.0, 1.0, 1.0 ) );
}

WManipulatorScaling::~WManipulatorScaling()
{
}

WMatrixFixed< double, 4, 4 > WManipulatorScaling::getTransformationMatrix() const
{
    WMatrixFixed< double, 4, 4 > m = WMatrixFixed< double, 4, 4 >::identity();
    WPosition s = m_scaling->get( true );
    m( 0, 0 ) = s[ 0 ];
    m( 1, 1 ) = s[ 1 ];
    m( 2, 2 ) = s[ 2 ];

    return m;
}

bool WManipulatorScaling::transformationChanged() const
{
    return m_scaling->changed();
}

void WManipulatorScaling::reset()
{
    m_scaling->set( WPosition( 1.0, 1.0, 1.0 ) );
}

