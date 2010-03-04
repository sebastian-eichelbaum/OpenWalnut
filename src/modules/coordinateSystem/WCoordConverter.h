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

#ifndef WCOORDCONVERTER_H
#define WCOORDCONVERTER_H

#include "../../common/math/WVector3D.h"
#include "../../common/math/WMatrix.h"

using wmath::WMatrix;
using wmath::WVector3D;

/**
 * Class to provide conversion for a given coordinate in 3D space
 */
class WCoordConverter
{
public:
    /**
     * standard constructor
     *
     * \param rotMat rotation matrix
     * \param origin offset to point of origin
     * \param scale voxel size
     */
    WCoordConverter( WMatrix<double> rotMat, WVector3D origin = WVector3D( 0, 0 , 0 ), WVector3D scale = WVector3D( 1.0, 1.0 , 1.0 ) );

    /**
     * destructor
     */
    virtual ~WCoordConverter();

    /**
     * Operator ()
     *
     * \param in vector to convert
     *
     * \return converted vector
     */
    WVector3D operator()( WVector3D in );

protected:
private:
    /**
     * the rotation matrix to use
     */
    wmath::WMatrix<double> m_rotMat;

    /**
     * the offset to the point of origin
     */
    wmath::WVector3D m_origin;

    /**
     * scaling of voxels
     */
    wmath::WVector3D m_scale;
};

#endif  // WCOORDCONVERTER_H
