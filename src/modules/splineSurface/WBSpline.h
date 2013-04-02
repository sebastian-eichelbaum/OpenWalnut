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

#ifndef WBSPLINE_H
#define WBSPLINE_H

#include <vector>
#include "core/common/math/linearAlgebra/WVectorFixed.h"


/**
 * A B-spline.
 * @author Patrick Oesterling
 */
class WBSpline
{
public:
    /**
     * Constructor for the spline that takes the order and de Boor points while constructing a normalized knotVector automatically.
     * \param order Order of the spline.
     * \param deBoorPoints The de Boor points for the spline
     */
    WBSpline( int order, std::vector< WVector3d > deBoorPoints );

    /**
     * Constructor for the spline that takes the order de Boor points and knots as arguments.
     * \param order Order of the spline.
     * \param deBoorPoints The de Boor points for the spline
     * \param knots The knots for the spline
     */
    WBSpline( int order, std::vector< WVector3d > deBoorPoints, std::vector<double> knots );

    /**
     * Empty destructor.
     */
    ~WBSpline();

    /**
     * Returns the stored de Boor points.
     * \return The stored de Boor points
     */
    std::vector< WVector3d > getDeBoorPoints();

    /**
     * Returns the stored knots.
     * \return The stored knots
     */
    std::vector<double> getKnots();

    /**
     * Returns the order of the spline.
     * \return The order of the spline
     */
    int getOrder();

    /**
     * Sets new de Boor points for the spline.
     * \param deBoorPoints The new de Boor points for the spline.
     */
    void setDeBoorPoints( std::vector< WVector3d > deBoorPoints );

    /**
     * Sets new knots for the spline.
     * \param knots The new knots for the spline.
     */
    void setKnots( std::vector<double> knots );

    /**
     * Sets a new order for the spline.
     * \param order The new order of the spline.
     */
    void setOrder( int order );

    /**
     * Compute sample points on the spline for a given resolution.
     * The sample points are stored in the vector given by the first argument.
     * \param p The pointer for returning the sample points.
     * \param resolution The resolution of the sample points.
     */
    void samplePoints( std::vector< WVector3d > *p, double resolution );

    /**
     * Compute a single sample point on the spline for the given parameter.
     * \param t parameter on spline curve.
     * \return Sample point on the curve at t.
     */
    WVector3d f( double t );

private:
    int m_order; //!< The order of the spline.
    std::vector< WVector3d > m_deBoorPoints; //!< The de Boor points of the spline.
    std::vector<double> m_knots; //!< The knots of the spline.
    double m_t; //!< The parameter value of the last spline evaluation i.e. the last call to f().

    /**
     * Compute the alpha of the de Boor algorithm for the given parameters
     * \param _i First recursion parameter.
     * \param _j Second recursion parameter.
     * \return De Boor alpha for (_i,_j)
     */
    double getAlpha_i_j( int _i, int _j );

    /**
     * Compute a control point of the de Boor algorithm for the given parameters
     * \param _i First recursion parameter.
     * \param _j Second recursion parameter.
     * \return De Boor control point at (_i,_j).
     */
    WVector3d controlPoint_i_j( int _i, int _j );
};

#endif  // WBSPLINE_H
