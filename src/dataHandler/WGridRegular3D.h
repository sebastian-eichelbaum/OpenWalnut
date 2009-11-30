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

#ifndef WGRIDREGULAR3D_H
#define WGRIDREGULAR3D_H

#include "../math/WPosition.h"
#include "../math/WVector3D.h"
#include "../math/WMatrix.hpp"

#include <osg/Vec3>

#include "WGrid.h"

/**
 * A grid that has parallelepiped cells which all have the same proportion. I.e.
 * the samples along a single axis are equidistant. The distance of samples may
 * vary between axes.
 * \ingroup dataHandler
 */
class WGridRegular3D : public WGrid
{
    /**
     * Only test are allowed as friends.
     */
    friend class WGridRegular3DTest;
public:
    /**
     * Defines the position of the origin of the grid, the number of
     * samples in each coordinate direction and the offset between the
     * samples in the different coordinate directions as vector.
     */
    WGridRegular3D(
                   unsigned int nbPosX, unsigned int nbPosY, unsigned int nbPosZ,
                   double originX, double originY, double originZ,
                   const wmath::WVector3D& directionX,
                   const wmath::WVector3D& directionY,
                   const wmath::WVector3D& directionZ,
                   double offsetX, double offsetY, double offsetZ );

    /**
     * Defines the number of samples in each coordinate direction as ints,
     * and the position of the origin of the grid and the offset between the
     * samples in the different coordinate directions as one 4x4 transformation
     * matrix using homogeneous coordinates (but only affine transformations are
     * allowed).
     */
    WGridRegular3D(
                   unsigned int nbPosX, unsigned int nbPosY, unsigned int nbPosZ,
                   const wmath::WMatrix< double >& mat,
                   double offsetX, double offsetY, double offsetZ );

    /**
     * Defines the position of the origin of the grid, the number of
     * samples in each coordinate direction and the offset between the
     * samples in the different coordinate directions as scalar.
     */
    WGridRegular3D(
                   unsigned int nbPosX, unsigned int nbPosY, unsigned int nbPosZ,
                   double originX, double originY, double originZ,
                   double offsetX, double offsetY, double offsetZ );

    /**
     * Convenience constructor that does the same as the one above but
     * uses the origin (0,0,0) as default.
     */
    WGridRegular3D(
                   unsigned int nbPosX, unsigned int nbPosY, unsigned int nbPosZ,
                   double offsetX, double offsetY, double offsetZ );

    /**
     * Returns the number of samples in x direction.
     */
    unsigned int getNbCoordsX() const
    {
        return m_nbPosX;
    }
    /**
     * Returns the number of samples in y direction.
     */
    unsigned int getNbCoordsY() const
    {
        return m_nbPosY;
    }
    /**
     * Returns the number of samples in z direction.
     */
    unsigned int getNbCoordsZ() const
    {
        return m_nbPosZ;
    }

    /**
     * Returns the distance between samples in x direction.
     */
    double getOffsetX() const
    {
        return m_directionX.norm();
    }
    /**
     * Returns the distance between samples in y direction.
     */
    double getOffsetY() const
    {
        return m_directionY.norm();
    }
    /**
     * Returns the distance between samples in z direction.
     */
    double getOffsetZ() const
    {
        return m_directionZ.norm();
    }

    /**
     * Returns the vector determining the direction of samples in x direction.
     */
    const wmath::WVector3D& getDirectionX() const
    {
        return m_directionX;
    }
    /**
     * Returns the vector determining the direction of samples in y direction.
     */
    const wmath::WVector3D& getDirectionY() const
    {
        return m_directionY;
    }
    /**
     * Returns the vector determining the direction of samples in z direction.
     */
    const wmath::WVector3D& getDirectionZ() const
    {
        return m_directionZ;
    }

    /**
     * Returns the position of the origin of the grid.
     */
    wmath::WPosition getOrigin() const
    {
        return m_origin;
    }

    /**
     * Returns the i-th position on the grid.
     */
    wmath::WPosition getPosition( unsigned int i ) const;

    /**
     * Returns the position that is the iX-th in x direction, the iY-th in
     * y direction and the iZ-th in z direction.
     */
    wmath::WPosition getPosition( unsigned int iX, unsigned int iY, unsigned int iZ ) const;

    /**
     * Return the matrix storing the transformation of the grid. This information is redundant.
     * Please use m_origin and m_direction? for all normal computations.
     * Use matrix only where you really need a matrix for multiplication.
     */
    wmath::WMatrix<double> getTransformationMatrix() const;

    /**
     *
     */
    wmath::WVector3D multMatrixWithVector3D( wmath::WMatrix<double> mat, wmath::WVector3D vec );

    /**
     *
     */
    wmath::WMatrix<double>  invertMatrix3x3( wmath::WMatrix<double> mat );

    /**
     *
     */
    osg::Vec3 transformTexCoord( osg::Vec3 point );

protected:
private:
    wmath::WPosition m_origin;

    unsigned int m_nbPosX;
    unsigned int m_nbPosY;
    unsigned int m_nbPosZ;

    wmath::WVector3D m_directionX;
    wmath::WVector3D m_directionY;
    wmath::WVector3D m_directionZ;

    double m_offsetX;
    double m_offsetY;
    double m_offsetZ;

    /**
     * Matrix storing the transformation of the grid. This is redundant.
     * Please use m_origin and m_direction? for all normal computations.
     * Use matrix only where you really need a matrix for multiplication.
     */
    wmath::WMatrix<double> m_matrix;

    wmath::WMatrix<double> m_matrixInverse;
};

#endif  // WGRIDREGULAR3D_H
