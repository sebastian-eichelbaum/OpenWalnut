//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2013 OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS
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

#ifndef WNEWVERTEX_H
#define WNEWVERTEX_H

#include <cstring>

#include <osg/Vec3>


namespace butterfly
{
/**
 * Depicts a point that is subdivided on a line between two vertices. It's stored in a vertex property. The second
 * relationship is depicted by the toID field.
 */
class WNewVertex
{
public:
    /**
     * Initializes properties of a new vertex with all its necessary parameters.
     * \param toID The other point which the new vertex is interpolated between. The first point stores the new
     *             vertex object. toID is always bigger.
     */
    explicit WNewVertex( size_t toID );

    /**
     * Destroys the new vertex property.
     */
    virtual ~WNewVertex();

    /**
     * Returns the proposed ID of that new vertex. The ID should be equal to the one of the
     * final Triangle Mesh.
     * \author schwarzkopf
     * \return ID of the new vertex.
     */
    size_t getNewVertexID();

    /**
     * Sets the proposed ID for that new vertex. The ID should be equal to the one of the
     * final Triangle Mesh.
     * \author schwarzkopf
     * \param newVertexID Vertex ID to set.
     */
    void setNewVertexID( size_t newVertexID );

    /**
     * Returns the second vertex ID where the new vertex lies between these two points.
     * It's still not necessary to store the first connected one. The second ID is always
     * bigger than the first one.
     * \author schwarzkopf
     * \return The second connected Vertex ID.
     */
    size_t getToID();

    /**
     * Sets the new vertex property whether it's valid for butterfly subdivision or not.
     * Invalid lines are not subdivided.
     * \author schwarzkopf
     * \param isValid Whether the new vertex is marked to be valid for butterfly subdivision or not.
     */
    void setValid( bool isValid );

    /**
     * Returns whether the new vertex is marked to be valid for the butterfly subdivison of a
     * line between two vertices. Invalid lines are not subdivided.
     * \author schwarzkopf
     * \return Whether the new vertex is marked to be valid for butterfly subdivision or not.
     */
    bool isValid();

    /**
     * Returns coordinates of the new vertex.
     * \author schwarzkopf
     * \return Coordinate of the new vertex ID.
     */
    osg::Vec3 getCoordinate();

    /**
     * Sets coordinates of the new vertex.
     * \author schwarzkopf
     * \param coordinate coordinate of the new vertex ID.
     */
    void setCoordinate( osg::Vec3 coordinate );

private:
    /**
     * The id of the second vertex the new vertex is connected to. The first point is defined by the
     * number of the stored vertex property where this object lies in. The toID is always bigger than
     * the first ID due to data organization issue.
     */
    size_t m_toID;

    /**
     * ID the new vertex is proposed to have in the final triangle mesh.
     */
    size_t m_newVertexID;

    /**
     * Coordinate of the new vertex.
     */
    osg::Vec3 m_coordinate;

    /**
     * Property whether the new vertex is valid for subdivision. Not validly marked lines are not
     * subdivided. Therefore an alternative triangulation is taken.
     */
    bool m_isValid;
};

} //namespace butterfly
#endif  // WNEWVERTEX_H
