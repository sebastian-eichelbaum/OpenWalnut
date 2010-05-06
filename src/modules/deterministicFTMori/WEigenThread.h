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

#ifndef WEIGENTHREAD_H
#define WEIGENTHREAD_H

#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include "../../common/WThreadedRunner.h"
#include "../../common/WProgress.h"
#include "../../dataHandler/WValueSet.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../../common/math/WLinearAlgebraFunctions.h"
#include "../../common/datastructures/WFiber.h"
#include "../../common/math/WVector3D.h"
#include "../../common/math/WMatrix.h"

/**
 * A class that computes the largest eigenvectors on a subset of a tensor array
 * in its own thread.
 */
class WEigenThread : public WThreadedRunner
{
public:

    /**
     * Constructor.
     *
     * \param grid The grid of the input dataset.
     * \param values The input tensors.
     * \param progr The progress object.
     * \param index An index indicating what part of the grid this thread should work on.
     * \param eigenVectors A vector of (output) eigenvectors.
     * \param fa A vector of (output) FA values.
     */
    WEigenThread( boost::shared_ptr< WGridRegular3D > grid,
                  boost::shared_ptr< WValueSet< float > > values,
                  boost::shared_ptr< WProgress > progr,
                  uint32_t index,
                  boost::shared_ptr< std::vector< wmath::WVector3D > > eigenVectors,
                  boost::shared_ptr< std::vector< double > > fa );

/**
 * Destructor.
 */
    virtual ~WEigenThread();

protected:
private:

    /**
     * The actual thread routine.
     */
    virtual void threadMain();

    /**
     * Calculate the largest eigenvector and the FA at grid position i.
     * \param i A position in the input datasets' grid.
     */
    void eigen( size_t i );

    /**
     * Calculate the fractional anisotropy at the grid position from the eigenvalues t0, t1 and t2.
     * \param t0 One of the input tensor's eigenvalues.
     * \param t1 Another one of the input tensor's eigenvalues.
     * \param t2 The last eigenvalue of the input tensor.
     * \param i The position on the input grid.
     */
    void calcFA( double const t0, double const t1, double const t2, size_t const i );

    /**
     * The input grid.
     */
    boost::shared_ptr< WGridRegular3D > m_grid;

    /**
     * The input tensors.
     */
    boost::shared_ptr< WValueSet< float > > m_values;

    /**
     * The progress of the eigenvector calculation. All threads
     * share the same progress object.
     */
    boost::shared_ptr< WProgress > m_progress;

    /**
     * An index indicating in what part of the grid this thread should look for seed points.
     */
    uint32_t m_index;

    /**
     * The vector storing the computed eigenvectors.
     */
    boost::shared_ptr< std::vector< wmath::WVector3D > > m_eigenVectors;

    /**
     * The vector storing the fractional anisotropy.
     */
    boost::shared_ptr< std::vector< double > > m_FA;
};

#endif  // WEIGENTHREAD_H

