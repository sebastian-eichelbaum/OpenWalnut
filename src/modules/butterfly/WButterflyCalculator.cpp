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

// Additional copyright information:
// This class partially relies on the Butterfly Subdivision algorithm.
// that dates from Denis Zorin, Peter Schroeder, Wim Sweldens, Nira Dyn, David
// Levin and John A. Gregory. The original algorithm is depicted in:
// http://wwwmath.tau.ac.il/~niradyn/papers/butterfly.pdf
// http://mrl.nyu.edu/~dzorin/papers/zorin1996ism.pdf
// This work was required especially in the methods calcNewVertex and
// getInterpolatedValue.

#include <string>
#include <vector>

#include "WButterflyCalculator.h"
#include "WSubdivisionValidator.h"
#include "core/kernel/WKernel.h"


namespace butterfly
{
    float WButterflyCalculator::m_w = 0.0f;
    float WButterflyCalculator::m_weights[4][7] =
            {{0.75f, 5.0f / 12.0f, -1.0f / 12.0f, -1.0f / 12.0f, 0.0f, 0.0f, 0.0f}, /*Valence=3*///NOLINT
            {0.875f, 3.0f / 8.0f, -1.0f / 8.0f, 0.0f, -1.0f / 8.0f, 0.0f, 0.0f},/*Valence=4*///NOLINT
            {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},//NOLINT
            {0.5f - m_w, 0.0f, 0.0625f + m_w, -0.0625f - m_w, m_w, -0.0625f - m_w, 0.0625f + m_w}};/*Valence=6*///NOLINT
    float WButterflyCalculator::m_weightCenterAtBorder = 9.0f / 16.0f;
    float WButterflyCalculator::m_weightRimAtBorder = -1.0f / 16.0f;

    WButterflyCalculator::WButterflyCalculator()
    {
        m_verts = new WVertexFactory();
    }

    WButterflyCalculator::~WButterflyCalculator()
    {
        // TODO(schwarzkopf): Auto-generated destructor stub
    }
    void WButterflyCalculator::assignInputMesh( boost::shared_ptr< WTriangleMesh > inputMesh,
            WVertexFactory* vertexProperties )
    {
        this->m_inputMesh = inputMesh;
        this->m_verts = vertexProperties;
    }
    void WButterflyCalculator::setButterflySettingW( float butterflySettingW )
    {
        m_w = butterflySettingW;
        m_weights[3][0] = 0.5f - m_w;
        m_weights[3][2] = 0.0625f + m_w;
        m_weights[3][3] = -0.0625f - m_w;
        m_weights[3][4] = m_w;
        m_weights[3][5] = -0.0625f - m_w;
        m_weights[3][6] = 0.0625f + m_w;
    }

