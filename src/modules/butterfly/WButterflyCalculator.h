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

#ifndef WBUTTERFLYCALCULATOR_H_
#define WBUTTERFLYCALCULATOR_H_

#include <string>
#include <vector>

#include "core/kernel/WModule.h"
#include "structure/WVertexFactory.h"
#include "WSubdivisionValidator.h"


using osg::Vec3;

namespace butterfly
{
    /**
     * Class that depicts the whole Butterfly subdivision algorithm but nothing more as such.
     */
    class WButterflyCalculator
    {
    public:
        /**
         * Butterfly subdivision tool object creating instance.
         */
        WButterflyCalculator();

        /**
         * Destroys the Butterfly instance object and its substructures.
         */
        virtual ~WButterflyCalculator();

        /**
         * Assigns the input mesh andd its analyzed data that are required for the butterfly subdivision.
         * \author schwarzkopf
         * \param inputMesh Triangle mesh that should be subdivided
         * \param vertexProperties The analyzed data of the input triangle mesh
         */
        void assignInputMesh( boost::shared_ptr< WTriangleMesh > inputMesh,
                WVertexFactory* vertexProperties );

        /**
         * Set the general Butterfly Subdivision setting w that affects the subdivision. See the algorithm
         * documentation for the exact meaning. It's usually chosen substantially small. The original
         * authors used 0.0f.
         * \author schwarzkopf
         * \param butterflySettingW The general butterfly subdivision parameter w.
         */
        void setButterflySettingW( float butterflySettingW );

        /**
         * Calculate the subdivided new vertex between two vertices using the Butterfly Subdivision.
         * algorithm.
         * \author schwarzkopf
         * \param vertID1 First vertex to subdivide between.
         * \param vertID2 Second vertex ID to subdivide between
         * \return Calculated coordinates.
         */
        osg::Vec3 calcNewVertex( size_t vertID1, size_t vertID2 );


    private:
        /**
         * The general Butterfly Subdivision setting w that affects the subdivision. See the algorithm
         * documentation for the exact meaning. It's usually chosen substantially small. The original
         * authors used 0.0f.
         */
        static float m_w;

        /**
         * Butterfly subdivision Weights for each neighbor which always are applied for valences 3 and 4.
         * Valence 6 is applied for Butterfly stencils where both valences are 6.
         * The first row starts with valence 6. The first column starts with the center vertex weight.
         * Then comes the Neighbor vertex connected to the other stencil center vertex.
         */
        static float m_weights[4][7];

        /**
         * Weight where both stencil centers lie on a border. This weight is for a stencil center.
         */
        static float m_weightCenterAtBorder;

        /**
         * Weight where both stencil centers lie on a border. This weight is beside the stencil
         * centers.
         */
        static float m_weightRimAtBorder;

        /**
         * Calculate a subdivided mit point between two vertices using the mean calculation.
         * \author schwarzkopf
         * \param vertID1 First vertex to subdivide between.
         * \param vertID2 Second vertex ID to subdivide between.
         * \return Mean between the two vertices.
         */
        osg::Vec3 calcMean( size_t vertID1, size_t vertID2 );

        /**
         * Calculate the coordinates of a stencil half using the Butterfly subdivision algorithm.
         * \param stencilCenterVertID Vertex ID of the stencil center. Coordinates are calculated
         *                            for that value.
         * \param directedNeighbourVertID Vertex ID of the other half of the whole butterfly stencil.
         * \param isIrregular Calculate stencil half as an irregular. False is set calculating a
         *                    Stencil where both valences are 6.
         * \return Interpolated coordinates.
         */
        Vec3 getInterpolatedValue( size_t stencilCenterVertID, size_t directedNeighbourVertID, bool isIrregular );

        /**
         * Associated data set used for analyzation of the triangle mesh.
         */
        WVertexFactory* m_verts;

        /**
         * Base triangle mesh used for calculations.
         */
        boost::shared_ptr< WTriangleMesh > m_inputMesh;
    };
} /* namespace butterfly */
#endif  // WBUTTERFLYCALCULATOR_H
