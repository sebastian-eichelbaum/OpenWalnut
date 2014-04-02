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

#ifndef WCUSTOMVIEWEVENTHANDLER_H
#define WCUSTOMVIEWEVENTHANDLER_H

#include <boost/signals2/signal.hpp>

#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIEventHandler>

#include "../common/math/linearAlgebra/WVectorFixed.h"
#include "../common/WLogger.h"
#include "WCustomView.h"

/**
 * An event handler for a custom widget which eases interaction with GUIEvents within your module. Without you need to write your
 * own event handler and register it. However if you still need your own event handler you might consider subclassing from this
 * one. Basically there are two ways of getting notified of GUIEvents. First you can connect a member function to events or you
 * can overwrite member functions of this class to handle specific events.
 *
 * Use boost::bind and the subscribeXY methods to connect a member function of your module to a specific GUIEvent. Please note
 * that this is now called in context of the GUI thread and that you need to take care of threadsafety by yourself.
 *
 * Use the corresponding handleXY() member functions if you still need a custom event handler. But take care that you may need to
 * set the WCustomViewEventHandler::m_preselection event-mask accordingly within the constructor then.
 *
 * In case you might not know what the specific parameters of the handle function represent you might have luck looking into the
 * OpenSceneGraph documentation http://www.openscenegraph.org/documentation/OpenSceneGraphReferenceDocs within the GUIEventAdapter
 * class.
 */
class WCustomViewEventHandler : public osgGA::GUIEventHandler
{
public:
    /**
     * Constructor.
     *
     * \param widget The custom widget for which events should be handled.
     */
    explicit WCustomViewEventHandler( WCustomView::SPtr widget );

