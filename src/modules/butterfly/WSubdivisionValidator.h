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

#ifndef WSUBDIVISIONVALIDATOR_H_
#define WSUBDIVISIONVALIDATOR_H_

#include <cstring>

#include "structure/WVertexFactory.h"


namespace butterfly
{
/**
 * Class that validates the Butterfly subdivision. It contains preprocessing and validation during subdivision.
 */
class WSubdivisionValidator
{
public:
    /**
     * Creates the Butterfly subdivision validation instance.
     */
    WSubdivisionValidator();
    /**
     * Destroys the Butterfly subdivision validation instance.
     */
    virtual ~WSubdivisionValidator();

    /**
     * Assign analyzable triangle mesh.
     * \author schwarzkopf
     * \param processedMesh Triangle mesh that should be processed.
     * \param vertexFactory Vertex factory for Triangle mesh examination.
     */
    void setTriangleMesh( boost::shared_ptr< WTriangleMesh > processedMesh, WVertexFactory* vertexFactory );

    /**
     * Generating statistical information of the triangle mesh. The data is stored in the vertex factory.
     * Currently it's the maximal neighbor distance and sum of all distances.
     * \author schwarzkopf
     */
    void generateStatisticalInformation();

    /**
     * Joins Vertices where a triangle side is smaller than the longest one multiplied by a factor.
     * \author schwarzkopf
     * \param inputMesh Input mesh to process and put out
     * \return Processed triangle mesh.
     */
    boost::shared_ptr< WTriangleMesh > joinNarrowVertices(
            boost::shared_ptr< WTriangleMesh > inputMesh ); //TODO(schwarzkopf): setting param should be done over setTriangleMesh

    /**
     * Searchs for all triangles which has two angles below a preset value. Found values will be rotate
     * by one step with the triangle connected by these low angle points. The rotation is done over these
     * For vertices. No application if the final state would also have invalid angles.
     * \author schwarzkopf
     * \param inputMesh Input mesh to process and put out
     * \return Processed triangle mesh.
     */
    boost::shared_ptr< WTriangleMesh > flipTrianglesAtLowAngles(
            boost::shared_ptr< WTriangleMesh > inputMesh ); //TODO(schwarzkopf): setting param should be done over setTriangleMesh

    /**
     * Corrects Coordinates if they are determined to be invalid using validation settings.
     * Currently this function only calculates the mean between the proposed Butterfly calculated value
     * and the mean between the two vertices. The weight is used comparing the current smallest angle
     * compared to the minimal allowed. It is applied if the interpolated line cuts a triangle with an
     * angle at this side below the set up angle.
     * \author schwarzkopf
     * \param vertID1 First vertex of the interpolated line.
     * \param vertID2 Second vertex of the interpolated line.
     * \param interpolatedVertex The interpolated vertex between the two vertices.
     * \return Corrected coordinate value.
     */
    osg::Vec3 getValidatedSubdivision( size_t vertID1, size_t vertID2, osg::Vec3 interpolatedVertex );

    /**
     * Get the minimal triangle angle at the two vertices searching all triangles connected to that
     * points.
     * \author schwarzkopf
     * \param vertID1 First connected vertex to triangles.
     * \param vertID2 Second connected vertex to triangles.
     * \param maxInTriangle Taking the bigger of both angles for minimum angle search.
     * \return Minimal angle of all triangles searched at the two vertices.
     */
    float getMinTrianglesAngle( size_t vertID1, size_t vertID2, bool maxInTriangle );



    /**
     * This function checks a subdivided line between two vertices for correctness.
     * In order to check all functions with he name "isValid*" below are executed.
     * If one of them returns false then a line should not be subdivided.
     * \author schwarzkopf.
     * \param vertID1 First connecting vertex.
     * \param vertID2 Second connecting vertex.
     * \param newVert New interpolated point that should be checked.
     * \return Validity of a subdivision.
     */
    bool isValidSubdivision( size_t vertID1, size_t vertID2, osg::Vec3 newVert );

    /**
     * Subdivision is valid if the subdivided line is at least as long as the longest line to a
     * neighbor multiplied by a set up factor.
     * \author schwarzkopf.
     * \param vertID1 First vertex of the subdivided line.
     * \param vertID2 Second vertex of a subdivided line.
     * \param lengthC Line length between the two points to be checked.
     * \return Line is long enough in comparison to all lines of neighbors.
     */
    bool isValidMinAmountOfMax( size_t vertID1, size_t vertID2, float lengthC );

    /**
     * Subdivision is valid if the subdivided line is at least as long as the average line to a
     * neighbor multiplied by a set up factor.
     * \author schwarzkopf.
     * \param vertID1 First vertex of the subdivided line.
     * \param vertID2 Second vertex of a subdivided line.
     * \param lengthC Line length between the two points to be checked.
     * \return Line is long enough in comparison to the average line of neighbors.
     */
    bool isValidMinAmountOfAverage( size_t vertID1, size_t vertID2, float lengthC );

