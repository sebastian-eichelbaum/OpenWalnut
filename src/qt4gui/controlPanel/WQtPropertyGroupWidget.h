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
#include <map>

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
#include "WPropertyTransferFunctionWidget.h"
#include "WPropertyStructWidget.h"

#include "core/common/WPropertyVariable.h"
#include "core/common/WPropertyStruct.h"
#include "core/common/WPropertyGroupBase.h"

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
     * \param depth the nesting depth of this group, it usually is parents depth + 1.
     * \param parent The widget managing this widget
     */
    WQtPropertyGroupWidget( WPropertyGroupBase::SPtr group, size_t depth = 0, QWidget* parent = 0 );

    /**
     * This function creates the fancy box around your specified group widget.
     *
     * \param widget the widget to put into the box
     * \param asScrollArea if true, the widget is embedded into a scroll area
     * \param parent the parent widget
     * \param title the title to use for the box. If empty, the property group name is used.
     *
     * \return the box widget
     */
    static QWidget* createPropertyGroupBox( WQtPropertyGroupWidget* widget, bool asScrollArea = false, QWidget* parent = NULL,
                                            const QString& title = "" );

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
     * Adds a new property widget to the PropertyGroup. Returns NULL if property not supported. This also supports groups.
     *
     * \param property the property to add.
     */
    void addProp( WPropertyBase::SPtr property );

    /**
     * Adds an widget containing another property group to this widget. It encloses it with a GroupBox.
     *
     * \param widget the widget
     * \param asScrollArea true if the group should be embedded into a scroll area
     */
    void addGroup( WQtPropertyGroupWidget* widget, bool asScrollArea = false );

    /**
     * Add the given prop group to this widget.
     *
     * \param prop the property group
     */
    void addGroup( WPropertyGroupBase::SPtr prop );

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

    /**
     * Get the managed group of this widget.
     *
     * \return the group
     */
    WPropertyGroupBase::SPtr getPropertyGroup();

public slots:
    /**
     * Change visibility of this group.
     * \param who the widget to switch
     */
    void switchVisibility( QWidget* who );

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
    WPropertyGroupBase::SPtr m_group;

    /**
     * The map between a prop and the widget handling it.
     */
    typedef std::map< WPropertyBase::SPtr, QWidget* > PropertyWidgets;

    /**
     * The map if property pointer to actual property widget.
     */
    PropertyWidgets m_propWidgets;

    /**
     * The connection for propertyChangeNotifier().
     */
    boost::signals2::connection m_connection;

    /**
     * The depth of this group.
     */
    size_t m_nestingDepth;
};

#endif  // WQTPROPERTYGROUPWIDGET_H
