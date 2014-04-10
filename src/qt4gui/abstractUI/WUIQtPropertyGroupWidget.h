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

#ifndef WUIQTPROPERTYGROUPWIDGET_H
#define WUIQTPROPERTYGROUPWIDGET_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "core/ui/WUIPropertyGroupWidget.h"
#include "../WMainWindow.h"
#include "../controlPanel/WQtPropertyGroupWidget.h"

#include "WUIQtWidgetBase.h"

/**
 * Implementation of \ref WUIPropertyGroupWidget.
 */
class WUIQtPropertyGroupWidget: public WUIPropertyGroupWidget,
                                public WUIQtWidgetBase
{
public:
    /**
     * Convenience typedef for a boost::shared_ptr< WUIQtPropertyGroupWidget >.
     */
    typedef boost::shared_ptr< WUIQtPropertyGroupWidget > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WUIQtPropertyGroupWidget >.
     */
    typedef boost::shared_ptr< const WUIQtPropertyGroupWidget > ConstSPtr;

    /**
     * Default constructor.
     *
     * \param mainWindow the main window instance
     * \param properties the properties to show
     * \param parent the Qt parent. Can be NULL.
     * \param title the title of the widget
     */
    WUIQtPropertyGroupWidget(
            std::string title,
            WPropGroup properties,
            WMainWindow* mainWindow,
            WUIQtWidgetBase::SPtr parent = WUIQtWidgetBase::SPtr() );

    /**
     * Destructor.
     */
    virtual ~WUIQtPropertyGroupWidget();

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
     * The property group shown by this widget.
     *
     * \return the group
     */
    virtual WPropGroup getPropertyGroup() const;
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
private:
    /**
     * The prop widget
     */
    QWidget* m_propWidget;

    /**
     * The properties to show
     */
    WPropGroup m_properties;
};

#endif  // WUIQTPROPERTYGROUPWIDGET_H