    /**
     * Checks whether both angles at the subdivided line ends are at least as big as a defined
     * minimal value. The regarded triangle consists of the two subdivided line end points
     * and the new vertex..
     * \author schwarzkopf
     * \param lengthA Line length from the new point to the first line end point.
     * \param lengthB Line length from the new point to the second line end point.
     * \param lengthC Line length between the two subdivided line end points.
     * \return Validity of the minimal allowed angle.
     */
    bool isValidMinTransformationAngle( float lengthA, float lengthB, float lengthC );

    /**
     * Checks whether both angles at the subdivided line ends are not bigger than a defined
     * maximal value. The regarded triangle consists of the two subdivided line end points
     * and the new vertex..
     * \author schwarzkopf
     * \param lengthA Line length from the new point to the first line end point.
     * \param lengthB Line length from the new point to the second line end point.
     * \param lengthC Line length between the two subdivided line end points.
     * \return Validity of the minimal allowed angle.
     */
    bool isValidMaxTransformationAngle( float lengthA, float lengthB, float lengthC );

    /**
     * Regards the lengths from the new subdivided new vertex. Returns whether the distance to
     * one subdivided line end to the distance of from the new point to the other line end
     * not more than by a set up factor..
     * \author schwarzkopf
     * \param lengthA Length from the subdivided line new vertex to the first line end.
     * \param lengthB Length from the subdivided line new vertex to the second line end.
     * \return Validity of the maximal allowed difference.
     */
    bool isValidMinLengthsQuotient( float lengthA, float lengthB );

    /**
     * Checks whether a subdividable line length is not longer than a set up length setting.
     * \param lengthC Subdividable line length to examine..
     * \author schwarzkopf
     * \return Line is at least as long as the minimum set uplength.
     */
    bool isValidMinSubdividedLineLength( float lengthC );

    /**
     * Checks whether .the angle between the subdividable line and any other line of all
     * neighbor triangles not exceeds the set up angle. Vertices aren't invalidated if the
     * connected line is shorter than the ratio ov the subdivided line length.
     * \param vertID1 First vertex of the subdividable line
     * \param vertID2 Second vertex of the subdividable line
     * \param lengthC Precalculated scalar distance between the two vertices.
     * \return Subdivision is valid or not.
     */
    bool isValidMaxNeighbourTriangleAngle( size_t vertID1, size_t vertID2, float lengthC );

    /**
     * Sets up the angle setting which both angles at the subdivided line ends should be at
     * least as big as this value. The regarded triangle consists of the two subdivided
     * line end points and the new vertex..
     * \author schwarzkopf
     * \param minTransformationAngle Min angle which both line ends should exceed.
     */
    void setMinTransformationAngle( float minTransformationAngle );

    /**
     * Sets up the angle setting which both angles at the subdivided line ends should be not
     * bigger than this value. The regarded triangle consists of the two subdivided
     * line end points and the new vertex..
     * \author schwarzkopf
     * \param maxTransformationAngle Max angle which both line ends should not exceed.
     */
    void setMaxTransformationAngle( float maxTransformationAngle );

    /**
     * Sets the maximum factor which the following lines should differ to show a valid
     * subdivision. Both lines are measured from the new subdivided point and end at the
     * ends of the subdivided line end..
     * \author schwarzkopf
     * \param minLenghtsQuotient Maximal allowed difference factor. The smaller the value
     *                           the bigger is the allowed difference.
     */
    void setMinLenghtsQuotient( float minLenghtsQuotient );

    /**
     * Sets the factor for multiplying the maximal distance to a neighbour within the Butterfly
     * stencil. Lines are not subdivided being smaller than this distance multiplied by it.
     * \author schwarzkopf.
     * \author schwarzkopf
     * \param minAmountOfMax Factor to determine whether the line is subdividable.
     */
    void setMinAmountOfMax( float minAmountOfMax );

    /**
     * Sets the factor for multiplying the average distance to a neighbour within the Butterfly
     * stencil. Lines are not subdivided being smaller than the average distance multiplied by it.
     * \author schwarzkopf.
     * \author schwarzkopf
     * \param minAmountOfAverage Factor to determine whether the line is subdividable.
     */
    void setMinAmountOfAverage( float minAmountOfAverage );

    /**
     * Gets the minimal subdividable line length. Lines of smaller distance than this are not
     * subdivided..
     * \author schwarzkopf
     * \return Minimal subdidvidable line length
     */
    float getMinSubdividedLineLength();

