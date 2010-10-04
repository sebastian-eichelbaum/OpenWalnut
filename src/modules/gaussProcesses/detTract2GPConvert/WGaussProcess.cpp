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


#include <Eigen/QR>

#include "../../../common/WAssert.h"
#include "WGaussProcess.h"

WGaussProcess::WGaussProcess( const wmath::WFiber& tract, boost::shared_ptr< const WDataSetDTI > tensors, double maxLevel )
    : m_tract( tract ),
      m_tensors( tensors ),
      m_CffInverse( static_cast< int >( tract.size() ), static_cast< int >( tract.size() ) ),
      m_R( tract.maxSegmentLength() ),
      m_maxLevel( maxLevel )
{
    generateCffInverse();
    generateTauParameter();
}

WGaussProcess::~WGaussProcess()
{
}

double WGaussProcess::mean( const wmath::WPosition& p ) const
{
    double result = 0.0;
    Eigen::VectorXd Sf( m_CffInverse.rows() );

    for( size_t i = 0; i < m_tract.size(); ++i )
    {
        Sf( i ) = cov_s( m_tract[i], p );
    }

    Eigen::VectorXd l = Eigen::VectorXd::Ones( m_CffInverse.rows() ) * m_maxLevel;
    Eigen::VectorXd matrixVector = m_CffInverse * l;
    result = Sf.dot( matrixVector );
    return result;
}

void WGaussProcess::generateCffInverse()
{
    Eigen::MatrixXd Cff( static_cast< int >( m_tract.size() ), static_cast< int >( m_tract.size() ) );
    size_t i = 0, j = 0;
    for( wmath::WFiber::const_iterator cit = m_tract.begin(); cit != m_tract.end(); ++cit, ++i )
    {
        for( wmath::WFiber::const_iterator cit2 = m_tract.begin(); cit2 != m_tract.end(); ++cit2, ++j )
        {
            Cff( i, j ) = cov( *cit, *cit2 );
        }
        j = 0; // reset every loop!
    }

    // Note: If Cff is constructed via a positive definite function itself is positive definite,
    // hence invertible
    Eigen::ColPivHouseholderQR< Eigen::MatrixXd > qrOfCff( Cff );
    m_CffInverse = qrOfCff.inverse();
}

double WGaussProcess::generateTauParameter()
{
    double result = 0.0;
    for( wmath::WFiber::const_iterator cit = m_tract.begin(); cit != m_tract.end(); ++cit )
    {
        // According to Demian this function is very complex, involing tensor interpolation (not
        // component wise) which is not trivial, but the out come does not contribute significantly to
        // the result, so I ommit the implementation at first.
        //
        // wmath::WTensorSym< 2, 3 > t = tensors->interpolate( *cit );
        // it may occur due to interpolation and noise that negative eigenvalues will occour!
        // double lambda_1 = 0.0; // = t.eigenvalues(
        // newTau = m_R / std::sqrt( lambda_1 );
    }
    return result;
}

double WGaussProcess::cov_d( const wmath::WPosition& /* p1 */, const wmath::WPosition& /* p2 */ ) const
{
    // According to Demian this function is very complex, involing tensor interpolation (not
    // component wise) which is not trivial, but the out come does not contribute significantly to
    // the result, so I ommit the implementation at first.
    return 0.0;
}
