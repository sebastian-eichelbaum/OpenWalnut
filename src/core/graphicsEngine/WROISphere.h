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

#ifndef WROISPHERE_H
#define WROISPHERE_H

//#include <string>
#include <utility>

#include <boost/thread.hpp>

#include "../common/math/linearAlgebra/WLinearAlgebra.h"
#include "WPickHandler.h"
#include "WGEViewer.h"

#include "WROI.h"

#include "WExportWGE.h"

/**
 * A sphere representing a region of interest.
 */
class WGE_EXPORT WROISphere : public WROI
{
public:
    /**
     * Yields sphere with desired center point and radius
     * \param position position of the center of the sphere
     * \param radius radius of the sphere
     */
    WROISphere(  WPosition position, float radius = 5.0 );

    /**
     * standard destructor
     */
    virtual ~WROISphere();

    /**
     * getter
     * \return position
     */
    WPosition getPosition() const;

    /**
     * setter
     * \param position
     */
    void setPosition( WPosition position );

    /**
     * setter
     * \param x
     * \param y
     * \param z
     */
    void setPosition( float x, float y, float z );

    /**
     * Setter for standard color
     * \param color The new color.
     */
    void setColor( osg::Vec4 color );

    /**
     * Setter for color in negated state
     * \param color The new color.
     */
    void setNotColor( osg::Vec4 color );

    /**
     * removes the old drawable from the osg geode and adds a new one at the current position and size
     */
    void redrawSphere();

    /**
     * sets the flag that allows or disallows movement along the x axis
     *
     * \param value the flag
     */
    void setLockX( bool value = true );

    /**
     * sets the flag that allows or disallows movement along the y axis
     *
     * \param value the flag
     */
    void setLockY( bool value = true );

    /**
     * sets the flag that allows or disallows movement along the z axis
     *
     * \param value the flag
     */
    void setLockZ( bool value = true );

    /**
     * move the sphere with a given offset
     *
     * \param offset the distance to move
     */
    void moveSphere( WVector3d offset );

    /**
     * setter
     * \param x sets the x component of the position of this sphere
     */
    void setX( float x );

    /**
     * setter
     * \param y sets the y component of the position of this sphere
     */
    void setY( float y );

    /**
     * setter
     * \param z sets the z component of the position of this sphere
     */
    void setZ( float z );

    /**
     * setter
     * \param vector together witht he current position this sets line in space to which the movement of the
     * sphere is restricted
     */
    void  setLockVector( WVector3d vector );

    /**
     * setter
     * \param value if the the movement of the sphere is restricted to a given vector
     */
    void  setLockOnVector( bool value = true );

protected:

private:
    static size_t maxSphereId; //!< Current maximum boxId over all spheres.
    size_t sphereId; //!< Id of the current sphere.

    WPosition m_position; //!< The position of the sphere

    WPosition m_originalPosition; //!< The position of the sphere when created, used for locking

    float m_radius; //!< The radius  of the sphere

    bool m_isPicked; //!< Indicates whether the box is currently picked or not.

    WPosition m_pickedPosition; //!< Caches the old picked position to a allow for comparison

    WVector3d m_pickNormal; //!< Store the normal that occured when the pick action was started.

    WVector2d m_oldPixelPosition; //!< Caches the old picked position to a allow for cmoparison

    WPickInfo m_pickInfo; //!< Stores the pick information for potential redraw

    boost::shared_ptr< WGEViewer > m_viewer; //!< makes viewer available all over this class.

    osg::Vec4 m_color; //!< the color of the box

    osg::Vec4 m_notColor; //!< the color of the box when negated

    WVector3d m_lockPoint; //!< stores to point of origin of the lock vector

    WVector3d m_lockVector; //!< stores the lock vector

    bool m_lockOnVector; //!< flag indicatin wether the movement of the sphere is restricted

    bool m_lockX; //!< flag indicatin wether the movement of the sphere is restricted
    bool m_lockY; //!< flag indicatin wether the movement of the sphere is restricted
    bool m_lockZ; //!< flag indicatin wether the movement of the sphere is restricted


    /**
     * note that there was a pick
     * \param pickInfo info from pick
     */
    void registerRedrawRequest( WPickInfo pickInfo );

    /**
     *  updates the graphics
     */
    virtual void updateGFX();
};

#endif  // WROISPHERE_H
