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

#ifndef WPRINCIPALCOMPONENTANALYSIS_H
#define WPRINCIPALCOMPONENTANALYSIS_H

#include <vector>
#include <Eigen/Dense>  //TODO(aschwarzkopf): Consider reimplementing Eigen from your JAVA implementation.
#include "WCovarianceSolver.h"
#include "core/common/math/linearAlgebra/WVectorFixed.h"
#include "core/common/math/linearAlgebra/WPosition.h"

using std::vector;
using std::complex;
using std::cout;
using std::endl;
using Eigen::EigenSolver;
using Eigen::MatrixXcd;
using Eigen::VectorXcd;

/**
 * This is a class that analyzes the point distribution of point couds using the 
 * Principal Component Analysis. This class puts out vectors of main point distribution 
 * directions and how much they are distributed in that directions.
 * 
 * The algorithms are based mainly on Covariance calculation and Eigen Value and Vector 
 * analysis. General about Principal Component Analysis:
 * http://georgemdallas.wordpress.com/2013/10/30/principal-component-
 *         analysis-4-dummies-eigenvectors-eigenvalues-and-dimension-reduction/
 * See principle of algorithm here: 
 *         http://www.cs.otago.ac.nz/cosc453/student_tutorials/principal_components.pdf
 */
class WPrincipalComponentAnalysis
{
public:
    /**
     * Creates the Principal Component Analysis instance.
     */
    WPrincipalComponentAnalysis();
    /**
     * Destroys the Principal Component Analysis instance.
     */
    virtual ~WPrincipalComponentAnalysis();
    /**
     * Analyzes a point data set using the Principal Component Analysis algorithm.
     * \param inputData Point data to be analyzed.
     */
    void analyzeData( std::vector<WPosition> inputData );
    /**
     * Returns the mean coordinate of all input points.
     * \return The mean coordinate of all input points.
     */
    WPosition getMean();
    /**
     * Returns the point distribution directions. The first one is the strongest and 
     * the last is the weakest point distribution direction. Their index corresponds to 
     * getVariance().
     * \return Point distribution directions.
     */
    vector<WVector3d> getDirections();
    /**
     * Returns Eigen Values (equals how much the directions of getDirections() are 
     * distributed). Its index corresponds to the indices of that method. The strengths 
     * are sorted descending.
     * \return Distribution strength in a direction.
     */
    vector<double> getEigenValues();

private:
    /**
     * Does the actual Eigen Value and Vector Analysis.
     */
    void extractEigenData();
    /**
     * Sorts the Point distribution direction by the descending strength.
     */
    void sortByVarianceDescending();
    /**
     * Replaces two Eigen Vectors and its Variances by each other.
     * \param eigenVectorIndex1 Index of the first Eigen Value and Vector
     * \param eigenVectorIndex2 Index of the second Eigen Value and Vector
     */
    void swapEigenVectors( size_t eigenVectorIndex1, size_t eigenVectorIndex2 );
    /**
     * Clears the input point data.
     */
    void clearInputData();

    /**
     * Point data to be analyzed.
     */
    vector<WPosition> m_inputData;
    /**
     * Instance to analyze the covariances of the point data between each dimension pair.
     */
    WCovarianceSolver m_covarianceSolver;
    /**
     * Main point distribution directions. After process they're sorted by their strength descending.
     */
    vector<WVector3d> m_directions;
    /**
     * Lambda values for A*x-Lambda*x=0.
     */
    vector<double> m_eigenValues;
    /**
     * Instance for solving Eigen Values and Vectors.
     */
    EigenSolver<MatrixXd> m_eigenSolver;
    /**
     * Indicator whether calculation is still valid.
     */
    bool m_isValidPCA;  //TODO(aschwarzkopf): Still no effect.
};

#endif  // WPRINCIPALCOMPONENTANALYSIS_H
