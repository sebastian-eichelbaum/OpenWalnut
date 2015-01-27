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
#include "WCovarianceSolver.h"

WCovarianceSolver::WCovarianceSolver()
{
}

WCovarianceSolver::~WCovarianceSolver()
{
}
void WCovarianceSolver::analyzeData( const vector<WPosition>& dataSet )
{
    calculateMean( dataSet );
    calculateCovariance( dataSet );
}
WPosition WCovarianceSolver::getMean()
{
    return m_mean;
}
void WCovarianceSolver::calculateMean( const vector<WPosition>& dataSet )
{
    if( dataSet.size() == 0 )
        return;
    size_t dimensions = dataSet[0].size();
    for( size_t dimension = 0; dimension < dimensions; dimension++ )
        m_mean[dimension] = 0.0;
    for( size_t index = 0; index < dataSet.size(); index++ )
        for( size_t dimension = 0; dimension < dataSet[index].size(); dimension++ )
            m_mean[dimension] += dataSet[index][dimension];
    for( size_t dimension = 0; dimension < dimensions; dimension++ )
        m_mean[dimension] /= static_cast<double>( dataSet.size() );
}
void WCovarianceSolver::calculateCovariance( const vector<WPosition>& dataSet )
{
    size_t width = m_mean.size();
    MatrixXd newCovariance( width, width );
    m_covariance = newCovariance;
    for( size_t row = 0; row < width; row++ )
        for( size_t col = 0; col < width; col++ )
            m_covariance( row, col ) = 0.0;
    for( size_t index = 0; index < dataSet.size(); index++ )
        addPointToCovariance( dataSet[index] );
    for( size_t row = 0; row < width; row++ )
        for( size_t col = 0; col < width; col++ )
            m_covariance( row, col ) /= static_cast<double>( dataSet.size() ) - 1.0;
}
void WCovarianceSolver::addPointToCovariance( const WPosition& point )
{
    size_t width = m_mean.size();
    for( size_t row = 0; row < width; row++ )
    {
        double rowValue = point[row] - m_mean[row];
        for( size_t col = 0; col < width; col++ )
        {
            double colValue = point[col] - m_mean[col];
            m_covariance( row, col ) += rowValue * colValue;
        }
    }
}
MatrixXd WCovarianceSolver::getCovariance()
{
    return m_covariance;
}
