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

#ifndef WVERTEXFACTORY_H_
#define WVERTEXFACTORY_H_

#include <vector>
#include "WVertexProperty.h"
#include "core/kernel/WModule.h"
#include "WNewVertex.h"
#include "core/graphicsEngine/WTriangleMesh.h"
#include "osg/Vec3"


namespace butterfly
{
    /**
     * Class that manages all vertex properties. It depicts the outer object that manages the entire set of the
     * triangle mesh that is not possible to analyze using a triangle mesh only.
     */
    class WVertexFactory
    {
    public:
        /**
         * Creates a vertex factory object and assigns the triangle mesh that should be analyzed in future.#
         * \param triangleMesh Associated triangle mesh that should be analyzed.
         */
        explicit WVertexFactory( boost::shared_ptr< WTriangleMesh > triangleMesh );
        /**
         * Creates a vertex factory object.
         */
        WVertexFactory();
        /**
         * Destroys the vertex factory object.
         */
        virtual ~WVertexFactory();

        /**
         * Sets the WTriangleMesh. It's necessary for the correct triangle mesh examination.
         * \author schwarzkopf
         * \param triangleMesh Triangle mesh to be registered.
         */
        void setTriangleMesh( boost::shared_ptr< WTriangleMesh > triangleMesh );

        /**
         * Resizes the Vertex property list to the wished size.
         * \author schwarzkopf
         * \param count Required list size.
         */
        void resize( size_t count );

        /**
         * Registers triangle to all its vertex IDs.
         * \author schwarzkopf
         * \param vertexID The triangle will be registered in the vertex of this ID.
         * \param triangleID Triangle ID to be registered.
         */
        void registerTriangle( size_t vertexID, size_t triangleID );

        /**
         * Returns all Triangle IDs but the excluded attached to two particular vertices.
         * \author schwarzkopf
         * \param excludedTriangleID Triangle ID that should not be added to the returned list.
         * \param vertex1 First vertex the triangle should lie on.
         * \param vertex2 Second vertex the triangle should lie on.
         * \return All triangle IDs which lie on the two vertices but the triangleID.
         */
        vector<size_t> getAttachedTriangleIDs( size_t excludedTriangleID, size_t vertex1, size_t vertex2 );

        /**
         * Returns the third vertex ID of a triangle.
         * \author schwarzkopf
         * \param triangleID Triangle ID to examine.
         * \param vertex1 First vertexID to exclude.
         * \param vertex2 Second vertex ID to exclude.
         * \return Not excluded vertex ID.
         */
        size_t get3rdVertexID( size_t triangleID, size_t vertex1, size_t vertex2 );

        /**
         * Returns the Property List's vertex count
         * \return Vertex property count of the list.
         */
        size_t getVertexCount();

        /**
         * Returns whether properties of the subdividable new vertex between two vertices
         * alreadey exists.
         * \author schwarzkopf
         * \param fromID First connected vertex to the subdividable line.
         * \param toID Second connected vertex to the subdividable line.
         * \return Properties of a subdividable new vertex have been registered or not.
         */
        bool newVertexExists( size_t fromID, size_t toID );

        /**
         * Examine triangle by its ID for validity. Currently it basically checks whether all
         * vertex Ds are used once.
         * \param triangleID ID of a triangle to examine.
         * \return Triangle is valid or not.
         */
        bool isValidTriangle( size_t triangleID );

        /**
         * Registering a subdividable new vertex of an ID between two vertices. Invalid
         * subdividable new vertices are also registered. But there are market to be invalid
         * afterwards.
         * \author schwarzkopf
         * \param fromID The first neighbor vertex ID where the new vertex lies between.
         * \param toID The second neighbor vertex ID where the new vertex lies between.
         * \return The proposed ID of the new vertex.
         */
        void attachNewVertex( size_t fromID, size_t toID );

        /**
         * Get properties of a vertex
         * \author schwarzkopf
         * \param vertexID Vertex ID to get properties from.
         * \return Properties' data set pointer.
         */
        WVertexProperty* getProperty( size_t vertexID );

        /**
         * Get properties of a new vertex which subdivides a line between two vertices.
         * \author schwarzkopf
         * \param fromID The first neighbor vertex ID where the new vertex lies between.
         * \param toID The second neighbor vertex ID where the new vertex lies between.
         * \return Pointer to the properties of the new vertex.
         */
        WNewVertex* getNewVertexProperty( size_t fromID, size_t toID );

        /**
         * Examines Butterfly stencils of all vertices and applies settings data to the vertex.
         * properties. Due to this basically all neighbor vertices are sorted and stored
         * into the butterfly stencil neighbor vertex ID array. The sort order is circular
         * by each vertex. The neighbor vertex count is stored as valence.
         *
         * The first two parameters are to select the vertices to calculated. It's useful
         * for multithreading.
         * \author schwarzkopf
         * \param fromVertex First data set holding vertex property.
         * \param toVertex Last data set holding vertex property.
         * \param progress Progress which is actualized during the calculation.
         */
        void examineStencilRange( size_t fromVertex, size_t toVertex,
                boost::shared_ptr< WProgress > progress );

        /**
         * Calculate distance between two vertices.
         * \author schwarzkopf
         * \param point1 Vertex 1.
         * \param point2 Vertex 2.
         * \return Distance between two vertices
         */
        static float getDistance( osg::Vec3 point1, osg::Vec3 point2 );

        /**
         * Adding a sum multiplied by a factor to the base number
         * \author schwarzkopf
         * \param base The base number to add the summand
         * \param sum Summand that is added to the base number
         * \param factor Number that multiplies the summand
         * \return Base number + summand * factor
         */
        static osg::Vec3 add( osg::Vec3 base, osg::Vec3 sum, float factor );


    private:
        /**
         * Vertex properties data. The valid size is not the actual vector size but vertexCount.
         * It's been taken in order not to resize the whole array after each adding what would
         * be a big performance drawback.
         */
        std::vector<WVertexProperty*> m_vertProp;

        /**
         * Assigned Triangle mesh. It's necessary for the proper examination.
         */
        boost::shared_ptr< WTriangleMesh > m_triangleMesh;

        /**
         * Vertex count of the property list.
         */
        size_t m_vertexCount;

        /**
         * Triangle count.
         */
        size_t m_triangleCount; //TODO(schwarzkopf): Consider removing this variable or describe it thoroughly.
    };
} /* namespace butterfly */
#endif  // WVERTEXFACTORY_H
