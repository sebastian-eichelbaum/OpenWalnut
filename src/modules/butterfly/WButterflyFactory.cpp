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

#include <string>
#include <vector>

#include "WButterflyFactory.h"
#include "WSubdivisionValidator.h"
#include "core/kernel/WKernel.h"


namespace butterfly
{
    WButterflyFactory::WButterflyFactory()
    {
        m_iterations = 0;
        m_maxTriangles = 0;
        m_verts = new WVertexFactory();
        m_triCount = 0;
        m_vertCount = 0;
        m_validator = new WSubdivisionValidator();
        m_cpuThreadCount = 8;
        m_butterflyCalculator = new WButterflyCalculator();
    }

    WButterflyFactory::~WButterflyFactory()
    {
        // TODO(schwarzkopf): Auto-generated destructor stub
    }
    void WButterflyFactory::setButterflySettingW( float butterflySettingW )
    {
        m_butterflyCalculator->setButterflySettingW( butterflySettingW );
    }
    void WButterflyFactory::setIterationsSettings( float m_iterations, float m_maxTriangles10n )
    {
        this->m_iterations = m_iterations;
        this->m_maxTriangles = ( size_t ) pow( 10, m_maxTriangles10n );
    }
    void WButterflyFactory::setCpuThreadCount( size_t cpuThreadCount )
    {
        m_cpuThreadCount = cpuThreadCount;
        m_cpuThreads.reserve( m_cpuThreadCount );
        m_cpuThreads.resize( m_cpuThreadCount );
    }

    void WButterflyFactory::assignProgressCombiner( boost::shared_ptr< WProgressCombiner > progress )
    {
        this->m_associatedProgressCombiner = progress;
    }
    void WButterflyFactory::setProgressSettings( size_t iteration, size_t steps )
    {
        m_associatedProgressCombiner->removeSubProgress( m_progressStatus );
        std::ostringstream headerText;
        headerText << "Iteration " << iteration;
        m_progressStatus = boost::shared_ptr< WProgress >( new WProgress( headerText.str(), steps ) );
        m_associatedProgressCombiner->addSubProgress( m_progressStatus );
    }

