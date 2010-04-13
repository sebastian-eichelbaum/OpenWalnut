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

#include "WSphereCreator.h"

WSphereCreator::WSphereCreator()
{
}

WSphereCreator::~WSphereCreator()
{
}

boost::shared_ptr<WTriangleMesh2>WSphereCreator::createSphere( int resolution, float zoom, float xOff, float yOff, float zOff )
{
    boost::shared_ptr<WTriangleMesh2>sphere = createIcosahedronSphere( resolution );

    for ( size_t i = 0; i < sphere->vertSize(); ++i )
    {
        osg::Vec3 v = sphere->getVertex( i );
        v.normalize();
        osg::Vec4 c( fabs( v[0] ), fabs( v[1] ), fabs( v[2] ), 1.0 );
        sphere->setVertexColor( i, c );
    }

    sphere->zoomMesh( zoom );
    sphere->translateMesh( xOff, yOff, zOff );

    return sphere;
}

boost::shared_ptr<WTriangleMesh2>WSphereCreator::createIcosahedronSphere( int iterations )
{
    float t = ( 1 + sqrt( 5 ) ) / 2;
    float tau = t / sqrt( 1 + t * t );
    float one = 1 / sqrt( 1 + t * t );

    boost::shared_ptr<WTriangleMesh2>sphere = boost::shared_ptr<WTriangleMesh2>( new WTriangleMesh2( 12, 20 ) );

    sphere->addVertex(  tau,  one,  0.0 );
    sphere->addVertex( -tau,  one,  0.0 );
    sphere->addVertex( -tau, -one,  0.0 );
    sphere->addVertex(  tau, -one,  0.0 );
    sphere->addVertex(  one,  0.0,  tau );
    sphere->addVertex(  one,  0.0, -tau );
    sphere->addVertex( -one,  0.0, -tau );
    sphere->addVertex( -one,  0.0,  tau );
    sphere->addVertex(  0.0,  tau,  one );
    sphere->addVertex(  0.0, -tau,  one );
    sphere->addVertex(  0.0, -tau, -one );
    sphere->addVertex(  0.0,  tau, -one );

    sphere->addTriangle( 4, 8, 7 );
    sphere->addTriangle( 4, 7, 9 );
    sphere->addTriangle( 5, 6, 11 );
    sphere->addTriangle( 5, 10, 6 );
    sphere->addTriangle( 0, 4, 3 );
    sphere->addTriangle( 0, 3, 5 );
    sphere->addTriangle( 2, 7, 1 );
    sphere->addTriangle( 2, 1, 6 );
    sphere->addTriangle( 8, 0, 11 );
    sphere->addTriangle( 8, 11, 1 );
    sphere->addTriangle( 9, 10, 3 );
    sphere->addTriangle( 9, 2, 10 );
    sphere->addTriangle( 8, 4, 0 );
    sphere->addTriangle( 11, 0, 5 );
    sphere->addTriangle( 4, 9, 3 );
    sphere->addTriangle( 5, 3, 10 );
    sphere->addTriangle( 7, 8, 1 );
    sphere->addTriangle( 6, 1, 11 );
    sphere->addTriangle( 7, 2, 9 );
    sphere->addTriangle( 6, 10, 2 );



    for ( int i = 0; i < iterations; ++i )
    {
        sphere->doLoopSubD();
    }

    return sphere;
}
