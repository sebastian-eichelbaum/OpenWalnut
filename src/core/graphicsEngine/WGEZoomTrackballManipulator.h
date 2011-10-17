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
#include "WExportWGE.h"

/**
 * New OSG manipulator: TrackballManipulator with added mouse wheel zoom.
 */
class WGE_EXPORT WGEZoomTrackballManipulator: public osgGA::TrackballManipulator
{
public:
    /**
     * Default constructor
     */
    WGEZoomTrackballManipulator();

    /**
     * Set the position of the manipulator using a 4x4 matrix.
     * \param matrix position of the manipulator
     */
    virtual void setByMatrix( const osg::Matrixd& matrix );

    /**
     * Get the position of the manipulator as 4x4 matrix.
     *
     * \return the matrix representing the transformation
     */
    virtual osg::Matrixd getMatrix() const;

    /**
     * Get the manipulator only containing rotation and translation.
     *
     * \return the matrix with translation and rotation.
     */
    virtual osg::Matrixd getMatrixWithoutZoom() const;

    /**
     * Get the position of the manipulator as a inverse matrix of the
     * manipulator, typically used as a model view matrix.
     *
     * \return the inverse matrix
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
     * \param ea Event class for storing Keyboard, mouse and window events.
     * \param us Interface by which GUIEventHandlers may request actions of the GUI system
     *
     * \return true if event was handled
     */
    virtual bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );

    /**
     * Get zoom factor.
     *
     * \return the current zoom factor
     */
    double getZoom() const;

    /**
     * Set zoom factor.
     * \param zoom the zoom factor to be set.
     */
    void setZoom( double zoom );

    /**
     * setter for paint mode
     * when set to something different from 0, a left drag should move the scene
     *
     * \param mode the mode
     */
    void setPaintMode( int mode );

    /**
     * En-/Disables throwing.
     *
     * \param allowThrow if true, throwing is enabled.
     */
    void setThrow( bool allowThrow = true );

    /**
     * Checks whether throwing is active.
     *
     * \return true if active
     */
    bool getThrow() const;

protected:
private:

    /**
     * Handles events related to zooming.
     * \param ea Event class for storing Keyboard, mouse and window events.
     * \param us Interface by which GUIEventHandlers may request actions of the GUI system
     *
     * \return true if event was handled
     */
    bool zoom( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );

    double m_zoom; //!< Zoom factor.
    bool m_allowThrow; //!< Do we want the auto-rotation thingy?

    int m_paintMode; //!<paint mode
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
