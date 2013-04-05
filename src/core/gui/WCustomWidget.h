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
#include <queue>
#include <deque>

#include <boost/shared_ptr.hpp>

#include <osg/ref_ptr>

#include "../common/WCondition.h"
#include "../common/WSharedSequenceContainer.h"
#include "../graphicsEngine/WGEViewer.h"

class WGEGroupNode;

/**
 * Custom widget which is created by a module to display custom information.
 */
class WCustomWidget
{
public:
    /**
     * Forward EventType from OSG
     */
    typedef osgGA::GUIEventAdapter::EventType EventType;

    /**
     * Queue of GUI events.
     */
    typedef std::queue< osg::ref_ptr< osgGA::GUIEventAdapter > > EventQueue;

    /**
     * Convenience short hand for \c boost::shared_ptr on \c EventQueue.
     */
    typedef boost::shared_ptr< EventQueue > EventQueueSPtr;

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
     * \param subscription Binary mask on which event types you want to be notified.
     */
    explicit WCustomWidget( std::string title, EventType subscription = osgGA::GUIEventAdapter::NONE );

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
     * Returns the next unhandled GUI event on this widget. After calling this function you take responisbility on the event-copy,
     * (meaning destroy it when you do not need it anymore). Additionally the notification of new events will be triggered again
     * in case there are more events waiting to be processed.
     *
     * \return Next GUI event ( shallow copy ).
     */
    virtual EventQueueSPtr getNextEvents();

    /**
     * This condition fires whenever a new event has to be handled and there was no old event left.
     *
     * \return \c WCondition as event notifier.
     */
    virtual WCondition::SPtr getEventNotifier() const;

    /**
     * Determines if there are new events which you have subscribed to and need to be processed or not.
     *
     * \return True when there are new events, false otherwise.
     */
    virtual bool newEvents() const;

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
            if( ea.getEventType() & m_widget->m_subscription )
            {
                WSharedSequenceContainer< EventDeque >::WriteTicket t = m_widget->m_events->getWriteTicket();

                if( t->get().size() == 0 ) // when this is the first event, notify module only once
                {
                    m_widget->m_eventNotifier->notify();
                }
                // else: Do not spam the module for new events, but still collect events into queue. Module will need to make queue empty again

                t->get().push_back( new osgGA::GUIEventAdapter( ea ) );
                return true;
            }

            return false; // There was no subscription to this event
        }

    private:
        /**
         * Reference to the WCustomWidget which has the condition for such events and needs to be notified.
         */
        WCustomWidget* const m_widget;
    };

    /**
     * This is a short hand type for our FiFo queue containing events.
     *
     * \note A std::queue would be better, as we are using it as a FiFo queue, but inorder to have thread safe access,
     * we use WSharedSequenceContainer which only supports deque, list and vector at the moment.
     */
    typedef std::deque< osg::ref_ptr< osgGA::GUIEventAdapter > > EventDeque;

    /**
     * Short hand type for boost::shared_ptr on internal event deque.
     */
    typedef boost::shared_ptr< EventDeque > EventDequeSPtr;

private:
    /**
     * The widget's title string.
     */
    std::string m_title;

    /**
     * Condition fired if an GUI event has occured.
     */
    WCondition::SPtr m_eventNotifier;

    /**
     * Stores last GUI events.
     */
    WSharedSequenceContainer< EventDeque >::SPtr m_events;

    /**
     * Binary mask describing which events should be used for notification. For example you may use:
     * \c subscription=MOVE|SCROLL|RESIZE.
     */
    EventType m_subscription;
};

#endif  // WCUSTOMWIDGET_H