    osg::Vec3 WButterflyCalculator::calcNewVertex( size_t vertID1, size_t vertID2 )
    {
        int bound1 = m_verts->getProperty( vertID1 )->getBoundClass();
        int bound2 = m_verts->getProperty( vertID2 )->getBoundClass();
        int valence1 = m_verts->getProperty( vertID1 )->getValence();
        int valence2 = m_verts->getProperty( vertID2 )->getValence();
        if( bound1 == 1 )
            valence1 += valence1 - 2;
        if( bound2 == 1 )
            valence2 += valence2 - 2;
        Vec3 final = Vec3( 0.0f, 0.0f, 0.0f );
        if  ( bound1 < 2 && bound2 < 2 && bound1 >= 0 && bound2 >= 0 )
        {
            if( bound1 != 1 || bound2 != 1 )
            {  //Case 1: two vertices of valence 6
                if  ( valence1 == 6 && valence2 == 6 )
                {
                    final = m_verts->add( Vec3( 0.0f, 0.0f, 0.0f ),
                            getInterpolatedValue( vertID1, vertID2, false ), 1.0f );
                    final = m_verts->add( final,
                            getInterpolatedValue( vertID2, vertID1, false ), 1.0f );
                }
                else if( valence1 != 6 || valence2 != 6 )
                {  //Case 2/3: at least one vertex of valence!=6
                    final = m_verts->add( Vec3( 0.0f, 0.0f, 0.0f ),
                            getInterpolatedValue( vertID1, vertID2, true ), 0.5f );
                    final = m_verts->add( final,
                            getInterpolatedValue( vertID2, vertID1, true ), 0.5f );
                }
            }
            else
            {   //Case 4: >=1 vertex on a bound
                int start1 = m_verts->getProperty( vertID1 )->getStencilNeighbourIndex( vertID2 );
                int start2 = m_verts->getProperty( vertID2 )->getStencilNeighbourIndex( vertID1 );
                size_t boundNeighbour = m_verts->getProperty( vertID1 )->getStencilNeighbourID( valence1 / 2 - start1 );
                final = m_verts->add( Vec3( 0.0f, 0.0f, 0.0f ), m_inputMesh->getVertex( boundNeighbour ), m_weightRimAtBorder );
                boundNeighbour = m_verts->getProperty( vertID2 )->getStencilNeighbourID( valence2 / 2 - start2 );
                final = m_verts->add( final, m_inputMesh->getVertex( boundNeighbour ), m_weightRimAtBorder );
                final = m_verts->add( final, m_inputMesh->getVertex( vertID1 ), m_weightCenterAtBorder );
                final = m_verts->add( final, m_inputMesh->getVertex( vertID2 ), m_weightCenterAtBorder );
            }
        }
        else
            final = calcMean( vertID1, vertID2 );

        return final;
    }
    Vec3 WButterflyCalculator::calcMean( size_t vertID1, size_t vertID2 )
    {
        Vec3 vert1 = m_inputMesh->getVertex( vertID1 ),
            vert2 = m_inputMesh->getVertex( vertID2 );
        float coordX = ( vert1.x() + vert2.x() ) / 2.0f,
              coordY = ( vert1.y() + vert2.y() ) / 2.0f,
              coordZ = ( vert1.z() + vert2.z() ) / 2.0f;
        return osg::Vec3( coordX , coordY, coordZ );
    }
    Vec3 WButterflyCalculator::getInterpolatedValue( size_t stencilCenterVertID,
            size_t directedNeighbourVertID, bool isIrregular )
    {
        WVertexProperty* property = m_verts->getProperty( stencilCenterVertID );
        int start = property->getStencilNeighbourIndex( directedNeighbourVertID );
        float valence = property->getValence();
        int bounds = property->getBoundClass();
        if  ( bounds == 1 )
            valence += valence - 2;

        int weightRow = static_cast<int>( valence + 0.3f ) - 3;
        bool hasCustomWeight = ( valence == 3 || valence == 4 || ( valence == 6 && !isIrregular ) );
        Vec3 final = m_verts->add( Vec3( 0.0f, 0.0f, 0.0f ), m_inputMesh->getVertex( stencilCenterVertID ),
                          hasCustomWeight ? m_weights[weightRow][0] : m_weights[0][0] );

        for  ( int index = 0; index < valence; index++ )
        {
            float neighbour = index - start;
            if  ( neighbour < 0.0f )
                neighbour = valence + neighbour;

            bool isBehindBound = ( bounds == 1 && index > valence / 2 );
            int i_mirrored = isBehindBound ?index-valence/2 :index,
                weightCol = 1 + static_cast<int>( neighbour + 0.3f );
            int neighbourID = property->getStencilNeighbourID( i_mirrored );
            float weight = hasCustomWeight ?m_weights[weightRow][weightCol]
                :( 0.25f + cos( 2.0f * M_PI * neighbour / valence )
                  + 0.5f * cos( 4.0f * M_PI * neighbour / valence ) ) / valence;
            Vec3 value = m_inputMesh->getVertex( neighbourID );
            if  ( isBehindBound )
            {  //Case 4: >=1 vertex on a bound
                value = m_verts->add( m_inputMesh->getVertex( stencilCenterVertID ), value, -1.0f );
                value = m_verts->add( m_inputMesh->getVertex( stencilCenterVertID ), value, 1.0f );
            }
            final = m_verts->add( final, value, weight );
        }
        return final;
    }

} /* namespace std */
