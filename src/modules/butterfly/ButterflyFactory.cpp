//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2013 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

/*
 * ButterflyFactory.cpp
 *
 *  Created on: 30.06.2013
 *      Author: renegade
 */

#include "ButterflyFactory.h"
#include "core/kernel/WKernel.h"

namespace butterfly
{
    float ButterflyFactory::w = 0.0f,
        ButterflyFactory::factor[4][7] = {{0.75f, 5.0f/12.0f, -1.0f/12.0f, -1.0f/12.0f, 0.0f, 0.0f, 0.0f},   //NOLINT
                                          {0.75f, 3.0f/8.0f, -1.0f/8.0f, 0.0f, -1.0f/8.0f, 0.0f, 0.0f},//NOLINT
                                          {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},//NOLINT
                                          {0.5f-w, 0.0f, 0.125f+2*w, -0.0625f-w, w, -0.0625f-w, 0.125f+2*w}},//NOLINT
        ButterflyFactory::s1 = 9.0f / 16.0f,
        ButterflyFactory::s2 = -1.0f / 16.0f;

    ButterflyFactory::ButterflyFactory()
    {
        iterations = 0;
        maxTriangles = 0;
        verts = new VertexFactory();
        triCount = 0;
    }

    ButterflyFactory::~ButterflyFactory()
    {
        // TODO(schwarzkopf): Auto-generated destructor stub
    }

    void ButterflyFactory::assignSettings( WPropInt m_iterations, WPropDouble m_maxTriangles10n )
    {
        this->iterations = m_iterations->get();
        this->maxTriangles = ( size_t ) pow( 10, m_maxTriangles10n->get() );
        this->mesh = mesh;
    }

    void ButterflyFactory::examineInputMesh()
    {
        triCount = 0;
        verts = new VertexFactory( mesh );
        boost::shared_ptr< WTriangleMesh > tmpMesh( new WTriangleMesh( 0, 0 ) );
        triMesh = tmpMesh;
        try
        {
            while( true )
            {
                verts->registerTriangle( mesh->getTriVertId0( triCount ), triCount );
                verts->registerTriangle( mesh->getTriVertId1( triCount ), triCount );
                verts->registerTriangle( mesh->getTriVertId2( triCount ), triCount );
                triCount++;
            }
        }
        catch( ... )
        {
        }

        for( size_t i = 0; i < verts->getVertexCount(); i++ )
        {
            osg::Vec3d vec = mesh->getVertex( i );
            triMesh->addVertex( osg::Vec3( vec.x(), vec.y(), vec.z() ) );
        }
        verts->examineCircularityAll();
    }

    boost::shared_ptr< WTriangleMesh > ButterflyFactory::getInterpolatedMesh( boost::shared_ptr< WTriangleMesh > inputMesh )
    {
        this->mesh = inputMesh;
        examineInputMesh();
        for( size_t iteration = 0; iteration < iterations && triCount * 4 <= maxTriangles; iteration++ )
        {
            size_t vertCount = verts->getVertexCount();
            for( size_t triIdx = 0; triIdx < triCount; triIdx++ )
            {
                size_t id0 = mesh->getTriVertId0( triIdx );
                size_t id1 = mesh->getTriVertId1( triIdx );
                size_t id2 = mesh->getTriVertId2( triIdx );

                if( !verts->neighborExists( id0, id1 ) )
                {
                    verts->attachMid( id0, id1, vertCount++ );
                    triMesh->addVertex( calcMid( id0, id1 ) );
                }
                if( !verts->neighborExists( id0, id2 ) )
                {
                    verts->attachMid( id0, id2, vertCount++ );
                    triMesh->addVertex( calcMid( id0, id2 ) );
                }
                if( !verts->neighborExists( id1, id2 ) )
                {
                    verts->attachMid( id1, id2, vertCount++ );
                    triMesh->addVertex( calcMid( id1, id2 ) );
                }

                size_t mid0_1 = verts->getMidID( id0, id1 ),
                    mid0_2 = verts->getMidID( id0, id2 ),
                    mid1_2 = verts->getMidID( id1, id2 );

                triMesh->addTriangle( id0, mid0_1, mid0_2 );
                triMesh->addTriangle( id1, mid1_2, mid0_1 );
                triMesh->addTriangle( id2, mid0_2, mid1_2 );
                triMesh->addTriangle( mid0_1, mid1_2, mid0_2 );
            }
            mesh = triMesh;
            if( iteration + 1 < iterations )
                examineInputMesh();
        }
        return mesh;
    }

