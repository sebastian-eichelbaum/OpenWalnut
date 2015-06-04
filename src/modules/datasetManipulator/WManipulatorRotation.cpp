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

#include <Eigen/Dense>

#include "core/common/math/WMath.h"

#include "WManipulatorRotation.h"

WManipulatorRotation::WManipulatorRotation()
    : WObjectNDIP< WManipulatorInterface >( "Rotation", "Rotates the dataset." )
{
    m_axis = m_properties->addProperty( "Rotation axis", "Axis to rotate around in world coordinates.", WPosition( 0.0, 0.0, 1.0 ) );
    m_angle = m_properties->addProperty( "Rotation angle", "The angle to rotate.", 0.0 );
    m_angle->setMin( 0.0 );
    m_angle->setMax( 2.0 * piDouble );
}

WManipulatorRotation::~WManipulatorRotation()
{
}

WMatrixFixed< double, 4, 4 > WManipulatorRotation::getTransformationMatrix() const
{
    WPosition w = m_axis->get( true );
    Eigen::Vector3d v( w[ 0 ], w[ 1 ],  w[ 2 ] );

    Eigen::AngleAxisd aa( m_angle->get( true ), v );
    WMatrixFixed< double, 4, 4 > mat = WMatrixFixed< double, 4, 4 >::identity();
    for( std::size_t i = 0; i < 3; ++i )
    {
        for( std::size_t j = 0; j < 3; ++j )
        {
            mat( i, j ) = aa.toRotationMatrix()( i, j );
        }
    }

    return mat;
}

bool WManipulatorRotation::transformationChanged() const
{
    return m_angle->changed() || m_axis->changed();
}

void WManipulatorRotation::reset()
{
    m_angle->set( 0.0, false );
    m_axis->set( WPosition( 0.0, 0.0, 1.0 ), false );
}

