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

#ifndef WOSSIMHELPER_H
#define WOSSIMHELPER_H

#ifdef OW_USE_OSSIM
    #include <algorithm>
    #include <iostream>
    #include <vector>

    #include <matrix/newmat.h>

    #include <boost/shared_ptr.hpp>

    #include "WMatrix.h"
    #include "WValue.h"

    /**
     * Class with functions to use the GNU scientific library
     */
    class WOSSIMHelper
    {
    public:
      static boost::shared_ptr< NEWMAT::Matrix > OWMatrixToOSSIMMatrix( const WMatrix< double > &input );
      static WMatrix< double > OSSIMMatrixToOWMatrix( const NEWMAT::Matrix& input );
      static WMatrix< double > OSSIMDiagonalMatrixToOWMatrix( const NEWMAT::DiagonalMatrix& input );
      static WValue< double > OSSIMDiagonalMatrixToOWVector( const NEWMAT::DiagonalMatrix& input );
      // static gsl_vector* OWVectorToOSSIMVector( const WValue<double> &input );
      // static WValue<double> OSSIMVectorToOWVector( const gsl_vector* input );

      // computes the SVD of the Matrix A
      static void computeSVD( const WMatrix< double >& A,
                              WMatrix< double >& U,
                              WMatrix< double >& V,
                              WValue< double >& S );
      // computes the pseudo inverse of the Matrix A
      // static WMatrix<double> pseudoInverse( const WMatrix<double>& input );
    };
#endif  // OW_USE_OSSIM

#endif  // WOSSIMHELPER_H
