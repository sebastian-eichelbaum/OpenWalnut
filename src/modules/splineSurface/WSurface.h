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
     * \param numRows
     * \param numCols
     */
    void getSplineSurfaceDeBoorPoints( std::vector< std::vector< double > > &givenPoints, std::vector< std::vector< double > > &deBoorPoints, int numRows, int numCols ); // NOLINT

    boost::shared_ptr< WTriangleMesh2 > m_tMesh; //!< Triangle mesh of the surface

    // // TODO(schurade): comments need to be fixed
    // i know people want real comments and want to know what each parameter does and what it means in the
    // grand scheme of things and the universe, but the fantom code where this is from, isn't commented either,
    // so we can only guess, be happy that it does what it does
    double m_radius; //!< param for the algo
    double m_my; //!< param for the algo
    int m_numDeBoorRows; //!< param for the algo
    int m_numDeBoorCols; //!< param for the algo
    int m_order; //!< param for the algo
    double m_sampleRateT; //!< param for the algo
    double m_sampleRateU; //!< param for the algo
    double m_xAverage; //!< param for the algo
    double m_yAverage; //!< param for the algo
    double m_zAverage; //!< param for the algo

    std::vector< std::vector< double > > m_splinePoints; //!< stores the input points

    int m_renderpointsPerCol; //!< resolution of the output mesh
    int m_renderpointsPerRow; //!< resolution of the output mesh
};

#endif  // WSURFACE_H
