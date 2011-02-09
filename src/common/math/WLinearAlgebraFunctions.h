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

#ifndef WLINEARALGEBRAFUNCTIONS_H
#define WLINEARALGEBRAFUNCTIONS_H

#include "../WExportCommon.h"

namespace osg
{
    class Matrixd;
}

namespace wmath
{
    class WVector3D;
    typedef osg::Matrixd WMatrix4x4;
    typedef WVector3D WPosition;
    template< typename > class WValue;
    template< typename > class WMatrix;

    /**
     * helper routine to multiply a 3x3 matrix with a vector
     *
     * \param mat 3x3 matrix
     * \param vec vector
     */
    WVector3D OWCOMMON_EXPORT multMatrixWithVector3D( WMatrix<double> mat, WVector3D vec );

    /**
     * Applies a coordinate transformation in homogenous coordinates to a vector.
     * This differs from transformPosition3DWithMatrix4D in that it DOES NOT incorporate the translation
     *
     * \param mat 4x4 matrix
     * \param vec vector
     */
    WVector3D OWCOMMON_EXPORT transformVector3DWithMatrix4D( WMatrix<double> mat, WVector3D vec );

    /**
     * Applies a coordinate transformation in homogenous coordinates to a position.
     * This differs from transformVector3DWithMatrix4D in that it incorporates the translation.
     *
     * \param mat 4x4 matrix
     * \param vec vector
     */
    WVector3D OWCOMMON_EXPORT transformPosition3DWithMatrix4D( WMatrix<double> mat, WPosition vec );

    /**
     * Applies a coordinate transformation in homogenous coordinates to a position.
     * This differs from transformVector3DWithMatrix4D in that it incorporates the translation.
     *
     * \param mat 4x4 matrix
     * \param vec vector
     */
    WVector3D OWCOMMON_EXPORT transformPosition3DWithMatrix4D( WMatrix4x4 mat, WPosition vec );

    /**
     * helper routine to invert a 3x3 matrix
     *
     * \param mat 3x3 matrix
     *
     * \return inverted 3x3 matrix
     */
    WMatrix<double> OWCOMMON_EXPORT invertMatrix3x3( WMatrix<double> mat );

    /**
     * helper routine to invert a 4x4 matrix
     *
     * \param mat 4x4 matrix
     *
     * \return inverted 4x4 matrix
     */
    WMatrix<double> OWCOMMON_EXPORT invertMatrix4x4( WMatrix<double> mat );

    /**
     * Checks if the given two vectors are linearly independent.
     *
     * \param u First vector
     * \param v Second vector
     *
     * \return True if they are linear independent.
     *
     * \note This check is performed with the cross product != (0,0,0) but in numerical stability with FLT_EPS.
     */
    bool OWCOMMON_EXPORT linearIndependent( const wmath::WVector3D& u, const wmath::WVector3D& v );

    /**
     * Computes the SVD for the Matrix \param A
     *
     * A=U*S*V^T
     *
     * \param A Input Matrix
     * \param U Output Matrix
     * \param S Output of the entries in the diagonal matrix
     * \param V Output Matrix
     *
     * \note This function needs the OSSIM library.
     */
    void OWCOMMON_EXPORT computeSVD( const wmath::WMatrix< double >& A, wmath::WMatrix< double >& U,
                     wmath::WMatrix< double >& V, wmath::WValue< double >& S );

    /**
     * Calculates for a matrix the pseudo inverse.
     *
     * \param input Matrix to invert
     *
     * \return Inverted Matrix
     *
     */
    wmath::WMatrix< double > OWCOMMON_EXPORT pseudoInverse( const WMatrix<double>& input );
}

#endif  // WLINEARALGEBRAFUNCTIONS_H
