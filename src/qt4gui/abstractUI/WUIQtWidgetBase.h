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

#ifndef WUIQTWIDGETBASE_H
#define WUIQTWIDGETBASE_H

#include <QtGui/QWidget>

#include <boost/shared_ptr.hpp>

#include "core/common/WCondition.h"

#include "../WMainWindow.h"

/**
 * Implementation of \ref WUIWidgetBase. Not really. It is an base class for all WUIWidgetBase implementations to comfortably allow thread-save
 * GUI ops.
 */
class WUIQtWidgetBase
{
public:
    /**
     * Convenience typedef for a boost::shared_ptr< WUIQtWidgetBase >.
     */
    typedef boost::shared_ptr< WUIQtWidgetBase > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WUIQtWidgetBase >.
     */
    typedef boost::shared_ptr< const WUIQtWidgetBase > ConstSPtr;

    /**
     * Default constructor.
     *
     * \param mainWindow the main window instance
     * \param parent the parent widget. Can be NULL.
     */
    WUIQtWidgetBase( WMainWindow* mainWindow, WUIQtWidgetBase::SPtr parent );

    /**
     * Destructor.
     */
    virtual ~WUIQtWidgetBase();

    /**
     * The title as QString.
     *
     * \return  the title.
     */
    virtual QString getTitleQString() const = 0;

    /**
     * The widget was created and can be used.
     *
     * \return true if the widget is valid.
     */
    virtual bool isReal();

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
     * Realize the widget. This method blocks until the GUI thread created the widget.
     *
     * \param abortCondition a condition enforcing abort of widget creation.
     */
    virtual void realize( WCondition::SPtr abortCondition = WCondition::SPtr() );

    /**
     * Handle shutdown. This includes notification of the creating module and closing the widget. Can be called from any thread.
     * Implement in your implementation.
     */
    virtual void close() = 0;

    /**
     * Get the widget representation. Can be NULL if not yet created.
     *
     * \return the widget
     */
    QWidget* getWidget() const;

    /**
     * Get the parent as Qt widget. Can be NULL.
     *
     * \return the parent
     */
    QWidget* getParentAsQtWidget() const;

    /**
     * Parent widget. Can be NULL.
     *
     * \return the parent
     */
    WUIQtWidgetBase::SPtr getQtParent() const;
protected:
    /**
     * Realize the widget. This method blocks until the GUI thread created the widget. Called from within the GUI thread! So you can safely do Qt
     * stuff.
     */
    virtual void realizeImpl() = 0;

    /**
     * Show this widget if not yet visible. Called in GUI Thread (GT).
     */
    virtual void showGT();

    /**
     * Hide/show this widget. Unlike close(), you can show the widget again using show(). Called in GUI Thread (GT).
     *
     * \param visible false to hide widget
     */
    virtual void setVisibleGT( bool visible = true );

    /**
     * Check if the widget is hidden or not. Called in GUI Thread (GT).
     *
     * \return true if visible.
     */
    virtual bool isVisibleGT() const;

    /**
     * Close the widget. When done, the widget can be safely deleted.
     */
    virtual void closeImpl();

    /**
     * Close the widget. When done, the widget can be safely deleted. Called in GUI Thread (GT).
     */
    virtual void closeGT();

    /**
     * Clean up all the memory in Gui Thread.
     */
    virtual void cleanUpGT() = 0;

    /**
     * The main window instance.
     */
    WMainWindow* m_mainWindow;

    /**
     * The widget representing this abstract UI element.
     */
    QWidget* m_widget;

    /**
     * Parent widget. Can be NULL.
     */
    WUIQtWidgetBase::SPtr m_parent;

    /**
     * Returns the parent to use for your implementation in \ref realizeImpl. It is important to understand that this always returns a parent,
     * regardless of m_parent being NULL or not. This is the parent of your QWidget.
     *
     * \return the parent
     */
    QWidget* getCompellingQParent() const;

    /**
     * Check if the widget is embedded into another WUI widget.
     *
     * \return true if there is a WUIQtWidgetBase parent
     */
    bool hasUIParent() const;

    /**
     * This method can be used if you just create some QWidget and do not want to take care about embedding the content in a dock if there is no
     * parent, setting size constraints and similar. This method handles this. It is also aware of QDckWidgets. This means it does not embed
     * them if you create your own QDockWidgets. It then just sets the defaults and registers it at WMainWindow.
     *
     * \return the container (usually WQtDockWidget) or NULL if the content is used stand-alone.
     */
    QWidget* embedContent( QWidget* content );
private:
    /**
     * Forwards call from a boost function to the virtual realizeImpl method
     */
    void realizeGT();
};

#endif  // WUIQTWIDGETBASE_H

