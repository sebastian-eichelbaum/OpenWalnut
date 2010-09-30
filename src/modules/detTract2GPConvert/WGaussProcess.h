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

#ifndef WGAUSSPROCESS_H
#define WGAUSSPROCESS_H

#include <Eigen/Core>

#include "../../common/datastructures/WFiber.h"
#include "../../common/math/WMatrix.h"
#include "../../common/math/WValue.h"
#include "../../dataHandler/WDataSetDTI.h"

/**
 * Represents a basic gaussian process with its mean- and covariance function. Basically this aims
 * to implement a part of the gaussian process framework as presented by Wasserman et. al:
 * http://dx.doi.org/10.1016/j.neuroimage.2010.01.004
 */
class WGaussProcess
{
public:
    /**
     * Default constructor.
     */
    WGaussProcess();

    /**
     * Constructs a gaussian process out of a fiber with the help of underlying diffusion tensor
     * information.
     *
     * \param tract One deterministic tractogram
     * \param tensors All 2nd order diffusion tensors
     */
    WGaussProcess( const wmath::WFiber& tract, boost::shared_ptr< const WDataSetDTI > tensors );

    /**
     * Default destructor.
     */
    virtual ~WGaussProcess();

protected:
private:
    /**
     * Covariance function of two points representing the smoothness of the tract.
     *
     * \param p1 First point
     * \param p2 Second point
     * \param R The threshold when the gaussian will stop and is equal to zero
     *
     * \return Real number indicating the covariance between two points representing the smoothness
     * of the tract
     */
    double cov_s( const wmath::WPosition& p1, const wmath::WPosition& p2, const double R = 1.0 ) const;

    /**
     * Covariance function of two points representing the blurring of the tract's diffusion.
     *
     * \param p1 First point
     * \param p2 Second point
     * \param t The tau parameter as described in equation (13) in the Wasserman parameter.
     * TODO(math): A more elaborate description would be nice.
     *
     * \return Real number indicating the covariance between two points representing the diffusion
     * associated blurring.
     */
    double cov_d( const wmath::WPosition& p1, const wmath::WPosition& p2, const double t = 1.0 ) const;

    /**
     * Covariance function of this gaussian process.
     *
     * \note The reason why this isn't realized as member is just simplicity. Maybe we have time to
     * change this!
     *
     * \param p1 First point
     * \param p2 Second point
     *
     * \return The sum of the cov_s and cov_d covariance function.
     */
    double cov( const wmath::WPosition& p1, const wmath::WPosition& p2 ) const;

    /**
     * Read-only reference to the tensor field used for the covariance function \see cov_d.
     */
    boost::shared_ptr< const WDataSetDTI > m_tensors;

    /**
     * A copy of the tract is saved here for covariance function computation.
     */
    wmath::WFiber m_tract;

    /**
     * Covariance matrix of all pairs of sample points of the tract using the \see cov function.
     */
    Eigen::MatrixXd m_CffInverse;
};

inline double WGaussProcess::cov_s( const wmath::WPosition& p1, const wmath::WPosition& p2, const double R ) const
{
    double r = ( p1 - p2 ).norm();
    if( r > R )
    {
        return 0.0;
    }
    return 2 * std::abs( r * r * r ) - 3 * R * r * r + R * R * R;
}

inline double WGaussProcess::cov_d( const wmath::WPosition& p1, const wmath::WPosition& p2, const double t ) const
{
    return 0.0;
}

inline double WGaussProcess::cov( const wmath::WPosition& p1, const wmath::WPosition& p2 ) const
{
    return cov_s( p1, p2 ) + cov_d( p1, p2 );
}

#endif  // WGAUSSPROCESS_H
