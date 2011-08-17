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

#ifndef WQTPROPERTYGROUPWIDGET_H
#define WQTPROPERTYGROUPWIDGET_H

#include <string>

#include <QtGui/QGridLayout>
#include <QtGui/QWidget>

#include "WPropertyBoolWidget.h"
#include "WPropertyIntWidget.h"
#include "WPropertyStringWidget.h"
#include "WPropertyDoubleWidget.h"
#include "WPropertyColorWidget.h"
#include "WPropertyFilenameWidget.h"
#include "WPropertyTriggerWidget.h"
#include "WPropertySelectionWidget.h"
#include "WPropertyPositionWidget.h"
#include "WPropertyMatrix4X4Widget.h"

#include "core/common/WPropertyVariable.h"

/**
 * Container widget to contain a number of properties for the module context in the control panel.
 */
class WQtPropertyGroupWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * Creates new widget for a property group. Use this constructor to provide automatic hidden-flag management.
     * \param group The group
     * \param parent The widget managing this widget
     */
    WQtPropertyGroupWidget( WPropGroup group, QWidget* parent = 0 );

    /**
     * destructor
     */
    virtual ~WQtPropertyGroupWidget();

    /**
     * True if there are no widgets inside.
     *
     * \return true if number of widgets is 0.
     */
    bool isEmpty() const;

    /**
     * helper function to add a generic button
     * \param label Text on the push button
     *
     * \return the push button instance
     */
    QPushButton* addPushButton( QString label );

    /**
     * Adds a new bool property to the PropertyGroup.
     *
     * \param property the property to add.
     *
     * \return the widget that has been added.
     */
    WPropertyBoolWidget* addProp( WPropBool property );

    /**
     * Adds a new int property to the PropertyGroup.
     *
     * \param property the property to add.
     *
     * \return the widget that has been added.
     */
    WPropertyIntWidget* addProp( WPropInt property );

    /**
     * Adds a new double property to the PropertyGroup.
     *
     * \param property the property to add.
     *
     * \return the widget that has been added.
     */
    WPropertyDoubleWidget* addProp( WPropDouble property );

    /**
     * Adds a new string property to the PropertyGroup.
     *
     * \param property the property to add.
     *
     * \return the widget that has been added.
     */
    WPropertyStringWidget* addProp( WPropString property );

    /**
     * Adds a new path property to the PropertyGroup.
     *
     * \param property the property to add.
     *
     * \return the widget that has been added.
     */
    WPropertyFilenameWidget* addProp( WPropFilename property );

    /**
     * Adds a new list property to the PropertyGroup.
     *
     * \param property the property to add.
     *
     * \return the widget that has been added.
     */
    WPropertySelectionWidget* addProp( WPropSelection property );

    /**
     * Adds a new position property to the PropertyGroup.
     *
     * \param property the property to add.
     *
     * \return the widget that has been added.
     */
    WPropertyPositionWidget* addProp( WPropPosition property );

    /**
     * Adds a new color property to the PropertyGroup.
     *
     * \param property the property to add.
     *
     * \return the widget that has been added.
     */
    WPropertyColorWidget* addProp( WPropColor property );

    /**
     * Adds a new trigger property to the PropertyGroup.
     *
     * \param property the property to add.
     *
     * \return the widget that has been added.
     */
    WPropertyTriggerWidget* addProp( WPropTrigger property );

    /**
     * Adds a new trigger property to the PropertyGroup.
     *
     * \param property the property to add.
     *
     * \return the widget that has been added.
     */
    WPropertyMatrix4X4Widget* addProp( WPropMatrix4X4 property );

    /**
     * Adds an widget containing another property group to this widget. It encloses it with a GroupBox.
     *
     * \param widget the widget
     * \param asScrollArea true if the group should be embedded into a scroll area
     */
    void addGroup( WQtPropertyGroupWidget* widget, bool asScrollArea = false );

    /**
     * helper function to add a spacer at the end
     */
    void addSpacer();

    /**
     * The property group name.
     *
     * \return the group name
     */
    QString getName();

    /**
     * Sets the name of this widget.
     *
     * \param name the name.
     */
    void setName( QString name );

signals:

    /**
     * A Signal which gets emitted whenever the widget should be hidden. This is a useful signal for containers which embed this group.
     *
     * \param hide if true, the widget should be hidden.
     */
    void hideSignal( bool hide );

protected:

    /**
     * Callback for WPropertyBase::getChangeCondition. It emits an event to ensure all updates are done in gui thread.
     */
    virtual void propertyChangeNotifier();

    /**
     * Custom event dispatcher. Gets called by QT's Event system every time an event got sent to this widget. This event handler
     * processes property change events.
     *
     * \note QT Doc says: use event() for custom events.
     * \param event the event that got transmitted.
     *
     * \return true if the event got handled properly.
     */
    virtual bool event( QEvent* event );

private:

    /**
     * The name used for this widget.
     */
    QString m_name;

    /**
     * Layout used for each "widget combination".
     */
    QGridLayout *m_controlLayout;

    /**
     * Layout used for the whole widget.
     */
    QVBoxLayout *m_pageLayout;

    /**
     * The number of widgets inside this one.
     */
    unsigned int m_numberOfWidgets;

    /**
     * The property group handled here.
     */
    WPropGroup m_group;

    /**
     * The connection for propertyChangeNotifier().
     */
    boost::signals2::connection m_connection;
};

#endif  // WQTPROPERTYGROUPWIDGET_H
