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

#include <QDockWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QLabel>
#include <QResizeEvent>
#include <QWidgetAction>

#include "core/common/WProperties.h"
#include "core/graphicsEngine/WGEImage.h"

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
     * Add a property to the title of this dock. Image should be specified but is optional.
     *
     * \param prop the property
     * \param icon the icon to use.
     */
    virtual void addTitleProperty( WPropTrigger prop, WGEImage::SPtr icon = WGEImage::SPtr() );

    /**
     * Add a property to the title of this dock. Image should be specified but is optional.
     *
     * \param prop the property
     * \param icon the icon to use.
     */
    virtual void addTitleProperty( WPropBool prop, WGEImage::SPtr icon = WGEImage::SPtr() );

    /**
     * Add a property to the title of this dock. Image should be specified but is optional.
     *
     * \param prop the property
     * \param icon the icon to use.
     */
    virtual void addTitleProperty( WPropGroup prop, WGEImage::SPtr icon = WGEImage::SPtr() );

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

    /**
     * Set a string which identifies help material for this dock. If you specify a help ID which then leads to multiple help documents, a help
     * menu is shown instead of a simple help button. If no help context id was specified, the help button is not show.
     *
     * \param id the help context id for this dock
     */
    virtual void setHelpContext( const QString& id );

    /**
     * Return the help context id.
     *
     * \return the id.
     */
    virtual const QString& getHelpContext();

    /**
     * Save settings.
     */
    virtual void saveSettings();

    /**
     * Restore settings
     */
    virtual void restoreSettings();

    /**
     * Disable close button? Might come in handy when embedding these widgets into others
     *
     * \param disable true to disable
     */
    void disableCloseButton( bool disable = true );

public slots:
    /**
     * Show help.
     */
    void showHelp();

protected:
    /**
     * Called whenever a close event is received.
     *
     * \param event the event.
     */
    virtual void closeEvent( QCloseEvent *event );

private:
    /**
     * Title widget.
     */
    WQtDockTitleWidget* m_titleBar;

    /**
     * The ID for the help context associated with this dock.
     */
    QString m_helpContextId;
};

#endif  // WQTDOCKWIDGET_H

