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

#include "core/common/math/WTensorSym.h"
#include "core/graphicsEngine/WTriangleMesh.h"
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
    void execute();

    /**
     * Returns a copy of the spline point vector.
     *
     * \return points
     */
    std::vector< WVector3d > getSplinePoints();

    /**
     * SEts the sample rate for the splines.
     *
     * \param r the new sample rate
     */
    void setSetSampleRate( float r );

    /**
     * sets the vector of support points the surface is calculated from
     * \param supportPoints vector of support points
     * \param forceUpdate if true the surface will be updated with the new support points
     */
    void setSupportPoints( std::vector< WVector3d> supportPoints, bool forceUpdate = false );

    /**
     * getter
     * \return the triangle mesh representing the surface
     */
    boost::shared_ptr< WTriangleMesh > getTriangleMesh();

private:
    /**
     * Calculates the covariance matrix for a given number of points inspace.
     *
     * \param points vector of points
     * \return the matrix
     */
    WTensorSym< 2, 3, double > getCovarianceMatrix( std::vector< WVector3d > points );

    /**
     * Calculates numRows*numCols deBoor points from given input points.
     *
     * \param givenPoints the input points
     * \param deBoorPoints reference to the output vector
     * \param numRows number of points in first direction of spline surface
     * \param numCols number of points in second direction of spline surface
     */
    void getSplineSurfaceDeBoorPoints( std::vector< WVector3d > &givenPoints, std::vector< WVector3d > &deBoorPoints, int numRows, int numCols ); // NOLINT

    boost::shared_ptr< WTriangleMesh > m_tMesh; //!< Triangle mesh of the surface

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

    std::vector< WVector3d > m_supportPoints; //!< stores the support points

    std::vector< WVector3d > m_splinePoints; //!< stores the input points ????

    int m_renderpointsPerCol; //!< resolution of the output mesh
    int m_renderpointsPerRow; //!< resolution of the output mesh
};

#endif  // WSURFACE_H
