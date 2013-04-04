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

#include <utility>

#include "core/common/WBoundingBox.h"
#include "core/common/math/WMatrix.h"
#include "core/common/math/linearAlgebra/WPosition.h"

#include "WTalairachConverter.h"


typedef enum
{
    CS_WORLD = 0,
    CS_CANONICAL,
    CS_TALAIRACH
}
coordinateSystemMode;

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
    WCoordConverter( WMatrix<double> rotMat, WVector3d origin = WVector3d( 0, 0 , 0 ), WVector3d scale = WVector3d( 1.0, 1.0 , 1.0 ) );

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
    WVector3d operator()( WVector3d in );

    /**
     * Transforms world coordinates.
     * \param point The point which will be transformed.
     *
     * \return the transformed world coordinate
     */
    WVector3d worldCoordTransformed( WPosition point );

    /**
     * Gets the bounding box
     *
     * \return The axis aligned bounding box
     */
    WBoundingBox getBoundingBox();

    /**
     * Sets the bounding box of the volume.
     *
     * \param boundingBox The axis aligned bounding box.
     */
    void setBoundingBox( WBoundingBox boundingBox );

    /**
     * setter for coordinate system mode
     *
     * \param mode
     */
    void setCoordinateSystemMode( coordinateSystemMode mode );

    /**
     * getter for coordinate system mode
     *
     * \return the mode
     */
    coordinateSystemMode getCoordinateSystemMode();

    /**
     * converts a number on the x axis according to the currently selected coordinate system
     * \param number
     * \return the number transformed
     */
    int numberToCsX( int number );

    /**
     * converts a number on the y axis according to the currently selected coordinate system
     * \param number
     * \return the number transformed
     */
    int numberToCsY( int number );

    /**
     * converts a number on the z axis according to the currently selected coordinate system
     * \param number
     * \return the number transformed
     */
    int numberToCsZ( int number );

    /**
     * transforms a vector from the world coordinate system to the canonical system
     * \param in vector to transform
     * \return WVector3d the transformed vector
     */
    WVector3d w2c( WVector3d in );

    /**
     * transforms a vector from the  canonical to the world coordinate system system
     * \param in vector to transform
     * \return WVector3d the transformed vector
     */
    WVector3d c2w( WVector3d in );

    /**
     * transforms a vector from the world coordinate system to the talairach system
     * \param in vector to transform
     * \return WVector3d the transformed vector
     */
    WVector3d w2t( WVector3d in );

    /**
     * transforms a vector from the canonical system to the world coordinate system
     * \param in vector to transform
     * \return WVector3d the transformed vector
     */
    WVector3d t2w( WVector3d in );

    /**
     * setter for the talairach converter pointer
     * \param tc
     */
    void setTalairachConverter( boost::shared_ptr<WTalairachConverter> tc );

    /**
     * getter for the talairach converter pointer
     * \return pointer
     */
    boost::shared_ptr<WTalairachConverter> getTalairachConverter();

protected:
private:
    WMatrix<double> m_rotMat; //!< the rotation matrix to use

    WVector3d m_origin; //!< the offset to the point of origin

    WVector3d m_scale; //!< scaling of voxels

    WBoundingBox m_boundingBox; //!< bounding box of the wholy volume as provided by the dataset

    coordinateSystemMode m_coordinateSystemMode; //!< the currently selected coordinate system mode

    boost::shared_ptr<WTalairachConverter>m_talairachConverter; //!< pointer to talairach convert for easy access
};

#endif  // WCOORDCONVERTER_H
