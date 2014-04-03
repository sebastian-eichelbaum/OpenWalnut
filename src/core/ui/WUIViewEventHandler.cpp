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

#include "../common/WLogger.h"
#include "WUIViewEventHandler.h"


WUIViewEventHandler::WUIViewEventHandler( WUIView::SPtr widget )
    : m_widget( widget ),
      m_preselection( GUIEvents::NONE )
{
}

bool WUIViewEventHandler::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& /* aa */ )
{
    GUIEvents::EventType et = ea.getEventType();
    if( et & m_preselection )
    {
        switch( et )
        {
            case( GUIEvents::NONE ) :
            {
                break; // Nothing todo for NONE events
            }
            case( GUIEvents::PUSH ):
            {
                m_sigPush( WVector2f( ea.getX(), ea.getY() ), ea.getButton() );
                handlePush( WVector2f( ea.getX(), ea.getY() ), ea.getButton() );
                break;
            }
            case( GUIEvents::RELEASE ):
            {
                m_sigRelease( WVector2f( ea.getX(), ea.getY() ), ea.getButton() );
                handleRelease( WVector2f( ea.getX(), ea.getY() ), ea.getButton() );
                break;
            }
            case( GUIEvents::DOUBLECLICK ):
            {
                m_sigDoubleclick( WVector2f( ea.getX(), ea.getY() ), ea.getButton() );
                handleDoubleclick( WVector2f( ea.getX(), ea.getY() ), ea.getButton() );
                break;
            }
            case( GUIEvents::DRAG ):
            {
                m_sigDrag( WVector2f( ea.getX(), ea.getY() ), ea.getButtonMask() );
                handleDrag( WVector2f( ea.getX(), ea.getY() ), ea.getButtonMask() );
                break;
            }
            case( GUIEvents::MOVE ):
            {
                m_sigMove( WVector2f( ea.getX(), ea.getY() ) );
                handleMove( WVector2f( ea.getX(), ea.getY() ) );
                break;
            }
            case( GUIEvents::KEYDOWN ):
            {
                m_sigKeydown( ea.getKey(), ea.getModKeyMask() );
                handleKeydown( ea.getKey(), ea.getModKeyMask() );
                break;
            }
            case( GUIEvents::KEYUP ):
            {
                m_sigKeyup( ea.getKey(), ea.getModKeyMask() );
                handleKeyup( ea.getKey(), ea.getModKeyMask() );
                break;
            }
            case( GUIEvents::FRAME ): // every frame triggered!
            {
                m_sigFrame();
                handleFrame();
                break;
            }
            case( GUIEvents::RESIZE ):
            {
                m_sigResize( ea.getWindowX(), ea.getWindowY(), ea.getWindowWidth(), ea.getWindowHeight() );
                handleResize( ea.getWindowX(), ea.getWindowY(), ea.getWindowWidth(), ea.getWindowHeight() );
                break;
            }
            case( GUIEvents::SCROLL ):
            {
                m_sigScroll( ea.getScrollingMotion(), ea.getScrollingDeltaX(), ea.getScrollingDeltaY() );
                handleScroll( ea.getScrollingMotion(), ea.getScrollingDeltaX(), ea.getScrollingDeltaY() );
                break;
            }
            case( GUIEvents::PEN_PRESSURE ):
            {
                m_sigPenPressure( ea.getPenPressure() );
                handlePenPressure( ea.getPenPressure() );
                break;
            }
            case( GUIEvents::PEN_ORIENTATION ):
            {
                m_sigPenOrientation( ea.getPenOrientation() );
                handlePenOrientation( ea.getPenOrientation() );
                break;
            }
            case( GUIEvents::PEN_PROXIMITY_ENTER ):
            {
                m_sigPenProximityEnter();
                handlePenProximityEnter();
                break;
            }
            case( GUIEvents::PEN_PROXIMITY_LEAVE ):
            {
                m_sigPenProximityLeave();
                handlePenProximityLeave();
                break;
            }
            case( GUIEvents::CLOSE_WINDOW ):
            {
                m_sigCloseWindow();
                handleCloseWindow();
                break;
            }
            case( GUIEvents::QUIT_APPLICATION ):
            {
                m_sigQuitApplication();
                handleQuitApplication();
                break;
            }
            case( GUIEvents::USER ):
            {
                m_sigUser();
                handleUser();
                break;
            }
            default:
                errorLog() << "Unknown GUI Event: " << et;
                return false;
        }

        return true;
    }

    return false; // There was no subscription to this event
}

wlog::WStreamedLogger WUIViewEventHandler::errorLog() const
{
    return wlog::error( "CustomViewEventHandler" ) << m_widget->getTitle() << ": ";
}

void WUIViewEventHandler::subscribePush( ButtonSignalType::slot_type slot )
{
    m_preselection |= GUIEvents::PUSH;
    m_sigPush.connect( slot );
}