    osg::Vec3 ButterflyFactory::calcMid( size_t vertID1, size_t vertID2 )
    {
        int bound1 = verts->getBoundCountClass( vertID1 ),
            bound2 = verts->getBoundCountClass( vertID2 ),
            k1 = verts->getNeighbourVertexCount( vertID1 ),
            k2 = verts->getNeighbourVertexCount( vertID2 );
        if( bound1 == 1 )
            k1 += k1 - 2;
        if( bound2 == 1 )
            k2 += k2 - 2;
        Vec3 final = Vec3( 0.0f, 0.0f, 0.0f );
        if( bound1 < 2 && bound2 < 2 && bound1 >= 0 && bound2 >= 0 )
        {
            if( bound1 != 1 || bound2 != 1 )
            {  //Case 1: two vertices of valence 6
                if( k1 == 6 && k2 == 6 )
                {
                    final = add( Vec3( 0.0f, 0.0f, 0.0f ), getKNeighbourValueBoundary( vertID1, vertID2, false, false ), 1.0f );
                    final = add( final, getKNeighbourValueBoundary( vertID2, vertID1, true, false ), 1.0f );
                }
                else if( false/*(k1==6) != (k2==6)*/ )
                { //Case 2: one vertex of valence=6 and onother has valence!=6
                    if( k1 != 6 )
                    {
                        final = getKNeighbourValueBoundary( vertID1, vertID2, false, false );
                    }
                    else
                    {
                        final = getKNeighbourValueBoundary( vertID2, vertID1, false, false );
                    }
                }
                else if( k1 != 6 || k2 != 6 )
                {  //Case 3: two vertices of valence!=6
                    final = add( Vec3( 0.0f, 0.0f, 0.0f ), getKNeighbourValueBoundary( vertID1, vertID2, false, true ), 0.5f );
                    final = add( final, getKNeighbourValueBoundary( vertID2, vertID1, false, true ), 0.5f );
                }
            }
            else
            {       //Case 4: >=1 vertex on a bound
                int start1 = verts->getNeighbourIndex( vertID1, vertID2 ),
                    start2 = verts->getNeighbourIndex( vertID2, vertID1 );
                size_t boundNeighbour = verts->getVertexProperty( vertID1 )->getNeighbourVertices()[k1/2-start1];
                final = add( Vec3( 0.0f, 0.0f, 0.0f ), mesh->getVertex( boundNeighbour ), s2 );
                boundNeighbour = verts->getVertexProperty( vertID2 )->getNeighbourVertices()[k2/2-start2];
                final = add( final, mesh->getVertex( boundNeighbour ), s2 );
                final = add( final, mesh->getVertex( vertID1 ), s1 );
                final = add( final, mesh->getVertex( vertID2 ), s1 );
            }
        }
        else
            final = calcMean( vertID1, vertID2 );
        return final;
    }
    Vec3 ButterflyFactory::calcMean( size_t vertID1, size_t vertID2 )
    {
        Vec3 p1 = mesh->getVertex( vertID1 ),
            p2 = mesh->getVertex( vertID2 );
        return osg::Vec3( ( p1.x() + p2.x() ) / 2, ( p1.y() + p2.y() ) / 2, ( p1.z() + p2.z() ) / 2 );
    }

    Vec3 ButterflyFactory::add( Vec3 base, Vec3 sum, float factor )
    {
        float baseX = base.x(), baseY = base.y(), baseZ = base.z();
        float sumX = sum.x(), sumY = sum.y(), sumZ = sum.z();
        float x = baseX + sumX * factor,
            y = baseY + sumY * factor,
            z = baseZ + sumZ * factor;
        return Vec3( x, y, z );
    }

    Vec3 ButterflyFactory::getKNeighbourValueBoundary( size_t stencilCenterVertID, size_t directedNeighbourVertID, bool isSecondK6, bool treatK6AsKn )
    {
        int start = verts->getNeighbourIndex( stencilCenterVertID, directedNeighbourVertID );
        float k = verts->getVertexProperty( stencilCenterVertID )->getNeighbourVertexCount();
        int bounds = verts->getBoundCountClass( stencilCenterVertID );
        if( bounds == 1 )
            k += k - 2;
        int k_row = static_cast<int>( k + 0.3f ) - 3;
        bool isDefaultButterfly = ( k == 3 || k == 4 || ( k == 6 && !treatK6AsKn ) );
        Vec3 final = add( Vec3( 0.0f, 0.0f, 0.0f ), mesh->getVertex( stencilCenterVertID ),
                         isDefaultButterfly ? factor[k_row][0] : factor[0][0] );
        for( int i = 0; i < k; i++)
        {
            float j = i - start;
            if( j < 0.0f )
                j = k + j;
            bool isBehindBound = ( bounds == 1 && i > k / 2 );
            int i_mirrored = isBehindBound ?i-k/2 :i,
                k_col = 1 + static_cast<int>( j + 0.3f );
            int neighbourID = verts->getVertexProperty( stencilCenterVertID )->getNeighbourVertices()[i_mirrored];
            float s_j = isDefaultButterfly ?factor[k_row][k_col]
                :( 0.25f + cos( 2.0f * M_PI * j / k ) + 0.5f * cos( 4.0f * M_PI * j / k ) ) / k;
            Vec3 value = mesh->getVertex( neighbourID );
            if( isBehindBound )
            {  //Case 4: >=1 vertex on a bound
                value = add( mesh->getVertex( stencilCenterVertID ), value, -1.0f );
                value = add( mesh->getVertex( stencilCenterVertID ), value, 1.0f );
            }
            if( !isSecondK6 || ( j != 1 && j != 5 ) )
                final = add( final, value, s_j );
        }
        return final;
    }

} /* namespace std */
