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

#include <osgGA/GUIActionAdapter>
#include <osgGA/GUIEventAdapter>

#include "../../common/WPropertyTypes.h"
#include "../../common/WPropertyVariable.h"
#include "WEEGViewHandler.h"


WEEGViewHandler::WEEGViewHandler( WPropInt labelsWidth,
                                  WPropDouble timePos,
                                  WPropDouble timeRange,
                                  WPropInt graphWidth,
                                  WPropDouble yPos,
                                  WPropDouble ySpacing,
                                  WPropDouble ySensitivity )
    : m_labelsWidth( labelsWidth ),
      m_timePos( timePos ),
      m_timeRange( timeRange ),
      m_graphWidth( graphWidth ),
      m_yPos( yPos ),
      m_ySpacing( ySpacing ),
      m_ySensitivity( ySensitivity )
{
}

bool WEEGViewHandler::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& /*aa*/ )
{
    bool handled = false;

    switch( ea.getEventType() )
    {
        case osgGA::GUIEventAdapter::PUSH:
        {
            if( ea.getButton() == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON
                || ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON )
            {
                // save old mouse positions for panning and zooming
                m_oldX = ea.getX();
                m_oldY = ea.getY();

                handled = true;
            }

            break;
        }
        case osgGA::GUIEventAdapter::DRAG:
        {
            if( ea.getButtonMask() & ( osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON | osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON ) )
            {
                if( ea.getButtonMask() & osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON )
                {
                    // panning
                    m_timePos->set( m_timePos->get() - ( ea.getX() - m_oldX ) * m_timeRange->get() / m_graphWidth->get() );
                    m_yPos->set( m_yPos->get() - ( ea.getY() - m_oldY ) );
                }

                if( ea.getButtonMask() & osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON )
                {
                    // zooming in time
                    const int labelsWidth = m_labelsWidth->get();
                    if( labelsWidth < m_oldX && labelsWidth < ea.getX() )
                    {
                        m_timeRange->set( m_timeRange->get() * ( ( m_oldX - labelsWidth ) / ( ea.getX() - labelsWidth ) ) );
                    }
                }

                // save old mouse positions
                m_oldX = ea.getX();
                m_oldY = ea.getY();

                handled = true;
            }

            break;
        }
        case osgGA::GUIEventAdapter::RESIZE:
        {
            // windows resize
            m_graphWidth->set( ea.getWindowWidth() - m_labelsWidth->get() );

            handled = true;

            break;
        }
        case osgGA::GUIEventAdapter::SCROLL:
        {
            float delta;
            switch( ea.getScrollingMotion() )
            {
                case osgGA::GUIEventAdapter::SCROLL_UP:
                {
                    delta = 120.0f;
                    break;
                }
                case osgGA::GUIEventAdapter::SCROLL_DOWN:
                {
                    delta = -120.0f;
                    break;
                }
                case osgGA::GUIEventAdapter::SCROLL_2D:
                {
                    delta = ea.getScrollingDeltaY();
                    break;
                }
                default:
                {
                    delta = 0.0f;
                    break;
                }
            }

            if( delta != 0.0f )
            {
                if( ea.getButtonMask() & osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON )
                {
                    // change the spacing of the different graphs
                    const float addend = -0.001f * delta;
                    m_ySpacing->set( m_ySpacing->get() * ( 1.0f + addend ) );

                    // adjust yPos to zoom into the current mouse position
                    m_yPos->set( m_yPos->get() * ( 1.0f + addend ) + ea.getY() * addend );
                }
                else
                {
                    // change the sensitivity of the graphs
                    m_ySensitivity->set( m_ySensitivity->get() * ( 1.0f + 0.001f * delta ) );
                }

                handled = true;
            }

            break;
        }
        default:
        {
            // do nothing
            break;
        }
    }
    return handled;
}