    /**
     * Sets the minimal subdividable line length. Lines of smaller distance than this are not
     * subdivided..
     * \author schwarzkopf
     * \param minSubdividedLineLength Minimal subdidvidable line length.
     */
    void setMinSubdividedLineLength( float minSubdividedLineLength );

    /**
     * Gets the factor where the minimal subdivided line length is multiplied by in each
     * Subdivision iteration step.
     * \return The factor which multiplies the mimimal subdividable line length each
     *         Iteration step.
     */
    float getMinSubdividedLineLengthMultiplierPerIteration();

    /**
     * Sets the factor where the minimal subdivided line length is multiplied by in each
     * Subdivision iteration step.
     * \author schwarzkopf
     * \param minSubdividedLineLengthMultiplierPerIteration The factor which multiplies the
     *                                                      mimimal subdividable line length
     *                                                      each Iteration step.
     */
    void setMinSubdividedLineLengthMultiplierPerIteration( float minSubdividedLineLengthMultiplierPerIteration );

    /**
     * Sets the Minimal allowed angle between two Subdivided line ends. If the two points have
     * a triangle where the angle is at least in one triangle cutting that vertices amaller than
     * this so the coordinates will be interpolated between the mean of the two neighbors and
     * its Butterfly subdivision used one, weighted using the minimal transformation angle.
     * \author schwarzkopf
     * \param minNeighbourTriangleAngle Minimal allowed angle between two Subdivided line ends.
     */
    void setMinMeighbourTriangleAngle( float minNeighbourTriangleAngle );

    /**
     * Sets settings of the further feature:
     * A subdivision is marked as invalid if the angle between any neighbor triangle
     * line is above that angle.
     * \author schwarzkopf
     * \param maxNeighbourTriangleAngle Maximal neighbour triangle angle not to mark a
     *                                  subdivision as invalid.
     * \param maxNeighbourTriangleAngleLengthRatio Subdivision isn't marked as invalid if the
     *                                             neighbor line at the too high angle is shorter
     *                                             in comparison to the subdividable line than
     *                                             by this ratio.
     */
    void setMaxNeighbourTriangleAngle( float maxNeighbourTriangleAngle,
            float maxNeighbourTriangleAngleLengthRatio );

    /**
     * Sets the Factor for comparison to the maximal length of a triangle. Lines smaller than this
     * force a joint of the corresponding two vertices. The triangle falls finally away.
     * \author schwarzkopf
     * \param maxAmountOfMaxForVertexJoin The factor of the smallest line end in comparison
     *                                    to the longest one within a triangle.
     */
    void setMaxAmountOfMaxForVertexJoin( float maxAmountOfMaxForVertexJoin );

    /**
     * Sets the Minimal allowed angle which fits in both vertices at a subdividable line end.
     * An existing triangle where both angles fit in forces it to rotate it with the other triangle
     * connected with that vertices araund the four vertices of that two triangles. There is no
     * application if the final state would be also invalid..
     * \author schwarzkopf
     * \param maxNeighbourTriangleAngleTriangleFlip The minimal angle at the two subdividable line
     *                                              ends thaf force the rotation.
     */
    void setMaxNeighbourTriangleAngleTriangleFlip( float maxNeighbourTriangleAngleTriangleFlip );

    /**
     * The law of cosines. It returns the angle at point A. Exchange of lengthB and lengthC has
     * no bad result effect..
     * \author schwarzkopf
     * \param lengthA The length of the triangle at the opposite to the point A
     * \param lengthB The length of the triangle at the opposite to the point B
     * \param lengthC The length of the triangle at the opposite to the point C
     * \return The angle at the side A.
     */
    static float getAlphaLawOfCosines( float lengthA, float lengthB, float lengthC );

    /**
     * Subdivides the triangle at three vertex IDs regarding the not subdividable new vertices
     * at lines between some vertices at the triangle. There's no interpolation but putting
     * triangles at existing and calculated vertices..
     * \author schwarzkopf
     * \param vertID0 First vertex ID of the triangle.
     * \param vertID1 Second vertex ID of the triangle.
     * \param vertID2 Third vertex ID of the triangle.
     */
    void subdivideTriangle( size_t vertID0, size_t vertID1, size_t vertID2 );

private:
    /**
     * Subdivides a triangle with one valid subdividable new vertex: Vertices should be rotated so that
     * the subdividable point lies between vertex ID 0 and vertex ID 1. There's no interpolation but
     * putting triangles at existing and calculated vertices..
     * \author schwarzkopf
     * \param vertID0 First vertex ID of the triangle.
     * \param vertID1 Second vertex ID of the triangle.
     * \param vertID2 Third vertex ID of the triangle.
     * \param new0_1id Subdivided Vertex ID between vertID0 and vertID1
     */
    void subdivideTriangleValidSum1( size_t vertID0, size_t vertID1, size_t vertID2, size_t new0_1id );

