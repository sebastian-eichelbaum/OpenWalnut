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

#ifndef WBUTTERFLYFACTORY_H_
#define WBUTTERFLYFACTORY_H_

#include <string>
#include <vector>

#include "core/kernel/WModule.h"
#include "structure/WVertexFactory.h"
#include "WSubdivisionValidator.h"
#include "WButterflyCalculator.h"


using osg::Vec3;

namespace butterfly
{
    /**
     * Class that depicts the whole Butterfly subdivision algorithm but nothing more as such.
     */
    class WButterflyFactory
    {
    public:
        /**
         * Butterfly subdivision tool object creating instance.
         */
        WButterflyFactory();

        /**
         * Destroys the Butterfly instance object and its substructures.
         */
        virtual ~WButterflyFactory();

        /**
         * Set the general Butterfly Subdivision setting w that affects the subdivision. See the algorithm
         * documentation for the exact meaning. It's usually chosen substantially small. The original
         * authors used 0.0f.
         * \author schwarzkopf
         * \param butterflySettingW The general butterfly subdivision parameter w.
         */
        void setButterflySettingW( float butterflySettingW );

        /**
         * Assign main butterfly subdivision iterations settings.
         * \author schwarzkopf
         * \param m_iterations Count how many times the butterfly subdivision should be iteratedly
         * applied.
         * \param m_maxTriangles10n Maximal triangle count. If there are more triangles then the
         * subdivision will be stop.
         */
        void setIterationsSettings( float m_iterations, float m_maxTriangles10n );

        /**
         * Sets the count of CPU threads to use.
         * \author schwarzkopf
         * \param cpuThreadCount CPU thread count to use.
         */
        void setCpuThreadCount( size_t cpuThreadCount );

        /**
         * Assigns a ProbressCombiner to the Butterfly factory. After that the algorithm will be
         * able to actualize the progress within the module overview.
         * \author schwarzkopf
         * \param associatedProgressCombiner Progress combiner to be associated.
         */
        void assignProgressCombiner( boost::shared_ptr< WProgressCombiner > associatedProgressCombiner );

        /**
         * Sets progress which iteration step and kind of cumputing step is currently done.
         * The scale of the computing step is also set.
         * \author schwarzkopf
         * \param iteration Butterfly iteration step.
         * \param steps Scale of the progress bar.
         */
        void setProgressSettings( size_t iteration, size_t steps );

        /**
         * Returns the Butterfly subdivision validator. Therefore settings can be changed externally.
         * \author schwarzkopf
         * \return Subdivision validator pointer.
         */
        WSubdivisionValidator* getValidator();

        /**
         * Launch the Butterfly subdivision. Afterwards the interpolated Triangle mesh is returned.
         * \param edgedMmesh Triangle mesh to be interpolated.
         * \return Interpolated triangle mesh.
         */
        boost::shared_ptr< WTriangleMesh > getSubdividedMesh( boost::shared_ptr< WTriangleMesh > edgedMmesh );


    private:
        /**
         * Examines Butterfly stencils of all vertices and stores data
         * \author schwarzkopf
         */
        void examineStencilAll();

        /**
         * Attachs new vertices that are used for the subdivision. The data sets are only assigned.
         * The calculation itself will be applied afterwards.
         * \author schwarzkopf
         */
        void attachUncalculatedNewVertices();

        /**
         * Applies triangle mesh preprocessing if vertex flip or triangle flip setting is not
         * default.
         * \author schwarzkopf
         * \param iteration Preprocessing is applied only before the first iteration
         */
        void applyMeshPreprocessing( size_t iteration );

        /**
         * Examine The triangle mesh before Butterfly subdivision. Following steps are included:
         *   - Register all triangles to corresponding vertices
         *   - Optionally preprocess Triangle mesh.
         *   - Examine  Butterfly stencils (Vertex neighbours only) and valence of all vertices
         *   - Collect statistical information required for validation of subdividable new vertices.
         *     Currently it includes maximal and averate neighbour distance from each vertex.
         * \author schwarzkopf
         * \param iteration Consecutive Iteration step of the Butterfly subdivision.
         */
        void examineVertexNeighborhood( size_t iteration );

        /**
         * Calculates coordinates of the new vertices. It uses multithreading.
         * \author schwarzkopf
         */
        void interpolateNewVertices();

        /**
         * Calculates coordinates of the new vertices. The parameters are to select the vertices
         * to calculated. It's useful for multithreading.
         * \author schwarzkopf
         * \param fromVertex First data set holding vertex property.
         * \param toVertex Last data set holding vertex property.
         */
        void interpolateNewVerticesRange( size_t fromVertex, size_t toVertex );

        /**
         * Adds interpolated vertices and triangles to the output triangle mesh.
         * \author schwarzkopf
         */
        void addInterpolatedContent();


        /**
         * Iteration steps to apply iteratedly.
         */
        size_t m_iterations;

        /**
         * Maximal allowed triangle count applyable for subdivision.
         */
        size_t m_maxTriangles;

        /**
         * Base triangle mesh used for calculations.
         */
        boost::shared_ptr< WTriangleMesh > m_inputMesh;

        /**
         * Triangle mesh that is set up by interpolation.
         */
        boost::shared_ptr< WTriangleMesh > m_outputMesh;

        /**
         * Data set used for analyzation of the triangle mesh.
         */
        WVertexFactory* m_verts;

        /**
         * Triangle mesh validation instance.
         */
        WSubdivisionValidator* m_validator;

        /**
         * Total triangle count.
         */
        size_t m_triCount; //TODO(schwarzkopf): Consider removing this variable

        /**
         * Current count of currently added vertices during interpolation.
         */
        size_t m_vertCount;

        /**
         * Progress combiner for changing the plugin status in the modules overview.
         */
        boost::shared_ptr< WProgressCombiner > m_associatedProgressCombiner;

        /**
         * Current progress status.
         */
        boost::shared_ptr< WProgress > m_progressStatus;

        /**
         * CPU threads count for multithreading support.
         */
        size_t m_cpuThreadCount;

        /**
         * CPU threads object for multithreading support.
         */
        vector<boost::thread*> m_cpuThreads;

        /**
         * Instance for interpolating new vertices.
         */
        WButterflyCalculator* m_butterflyCalculator;
    };
} /* namespace butterfly */
#endif  // WBUTTERFLYFACTORY_H
