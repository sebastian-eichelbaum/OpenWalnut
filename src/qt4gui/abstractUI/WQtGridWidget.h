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

#ifndef WQTGRIDWIDGET_H
#define WQTGRIDWIDGET_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "core/ui/WUIGridWidget.h"
#include "../WMainWindow.h"

#include "../guiElements/WQtDockWidget.h"

#include "WQtWidgetBase.h"

/**
 * Implementation of \ref WUIGridWidget.
 */
class WQtGridWidget: public WUIGridWidget,
                     public WQtWidgetBase
{
public:
    /**
     * Convenience typedef for a boost::shared_ptr< WQtGridWidget >.
     */
    typedef boost::shared_ptr< WQtGridWidget > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WQtGridWidget >.
     */
    typedef boost::shared_ptr< const WQtGridWidget > ConstSPtr;

    /**
     * Default constructor.
     *
     * \param mainWindow the main window instance
     * \param parent the Qt parent. Can be NULL.
     * \param title the title of the widget
     */
    WQtGridWidget(
            std::string title,
            WMainWindow* mainWindow,
            WQtWidgetBase::SPtr parent = WQtWidgetBase::SPtr() );

    /**
     * Destructor.
     */
    virtual ~WQtGridWidget();

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
     * Cleanup the GUI.
     */
    virtual void cleanUpGT();

    /**
     * Place the given widget in this grid at the given coordinates. The widget to be placed must be created with this grid as parent.
     *
     * \param widget the widget
     * \param x x coord ( 0 is left )
     * \param y y coord ( 0 is top )
     */
    virtual void placeWidgetImpl( WUIWidgetBase::SPtr widget, int x, int y );

    /**
     * Place the given widget in this grid at the given coordinates. The widget to be placed must be created with this grid as parent. GUI thread
     * version.
     *
     * \param widget the widget
     * \param x x coord ( 0 is left )
     * \param y y coord ( 0 is top )
     */
    virtual void placeWidgetImplGT( QWidget* widget, int x, int y );

private:
    /**
     * The Qt widget representing this abstract widget. Might be null. Check before use!
     * \note this is the same pointer as WQtWidgetBase::m_widget as WQtDockWidget.
     */
    WQtDockWidget* m_widgetDock;

    /**
     * The grid used for managing child widgets
     */
    QGridLayout* m_gridLayout;
};

#endif  // WQTGRIDWIDGET_H

