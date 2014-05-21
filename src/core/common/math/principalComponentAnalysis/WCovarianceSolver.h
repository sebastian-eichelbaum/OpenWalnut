//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

#ifndef WCOVARIANCESOLVER_H
#define WCOVARIANCESOLVER_H

#include <vector>
#include <Eigen/Dense>
#include "core/common/math/linearAlgebra/WPosition.h"

using std::vector;

using Eigen::MatrixXd;

/**
 * Calculates covariances of a point data set.
 */
class WCovarianceSolver
{
public:
    /**
     * Creates an instance of the Covariance solver.
     */
    WCovarianceSolver();
    /**
     * Destroys the instance of the Covariance solver.
     */
    virtual ~WCovarianceSolver();
    /**
     * Analyzes the dimension covariances of a point data set.
     * \param dataSet The point data to be analyzed.
     */
    void analyzeData( const vector<WPosition>* dataSet );
    /**
     * Returns the mean coordinate of the input point data set.
     * \return The mean of the point data set.
     */
    WPosition getMean();
    /**
     * Returns the covariance matrix corresponding to the input point data set.
     * \return The covariances between all dimensions.
     */
    MatrixXd getCovariance();

private:
    /**
     * Calculates the mean of the input point data coordinates.
     */
    void calculateMean();
    /**
     * Calculates the covariance of the input point data.
     */
    void calculateCovariance();
    /**
     * Adds a point to the covariance solver.
     * \param pointIndex Index of a point in the data set to add to the calculation.
     */
    void addPointToCovariance( size_t pointIndex );

    /**
     * Input point data set to be analyzed.
     */
    const vector<WPosition>* m_dataSet;
    /**
     * The mean of all input points.
     */
    WPosition m_mean;
    /**
     * The calculated covariance matrix.
     */
    MatrixXd m_covariance;
};

#endif  // WCOVARIANCESOLVER_H
