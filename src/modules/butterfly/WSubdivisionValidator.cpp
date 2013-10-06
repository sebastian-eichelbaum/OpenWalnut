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

#include <vector>
#include <cmath>

#include "WSubdivisionValidator.h"


namespace butterfly
{
WSubdivisionValidator::WSubdivisionValidator()
{
    this->m_processedMesh = m_processedMesh;
    this->m_vertexFactory = new WVertexFactory();
    this->m_minTransformationAngle = 0.0f;
    this->m_maxTransformationAngle = 180.0f;
    this->m_minLenghtsQuotient = 0.0f;
    this->m_minAmountOfMax = 0.0f;
    this->m_minAmountOfAverage = 0.0f;
    this->m_minSubdividedLineLength = 0.0f;
    this->m_minSubdividedLineLengthMultiplierPerIteration = 1.0f;
    this->m_minNeighbourTriangleAngle = 0.0f;
    this->m_maxNeighbourTriangleAngle = 180.0f;
    this->m_maxNeighbourTriangleAngleLengthRatio = 0.3f;
    this->m_maxAmountOfMaxForVertexJoin = 0.0f;
    this->m_maxNeighbourTriangleAngleTriangleFlip = 0.0f;
}

WSubdivisionValidator::~WSubdivisionValidator()
{
    // TODO(schwarzkopf): Auto-generated destructor stub
}

void WSubdivisionValidator::setTriangleMesh( boost::shared_ptr< WTriangleMesh > processedMesh, WVertexFactory* vertexFactory )
{
    this->m_processedMesh = processedMesh;
    this->m_vertexFactory = vertexFactory;
}
void WSubdivisionValidator::generateStatisticalInformation()
{
    for  ( size_t vertexID = 0; vertexID < m_vertexFactory->getVertexCount(); vertexID++ )
    {
        float maxDistance = 0.0f;
        float summedDistance = 0.0f;

        WVertexProperty* property = m_vertexFactory->getProperty( vertexID );
        size_t neighbourCount = property->getValence();
        for  ( size_t neighbour = 0; neighbour < neighbourCount; neighbour++ )
        {
            size_t neighbourID = property->getStencilNeighbourID( neighbour );
            osg::Vec3 point1 = m_processedMesh->getVertex( vertexID );
            osg::Vec3 point2 = m_processedMesh->getVertex( neighbourID );
            float distance = WVertexFactory::getDistance( point1, point2 );
            maxDistance = distance>maxDistance ?distance :maxDistance;
            summedDistance += distance;
        }
        property->setMaxNeighbourDistance( maxDistance );
        property->setSumNeighbourDistance( summedDistance );
    }
}

boost::shared_ptr< WTriangleMesh > WSubdivisionValidator::joinNarrowVertices(
        boost::shared_ptr< WTriangleMesh > inputMesh )
{
    if  ( m_maxAmountOfMaxForVertexJoin > 0.0f )
    {
        size_t vertCount = inputMesh->vertSize();
        boost::shared_ptr< WTriangleMesh > outputMesh( new WTriangleMesh( 0, 0 ) );
        size_t triCount = inputMesh->triangleSize();
        std::vector<osg::Vec3> vertices;
        vertices.reserve( vertCount );
        vertices.resize( vertCount );
        for  ( size_t idx = 0; idx < vertCount; idx++ )
            vertices[idx] = osg::Vec3( inputMesh->getVertex( idx ) );
        std::vector<size_t> triangles;
        triangles.reserve( triCount * 3 );
        triangles.resize( triCount * 3 );
        for  (size_t triangle = 0; triangle < triCount; triangle++ )
        {
            triangles[triangle * 3 + 0] = inputMesh->getTriVertId0( triangle );
            triangles[triangle * 3 + 1] = inputMesh->getTriVertId1( triangle );
            triangles[triangle * 3 + 2] = inputMesh->getTriVertId2( triangle );
        }
        for  ( size_t triangle = 0; triangle < triCount; triangle++ )
        {
            size_t id0 = triangles[triangle * 3 + 0];
            size_t id1 = triangles[triangle * 3 + 1];
            size_t id2 = triangles[triangle * 3 + 2];
            for  ( size_t orientation = 0; orientation < 3 && id0 != id1 && id1 != id2 && id2 != id0; orientation++)
            {
                size_t tmp = id0;
                id0 = id1;
                id1 = id2;
                id2 = tmp;

                float distanceA = WVertexFactory::getDistance( vertices[id0], vertices[id1] );
                float distanceB = WVertexFactory::getDistance( vertices[id0], vertices[id2] );
                float distanceC = WVertexFactory::getDistance( vertices[id1], vertices[id2] );
                float distanceCquot = distanceC/m_maxAmountOfMaxForVertexJoin;
                if  ((distanceCquot < distanceA || distanceCquot < distanceB) && distanceC < distanceA && distanceC < distanceB )
                {
                    size_t vert1 = id1 <= id2 ?id1 :id2;
                    size_t vert2 = id1 <= id2 ?id2 :id1;

                    vertices[vert1] = WVertexFactory::add( osg::Vec3( 0, 0, 0 ), vertices[vert1], 0.5f );
                    vertices[vert1] = WVertexFactory::add( vertices[vert1], vertices[vert2], 0.5f );
                    vertices[vert2] = vertices[vert1];
                    WVertexProperty* prop1 = m_vertexFactory->getProperty( vert1 );
                    WVertexProperty* prop2 = m_vertexFactory->getProperty( vert2 );
                    vector<size_t> attachedTriangles = prop2->getAttachedTriangles();
                    for  ( size_t attachedTri = 0; attachedTri < attachedTriangles.size(); attachedTri++ )
                    {
                        size_t currentTriangle = attachedTriangles[attachedTri];
                        prop1->attachTriangle( currentTriangle );
                        prop2->detachTriangle( currentTriangle );
                        for  ( size_t edge = 0; edge < 3; edge++ )
                            if  ( triangles[currentTriangle * 3 + edge] == vert2 )
                                triangles[currentTriangle * 3 + edge] = vert1;
                    }
                    id1 = id1 <= id2 ?id1 :id2;
                    id2 = id1 <= id2 ?id1 :id2;
                }
            }
        }
        for  ( size_t vertexID = 0; vertexID < vertCount; vertexID++ )
        {
            outputMesh->addVertex( vertices[vertexID] );

            vector<size_t> tris = m_vertexFactory->getProperty( vertexID )->getAttachedTriangles();
        }
        for  ( size_t idx = 0; idx < triCount; idx++ )
            outputMesh->addTriangle( triangles[idx * 3 + 0], triangles[idx * 3 + 1], triangles[idx * 3 + 2] );

        return outputMesh;
    }
    return inputMesh;
}
boost::shared_ptr< WTriangleMesh > WSubdivisionValidator::flipTrianglesAtLowAngles(
        boost::shared_ptr< WTriangleMesh > inputMesh )
{
    if  ( m_maxNeighbourTriangleAngleTriangleFlip > 0.0f )
    {
        size_t vertCount = inputMesh->vertSize();
        boost::shared_ptr< WTriangleMesh > outputMesh( new WTriangleMesh( 0, 0 ) );
        for  ( size_t vertID = 0; vertID < vertCount; vertID++)
            outputMesh->addVertex( inputMesh->getVertex( vertID ) );

        size_t triCount = inputMesh->triangleSize();
        std::vector<bool> isProcessed;
        isProcessed.reserve( triCount );
        isProcessed.resize( triCount );
        std::vector<size_t> triangles;
        triangles.reserve( triCount * 3 );
        triangles.resize( triCount * 3 );
        for  ( size_t triangle = 0; triangle < triCount; triangle++ )
        {
            isProcessed[triangle] = false;
            triangles[triangle * 3 + 0] = inputMesh->getTriVertId0( triangle );
            triangles[triangle * 3 + 1] = inputMesh->getTriVertId1( triangle );
            triangles[triangle * 3 + 2] = inputMesh->getTriVertId2( triangle );
        }
        for  (size_t triangle = 0; triangle < triCount; triangle++ )
        {
            size_t id0 = triangles[ triangle * 3 + 0 ];
            size_t id1 = triangles[ triangle * 3 + 1 ];
            size_t id2 = triangles[ triangle * 3 + 2 ];
            for  ( size_t index = 0; index < 3; index++ )
            {
                size_t tmp = id0;
                id0 = id1;
                id1 = id2;
                id2 = tmp;

                std::vector<size_t> neighbours = m_vertexFactory->getAttachedTriangleIDs( triangle, id0, id1 );
                if  ( neighbours.size() == 1 && !isProcessed[triangle] )
                {
                    size_t triangle2 = neighbours[0];
                    size_t vecID3OtherTriangle = m_vertexFactory->get3rdVertexID( triangle2, id0, id1 );

                    osg::Vec3 point0 = m_processedMesh->getVertex( id0 );
                    osg::Vec3 point1 = m_processedMesh->getVertex( id1 );
                    osg::Vec3 point2 = m_processedMesh->getVertex( id2 );
                    osg::Vec3 point2other = m_processedMesh->getVertex( vecID3OtherTriangle );
                    float lengthA1 = WVertexFactory::getDistance( point0, point2 );
                    float lengthB1 = WVertexFactory::getDistance( point1, point2 );
                    float lengthA2 = WVertexFactory::getDistance( point0, point2other );
                    float lengthB2 = WVertexFactory::getDistance( point1, point2other );
                    float lengthC = WVertexFactory::getDistance( point0, point1 );
                    float lengthD = WVertexFactory::getDistance( point2, point2other );
                    float alpha1 = getAlphaLawOfCosines( lengthB1, lengthA1, lengthC );
                    float beta1 = getAlphaLawOfCosines( lengthA1, lengthB1, lengthC );
                    float alpha2 = getAlphaLawOfCosines( lengthB1, lengthA1, lengthC );
                    float beta2 = getAlphaLawOfCosines( lengthA1, lengthB1, lengthC );
                    float gammaA1 = getAlphaLawOfCosines( lengthA2, lengthA1, lengthD );
                    float gammaB1 = getAlphaLawOfCosines( lengthB2, lengthB1, lengthD );

                    if  ( gammaA1 >= m_maxNeighbourTriangleAngleTriangleFlip
                            && gammaB1 >= m_maxNeighbourTriangleAngleTriangleFlip
                            && 180.0f-gammaA1 - alpha1 - alpha2 >= m_maxNeighbourTriangleAngleTriangleFlip
                            && 180.0f-gammaB1 - beta1 - beta2 >= m_maxNeighbourTriangleAngleTriangleFlip
                            && alpha1 + alpha2 < 180.0f && beta1 + beta2 < 180.0f
                            && getMinTrianglesAngle( id0, id1, true ) <= m_maxNeighbourTriangleAngleTriangleFlip
                            && !isProcessed[triangle] && !isProcessed[triangle2] )
                    {
                        m_vertexFactory->getProperty( id0 )->detachTriangle( triangle2 );
                        m_vertexFactory->getProperty( id1 )->detachTriangle( triangle );
                        m_vertexFactory->getProperty( id2 )->attachTriangle( triangle2 );
                        m_vertexFactory->getProperty( vecID3OtherTriangle )->attachTriangle( triangle );

                        for  ( size_t idx = 0; idx < 3; idx++ )
                        {
                            if  ( triangles[triangle * 3 + idx] == id1 )
                                triangles[triangle * 3 + idx] = vecID3OtherTriangle;
                            if  ( triangles[triangle2 * 3 + idx] == id0 )
                                triangles[triangle2 * 3 + idx] = id2;
                        }

                        isProcessed[triangle] = true;
                        isProcessed[triangle2] = true;
                    }
                }
            }
        }
        for  ( size_t idx = 0; idx < triCount; idx++ )
            outputMesh->addTriangle( triangles[idx * 3 + 0], triangles[idx * 3 + 1], triangles[idx * 3 + 2] );

        return outputMesh;
    }
    return inputMesh;
}

osg::Vec3 WSubdivisionValidator::getValidatedSubdivision( size_t vertID1, size_t vertID2, osg::Vec3 newVert )
{
    if  ( m_minNeighbourTriangleAngle < 90.0f )
    {
        float minAngle = getMinTrianglesAngle( vertID1, vertID2, false );
        if  ( minAngle < m_minNeighbourTriangleAngle )
        {
            float weight = minAngle/m_minNeighbourTriangleAngle;
            newVert = WVertexFactory::add( osg::Vec3( 0, 0, 0 ), newVert, weight );
            newVert = WVertexFactory::add( newVert, m_processedMesh->getVertex( vertID1 ), 0.5f * ( 1-weight ) );
            newVert = WVertexFactory::add( newVert, m_processedMesh->getVertex( vertID2 ), 0.5f * ( 1-weight ) );
        }
    }
    return newVert;
}
bool WSubdivisionValidator::isValidSubdivision( size_t vertID1, size_t vertID2, osg::Vec3 vert3 )
{
    if  ( vertID1 == vertID2 )
        return false;

    osg::Vec3 vert1 = m_processedMesh->getVertex( vertID1 );
    osg::Vec3 vert2 = m_processedMesh->getVertex( vertID2 );
    float lengthA = WVertexFactory::getDistance( vert1, vert3 );
    float lengthB = WVertexFactory::getDistance( vert2, vert3 );
    float lengthC = WVertexFactory::getDistance( vert1, vert2 );
    if  ( !isValidMinAmountOfMax( vertID1, vertID2, lengthC ) )
        return false;

    if  ( !isValidMinAmountOfAverage( vertID1, vertID2, lengthC ) )
        return false;

    if  ( !isValidMinLengthsQuotient( lengthA, lengthB ) )
        return false;

    if  ( !isValidMinTransformationAngle( lengthA, lengthB, lengthC ) )
        return false;

    if  ( !isValidMaxTransformationAngle( lengthA, lengthB, lengthC ) )
        return false;

    if  ( !isValidMinSubdividedLineLength( lengthC ) )
        return false;

    if  ( !isValidMaxNeighbourTriangleAngle( vertID1, vertID2, lengthC ) )
        return false;

    return true;
}
bool WSubdivisionValidator::isValidMinAmountOfMax( size_t vertID1, size_t vertID2, float lengthC )
{
    if  ( this->m_minAmountOfMax == 0.0f )
        return true;
    float vertex1max = m_vertexFactory->getProperty( vertID1 )->getMaxNeighbourDistance();
    float vertex2max = m_vertexFactory->getProperty( vertID2 )->getMaxNeighbourDistance();
    return lengthC >= vertex1max*m_minAmountOfMax
            && lengthC >= vertex2max*m_minAmountOfMax;
}
bool WSubdivisionValidator::isValidMinAmountOfAverage( size_t vertID1, size_t vertID2, float lengthC )
{
    if  ( this->m_minAmountOfAverage == 0.0f )
        return true;
    WVertexProperty* vertex1 = m_vertexFactory->getProperty( vertID1 );
    WVertexProperty* vertex2 = m_vertexFactory->getProperty( vertID2 );
    float vertex1Valence = vertex1->getValence();
    float vertex2Valence = vertex2->getValence();
    float vertex1Sum = vertex1->getSumNeighbourDistance();
    float vertex2Sum = vertex2->getSumNeighbourDistance();
    float average = vertex1Sum + vertex2Sum - 2.0f * lengthC;
    average /= vertex1Valence + vertex2Valence - 2.0f;
    return average * m_minAmountOfAverage <= lengthC;
}
bool WSubdivisionValidator::isValidMinTransformationAngle( float lengthA, float lengthB, float lengthC )
{
    if  ( m_minTransformationAngle <= 0.0f )
        return true;

    if  ( getAlphaLawOfCosines( lengthA, lengthB, lengthC) >= m_minTransformationAngle )
        return true;

    if  ( getAlphaLawOfCosines( lengthB, lengthA, lengthC) >= m_minTransformationAngle )
        return true;

    return false;
}
bool WSubdivisionValidator::isValidMaxTransformationAngle( float lengthA, float lengthB, float lengthC )
{
    if  ( m_maxTransformationAngle >= 180.0f )
        return true;

    if  ( getAlphaLawOfCosines( lengthA, lengthB, lengthC) >= m_maxTransformationAngle )
        return false;

    if  ( getAlphaLawOfCosines( lengthB, lengthA, lengthC) >= m_maxTransformationAngle )
        return false;

    return true;
}
bool WSubdivisionValidator::isValidMinLengthsQuotient( float lengthA, float lengthB )
{
    if  ( m_minLenghtsQuotient <= 0 )
        return true;

    return lengthA / lengthB >= m_minLenghtsQuotient && lengthB / lengthA >= m_minLenghtsQuotient;
}
bool WSubdivisionValidator::isValidMinSubdividedLineLength( float lengthC )
{
    if  ( m_minSubdividedLineLength <= 0 )
        return true;

    return lengthC >= m_minSubdividedLineLength;
}
bool WSubdivisionValidator::isValidMaxNeighbourTriangleAngle( size_t vertID1, size_t vertID2, float lengthC )
{
    float maxAngle = m_maxNeighbourTriangleAngle;
    float maxRatio = m_maxNeighbourTriangleAngleLengthRatio;
    if  ( m_maxNeighbourTriangleAngle >= 180.0f )
        return true;

    std::vector<size_t> sharedTriangleIDs = m_vertexFactory->getAttachedTriangleIDs( -1, vertID1, vertID2 );
    if  ( sharedTriangleIDs.size() == 0 )
        return false;

    osg::Vec3 vertex1 = m_processedMesh->getVertex( vertID1 );
    osg::Vec3 vertex2 = m_processedMesh->getVertex( vertID2 );
    for  ( size_t vertIndex = 0; vertIndex < sharedTriangleIDs.size(); vertIndex++ )
    {
        size_t triangleID = sharedTriangleIDs[ vertIndex ];
        size_t vertID3 = m_vertexFactory->get3rdVertexID( triangleID, vertID1, vertID2 );
        osg::Vec3 vertex3 = m_processedMesh->getVertex( vertID3 );
        float lengthA = WVertexFactory::getDistance( vertex1, vertex3 );
        float lengthB = WVertexFactory::getDistance( vertex2, vertex3 );
        float angleA = getAlphaLawOfCosines( lengthB, lengthA, lengthC );
        float angleB = getAlphaLawOfCosines( lengthA, lengthB, lengthC );
        if  ( ( angleA > maxAngle && lengthA > lengthC * maxRatio ) ||
                ( angleB > maxAngle && lengthB > lengthC * maxRatio ) )
            return false;
    }
    return true;
}
float WSubdivisionValidator::getMinTrianglesAngle( size_t vertID1, size_t vertID2, bool maxInTriangle )
{
    std::vector<size_t> sharedTriangleIDs = m_vertexFactory->getAttachedTriangleIDs( -1, vertID1, vertID2 );
    size_t size = sharedTriangleIDs.size();
    float minTriangleAngle = 90.0f;
    if  ( size == 0 )
        return false;

    osg::Vec3 vertex1 = m_processedMesh->getVertex( vertID1 );
    osg::Vec3 vertex2 = m_processedMesh->getVertex( vertID2 );
    for  ( size_t vertIndex = 0; vertIndex < size; vertIndex++ )
    {
        size_t triangleID = sharedTriangleIDs[ vertIndex ];
        size_t vertID3 = m_vertexFactory->get3rdVertexID( triangleID, vertID1, vertID2 );
        osg::Vec3 vertex3 = m_processedMesh->getVertex( vertID3 );
        float lengthA = WVertexFactory::getDistance( vertex1, vertex3 );
        float lengthB = WVertexFactory::getDistance( vertex2, vertex3 );
        float lengthC = WVertexFactory::getDistance( vertex1, vertex2 );
        float angle1 = getAlphaLawOfCosines( lengthA, lengthB, lengthC );
        float angle2 = getAlphaLawOfCosines( lengthB, lengthA, lengthC );
        angle1 = ( angle1 < angle2 ) != maxInTriangle ?angle1 :angle2;
        if  ( angle1 < minTriangleAngle )
            minTriangleAngle = angle1;
    }
    return minTriangleAngle;
}

/**
 * a^2=b^2+c^2-2bc cos alpha
 */
float WSubdivisionValidator::getAlphaLawOfCosines( float lengthA, float lengthB, float lengthC )
{
    float angle = pow( lengthA, 2.0f ) - pow( lengthB, 2.0f ) - pow( lengthC, 2.0f );
    angle /= -2 * lengthB * lengthC;
    if  ( angle >= 1.0f )
        return 0.0f;

    if  ( angle <= -1.0f )
        return 180.0f;

    return acos( angle ) / M_PI * 180.0f;
}
void WSubdivisionValidator::setMinTransformationAngle( float minTransformationAngle )
{
    this->m_minTransformationAngle = minTransformationAngle;
}
void WSubdivisionValidator::setMaxTransformationAngle( float maxTransformationAngle )
{
    this->m_maxTransformationAngle = maxTransformationAngle;
}
void WSubdivisionValidator::setMinLenghtsQuotient( float minLenghtsQuotient )
{
    this->m_minLenghtsQuotient = minLenghtsQuotient;
}
void WSubdivisionValidator::setMinAmountOfMax( float minAmountOfMax )
{
    this->m_minAmountOfMax = minAmountOfMax;
}
void WSubdivisionValidator::setMinAmountOfAverage( float minAmountOfAverage )
{
    this->m_minAmountOfAverage = minAmountOfAverage;
}
float WSubdivisionValidator::getMinSubdividedLineLength()
{
    return this->m_minSubdividedLineLength;
}
void WSubdivisionValidator::setMinSubdividedLineLength( float minSubdividedLineLength )
{
    this->m_minSubdividedLineLength = minSubdividedLineLength;
}
float WSubdivisionValidator::getMinSubdividedLineLengthMultiplierPerIteration()
{
    return this->m_minSubdividedLineLengthMultiplierPerIteration;
}
void WSubdivisionValidator::setMinSubdividedLineLengthMultiplierPerIteration(
        float minSubdividedLineLengthMultiplierPerIteration )
{
    this->m_minSubdividedLineLengthMultiplierPerIteration = minSubdividedLineLengthMultiplierPerIteration;
}
void WSubdivisionValidator::setMinMeighbourTriangleAngle( float minNeighbourTriangleAngle )
{
    this->m_minNeighbourTriangleAngle = minNeighbourTriangleAngle;
}
void WSubdivisionValidator::setMaxNeighbourTriangleAngle( float maxNeighbourTriangleAngle,
        float maxNeighbourTriangleAngleLengthRatio )
{
    this->m_maxNeighbourTriangleAngle = maxNeighbourTriangleAngle;
    this->m_maxNeighbourTriangleAngleLengthRatio = maxNeighbourTriangleAngleLengthRatio;
}
void WSubdivisionValidator::setMaxAmountOfMaxForVertexJoin( float maxAmountOfMaxForVertexJoin )
{
    this->m_maxAmountOfMaxForVertexJoin = maxAmountOfMaxForVertexJoin;
}
void WSubdivisionValidator::setMaxNeighbourTriangleAngleTriangleFlip( float maxNeighbourTriangleAngleTriangleFlip )
{
    this->m_maxNeighbourTriangleAngleTriangleFlip = maxNeighbourTriangleAngleTriangleFlip;
}
void WSubdivisionValidator::subdivideTriangle( size_t vertID0, size_t vertID1, size_t vertID2 )
{
    WNewVertex* new0_1prop = m_vertexFactory->getNewVertexProperty( vertID0, vertID1 );
    WNewVertex* new0_2prop = m_vertexFactory->getNewVertexProperty( vertID0, vertID2 );
    WNewVertex* new1_2prop = m_vertexFactory->getNewVertexProperty( vertID1, vertID2 );
    size_t new0_1id = new0_1prop->getNewVertexID();
    size_t new0_2id = new0_2prop->getNewVertexID();
    size_t new1_2id = new1_2prop->getNewVertexID();
    bool new0_1valid = new0_1prop->isValid();
    bool new0_2valid = new0_2prop->isValid();
    bool new1_2valid = new1_2prop->isValid();
    size_t validSum = ( new0_1valid ?1 :0 ) + ( new0_2valid ?1 :0 ) + ( new1_2valid ?1 :0 );
    size_t validPos = ( new0_1valid ?4 :0 ) + ( new0_2valid ?2 :0 ) + ( new1_2valid ?1 :0 );

    if  ( validPos == 1 || validPos == 5 )
    {
        size_t tmp = vertID0;
        vertID0 = vertID1;
        vertID1 = vertID2;
        vertID2 = tmp;
        new0_2id = new0_1id;
        new0_1id = new1_2id;
    }
    if  ( validPos == 2 || validPos == 3 )
    {
        size_t tmp = vertID0;
        vertID0 = vertID2;
        vertID2 = vertID1;
        vertID1 = tmp;
        new0_1id = new0_2id;
        new0_2id = new1_2id;
    }

    switch( validSum )
    {
        case 0:
            m_processedMesh->addTriangle( vertID0, vertID1, vertID2 );
            break;
        case 1:
            subdivideTriangleValidSum1( vertID0, vertID1, vertID2, new0_1id );
            break;
        case 2:
            subdivideTriangleValidSum2( vertID0, vertID1, vertID2, new0_1id, new0_2id  );
            break;
        default:
            subdivideTriangleValidSum3( vertID0, vertID1, vertID2, new0_1id, new0_2id, new1_2id );
    }
}
void WSubdivisionValidator::subdivideTriangleValidSum1( size_t vertID0, size_t vertID1, size_t vertID2, size_t new0_1id )
{
    m_processedMesh->addTriangle( vertID0, new0_1id, vertID2 );
    m_processedMesh->addTriangle( new0_1id, vertID1, vertID2 );
}
void WSubdivisionValidator::subdivideTriangleValidSum2(
        size_t vertID0, size_t vertID1, size_t vertID2, size_t new0_1id, size_t new0_2id )
{
    m_processedMesh->addTriangle( vertID0, new0_1id, new0_2id );
    float distance1to0_2 = WVertexFactory::getDistance( m_processedMesh->getVertex( vertID1 ), m_processedMesh->getVertex( new0_2id ) );
    float distance2to0_1 = WVertexFactory::getDistance( m_processedMesh->getVertex( vertID2 ), m_processedMesh->getVertex( new0_1id ) );
    if  ( distance1to0_2 < distance2to0_1 )
    {
        m_processedMesh->addTriangle( new0_1id, vertID1, new0_2id );
        m_processedMesh->addTriangle( vertID1, vertID2, new0_2id );
    }
    else
    {
        m_processedMesh->addTriangle( new0_1id, vertID2, new0_2id );
        m_processedMesh->addTriangle( vertID1, vertID2, new0_1id );
    }
}
void WSubdivisionValidator::subdivideTriangleValidSum3( size_t vertID0, size_t vertID1, size_t vertID2,
        size_t new0_1id, size_t new0_2id, size_t new1_2id )
{
    m_processedMesh->addTriangle( vertID0, new0_1id, new0_2id );
    m_processedMesh->addTriangle( vertID1, new1_2id, new0_1id );
    m_processedMesh->addTriangle( vertID2, new0_2id, new1_2id );
    m_processedMesh->addTriangle( new0_1id, new1_2id, new0_2id );
}
} /* namespace butterfly */