    /**
     * The OSG calls this function whenever a new event has occured.
     *
     * \param ea Event class for storing GUI events such as mouse or keyboard interation etc.
     *
     * \return true if the event was handled.
     */
    bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& /* aa */ );

    /**
     * Short hand type for signal signature of PUSH, RELEASE and DOUBLECLICK event.
     */
    typedef boost::signals2::signal< void ( WVector2f, int ) >                           ButtonSignalType;

    /**
     * Short hand type for signal signature of DRAG event.
     */
    typedef boost::signals2::signal< void ( WVector2f, unsigned int ) >                  DragSignalType;

    /**
     * Short hand type for signal signature of MOVE event.
     */
    typedef boost::signals2::signal< void ( WVector2f ) >                                MoveSignalType;

    /**
     * Short hand type for signal signature of SCROLL event.
     */
    typedef boost::signals2::signal< void ( GUIEvents::ScrollingMotion, float, float ) > ScrollSignalType;

    /**
     * Short hand type for signal signature of FRAME, PEN_PROXIMITY_ENTER, -LEAVE, CLOSE_WINDOW, QUIT_APPLICATION and USER event.
     */
    typedef boost::signals2::signal< void ( void ) >                                     TriggerSignalType;

    /**
     * Short hand type for signal signature of KEYDOWN and KEYUP event.
     */
    typedef boost::signals2::signal< void ( int, unsigned int ) >                        KeySignalType;

    /**
     * Short hand type for signal signature of RESIZE event.
     */
    typedef boost::signals2::signal< void ( int, int, int, int ) >                       ResizeSignalType;

    /**
     * Short hand type for signal signature of PEN_PRESSURE event.
     */
    typedef boost::signals2::signal< void ( float ) >                                    PenPressureSignalType;

    /**
     * Short hand type for signal signature of PEN_ORIENTATION event.
     */
    typedef boost::signals2::signal< void ( const osg::Matrix ) >                        PenOrientationSignalType;

    /**
     * Registers a function slot to PUSH events. Whenever the event occurs, the slot is called with current parameters.
     *
     * \param slot Function object having the appropriate signature according to the used SignalType.
     */
    virtual void subscribePush( ButtonSignalType::slot_type slot );

    /**
     * Registers a function slot to RELEASE events. Whenever the event occurs, the slot is called with current parameters.
     * \copydetails subscribePush()
     */
    virtual void subscribeRelease( ButtonSignalType::slot_type slot );

    /**
     * Registers a function slot to DOUBLECLICK events.
     * \copydetails subscribePush()
     */
    virtual void subscribeDoubleclick( ButtonSignalType::slot_type slot );

    /**
     * Registers a function slot to DRAG events.
     * \copydetails subscribePush()
     */
    virtual void subscribeDrag( DragSignalType::slot_type slot );

    /**
     * Registers a function slot to MOVE events.
     *
     * \copydetails subscribePush()
     */
    virtual void subscribeMove( MoveSignalType::slot_type slot );

    /**
     * Registers a function slot to KEYDOWN events.
     *
     * \copydetails subscribePush()
     */
    virtual void subscribeKeydown( KeySignalType::slot_type slot );

    /**
     * Registers a function slot to KEYUP events.
     *
     * \copydetails subscribePush()
     */
    virtual void subscribeKeyup( KeySignalType::slot_type slot );

    /**
     * Registers a function slot to FRAME events.
     *
     * \copydetails subscribePush()
     */
    virtual void subscribeFrame( TriggerSignalType::slot_type );

    /**
     * Registers a function slot to RESIZE events.
     *
     * \copydetails subscribePush()
     */
    virtual void subscribeResize( ResizeSignalType::slot_type slot );

    /**
     * Registers a function slot to SCROLL events.
     *
     * \copydetails subscribePush()
     */
    virtual void subscribeScroll( ScrollSignalType::slot_type slot );

    /**
     * Registers a function slot to PEN_PRESSURE events.
     *
     * \copydetails subscribePush()
     */
    virtual void subscribePenPressure( PenPressureSignalType::slot_type slot );

    /**
     * Registers a function slot to PEN_ORIENTATION events.
     *
     * \copydetails subscribePush()
     */
    virtual void subscribePenOrientation( PenOrientationSignalType::slot_type slot );

    /**
     * Registers a function slot to PEN_PROXIMITY_ENTER events.
     *
     * \copydetails subscribePush()
     */
    virtual void subscribePenProximityEnter( TriggerSignalType::slot_type slot );

    /**
     * Registers a function slot to PEN_PROXIMITY_LEAVE events.
     *
     * \copydetails subscribePush()
     */
    virtual void subscribePenProximityLeave( TriggerSignalType::slot_type slot );

    /**
     * Registers a function slot to CLOSE_WINDOW events.
     *
     * \copydetails subscribePush()
     */
    virtual void subscribeCloseWindow( TriggerSignalType::slot_type slot );

    /**
     * Registers a function slot to QUIT_APPLICATION events.
     * \copydetails subscribePush()
     */
    virtual void subscribeQuitApplication( TriggerSignalType::slot_type slot );

    /**
     * Registers a function slot to USER events.
     * \copydetails subscribePush()
     */
    virtual void subscribeUser( TriggerSignalType::slot_type slot );

    /**
     * Called whenever the PUSH event occurs.
     *
     * \param mousePos Current mouse position in X and Y.
     * \param button The involved mouse button. See http://www.openscenegraph.org/documentation/OpenSceneGraphReferenceDocs
     * GUIEventAdapter class for values.
     */
    virtual void handlePush( WVector2f mousePos, int button );

    /**
     * Called whenever the RELEASE event occurs.
     * \copydetails handlePush()
     */
    virtual void handleRelease( WVector2f mousePos, int button );

    /**
     * Called whenever the DOUBLECLICK event occurs.
     * \copydetails handlePush()
     */
    virtual void handleDoubleclick( WVector2f mousePos, int button );

    /**
     * Called whenever the DRAG event occurs.
     *
     * \param mousePos Current mouse position in X and Y.
     * \param buttonMask The pushed mouse buttons as a mask. See http://www.openscenegraph.org/documentation/OpenSceneGraphReferenceDocs
     * GUIEventAdapter class for values.
     */
    virtual void handleDrag( WVector2f mousePos, int buttonMask );

    /**
     * Called whenever the MOVE event occurs.
     *
     * \param mousePos Current mouse position in X and Y.
     */
    virtual void handleMove( WVector2f mousePos );

    /**
     * Called whenever the KEYDOWN event occurs.
     *
     * \param keyID The pressed key. See http://www.openscenegraph.org/documentation/OpenSceneGraphReferenceDocs GUIEventAdapter
     * class for values.
     * \param modKeyMask Additional function keys pressed.
     */
    virtual void handleKeydown( int keyID, unsigned int modKeyMask );

    /**
     * Called whenever the KEYUP event occurs.
     *
     * \param keyID The released key. See http://www.openscenegraph.org/documentation/OpenSceneGraphReferenceDocs GUIEventAdapter
     * class for values.
     * \param modKeyMask Additional function keys pressed.
     */
    virtual void handleKeyup( int keyID, unsigned int modKeyMask );

    /**
     * Called whenever the FRAME event occurs. This is every new frame.
     */
    virtual void handleFrame();

    /**
     * Called whenever the widget has resized.
     *
     * \param xPos Position in X.
     * \param yPos Position in Y.
     * \param width Width of the widget.
     * \param height Height of the widget.
     */
    virtual void handleResize( int xPos, int yPos, int width, int height );

    /**
     * Called whenever the SCROLL event occurs.
     *
     * \param motion Scrolling motion. See http://www.openscenegraph.org/documentation/OpenSceneGraphReferenceDocs GUIEventAdapter class for values.
     * \param deltaX Delta in X.
     * \param deltaY Delta in Y.
     */
    virtual void handleScroll( GUIEvents::ScrollingMotion motion, float deltaX, float deltaY );

    /**
     * Called whenever the PEN_PRESSURE event occurs.
     *
     * \param pressure pressure of the pen.
     */
    virtual void handlePenPressure( float pressure );

    /**
     * Called whenever the PEN_ORIENTATION event occurs.
     *
     * \param orientation the orientation of the pen.
     */
    virtual void handlePenOrientation( const osg::Matrix orientation );

    /**
     * Called whenever the PEN_PROXIMITY_ENTER event occurs.
     */
    virtual void handlePenProximityEnter();

    /**
     * Called whenever the PEN_PROXIMITY_LEAVE event occurs.
     */
    virtual void handlePenProximityLeave();

    /**
     * Called whenever the CLOSE_WINDOW event occurs.
     */
    virtual void handleCloseWindow();

    /**
     * Called whenever the QUIT_APPLICATION event occurs.
     */
    virtual void handleQuitApplication();

    /**
     * Called whenever the USER event occurs.
     */
    virtual void handleUser();

