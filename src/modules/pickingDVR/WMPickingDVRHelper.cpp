//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2017 OpenWalnut Community
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

#include "WMPickingDVRHelper.h"

namespace PickingDVRHelper
{
    /**
     * Computes the first and second derivatives of a vector of values
     *
     * \param values Values for which the derivatives will be computed
     * \param vecFirstDerivative The first derivative of the values
     * \param vecSecondDerivative The second derivative of the values
     */
    void calculateDerivativesWYSIWYP( const std::vector<double>& values,
                                                    std::vector<double>& vecFirstDerivative,
                                                    std::vector<double>& vecSecondDerivative )
    {
        //Fourth Order Finite Differencing by Daniel Gerlicher
        unsigned int n  = values.size();
        double deriv = 0.0;
        double coeff = 1.0 / 12.0;

        //Calculate first derivative
        for( unsigned int j = 0; j < n; j++ )
        {
            //Forward Diff
            if( j == 0 )
            {
                deriv = coeff * ( -25 * values[j]
                                  + 48 * values[j+1]
                                  - 36 * values[j+2]
                                  + 16 * values[j+3]
                                  - 3 * values[j+4] );
            }
            else if( j == 1 )
            {
                deriv = coeff * ( -3 * values[j-1]
                                  - 10 * values[j]
                                  + 18 * values[j+1]
                                  - 6 * values[j+2]
                                  + 1 * values[j+3] );
            }

            //Backward Diff
            else if( j == n - 1 )
            {
                deriv = coeff * ( 25 * values[j]
                                  - 48 * values[j-1]
                                  + 36 * values[j-2]
                                  - 16 * values[j-3]
                                  + 3 * values[j-4] );
            }
            else if( j == n - 2 )
            {
                deriv = coeff * ( +3 * values[j+1]
                                  + 10 * values[j]
                                  - 18 * values[j-1]
                                  + 6 * values[j-2]
                                  - 1 * values[j-3] );
            }

            //Center
            else
            {
                deriv = coeff * ( -1 * values[j+2]
                                  + 8 * values[j+1]
                                  - 8 * values[j-1]
                                  + 1 * values[j-2] );
            }

            vecFirstDerivative.push_back( deriv );
        }

        //Calculate Second derivative, by applying the first derivative
        for( unsigned int j = 0; j < n; j++ )
        {
            //Forward Diff
            if( j == 0 )
            {
                deriv = coeff * ( -25 * vecFirstDerivative[j]
                                  + 48 * vecFirstDerivative[j+1]
                                  - 36 * vecFirstDerivative[j+2]
                                  + 16 * vecFirstDerivative[j+3]
                                  - 3 * vecFirstDerivative[j+4] );
            }
            else if( j == 1 )
            {
                deriv = coeff * ( -3 * vecFirstDerivative[j-1]
                                  - 10 * vecFirstDerivative[j]
                                  + 18 * vecFirstDerivative[j+1]
                                  - 6 * vecFirstDerivative[j+2]
                                  + 1 * vecFirstDerivative[j+3] );
            }

            //Backward Diff
            else if( j == n - 1 )
            {
                deriv = coeff * ( 25 * vecFirstDerivative[j]
                                  - 48 * vecFirstDerivative[j-1]
                                  + 36 * vecFirstDerivative[j-2]
                                  - 16 * vecFirstDerivative[j-3]
                                  + 3 * vecFirstDerivative[j-4] );
            }
            else if( j == n - 2 )
            {
                deriv = coeff * ( +3 * vecFirstDerivative[j+1]
                                  + 10 * vecFirstDerivative[j]
                                  - 18 * vecFirstDerivative[j-1]
                                  + 6 * vecFirstDerivative[j-2]
                                  - 1 * vecFirstDerivative[j-3] );
            }

            //Center
            else
            {
                deriv = coeff * ( -1 * vecFirstDerivative[j+2]
                                  + 8 * vecFirstDerivative[j+1]
                                  - 8 * vecFirstDerivative[j-1]
                                  + 1 * vecFirstDerivative[j-2] );
            }

            vecSecondDerivative.push_back( deriv );
        }
    }
}
