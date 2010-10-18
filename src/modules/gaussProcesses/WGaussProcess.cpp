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

#include "../../common/WAssert.h"
#include "WGaussProcess.h"

WGaussProcess::WGaussProcess( const size_t tractID,
                              boost::shared_ptr< const WDataSetFibers > tracts,
                              boost::shared_ptr< const WDataSetDTI > tensors,
                              double maxLevel )
    : m_tractID( tractID ),
      m_tracts( tracts ),
      m_tensors( tensors ),
      m_maxLevel( maxLevel )
{
    wmath::WFiber tract = ( *m_tracts )[ m_tractID ];
    m_Cff_1_l_product = Eigen::VectorXd( static_cast< int >( tract.size() ) );
    m_R = tract.maxSegmentLength();
    m_Cff_1_l_product = generateCffInverse( tract ) * ( Eigen::VectorXd::Ones( m_Cff_1_l_product.size() ) * m_maxLevel );
    generateTauParameter();
}

WGaussProcess::~WGaussProcess()
{
}

double WGaussProcess::mean( const wmath::WPosition& p ) const
{
    Eigen::VectorXd Sf( m_Cff_1_l_product.size() );

    // for further improvement we could work with the indices of the arrays inside of the WDataSetFibers instead of building up this point vector
    wmath::WFiber tract = ( *m_tracts )[ m_tractID ];

    for( size_t i = 0; i < tract.size(); ++i )
    {
        Sf( i ) = cov_s( tract[i], p );
    }

    return Sf.dot( m_Cff_1_l_product );
}

Eigen::MatrixXd WGaussProcess::generateCffInverse( const wmath::WFiber& tract )
{
    Eigen::MatrixXd Cff( static_cast< int >( tract.size() ), static_cast< int >( tract.size() ) );
    size_t i = 0, j = 0;
    for( wmath::WFiber::const_iterator cit = tract.begin(); cit != tract.end(); ++cit, ++i )
    {
        for( wmath::WFiber::const_iterator cit2 = tract.begin(); cit2 != tract.end(); ++cit2, ++j )
        {
            Cff( i, j ) = cov( *cit, *cit2 );
        }
        j = 0; // reset every loop!
    }

    // Note: If Cff is constructed via a positive definite function itself is positive definite,
    // hence invertible
    Eigen::ColPivHouseholderQR< Eigen::MatrixXd > qrOfCff( Cff );
    return qrOfCff.inverse();
}

double WGaussProcess::generateTauParameter()
{
    double result = 0.0;

    // According to Demian this function is very complex, involing tensor interpolation (not
    // component wise) which is not trivial, but the out come does not contribute significantly to
    // the result, so I ommit the implementation at first.
    //
    // wmath::WTensorSym< 2, 3 > t = tensors->interpolate( *cit );
    // it may occur due to interpolation and noise that negative eigenvalues will occour!
    // double lambda_1 = 0.0; // = t.eigenvalues(
    // newTau = m_R / std::sqrt( lambda_1 );

    return result;
}

double WGaussProcess::cov_d( const wmath::WPosition& /* p1 */, const wmath::WPosition& /* p2 */ ) const
{
    // According to Demian this function is very complex, involing tensor interpolation (not
    // component wise) which is not trivial, but the out come does not contribute significantly to
    // the result, so I ommit the implementation at first.
    return 0.0;
}
