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

#include <vector>
#include "WPrincipalComponentAnalysis.h"

WPrincipalComponentAnalysis::WPrincipalComponentAnalysis()
{
    m_isValidPCA = false;
}

WPrincipalComponentAnalysis::~WPrincipalComponentAnalysis()
{
}

void WPrincipalComponentAnalysis::analyzeData( const vector<WPosition>& inputData )
{
    m_isValidPCA = true;
    m_inputPointCount = inputData.size();
    if( m_inputPointCount > 0 )
        m_dimensions = inputData.at( 0 ).size();
    m_covarianceSolver.analyzeData( inputData );
    EigenSolver<MatrixXd> es( m_covarianceSolver.getCovariance() );
    m_eigenSolver = es;
    extractEigenData();
    sortByVarianceDescending();
}
void WPrincipalComponentAnalysis::extractEigenData()
{
    int count = m_eigenSolver.eigenvalues().size();
    m_eigenVectors.reserve( count );
    m_eigenVectors.resize( count );
    m_eigenValues.reserve( count );
    m_eigenValues.resize( count );
    for( int index = 0; index < count; index++ )
    {
        complex<double> lambda = m_eigenSolver.eigenvalues()[index];
        if( lambda.imag() != 0.0 )
            m_isValidPCA = false;
        m_eigenValues[index] = lambda.real();

        VectorXcd vector = m_eigenSolver.eigenvectors().col( index );
        WVector3d newPosition;
        size_t vectorSize = static_cast<size_t>( vector.size() );
        for( size_t dimension = 0; dimension < vectorSize && dimension < newPosition.size(); dimension++ )
        {
            complex<double> value = vector[dimension];
            newPosition[dimension] = value.real();
        }
        m_eigenVectors[index] = newPosition;
    }
    sortByVarianceDescending();
}
void WPrincipalComponentAnalysis::sortByVarianceDescending()
{
    if( m_inputPointCount == 0 ) return;
    for( size_t d1 = 0; d1 < m_dimensions - 1; d1++ )
        for( size_t d2 = d1 + 1; d2 < m_dimensions; d2++ )
            if( m_eigenValues[d1] < m_eigenValues[d2] )
                swapEigenVectors( d1, d2 );
}
void WPrincipalComponentAnalysis::swapEigenVectors( size_t eigenVectorIndex1, size_t eigenVectorIndex2 )
{
    for( size_t dimension = 0; dimension < m_eigenValues.size(); dimension++ )
    {
        double prevValue = m_eigenVectors[eigenVectorIndex1][dimension];
        m_eigenVectors[eigenVectorIndex1][dimension] = m_eigenVectors[eigenVectorIndex2][dimension];
        m_eigenVectors[eigenVectorIndex2][dimension] = prevValue;
    }
    double prevValue = m_eigenValues[eigenVectorIndex1];
    m_eigenValues[eigenVectorIndex1] = m_eigenValues[eigenVectorIndex2];
    m_eigenValues[eigenVectorIndex2] = prevValue;
}
WPosition WPrincipalComponentAnalysis::getMean()
{
    return m_covarianceSolver.getMean();
}
vector<WVector3d> WPrincipalComponentAnalysis::getEigenVectors()
{
    return m_eigenVectors;
}
vector<double> WPrincipalComponentAnalysis::getEigenValues()
{
    return m_eigenValues;
}
