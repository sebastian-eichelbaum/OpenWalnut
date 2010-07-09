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

#ifndef WSURFACE_H
#define WSURFACE_H

#include <vector>

#include "../../common/math/WTensorSym.h"
#include "../../graphicsEngine/WTriangleMesh2.h"
#include "WBSplineSurface.h"

/**
 * Constructs a triangle mesh representation of a spline surface from a given
 * number of input points.
 */
class WSurface
{
public:
    /**
     * Constructs new WSurface.
     */
    WSurface();

    /**
     * Destructs this WSurface.
     */
    ~WSurface();

    /**
     * Runs the algo and constructs a spine surface from the given input points
     *
     * \return a triangle mesh of the constructed spline surface
     */
    boost::shared_ptr< WTriangleMesh2 > execute();

    /**
     * Returns a copy of the spline point vector.
     *
     * \return points
     */
    std::vector< std::vector< double > > getSplinePoints();

    /**
     * SEts the sample rate for the splines.
     *
     * \param r the new sample rate
     */
    void setSetSampleRate( float r );

private:
    /**
     * Calculates the covariance matrix for a given number of points inspace.
     *
     * \param points vector of points
     * \return the matrix
     */
    wmath::WTensorSym< 2, 3, double > getCovarianceMatrix( std::vector< std::vector< double > > points );

    /**
     * Calculates numRows*numCols deBoor points from given input points.
     *
     * \param givenPoints the input points
     * \param deBoorPoints reference to the output vector
     * \param numRows number of points in first direction of spline surface
     * \param numCols number of points in second direction of spline surface
     */
    void getSplineSurfaceDeBoorPoints( std::vector< std::vector< double > > &givenPoints, std::vector< std::vector< double > > &deBoorPoints, int numRows, int numCols ); // NOLINT

    boost::shared_ptr< WTriangleMesh2 > m_tMesh; //!< Triangle mesh of the surface

    double m_radius; //!< param for the algo
    double m_mu; //!< parameter of local shepard with franke-little-weights
    int m_numDeBoorRows; //!< number of of rows for deBoor grid
    int m_numDeBoorCols; //!< number of of columns for deBoor grid
    int m_order; //!< order the splines
    double m_sampleRateT; //!< sampling rate of spline in first direction
    double m_sampleRateU; //!< sampling rate of spline in second direction
    double m_xAverage; //!< global mean of x values for covariance matrix
    double m_yAverage; //!< global mean of y values for covariance matrix
    double m_zAverage; //!< global mean of z values for covariance matrix

    std::vector< std::vector< double > > m_splinePoints; //!< stores the input points

    int m_renderpointsPerCol; //!< resolution of the output mesh
    int m_renderpointsPerRow; //!< resolution of the output mesh
};

#endif  // WSURFACE_H
