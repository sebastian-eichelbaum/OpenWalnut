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
 * PointProperty.h
 *
 *  Created on: 12.06.2013
 *      Author: renegade
 */

#ifndef WVERTEXPROPERTY_H_
#define WVERTEXPROPERTY_H_

#include <vector>
#include "WNewVertex.h"
#include "osg/Vec3"



using namespace butterfly; //NOLINT
using std::vector;

/**
 * Object that contains all necessary properties of a vertex necessary for a sufficient analysis.
 */
class WVertexProperty
{
public:
    /**
     * Initializes a vertex property object and sets settings to defaults.
     */
    WVertexProperty();

    /**
     * Destroys the vertex property object.
     */
    virtual ~WVertexProperty();

    /**
     * Returns the attached triangle count to the current vertex.
     * \return Attached triangle count to the current vertex.
     */
    size_t getTriangleCount();

    /**
     * Registers a triangle to the current vertex.
     * \author schwarzkopf
     * \param triangleID Triangle ID to be registered.
     */
    void attachTriangle( size_t triangleID );

    /**
     * Unregisters a triangle to the current vertex.
     * \author schwarzkopf
     * \param triangleID Triangle ID to be registered.
     */
    void detachTriangle( size_t triangleID );

    /**
     * Get all triangle IDs attached to the current vertex. They are not organized by the
     * correct circular order.
     * \author schwarzkopf
     * \return All triangle IDs attached to the current vertex.
     */
    vector<size_t> getAttachedTriangles();

    /**
     * Returns a neighbor ID within the stencil by the index.
     * Items must be added in the correct circular order in order to figure out the Butterfly
     * stencil correctly.
     * \author schwarzkopf
     * \param index Position in the list of the stencil neighbor id.
     * \return Neighbour count of a vertex that is used for the Butterfly stencil.
     */
    size_t getStencilNeighbourID( size_t index );

    /**
     * Pushes a stencil neighbor vertex ID before the first Item of the list.
     * Items must be added in the correct circular order in order to figure out the Butterfly
     * stencil correctly.
     * Use rather addStencilNeighbourIDToBack than this function due to the performance issue.
     * \author schwarzkopf
     * \param stencilNeighbourID Neighbour vertex ID to be regarded in the Butterfly stencil.
     */
    void insertStencilNeighbourIDToFront( size_t stencilNeighbourID );

    /**
     * Pushs a stencil neighbour vertex ID after the last Item of the list.
     * Items must be added in the correct circular order in order to figure out the Butterfly
     * stencil correctly.
     * \author schwarzkopf
     * \param stencilNeighbourID Neighbour vertex ID to be regarded in the Butterfly stencil.
     */
    void addStencilNeighbourIDToBack( size_t stencilNeighbourID );

    /**
     * Returns the Neighbor vertex count of the current vertex.
     * \author schwarzkopf
     * It's called "valence" in the Butterfly document sheet.
     * \return Valence (Neighbor vertex count of the current vertex).
     */
    size_t getValence();

    /**
     * Returns the bound count class of the stencil's center point
     * \author schwarzkopf
     * \return Bound class of the stencil (see description).
     */
    int getBoundClass();

    /**
     * Sets the bound count class of the stencil's center point
     * \author schwarzkopf
     * \param boundCountClass Bound class of the stencil (see description)
     */
    void setBoundClass( int boundCountClass );

    /**
     * Returns true if a triangle of the triangleID is connected to the vertex
     * \author schwarzkopf
     * \param triangleID Triangle ID which is checked for existence
     * \return Triangle ID exists or not
     */
    bool containsTriangle( size_t triangleID );

    /**
     * Attachs properties of a new vertex on a line which lies between two original
     * vertices.
     * \author schwarzkopf
     * \param toID The point lies between the current ID and toID.
     *             toID must always be greater than the current vertex ID.
     */
    void attachNewVertex( size_t toID );

    /**
     * Checks whether the properties of a new vertex exists between the current
     * vertex and toID.
     * \author schwarzkopf
     * \param toID The point lies between the current ID and toID.
     *             toID must always be greater than the current vertex ID.
     * \return Properties of new vertex exist or not.
     */
    bool newVertexExists( size_t toID );

    /**
     * Returns properties of the new vertex between the current vertex ID and toID
     * \author schwarzkopf
     * \param toID The point lies between the current ID and toID.
     *             toID must always be greater than the current vertex ID.
     * \return Properties of the new vertex.
     */
    WNewVertex* getNewVertexProperty( size_t toID );

    /**
     * Returns the maximal neighbor distance within the current vertex.
     * \author schwarzkopf
     * \return The Maximal neighbor distance within the current vertex.
     */
    float getMaxNeighbourDistance();

    /**
     * Returns summed neighbor distance within the current vertex.
     * \author schwarzkopf
     * \return The average neighbor distance within the current vertex.
     * */
    float getSumNeighbourDistance();

    /**
     * Sets maximal neighbor distance within the current vertex.
     * \author schwarzkopf
     * \param maxNeighbourDistance The average neighbor distance within the current vertex.
     */
    void setMaxNeighbourDistance( float maxNeighbourDistance );

    /**
     * Sets the summed neighbor distance within the current vertex.
     * \author schwarzkopf
     * \param sumNeighbourDistance The average neighbor distance within the current vertex.
     */
    void setSumNeighbourDistance( float sumNeighbourDistance );

    /**
     * Returns the Stencil neighbor index of a particular vertex ID
     * \author schwarzkopf
     * \param neighbourID Vertex ID to examine.
     * \return Array index of the Neighbour vertex ID.
     */
    int getStencilNeighbourIndex( size_t neighbourID );

    /**
     * Returns the properties of new registered vertices of the current vertex. New vertices
     * of that which are connected to a vertex with a lower ID than this vertex are not stored
     * because each data set should be stored one timed due to the data holding issue. The
     * method has the advantage that every new vertex can be traversed only once.
     * \author schwarzkopf
     * \return The properties of new registered vertices but only those which are
     *         connected to a higher neighbor vertex ID than the actual.
     */
    std::vector<WNewVertex*> getNewVerticesToHigherNeighborID();

private:
    /**
     * List of attached triangles to the current vertex.
     */
    std::vector<size_t> m_attachedTriangle;

    /**
     * List of attached new vertices of the current vertex. Due to Organization problems only that
     * vertices are in this list which are of a higher connected vertex ID than the current.
     * To get ones of the lower ID you should examine all neighbours of the lower ID and fetch all
     * new points where toID equals to the current neighbor vertex ID.
     */
    std::vector<WNewVertex*> m_newVertices;

    /**
     * List that keeps neighbour vertices IDs sorted by the circular order. The whole Butterfly
     * stencil can be compound using the neighbour array of the other stencil center vertex.
     */
    vector<size_t> m_stencilOnNeighbours;

    /**
     * Depicts the Vertex' kind of bound iit lies on
     *  <0: Extraordinary case. More triangles ran through than actually exist
     *   0: Vertex lies not on the bound.
     *   1: Vertex lies on a bound.
     * >=2: Vertex lies on at least two bounds that are separated by triangles.
     *      or it contains a vertex connection which hits at least three triangles.
     */
    int m_boundClass;

    /**
     * Maximal Neighbor vertex distance to the current vertex.
     */
    float m_maxNeighbourDistance;

    /**
     * Sum of all Neighbor vertex distances to the current vertex.
     */
    float m_sumNeighbourDistance;
};

#endif  // WVERTEXPROPERTY_H