    WSubdivisionValidator* WButterflyFactory::getValidator()
    {
        return this->m_validator;
    }
    void WButterflyFactory::examineStencilAll()
    {
        size_t vertSize = m_inputMesh->vertSize();
        size_t threads = m_cpuThreadCount <= vertSize ?m_cpuThreadCount :vertSize;
        for  ( size_t thread = 0; thread < threads; thread++ )
        {
            size_t firstVert = vertSize * thread / threads;
            size_t lastVert = vertSize * ( thread + 1 ) / threads - 1;
            m_cpuThreads[thread] = new boost::thread( &WVertexFactory::examineStencilRange,
                    m_verts, firstVert, lastVert, m_progressStatus );
        }
        for  ( size_t thread = 0; thread < threads; thread++ )
            m_cpuThreads[thread]->join();
    }
    void WButterflyFactory::attachUncalculatedNewVertices()
    {
        for  ( size_t triID = 0; triID < m_triCount; triID++ )
        {
            size_t vertID0 = m_inputMesh->getTriVertId0( triID );
            size_t vertID1 = m_inputMesh->getTriVertId1( triID );
            size_t vertID2 = m_inputMesh->getTriVertId2( triID );
            m_verts->attachNewVertex( vertID0, vertID1 );
            m_verts->attachNewVertex( vertID0, vertID2 );
            m_verts->attachNewVertex( vertID1, vertID2 );
            m_progressStatus->increment( 1 );
        }
    }
    void WButterflyFactory::applyMeshPreprocessing( size_t iteration )
    {
        if  ( iteration == 1 )
        {
            m_validator->setTriangleMesh( m_inputMesh, m_verts );
            m_inputMesh = m_validator->joinNarrowVertices( m_inputMesh );
            m_verts->setTriangleMesh( m_inputMesh );
            m_validator->setTriangleMesh( m_inputMesh, m_verts );
            m_inputMesh = m_validator->flipTrianglesAtLowAngles( m_inputMesh );
            m_verts->setTriangleMesh( m_inputMesh );
        }
    }
    void WButterflyFactory::examineVertexNeighborhood( size_t iteration )
    {
        boost::shared_ptr< WTriangleMesh > tmpMesh( new WTriangleMesh( 0, 0 ) );
        m_outputMesh = tmpMesh;
        m_verts = new WVertexFactory( m_inputMesh );
        m_butterflyCalculator->assignInputMesh( m_inputMesh, m_verts );
        m_triCount = m_inputMesh->triangleSize();
        for  ( size_t triID = 0; triID < m_triCount; triID++ )
        {
            m_verts->registerTriangle( m_inputMesh->getTriVertId0( triID ), triID );
            m_verts->registerTriangle( m_inputMesh->getTriVertId1( triID ), triID );
            m_verts->registerTriangle( m_inputMesh->getTriVertId2( triID ), triID );
            m_progressStatus->increment( 1 );
        }
        applyMeshPreprocessing( iteration );
        attachUncalculatedNewVertices();
        m_validator->setTriangleMesh( m_outputMesh, m_verts );

        for  ( size_t vertID = 0; vertID < m_verts->getVertexCount(); vertID++ )
        {
            m_progressStatus->increment( 1 );
            osg::Vec3d vec = m_inputMesh->getVertex( vertID );
            m_outputMesh->addVertex( osg::Vec3( vec.x(), vec.y(), vec.z() ) );
        }
        examineStencilAll();
        m_validator->generateStatisticalInformation();
    }
    boost::shared_ptr< WTriangleMesh > WButterflyFactory::getSubdividedMesh( boost::shared_ptr< WTriangleMesh > edgedMesh )
    {
        m_inputMesh = edgedMesh;
        for  ( size_t iteration = 1; iteration <= m_iterations &&
            ( iteration == 1 || m_outputMesh->triangleSize() <= m_maxTriangles ); iteration++ )
        {
            setProgressSettings( iteration, m_inputMesh->triangleSize() * 3
                    + m_inputMesh->vertSize() * 3 );

            examineVertexNeighborhood( iteration );
            m_vertCount = m_verts->getVertexCount();
            interpolateNewVertices();
            addInterpolatedContent();
            if  ( m_outputMesh->triangleSize() <= m_maxTriangles )
            {
                if  ( m_inputMesh != edgedMesh )
                {
                    m_inputMesh->~WTriangleMesh();
                    m_inputMesh->~WTransferable();
                    m_inputMesh->~WPrototyped();
                }

                m_inputMesh = m_outputMesh;
            }
            else
            {
                m_outputMesh->~WTriangleMesh();
                m_outputMesh->~WTransferable();
                m_outputMesh->~WPrototyped();
            }
            m_validator->setMinSubdividedLineLength( m_validator->getMinSubdividedLineLength()*
                    m_validator->getMinSubdividedLineLengthMultiplierPerIteration() );
            m_verts->~WVertexFactory();
        }
        m_progressStatus->finish();
        return m_inputMesh;
    }
    void WButterflyFactory::interpolateNewVertices()
    {
        size_t vertSize = m_inputMesh->vertSize();
        size_t threads = m_cpuThreadCount <= vertSize ?m_cpuThreadCount :vertSize;
        for  ( size_t thread = 0; thread < threads; thread++ )
        {
            size_t firstVert = vertSize * thread / threads;
            size_t lastVert = vertSize * ( thread + 1 ) / threads - 1;
            m_cpuThreads[thread] = new boost::thread( &WButterflyFactory::interpolateNewVerticesRange,
                    this, firstVert, lastVert );
        }
        for  ( size_t thread = 0; thread < threads; thread++ )
            m_cpuThreads[thread]->join();
    }
    void WButterflyFactory::interpolateNewVerticesRange( size_t fromVertex, size_t toVertex )
    {
        for  ( size_t vertID1 = fromVertex; vertID1 <= toVertex; vertID1++ )
        {
            WVertexProperty* property = m_verts->getProperty( vertID1 );
            vector<WNewVertex*> newVertices = property->getNewVerticesToHigherNeighborID();
            for  ( size_t newVert = 0; newVert < newVertices.size(); newVert++ )
            {
                WNewVertex* newVertex =  newVertices[newVert];
                size_t vertID2 = newVertex->getToID();
                Vec3 newCoord = m_butterflyCalculator->calcNewVertex( vertID1, vertID2 );
                newCoord = m_validator->getValidatedSubdivision( vertID1, vertID2, newCoord );
                newVertex->setValid( m_validator->isValidSubdivision( vertID1, vertID2, newCoord ) );
                newVertex->setCoordinate( newCoord );
            }
            m_progressStatus->increment( 1 );
        }
    }
    void WButterflyFactory::addInterpolatedContent()
    {
        for  ( size_t vertID1 = 0; vertID1 < m_inputMesh->vertSize(); vertID1++ )
        {
            WVertexProperty* property = m_verts->getProperty( vertID1 );
            vector<WNewVertex*> newVertices = property->getNewVerticesToHigherNeighborID();
            for  ( size_t newVert = 0; newVert < newVertices.size(); newVert++ )
            {
                WNewVertex* newVertex =  newVertices[newVert];
                if  ( newVertex->isValid() )
                {
                    m_outputMesh->addVertex( newVertex->getCoordinate() );
                    newVertex->setNewVertexID( m_vertCount++ );
                }
                m_progressStatus->increment( 1 );
            }
        }
        for  ( size_t triID = 0; triID < m_triCount &&
                m_outputMesh->triangleSize() <= m_maxTriangles; triID++ )
        {
            if  ( m_verts->isValidTriangle( triID ) )
            {
                size_t vertID0 = m_inputMesh->getTriVertId0( triID );
                size_t vertID1 = m_inputMesh->getTriVertId1( triID );
                size_t vertID2 = m_inputMesh->getTriVertId2( triID );
                m_validator->subdivideTriangle( vertID0, vertID1, vertID2 );
            }
            m_progressStatus->increment( 1 );
        }
    }
} /* namespace std */