protected:
    /**
     * Signal used for notification of the PUSH event.
     */
    ButtonSignalType         m_sigPush;

    /**
     * Signal used for notification of the RELEASE event.
     */
    ButtonSignalType         m_sigRelease;

    /**
     * Signal used for notification of the DOUBLECLICK event.
     */
    ButtonSignalType         m_sigDoubleclick;

    /**
     * Signal used for notification of the DRAG event.
     */
    DragSignalType           m_sigDrag;

    /**
     * Signal used for notification of the MOVE event.
     */
    MoveSignalType           m_sigMove;

    /**
     * Signal used for notification of the KEYDOWN event.
     */
    KeySignalType            m_sigKeydown;

    /**
     * Signal used for notification of the KEYUP event.
     */
    KeySignalType            m_sigKeyup;

    /**
     * Signal used for notification of the FRAME event.
     */
    TriggerSignalType        m_sigFrame;

    /**
     * Signal used for notification of the RESIZE event.
     */
    ResizeSignalType         m_sigResize;

    /**
     * Signal used for notification of the SCROLL event.
     */
    ScrollSignalType         m_sigScroll;

    /**
     * Signal used for notification of the PEN_PRESSURE event.
     */
    PenPressureSignalType    m_sigPenPressure;

    /**
     * Signal used for notification of the PEN_ORIENTATION event.
     */
    PenOrientationSignalType m_sigPenOrientation;

    /**
     * Signal used for notification of the PEN_PROXIMITY_ENTER event.
     */
    TriggerSignalType        m_sigPenProximityEnter;

    /**
     * Signal used for notification of the PEN_PROXIMITY_LEAVE event.
     */
    TriggerSignalType        m_sigPenProximityLeave;

    /**
     * Signal used for notification of the CLOSE_WINDOW event.
     */
    TriggerSignalType        m_sigCloseWindow;

    /**
     * Signal used for notification of the QUIT_APPLICATION event.
     */
    TriggerSignalType        m_sigQuitApplication;

    /**
     * Signal used for notification of the USER event.
     */
    TriggerSignalType        m_sigUser;

    /**
     * Reference to the WCustomView for which event handling should performed.
     */
    WCustomView::SPtr m_widget;

    /**
     * Binary mask describing which events should be used for notification or subscription.
     */
    unsigned int m_preselection;

    /**
     * Logger instance for comfortable error logging. Simply use errorLog() << "my error".
     *
     * \return the logger stream.
     */
    wlog::WStreamedLogger errorLog() const;
};

#endif  // WCUSTOMVIEWEVENTHANDLER_H
