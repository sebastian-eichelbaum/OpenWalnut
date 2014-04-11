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

#ifndef WUIQTTABBEDWIDGET_H
#define WUIQTTABBEDWIDGET_H

#include <string>

#include <QtGui/QTabWidget>

#include <boost/shared_ptr.hpp>

#include "core/ui/WUITabbedWidget.h"
#include "../WMainWindow.h"

#include "../guiElements/WQtDockWidget.h"

#include "WUIQtWidgetBase.h"

/**
 * Implementation of \ref WUITabbedWidget.
 */
class WUIQtTabbedWidget: public WUITabbedWidget,
                         public WUIQtWidgetBase
{
public:
    /**
     * Convenience typedef for a boost::shared_ptr< WUIQtTabbedWidget >.
     */
    typedef boost::shared_ptr< WUIQtTabbedWidget > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WUIQtTabbedWidget >.
     */
    typedef boost::shared_ptr< const WUIQtTabbedWidget > ConstSPtr;

    /**
     * Default constructor.
     *
     * \param mainWindow the main window instance
     * \param parent the Qt parent. Can be NULL.
     * \param title the title of the widget
     */
    WUIQtTabbedWidget(
                      std::string title,
                      WMainWindow* mainWindow,
                      WUIQtWidgetBase::SPtr parent = WUIQtWidgetBase::SPtr() );

    /**
     * Destructor.
     */
    virtual ~WUIQtTabbedWidget();

    /**
     * Title as QString.
     *
     * \return the title
     */
    virtual QString getTitleQString() const;

    /**
     * Show this widget if not yet visible.
     */
    virtual void show();

    /**
     * Hide/show this widget. Unlike close(), you can show the widget again using show().
     *
     * \param visible false to hide widget
     */
    virtual void setVisible( bool visible = true );

    /**
     * Check if the widget is hidden or not.
     *
     * \return true if visible.
     */
    virtual bool isVisible() const;

    /**
     * Handle shutdown. This includes notification of the creating module and closing the widget. Can be called from any thread.
     * Implement in your implementation.
     */
    virtual void close();

    /**
     * Set label of the tab with given index. If the tab does not exist, nothing happens.
     *
     * \param index the index of the tab
     * \param label the label of the tab
     */
    virtual void setTabText( int index, const std::string& label );

    /**
     * Set the given tab active. If it not exists, the current tab stays active.
     *
     * \param index the index of the tab
     */
    virtual void setActiveTab( int index );

    /**
     * Get the index of the currently active tab.
     *
     * \return the index, or -1 if error
     */
    virtual int getActiveTab() const;

    /**
     * Query the number of tabs.
     *
     * \return the number of tabs, or -1 if error.
     */
    virtual int getNumTabs() const;

    /**
     * Write some tool tip for a given tab. If the index is invalid, nothing happens.
     *
     * \param index the tab index
     * \param tip the tooltip text
     */
    virtual void setTabToolTip( int index, const std::string& tip );

    /**
     * Allows en/disabling a tab. If the index is invalid, nothing happens.
     *
     * \param index the index.
     * \param enable true to enable
     */
    virtual void setTabEnabled( int index, bool enable );

    /**
     * Specify where to place the tabs
     *
     * \param position the position
     */
    virtual void setTabPosition( TabPosition position );
protected:
    /**
     * Realize the widget. This method blocks until the GUI thread created the widget. Called from within the GUI thread! So you can safely do Qt
     * stuff.
     */
    virtual void realizeImpl();

    /**
     * Close the widget. When done, the widget can be safely deleted.
     */
    virtual void closeImpl();

    /**
     * Cleanup the GUI. Do not delete m_widget, or your content widget. This is done by WUIQtWidgetBase. This method allows you to free resources
     * that are not automatically freed by the Qt delete mechanism.
     */
    virtual void cleanUpGT();

    /**
     * Place the given widget in this tab widget in a new tab with a given label. The widget to be placed must be created with this tab widget as parent or an
     * exception will be thrown.
     *
     * \param widget the widget
     * \param label the label of the tab
     *
     * \return the index of the new tab
     */
    virtual int addTabImpl( WUIWidgetBase::SPtr widget, std::string label );
private:
    /**
     * The Qt widget representing this abstract widget. Might be null. Check before use!
     * \note this is the same pointer as WUIQtWidgetBase::m_widget as WQtDockWidget.
     */
    WQtDockWidget* m_widgetDock;

    /**
     * The tab widget used for managing child widgets
     */
    QTabWidget* m_tabWidget;
};

#endif  // WUIQTTABBEDWIDGET_H

