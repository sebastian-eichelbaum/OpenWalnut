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

#ifndef WQTDOCKTITLEWIDGET_H
#define WQTDOCKTITLEWIDGET_H

#include <QtGui/QDockWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QToolButton>
#include <QtGui/QLabel>
#include <QtGui/QResizeEvent>
#include <QtGui/QWidgetAction>

#include "WScaleLabel.h"

/**
 * Class for managing dock widget title bars.
 */
class WQtDockTitleWidget: public QWidget
{
    Q_OBJECT
public:
    /**
     * Constructor.
     *
     * \param parent the parent
     */
    explicit WQtDockTitleWidget( QDockWidget* parent );

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
    /**
     * Called upon resize. Used to switch between the more menu and the tools widget
     *
     * \param event the event
     */
    virtual void resizeEvent( QResizeEvent* event );

    /**
     * Apply default settings for dock widget title buttons.
     *
     * \param btn the button to setup
     */
    virtual void setupButton( QToolButton* btn );

    /**
     * Apply size setup to a given widget.
     *
     * \param widget the widget to setup
     */
    virtual void setupSizeConstraints( QWidget* widget );
private:
    /**
     * Construct the title and configure the widget.
     */
    void construct();

    /**
     * Updates the layouts according to the new width
     *
     * \param width the new width.
     */
    void updateLayouts( int width );

    /**
     * The tools buttons
     */
    QWidget* m_tools;

    /**
     * The tool inside the menu
     */
    QWidget* m_toolsMenu;

    /**
     * Layout containing the tools
     */
    QHBoxLayout* m_toolsLayout;

    /**
     * The tool button used when shrinking the title bar too much
     */
    QToolButton* m_moreBtn;

    /**
     * LAyout of the items in the moreBtn menu
     */
    QHBoxLayout* m_toolsMenuLayout;

    /**
     * Title label
     */
    WScaleLabel* m_title;

    /**
     * Close button
     */
    QToolButton* m_closeBtn;

    /**
     * Help button
     */
    QToolButton* m_helpBtn;

    /**
     * The parent as dock pointer
     */
    QDockWidget* dockParent;

    /**
     * We keep track of the widgets that we add
     */
    QList< QWidget* > m_titleActionWidgets;
};

#endif  // WQTDOCKTITLEWIDGET_H

