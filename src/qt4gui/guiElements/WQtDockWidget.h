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

#ifndef WQTDOCKWIDGET_H
#define WQTDOCKWIDGET_H

#include <QtGui/QDockWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QToolButton>
#include <QtGui/QLabel>
#include <QtGui/QResizeEvent>
#include <QtGui/QWidgetAction>

#include "WScaleLabel.h"

/**
 * Used for the title bar.
 */
class WQtDockTitleWidget;

/**
 * Advanced QDockWidget. This class allows you to add actions to the titlebar of the dock widget
 */
class WQtDockWidget: public QDockWidget
{
    Q_OBJECT
public:
    /**
     * Construct dock widget.
     *
     * \param title the title of the widget
     * \param parent the parent widget
     * \param flags optional window flags
     */
    WQtDockWidget( const QString& title, QWidget* parent = 0, Qt::WindowFlags flags = 0 );

    /**
     * Construct dock widget.
     *
     * \param parent the parent widget
     * \param flags optional window flags
     */
    WQtDockWidget( QWidget* parent = 0, Qt::WindowFlags flags = 0 );

    /**
     * Destructor.
     */
    virtual ~WQtDockWidget();

    /**
     * Add the given action to the titlebar. It gets added after the previously added ones.
     *
     * \param action the action to add.
     * \param instantPopup if true, the button does not trigger an action. Instead, it directly opens the menu.
     */
    virtual void addTitleAction( QAction* action, bool instantPopup = false );

    /**
     * Add the given tool button to the titlebar. It gets added after the previously added ones.
     *
     * \param button the action to add.
     */
    virtual void addTitleButton( QToolButton* button );

    /**
     * Remove the given action from the list
     *
     * \param action the action to add
     */
    virtual void removeTitleAction( QAction* action );

    /**
     * Add a separator.
     */
    virtual void addTitleSeperator();

    /**
     * Add an arbitrary widget. Please take care of its size! This method sets a size policy and a fixed height.
     *
     * \param widget the widget to add
     */
    virtual void addTitleWidget( QWidget* widget );

    /**
     * Remove the specified widget from the title bar.
     *
     * \param widget the widget to remove
     */
    virtual void removeTitleWidget( QWidget* widget );

protected:
private:
    /**
     * Title widget.
     */
    WQtDockTitleWidget* m_titleBar;
};

#endif  // WQTDOCKWIDGET_H
