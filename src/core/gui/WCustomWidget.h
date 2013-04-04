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

#ifndef WCUSTOMWIDGET_H
#define WCUSTOMWIDGET_H

#include <string>

#include <boost/shared_ptr.hpp>

#include <osg/ref_ptr>

#include "../graphicsEngine/WGEViewer.h"
#include "../common/WFlag.h"

class WGEGroupNode;

/**
 * Custom widget which is created by a module to display custom information.
 */
class WCustomWidget
{
public:
    /**
     * Abbreviation for a shared pointer on a instance of this class.
     */
    typedef boost::shared_ptr< WCustomWidget > SPtr;

    /**
     * Abbreviation for a const shared pointer on a instance of this class.
     */
    typedef boost::shared_ptr< const WCustomWidget > ConstSPtr;

    /**
     * Constructor. Create a custom widget instance.
     *
     * \param title the title of the widget
     */
    explicit WCustomWidget( std::string title );

    /**
     * Destructor
     */
    virtual ~WCustomWidget();

    /**
     * Get the scene which is displayed
     *
     * \return the scene as osg::ref_ptr
     */
    virtual osg::ref_ptr< WGEGroupNode > getScene() const = 0;

    /**
     * Get the viewer which is used
     *
     * \return the viewer as boost::shard_ptr
     */
    virtual boost::shared_ptr< WGEViewer > getViewer() const = 0;

    /**
     * Get the title of the widget.
     *
     * \return title as string
     */
    virtual std::string getTitle() const;

    /**
     * Returns the height of the viewport of the camera.
     *
     * \return Height in pixels.
     */
    virtual size_t height() const = 0;

    /**
     * Returns the width of the viewport of the camera.
     *
     * \return Width in pixels.
     */
    virtual size_t width() const = 0;

    /**
     * Returns the type of the last event captured by eventOccured. Meanwhile other events may have occured you may miss.
     *
     * \param reset If set to true, the flag is reset and the widget is ready for new events. Otherwise its just queried.
     *
     * \return OSG GUI event.
     */
    virtual const osgGA::GUIEventAdapter& getEvent( bool reset = false );

    /**
     * This condition fires whenever a new event has to be handled and there was no old event left.
     *
     * \return \c WCondition as event notifier.
     */
    virtual WCondition::SPtr getCondition() const;

    /**
     * True when there is an unhandled event left. False otherwise.
     *
     * \return Bool indicating unhandled events.
     */
    virtual bool eventOccured() const;

protected:
    /**
     * \class WindowHandler
     *
     * An event handler for a custom widget.
     */
    class WindowHandler : public osgGA::GUIEventHandler
    {
    public:
        /**
         * Constructor.
         *
         * \param widget The custom widget which should be notified.
         */
        explicit WindowHandler( WCustomWidget* widget )
            : m_widget( widget )
        {
        }

        /**
         * Deals with the events found by the osg.
         *
         * \param ea Event class for storing Keyboard, mouse and window events.
         *
         * \return true if the event was handled.
         */
        bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& /* aa */ )
        {
            if( m_widget->eventOccured() ) // if there is an unhandled event proceed
            {
                return false;
            }

            m_widget->m_event = new osgGA::GUIEventAdapter( ea );
            m_widget->m_eventOccured->set( true );
            return true;
        }

    private:
        /**
         * Reference to the WCustomWidget which has the condition for such events and needs to be notified.
         */
        WCustomWidget* const m_widget;
    };

private:
    /**
     * The widget's title string.
     */
    std::string m_title;

    /**
     * Flag indicating if an GUI event has occured. If so it is stored in m_lastEventType.
     */
    WBoolFlag::SPtr m_eventOccured;

    /**
     * Stores last GUI event.
     */
    osg::ref_ptr< osgGA::GUIEventAdapter > m_event;
};

#endif  // WCUSTOMWIDGET_H
