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

#ifndef WBSPLINESURFACE_H
#define WBSPLINESURFACE_H

#include <vector>

#include "core/common/math/linearAlgebra/WLinearAlgebra.h"

/**
 * A B-spline surface.
 * @author Patrick Oesterling
 *
   numDeBoorPoints1 -> t-parameter (knots1)
   0  1  2  3  4  5  .....
   n     ___________________________
   u    |  |  |  |  |  |  |  |  |  |
   m    |__________________________|
   D    |  |  |  |  |  |  |  |  |  |
   e    |__________________________|
   B    |  |  |  |  |  |  |  |  |  |
   o    |__________________________|
   o    |  |  |  |  |  |  |  |  |  |
   r    |__________________________|
   P    |  |  |  |  |  |  |  |  |  |
   2    |__________________________|
   ->   |  |  |  |  |  |  |  |  |  |
   u    |__________________________|
   -    |  |  |  |  |  |  |  |  |  |
   param|__________________________|
(knots2)|  |  |  |  |  |  |  |  |  |
        |__________________________|


*/
class WBSplineSurface
{
public:

    /**
     * Constructor for the surface that takes the orders and de Boor points of the underlying splines
     * while constructing normalized knotVectors automatically.
     * \param order1 Order of the spline in first direction.
     * \param order2 Order of the spline in second direction.
     * \param deBoorPoints The de Boor points for the spline
     * \param numDeBoorPoints1 The number of de Boor points for the spline in the first direction.
     * \param numDeBoorPoints2 The number of de Boor points for the spline in the second direction.
     */
    WBSplineSurface( int order1,
                     int order2,
                     std::vector< WVector3d > deBoorPoints,
                     int numDeBoorPoints1,
                     int numDeBoorPoints2 );
    /**
     * Constructor for the surface that takes the orders, de Boor points
     * knots of the underlying splines.
     * \param order1 Order of the spline in first direction.
     * \param order2 Order of the spline in second direction.
     * \param deBoorPoints The de Boor points for the spline
     * \param numDeBoorPoints1 The number of de Boor points for the spline in the first direction.
     * \param numDeBoorPoints2 The number of de Boor points for the spline in the second direction.
     * \param knots1 The new knots for the spline in the first direction.
     * \param knots2 The new knots for the spline in the second direction.
     */
    WBSplineSurface( int order1,
                     int order2,
                     std::vector< WVector3d > deBoorPoints,
                     int numDeBoorPoints1,
                     int numDeBoorPoints2,
                     std::vector<double> knots1,
                     std::vector<double> knots2 );

    /**
     * Empty destructor.
     */
    ~WBSplineSurface();

    /**
     * Returns the stored de Boor points.
     * \return All de Boor points of the B-spline
     */
    std::vector< WVector3d > getDeBoorPoints();

    /**
     * Returns the number of de Boor points in the first direction.
     * \return Number of de Boor points in first direction
     */
    int getNumDeBoorPoints1();
    /**
     * Returns the number of de Boor points in the second direction.
     * \return Number of de Boor points in second direction
     */
    int getNumDeBoorPoints2();

    /**
     * Returns the stored knots in the first direction.
     * \return Number of knots in the first direction
     */
    std::vector<double> getKnots1();

    /**
     * Returns the stored knots in the second direction.
     * \return Number of knots in the second direction
     */
    std::vector<double> getKnots2();

    /**
     * Returns the order of the spline in the first direction.
     * \return The order of the spline in the first direction.
     */
    int getOrder1();

    /**
     * Returns the order of the spline in the second direction.
     * \return The order of the spline in the second direction.
     */
    int getOrder2();

    /**
     * Returns the number of sample points in the first direction that were used for the last call to samplePoints().
     * \return the number of sample points in the first direction that were used for the last call to samplePoints().
     */
    int getNumSamplePointsT();

    /**
     * Returns the number of sample points in the second direction that were used for the last call to samplePoints().
     * \return the number of sample points in the second direction that were used for the last call to samplePoints().
     */
    int getNumSamplePointsU();

    /**
     * Sets new de Boor points for the splines.
     * \param deBoorPoints The new de Boor points for the splines.
     * \param numDeBoorPoints1 The number of de Boor points for the spline in the first direction.
     * \param numDeBoorPoints2 The number of de Boor points for the spline in the second direction.
     */
    void setDeBoorPoints( std::vector< WVector3d > deBoorPoints, int numDeBoorPoints1, int numDeBoorPoints2 );

    /**
     * Sets new knots for the spline in the first direction.
     * \param knots The new knots for the spline in the first direction.
     */
    void setKnots1( std::vector<double> knots );

    /**
     * Sets new knots for the spline in the second direction.
     * \param knots The new knots for the spline in the second direction.
     */
    void setKnots2( std::vector<double> knots );

    /**
     * Sets a new order for the spline in the first direction.
     * \param order The new order of the spline in the first direction.
     */
    void setOrder1( int order );

    /**
     * Sets a new order for the spline in the second direction.
     * \param order The new order of the spline in the second direction.
     */
    void setOrder2( int order );

    /**
     * Compute sample points on the spline surface for a given resolution in the two directions.
     * The sample points are stored in the vector given by the first argument.
     * \param points The pointer for returning the sample points.
     * \param tResolution The resolution in the first direction.
     * \param uResolution The resolution in the second direction.
     */
    void samplePoints( std::vector< WVector3d > *points, double tResolution, double uResolution );

private:
    /**
     * Compute a single sample point on the surface for the given parameters.
     * \param t parameter for first direction.
     * \param u parameter for second direction.
     * \return Sample point at (t,u).
     */
    WVector3d f( double t, double u );

    int m_order1; //!< order for the spline in the first direction.
    int m_order2; //!< order for the spline in the second direction.
    int m_numDeBoorPoints1; //!< number of de Boor points for the spline in the first direction.
    int m_numDeBoorPoints2; //!< number of de Boor points for the spline in the second direction.
    int m_numSamplePointsT; //!< The number of sample points in the first direction that were used for the last call to samplePoints().
    int m_numSamplePointsU; //!< The number of sample points in the second direction that were used for the last call to samplePoints().

    std::vector< WVector3d > m_deBoorPoints; //!< The de Boor points of the splines.

    std::vector<double> m_knots1; //!< The knots of the spline in the first direction.
    std::vector<double> m_knots2; //!< The knots of the spline in the second direction.
};

#endif  // WBSPLINESURFACE_H
