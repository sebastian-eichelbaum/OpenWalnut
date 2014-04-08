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

#ifndef WUIWIDGETBASE_H
#define WUIWIDGETBASE_H

#include <string>

#include "core/common/WCondition.h"

#include <boost/shared_ptr.hpp>

/**
 * Base class for all the widget abstraction the core UI provides. All the abstract widgets use the bridge pattern to let the UI/GUI implementor
 * actually handle everything.
 */
class WUIWidgetBase
{
    friend class WUIWidgetFactory;
public:
    /**
     * Convenience typedef for a boost::shared_ptr< WUIWidgetBase >.
     */
    typedef boost::shared_ptr< WUIWidgetBase > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WUIWidgetBase >.
     */
    typedef boost::shared_ptr< const WUIWidgetBase > ConstSPtr;

    /**
     * Destructor.
     */
    virtual ~WUIWidgetBase();

    /**
     * Get the title of the widget.
     *
     * \return title as string
     */
    virtual std::string getTitle() const;

    /**
     * Show this widget if not yet visible.
     */
    virtual void show() = 0;

    /**
     * Hide/show this widget. Unlike close(), you can show the widget again using show().
     *
     * \param visible false to hide widget
     */
    virtual void setVisible( bool visible = true ) = 0;

    /**
     * Check if the widget is hidden or not.
     *
     * \return true if visible.
     */
    virtual bool isVisible() const = 0;

    /**
     * Close the widget. When done, the widget can be safely deleted. You cannot re-open the widget with \ref show(). If you want to hide widget,
     * use setVisible( false ) instead.
     */
    virtual void close() = 0;

    /**
     * Return the condition that fires when the widgets closes.
     *
     * \return the condition fired whenever the widget closes
     */
    WCondition::SPtr getCloseCondition() const;

    /**
     * Get the parent widget of this widget if any. Can be NULL.
     *
     * \return the parent
     */
    virtual WUIWidgetBase::SPtr getParent() const;
protected:
    /**
     * Default constructor.
     *
     * \param title the title of the widget
     */
    explicit WUIWidgetBase( std::string title );

    /**
     * Called directly before closing the widget.
     */
    virtual void onClose();

    /**
     * Set the parent of this WUI widget. Do never call this after the widget was realized by the factory.
     *
     * \param parent the parent. Can be NULL if none.
     */
    virtual void setParent( WUIWidgetBase::SPtr parent );
private:
    /**
     * The widget's title string.
     */
    std::string m_title;

    /**
     * Close condition. Notified when widget closes.
     */
    WCondition::SPtr m_closeCondition;

    /**
     * The parent. Can be NULL.
     */
    WUIWidgetBase::SPtr m_parent;
};

#endif  // WUIWIDGETBASE_H

