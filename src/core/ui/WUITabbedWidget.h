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

#ifndef WUITABBEDWIDGET_H
#define WUITABBEDWIDGET_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "WUIWidgetBase.h"

/**
 * Widget which is a container for others. The child widgets can be arranged in tabs. The WUITabbedWidget itself is a widget, which means you can
 * embed tabs in other container widgets (like grids).
 *
 * \note Please read documentation of \ref WUIWidgetFactory for limitations, requirements and creation of these widgets.
 */
class WUITabbedWidget: public WUIWidgetBase
{
    friend class WUI;
public:
    /**
     * Convenience typedef for a boost::shared_ptr< WUITabbedWidget >.
     */
    typedef boost::shared_ptr< WUITabbedWidget > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WUITabbedWidget >.
     */
    typedef boost::shared_ptr< const WUITabbedWidget > ConstSPtr;

    /**
     * Destructor.
     */
    virtual ~WUITabbedWidget();

    /**
     * Tell the user whether this kind of widget can be used as parent. In other word, whether it is allowed to nest other widgets into this one.
     *
     * \return true if nesting other widgets INTO this one is allowed
     */
    virtual bool allowNesting() const;

    /**
     * Set label of the tab with given index. If the tab does not exist, nothing happens.
     *
     * \param index the index of the tab
     * \param label the label of the tab
     */
    virtual void setTabText( int index, const std::string& label ) = 0;

    /**
     * Set the given tab active. If it not exists, the current tab stays active.
     *
     * \param index the index of the tab
     */
    virtual void setActiveTab( int index ) = 0;

    /**
     * Get the index of the currently active tab.
     *
     * \return the index, or -1 if error
     */
    virtual int getActiveTab() const = 0;

    /**
     * Query the number of tabs.
     *
     * \return the number of tabs, or -1 if error.
     */
    virtual int getNumTabs() const = 0;

    /**
     * Write some tool tip for a given tab. If the index is invalid, nothing happens.
     *
     * \param index the tab index
     * \param tip the tooltip text
     */
    virtual void setTabToolTip( int index, const std::string& tip ) = 0;

    /**
     * Allows en/disabling a tab. If the index is invalid, nothing happens.
     *
     * \param index the index.
     * \param enable true to enable
     */
    virtual void setTabEnabled( int index, bool enable ) = 0;

    /**
     * Tab positions.
     */
    enum TabPosition
    {
        North = 0, //!< The tabs are drawn above the pages.
        South,     //!< The tabs are drawn below the pages.
        West,      //!< The tabs are drawn to the left of the pages.
        East      //!< The tabs are drawn to the right of the pages.
    };

    /**
     * Specify where to place the tabs
     *
     * \param position the position
     */
    virtual void setTabPosition( TabPosition position ) = 0;

    /**
     * Place the given widget in this tab widget in a new tab with a given label. The widget to be placed must be created with this tab widget as parent or an
     * exception will be thrown.
     *
     * \param widget the widget
     * \param label the label of the tab
     *
     * \return the index of the new tab, or -1 if error.
     */
    virtual int addTab( WUIWidgetBase::SPtr widget, const std::string& label );
protected:
    /**
     * Default constructor. Create an empty grid widget.
     *
     * \param title the widget title
     */
    explicit WUITabbedWidget( std::string title );

    /**
     * Place the given widget in this tab widget in a new tab with a given label. The widget to be placed must be created with this tab widget as parent or an
     * exception will be thrown.
     *
     * \param widget the widget
     * \param label the label of the tab
     *
     * \return the index of the new tab
     */
    virtual int addTabImpl( WUIWidgetBase::SPtr widget, std::string label ) = 0;
private:
};

#endif  // WUITABBEDWIDGET_H

