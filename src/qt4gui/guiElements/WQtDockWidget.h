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

protected:
    /**
     * Apply default settings for dock widget title buttons.
     *
     * \param btn the button to setup
     */
    virtual void setupButton( QToolButton* btn );

private:
    /**
     * Construct the title and configure the widget.
     */
    void construct();

    /**
     * The title of this dock
     */
    QWidget* m_titleWidget;

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
     * Title label
     */
    WScaleLabel* m_title;

    /**
     * Close button
     */
    QToolButton* m_closeBtn;
};

#endif  // WQTDOCKWIDGET_H

