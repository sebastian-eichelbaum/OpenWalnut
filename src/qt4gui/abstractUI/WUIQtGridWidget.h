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

#ifndef WUIQTGRIDWIDGET_H
#define WUIQTGRIDWIDGET_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "core/ui/WUIGridWidget.h"
#include "../WMainWindow.h"

#include "../guiElements/WQtDockWidget.h"

#include "WUIQtWidgetBase.h"

/**
 * Implementation of \ref WUIGridWidget.
 */
class WUIQtGridWidget: public WUIGridWidget,
                     public WUIQtWidgetBase
{
public:
    /**
     * Convenience typedef for a boost::shared_ptr< WUIQtGridWidget >.
     */
    typedef boost::shared_ptr< WUIQtGridWidget > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WUIQtGridWidget >.
     */
    typedef boost::shared_ptr< const WUIQtGridWidget > ConstSPtr;

    /**
     * Default constructor.
     *
     * \param mainWindow the main window instance
     * \param parent the Qt parent. Can be NULL.
     * \param title the title of the widget
     */
    WUIQtGridWidget(
            std::string title,
            WMainWindow* mainWindow,
            WUIQtWidgetBase::SPtr parent = WUIQtWidgetBase::SPtr() );

    /**
     * Destructor.
     */
    virtual ~WUIQtGridWidget();

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
     * Sets the stretch factor of a row to stretch. The first row is number 0. The stretch factor is relative to the other rows in this grid.
     * Rows with a higher stretch factor take more of the available space. The default stretch factor is 0. If the stretch factor is 0 and no
     * other row in this table can grow at all, the row may still grow.
     *
     * \param row the row to set this value for
     * \param stretch the stretch
     */
    virtual void setRowStretch( int row, int stretch );

    /**
     * Sets the stretch factor of a column to stretch. The first column is number 0. The stretch factor is relative to the other columns in this grid.
     * Columns with a higher stretch factor take more of the available space. The default stretch factor is 0. If the stretch factor is 0 and no
     * other column in this table can grow at all, the column may still grow.
     *
     * \param column the column to set this value for
     * \param stretch the stretch
     */
    virtual void setColumnStretch( int column, int stretch );

    /**
     * Implement \ref WUIWidgetBase::addAction.
     *
     * \param group the property to use.
     * \param icon the icon to use. Consider a minimum size of 32x32.
     */
    virtual void addAction( WPropGroup group, WGEImage::SPtr icon = WGEImage::SPtr() )
    {
        WUIQtWidgetBase::addAction( group, icon );
    }

    /**
     * Implement \ref WUIWidgetBase::addAction.
     *
     * \param trigger the property to use.
     * \param icon the icon to use. Consider a minimum size of 32x32.
     */
    virtual void addAction( WPropTrigger trigger, WGEImage::SPtr icon = WGEImage::SPtr() )
    {
        WUIQtWidgetBase::addAction( trigger, icon );
    }

    /**
     * Implement \ref WUIWidgetBase::addAction.
     *
     * \param toggle the property to use.
     * \param icon the icon to use. Consider a minimum size of 32x32.
     */
    virtual void addAction( WPropBool toggle, WGEImage::SPtr icon = WGEImage::SPtr() )
    {
        WUIQtWidgetBase::addAction( toggle, icon );
    }
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
     * \note this is the same pointer as WUIQtWidgetBase::m_widget as WQtDockWidget.
     */
    WQtDockWidget* m_widgetDock;

    /**
     * The grid used for managing child widgets
     */
    QGridLayout* m_gridLayout;
};

#endif  // WUIQTGRIDWIDGET_H