    /**
     * Subdivides a triangle with two valid subdividable new vertices: Vertices should be rotated so that
     * the not subdividable point lies between vertex ID 1 and vertex ID 2. There's no interpolation
     * but putting triangles at existing and calculated vertices..
     * \author schwarzkopf
     * \param vertID0 First vertex ID of the triangle.
     * \param vertID1 Second vertex ID of the triangle.
     * \param vertID2 Third vertex ID of the triangle.
     * \param new0_1id Subdivided Vertex ID between vertID0 and vertID1
     * \param new0_2id Subdivided Vertex ID between vertID0 and vertID2
     */
    void subdivideTriangleValidSum2( size_t vertID0, size_t vertID1, size_t vertID2, size_t new0_1id, size_t new0_2id );

    /**
     * Subdivides a triangle with all valid subdividable new vertices. There's no interpolation but
     * putting triangles at existing and calculated vertices..
     * \author schwarzkopf
     * \param vertID0 First vertex ID of the triangle.
     * \param vertID1 Second vertex ID of the triangle.
     * \param vertID2 Third vertex ID of the triangle.
     * \param new0_1id Subdivided Vertex ID between vertID0 and vertID1
     * \param new0_2id Subdivided Vertex ID between vertID0 and vertID2
     * \param new1_2id Subdivided Vertex ID between vertID1 and vertID2
     */
    void subdivideTriangleValidSum3( size_t vertID0, size_t vertID1, size_t vertID2, size_t new0_1id, size_t new0_2id, size_t new1_2id );

    /**
     * Associated triangle mesh for validation instances.
     */
    boost::shared_ptr< WTriangleMesh > m_processedMesh;

    /**
     * Associated analysis data of the the triangle mesh to be validated.
     */
    WVertexFactory* m_vertexFactory;

    /**
     * Angle setting which both angles at the subdivided line ends should be at
     * least as big as this value. The regarded triangle consists of the two subdivided
     * line end points and the new vertex.
     */
    float m_minTransformationAngle;

    /**
     * Angle setting which both angles at the subdivided line ends should be not
     * bigger than this value. The regarded triangle consists of the two subdivided
     * line end points and the new vertex.
     */
    float m_maxTransformationAngle;

    /**
     * Maximum factor which the following lines should differ to show a valid
     * subdivision. Both lines are measured from the new subdivided point and end at the
     * ends of the subdivided line end.
     */
    float m_minLenghtsQuotient;

    /**
     * Factor for multiplying the maximal distance to a neighbour within the Butterfly
     * stencil. Lines are not subdivided being smaller than this distance multiplied by it.
     */
    float m_minAmountOfMax;

    /**
     * Factor for multiplying the average distance to a neighbour within the Butterfly
     * stencil. Lines are not subdivided being smaller than the average distance multiplied by it.
     */
    float m_minAmountOfAverage;

    /**
     * The minimal subdividable line length. Lines of smaller distance than this are not
     * subdivided.
     */
    float m_minSubdividedLineLength;

    /**
     * The factor where the minimal subdivided line length is multiplied by in each
     * Subdivision iteration step.
     */
    float m_minSubdividedLineLengthMultiplierPerIteration;

    /**
     * The Minimal allowed angle between two Subdivided line ends. If the two points have
     * a triangle where the angle is at least in one triangle cutting that vertices amaller than
     * this so the coordinates will be interpolated between the mean of the two neighbors and
     * its Butterfly subdivision used one, weighted using the minimal transformation angle.
     */
    float m_minNeighbourTriangleAngle;

    /**
     * A subdivision is marked as invalid if the angle between any neighbor triangle line
     * is above that angle.
     */
    float m_maxNeighbourTriangleAngle;

    /**
     * This settings extends m_maxNeighbourTriangleAngle;
     * If the line length is smaller than the subdividable line by this ratio, then it won't
     * be marked as invalid.
     */
    float m_maxNeighbourTriangleAngleLengthRatio;

    /**
     * The Factor for comparison to the maximal length of a triangle. Lines smaller than this
     * force a joint of the corresponding two vertices. The triangle falls finally away.
     */
    float m_maxAmountOfMaxForVertexJoin;

    /**
     * The Minimal allowed angle which fits in both vertices at a subdividable line end.
     * An existing triangle where both angles fit in forces it to rotate it with the other triangle
     * connected with that vertices araund the four vertices of that two triangles. There is no
     * application if the final state would be also invalid.
     */
    float m_maxNeighbourTriangleAngleTriangleFlip;
};

} //namespace butterfly
#endif  // WSUBDIVISIONVALIDATOR_H
