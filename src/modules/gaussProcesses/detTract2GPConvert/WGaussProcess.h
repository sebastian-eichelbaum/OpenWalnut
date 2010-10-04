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

#include "../../../common/datastructures/WFiber.h"
#include "../../../common/math/WMatrix.h"
#include "../../../common/math/WValue.h"
#include "../../../dataHandler/WDataSetDTI.h"

/**
 * Represents a basic gaussian process with its mean- and covariance function. Basically this aims
 * to implement a part of the gaussian process framework as presented by Wasserman et. al:
 * http://dx.doi.org/10.1016/j.neuroimage.2010.01.004
 */
class WGaussProcess
{
friend class WGaussProcessTest;
public:
    /**
     * Constructs a gaussian process out of a fiber with the help of underlying diffusion tensor
     * information.
     *
     * \param tract One deterministic tractogram
     * \param tensors All 2nd order diffusion tensors
     * \param maxLevel The real number which represents the maximum intensity
     */
    WGaussProcess( const wmath::WFiber& tract, boost::shared_ptr< const WDataSetDTI > tensors, double maxLevel = 1.0 );

    /**
     * Default destructor.
     */
    virtual ~WGaussProcess();

    /**
     * Computes the mean function describing this gausprocess as described in the Wassermann paper,
     * see equation (16) on page 12.
     *
     * \param p The point where to evaluate the gauss process.
     *
     * \return The mean value of this gaussian process at the point \e p.
     */
    double mean( const wmath::WPosition& p ) const;

protected:
private:
    /**
     * Computes the covariance matrix and invert it. This shall always be possible since the
     * creating function is positive definit. (TODO(math): at least Demain said this) Finally the
     * inverse is saved to the member variable \ref m_CffInverse.
     */
    void generateCffInverse();

    /**
     * Computes tau parameter representing the max diffusion time.
     *
     * \see m_tau
     *
     * \return The parameter tau
     */
    double generateTauParameter();

    /**
     * Covariance function of two points representing the smoothness of the tract.
     *
     * \param p1 First point
     * \param p2 Second point
     *
     * \return Real number indicating the covariance between two points representing the smoothness
     * of the tract
     */
    double cov_s( const wmath::WPosition& p1, const wmath::WPosition& p2 ) const;

    /**
     * Covariance function of two points representing the blurring of the tract's diffusion.
     *
     * \param p1 First point
     * \param p2 Second point
     *
     * \return Real number indicating the covariance between two points representing the diffusion
     * associated blurring.
     */
    double cov_d( const wmath::WPosition& p1, const wmath::WPosition& p2 ) const;

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
     * Copy of the sample points of the tract, since they are always needed for the mean computation
     * (S_f(p) will need all f_i ).
     */
    wmath::WFiber m_tract;

    /**
     * Read-only reference to the tensor field used for the covariance function \ref cov.
     */
    boost::shared_ptr< const WDataSetDTI > m_tensors;

    /**
     * Covariance matrix of all pairs of sample points of the tract using the \ref cov function.
     */
    Eigen::MatrixXd m_CffInverse;

    /**
     * The maximal diffusion time (tau) as described in the Wassermann paper line 790, page 12 after
     * equation (16).
     */
    double m_tau;

    /**
     * Max segment length of the tract describing the R environment.
     */
    double m_R;

    /**
     * The MaximumLevelSet, representing the real number which is used for blurring the sample
     * point. No other point has a bigger values after the blurring.
     */
    double m_maxLevel;
};

inline double WGaussProcess::cov_s( const wmath::WPosition& p1, const wmath::WPosition& p2 ) const
{
    double r = ( p1 - p2 ).norm();
    if( r >  m_R )
    {
        return 0.0;
    }
    return 2 * std::abs( r * r * r ) - 3 * m_R * r * r + m_R * m_R * m_R;
}

inline double WGaussProcess::cov( const wmath::WPosition& p1, const wmath::WPosition& p2 ) const
{
    return cov_s( p1, p2 ) + cov_d( p1, p2 );
}

#endif  // WGAUSSPROCESS_H
