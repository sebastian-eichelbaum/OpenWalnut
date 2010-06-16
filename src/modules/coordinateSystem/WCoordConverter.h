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

#include "../../common/math/WMatrix.h"
#include "../../common/math/WPosition.h"
#include "../../common/math/WVector3D.h"

#include "WTalairachConverter.h"

using wmath::WMatrix;
using wmath::WVector3D;

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

    /**
     * Transforms world coordinates.
     * \param point The point which will be transformed.
     */
    wmath::WVector3D worldCoordTransformed( wmath::WPosition point );

    /**
     * getter for bounding box
     *
     * \return the bounding box
     */
    std::pair< wmath::WPosition, wmath::WPosition > getBoundingBox();

    /**
     * setter for boundign box of the volume
     *
     * \param boundingBox
     */
    void setBoundingBox( std::pair< wmath::WPosition, wmath::WPosition > boundingBox );

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
     * \return WVector3D the transformed vector
     */
    WVector3D w2c( WVector3D in );

    /**
     * transforms a vector from the  canonical to the world coordinate system system
     * \param in vector to transform
     * \return WVector3D the transformed vector
     */
    WVector3D c2w( WVector3D in );

    /**
     * transforms a vector from the world coordinate system to the talairach system
     * \param in vector to transform
     * \return WVector3D the transformed vector
     */
    WVector3D w2t( WVector3D in );

    /**
     * transforms a vector from the canonical system to the world coordinate system
     * \param in vector to transform
     * \return WVector3D the transformed vector
     */
    WVector3D t2w( WVector3D in );

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
    wmath::WMatrix<double> m_rotMat; //!< the rotation matrix to use

    wmath::WVector3D m_origin; //!< the offset to the point of origin

    wmath::WVector3D m_scale; //!< scaling of voxels

    std::pair< wmath::WPosition, wmath::WPosition >m_boundingBox; //!< bounding box of the wholy volume as provided by the dataset

    coordinateSystemMode m_coordinateSystemMode; //!< the currently selected coordinate system mode

    boost::shared_ptr<WTalairachConverter>m_talairachConverter; //!< pointer to talairach convert for easy access
};

#endif  // WCOORDCONVERTER_H
