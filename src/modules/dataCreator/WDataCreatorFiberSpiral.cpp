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

#include <algorithm>

#include <core/common/math/WMath.h>

#include "WDataCreatorFiberSpiral.h"

WDataCreatorFiberSpiral::WDataCreatorFiberSpiral():
    WObjectNDIP< WDataSetFibersCreatorInterface >( "Spiral", "Create a spiral of fibers." )
{
    // initialize members
    m_numRotations = m_properties->addProperty( "Num Rotations", "The number of rotations.", 25 );
    m_numRotations->setMin( 1 );
    m_numRotations->setMax( 1000 );
    m_tubeRadius = m_properties->addProperty( "Tube Radius", "The radius of the tube spiralling up.", 1.0  );
    m_tubeRadius->setMin( 0.01 );
    m_tubeRadius->setMax( 5.0 );
}

WDataCreatorFiberSpiral::~WDataCreatorFiberSpiral()
{
    // cleanup
}

void WDataCreatorFiberSpiral::operator()( WProgress::SPtr progress,
                                          const WColor& color,
                                          size_t numFibers,
                                          size_t numVertsPerFiber,
                                          const WPosition& origin,
                                          const WPosition& size,
                                          WDataSetFibers::VertexArray vertices,
                                          WDataSetFibers::IndexArray fibIdx,
                                          WDataSetFibers::LengthArray lengths,
                                          WDataSetFibers::IndexArray fibIdxVertexMap,
                                          WDataSetFibers::ColorArray colors )
{
    std::srand( time( 0 ) );

    WPosition originOffset = origin / 2.0;
    double spiralRadius = std::min( size.x(), size.y() );
    double tubeRadius = m_tubeRadius->get();
    double height = size.z();
    double numRotations = m_numRotations->get();
    WColor fibColor = color;

    // create each
    for( size_t fidx = 0; fidx < numFibers; ++fidx )
    {
        size_t vertOffset = fidx * numVertsPerFiber;
        fibIdx->push_back( vertOffset );
        lengths->push_back( numVertsPerFiber );

        double a1 = static_cast< double >( std::rand() % 255 ) / 255.0;
        double a2 = static_cast< double >( std::rand() % 255 ) / 255.0;

        double seedX = cos( 2.0 * piDouble * a1 ) * tubeRadius;
        double seedY = sin( 2.0 * piDouble * a2 ) * tubeRadius;
        double seedZ = sqrt( tubeRadius - ( seedX * seedX ) - ( seedY * seedY ) );
        WPosition seed( seedX, seedY, seedZ );

        // create the vertices
        for( size_t vidx = 0; vidx < numVertsPerFiber; ++vidx )
        {
            double v = static_cast< double >( vidx ) / static_cast< double >( numVertsPerFiber - 1 );
            double degree = v * 2.0 * piDouble * numRotations;

            double X = seed.x() + cos( degree ) * v * spiralRadius;
            double Y = seed.y() + sin( degree ) * v * spiralRadius;
            double Z = seed.z() + ( v * height );

            vertices->push_back( originOffset.x() + X );
            vertices->push_back( originOffset.y() + Y );
            vertices->push_back( originOffset.z() + Z );

            colors->push_back( fibColor.x() );
            colors->push_back( fibColor.y() );
            colors->push_back( fibColor.z() );
            fibIdxVertexMap->push_back( fidx );
        }

        ++( *progress );
    }
}