void WUIViewEventHandler::subscribeRelease( ButtonSignalType::slot_type slot )
{
    m_preselection |= GUIEvents::RELEASE;
    m_sigRelease.connect( slot );
}

void WUIViewEventHandler::subscribeDoubleclick( ButtonSignalType::slot_type slot )
{
    m_preselection |= GUIEvents::DOUBLECLICK;
    m_sigDoubleclick.connect( slot );
}

void WUIViewEventHandler::subscribeDrag( DragSignalType::slot_type slot )
{
    m_preselection |= GUIEvents::DRAG;
    m_sigDrag.connect( slot );
}

void WUIViewEventHandler::subscribeMove( MoveSignalType::slot_type slot )
{
    m_preselection |= GUIEvents::MOVE;
    m_sigMove.connect( slot );
}

void WUIViewEventHandler::subscribeFrame( TriggerSignalType::slot_type slot )
{
    m_preselection |= GUIEvents::FRAME;
    m_sigFrame.connect( slot );
}

void WUIViewEventHandler::subscribeKeydown( KeySignalType::slot_type slot )
{
    m_preselection |= GUIEvents::KEYDOWN;
    m_sigKeydown.connect( slot );
}

void WUIViewEventHandler::subscribeKeyup( KeySignalType::slot_type slot )
{
    m_preselection |= GUIEvents::KEYUP;
    m_sigKeyup.connect( slot );
}

void WUIViewEventHandler::subscribeResize( ResizeSignalType::slot_type slot )
{
    m_preselection |= GUIEvents::RESIZE;
    m_sigResize.connect( slot );
}

void WUIViewEventHandler::subscribeScroll( ScrollSignalType::slot_type slot )
{
    m_preselection |= GUIEvents::SCROLL;
    m_sigScroll.connect( slot );
}

void WUIViewEventHandler::subscribePenPressure( PenPressureSignalType::slot_type slot )
{
    m_preselection |= GUIEvents::PEN_PRESSURE;
    m_sigPenPressure.connect( slot );
}

void WUIViewEventHandler::subscribePenOrientation( PenOrientationSignalType::slot_type slot )
{
    m_preselection |= GUIEvents::PEN_ORIENTATION;
    m_sigPenOrientation.connect( slot );
}

void WUIViewEventHandler::subscribePenProximityEnter( TriggerSignalType::slot_type slot )
{
    m_preselection |= GUIEvents::PEN_PROXIMITY_ENTER;
    m_sigPenProximityEnter.connect( slot );
}

void WUIViewEventHandler::subscribePenProximityLeave( TriggerSignalType::slot_type slot )
{
    m_preselection |= GUIEvents::PEN_PROXIMITY_LEAVE;
    m_sigPenProximityLeave.connect( slot );
}

void WUIViewEventHandler::subscribeCloseWindow( TriggerSignalType::slot_type slot )
{
    m_preselection |= GUIEvents::CLOSE_WINDOW;
    m_sigCloseWindow.connect( slot );
}

void WUIViewEventHandler::subscribeQuitApplication( TriggerSignalType::slot_type slot )
{
    m_preselection |= GUIEvents::QUIT_APPLICATION;
    m_sigQuitApplication.connect( slot );
}

void WUIViewEventHandler::subscribeUser( TriggerSignalType::slot_type slot )
{
    m_preselection |= GUIEvents::USER;
    m_sigUser.connect( slot );
}

void WUIViewEventHandler::handlePush( WVector2f /* mousePos */, int /* button */ )
{
}

void WUIViewEventHandler::handleRelease( WVector2f /* mousePos */, int /* button */ )
{
}

void WUIViewEventHandler::handleDoubleclick( WVector2f /* mousePos */, int /* button */ )
{
}

void WUIViewEventHandler::handleDrag( WVector2f /* mousePos */, int /* buttonMask */ )
{
}

void WUIViewEventHandler::handleMove( WVector2f /* mousePos */ )
{
}

void WUIViewEventHandler::handleKeydown( int /* keyID */, unsigned int /* modKeyMask */ )
{
}

void WUIViewEventHandler::handleKeyup( int /* keyID */, unsigned int /* modKeyMask */ )
{
}

void WUIViewEventHandler::handleFrame()
{
}

void WUIViewEventHandler::handleResize( int /* xPos */, int /* yPos */, int /* width */, int /* height */ )
{
}

void WUIViewEventHandler::handleScroll( GUIEvents::ScrollingMotion /* motion */, float /* deltaX */, float /* deltaY */ )
{
}

void WUIViewEventHandler::handlePenPressure( float /* pressure */ )
{
}

void WUIViewEventHandler::handlePenOrientation( const osg::Matrix /* orientation */ )
{
}

void WUIViewEventHandler::handlePenProximityEnter()
{
}

void WUIViewEventHandler::handlePenProximityLeave()
{
}

void WUIViewEventHandler::handleCloseWindow()
{
}

void WUIViewEventHandler::handleQuitApplication()
{
}

void WUIViewEventHandler::handleUser()
{
}
