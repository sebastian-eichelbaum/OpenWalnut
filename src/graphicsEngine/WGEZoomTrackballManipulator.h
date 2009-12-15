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

#ifndef WGEZOOMTRACKBALLMANIPULATOR_H
#define WGEZOOMTRACKBALLMANIPULATOR_H

#include <osgGA/TrackballManipulator>

/**
 * New OSG manipulator: TrackballManipulator with added mouse wheel zoom.
 */
class WGEZoomTrackballManipulator: public osgGA::TrackballManipulator
{
public:
    /**
     * Default constructor
     */
    WGEZoomTrackballManipulator();

    /**
     * Set the position of the matrix manipulator using a 4x4 matrix.
     */
    virtual void setByMatrix( const osg::Matrixd& matrix );

    /**
     * Get the position of the manipulator as 4x4 matrix.
     */
    virtual osg::Matrixd getMatrix() const;

    /**
     * Get the position of the manipulator as a inverse matrix of the
     * manipulator, typically used as a model view matrix.
     */
    virtual osg::Matrixd getInverseMatrix() const;

    /**
     * Move the camera to the default position.
     * May be ignored by manipulators if home functionality is not appropriate.
     * \param currentTime Variable specified in osg class but not used.
     */
    virtual void home( double currentTime );

    /**
     * Handle events, return true if handled, false otherwise.
     */
    virtual bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );

    /**
     * Get zoom factor.
     */
    double getZoom() const;

    /**
     * Set zoom factor.
     */
    void setZoom( double zoom );

protected:
private:

    /**
     * Handles events related to zooming.
     */
    bool zoom( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );

    double m_zoom; //!< Zoom factor.
};

inline double WGEZoomTrackballManipulator::getZoom() const
{
    return m_zoom;
}

inline void WGEZoomTrackballManipulator::setZoom( double zoom )
{
    m_zoom = zoom;
}

#endif  // WGEZOOMTRACKBALLMANIPULATOR_H
