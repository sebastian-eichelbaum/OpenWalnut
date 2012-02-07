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

#ifndef WGE2DMANIPULATOR_H
#define WGE2DMANIPULATOR_H

#include <osg/Version>

// OSG interface changed in 2.9.7, to make it compile also with those versions we do this:
// OSG_MIN_VERSION_REQUIRED(2, 9, 8) macro is not available in e.g. OSG 2.8.1, hence we use the old way
#if ( ( OPENSCENEGRAPH_MAJOR_VERSION > 2 ) || ( OPENSCENEGRAPH_MAJOR_VERSION == 2 && ( OPENSCENEGRAPH_MINOR_VERSION > 9 || \
                            ( OPENSCENEGRAPH_MINOR_VERSION == 9 && OPENSCENEGRAPH_PATCH_VERSION >= 8 ) ) ) )
    #include <osgGA/CameraManipulator>
    namespace osgGA
    {
        typedef CameraManipulator MatrixManipulator;
    }
#else
    #include <osgGA/MatrixManipulator>
#endif



/**
 * A manipulator which changes the view of a 2D scene. Does things like panning
 * and zooming.
 */
class WGE2DManipulator : public osgGA::MatrixManipulator
{
public:
    /**
     * Constructor
     */
    WGE2DManipulator();

    /**
     * Return the name of the object's class type.
     *
     * \return the name of the object's class type
     */
    virtual const char* className() const;

    /**
     * Set the position of the matrix manipulator using a 4x4 matrix.
     *
     * \param matrix a 4x4 matrix
     */
    virtual void setByMatrix( const osg::Matrixd& matrix );

    /**
     * Set the position of the matrix manipulator using a 4x4 matrix.
     *
     * \param matrix a 4x4 matrix
     */
    virtual void setByInverseMatrix( const osg::Matrixd& matrix );

    /**
     * Get the position of the manipulator as 4x4 matrix.
     *
     * \return the position of the manipulator as 4x4 matrix
     */
    virtual osg::Matrixd getMatrix() const;

    /**
     * Get the position of the manipulator as a inverse matrix of the
     * manipulator, typically used as a model view matrix.
     *
     * \return the position of the manipulator as a inverse matrix
     */
    virtual osg::Matrixd getInverseMatrix() const;

    /**
     * Move the camera to the default position.
     *
     * \param us the action adapter used to request actions of the GUI
     */
    virtual void home( const osgGA::GUIEventAdapter& /*ea*/, osgGA::GUIActionAdapter& us ); // NOLINT We can not change the interface of OSG

    /**
     * Start/restart the manipulator.
     *
     * \param us the action adapter used to request actions of the GUI
     */
    virtual void init( const osgGA::GUIEventAdapter& /*ea*/, osgGA::GUIActionAdapter& us ); // NOLINT We can not change the interface of OSG

    /**
     * Handle events
     *
     * \param ea event class for storing keyboard, mouse and window events
     * \param us the action adapter used to request actions of the GUI
     * \return true if handled, false otherwise
     */
    virtual bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );

    /**
     * Get the keyboard and mouse usage of this manipulator.
     *
     * \param usage the application usage
     */
    virtual void getUsage( osg::ApplicationUsage& usage ) const; // NOLINT We can not change the interface of OSG

protected:
    /**
     * Destructor
     *
     * Note, is protected so that objects cannot be deleted other than by being
     * dereferenced and the reference count being zero (see osg::Referenced),
     * preventing the deletion of objects which are still in use.
     */
    virtual ~WGE2DManipulator();

    /**
     * Reset the internal GUIEvent stack.
     */
    void flushMouseEventStack();

    /**
     * Add the current mouse GUIEvent to the internal stack.
     *
     * \param ea the current event class with a mouse event
     */
    void addMouseEvent( const osgGA::GUIEventAdapter& ea );

    /**
     * Calculate the movement of the camera for the given mouse movement.
     *
     * \return true is camera has moved and a redraw is required
     */
    bool calcMovement();

    /**
     * The older event from the internal event stack.
     */
    osg::ref_ptr< const osgGA::GUIEventAdapter > m_ga_t1;

    /**
     * The newer event from the internal event stack.
     */
    osg::ref_ptr< const osgGA::GUIEventAdapter > m_ga_t0;

private:
    /**
     * Handles events related to zooming.
     *
     * \param ea event class for storing keyboard, mouse and window events
     *
     * \return true if event was handled
     */
    bool zoom( const osgGA::GUIEventAdapter& ea );

    /**
     * The x-position of the viewing window's lower left corner.
     */
    double m_positionX;

    /**
     * The y-position of the viewing window's lower left corner.
     */
    double m_positionY;

    /**
     * zoom factor
     */
    double m_zoom;
};

#endif  // WGE2DMANIPULATOR_H
