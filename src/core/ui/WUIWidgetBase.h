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

#ifndef WUIWIDGETBASE_H
#define WUIWIDGETBASE_H

#include <string>
#include <vector>

#include "core/common/WCondition.h"
#include "core/common/WProperties.h"
#include "core/common/WSharedSequenceContainer.h"
#include "core/graphicsEngine/WGEImage.h"

#include <boost/shared_ptr.hpp>

/**
 * Base class for all the widget abstraction the core UI provides. All the abstract widgets use the bridge pattern to let the UI/GUI implementor
 * actually handle everything.
 *
 * \note Please read documentation of \ref WUIWidgetFactory for limitations, requirements and creation of these widgets.
 */
class WUIWidgetBase
{
    friend class WUIWidgetFactory; //!< The factory needs to be able to call the constructor
public:
    /**
     * Convenience typedef for a boost::shared_ptr< WUIWidgetBase >.
     */
    typedef boost::shared_ptr< WUIWidgetBase > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WUIWidgetBase >.
     */
    typedef boost::shared_ptr< const WUIWidgetBase > ConstSPtr;

    /**
     * Destructor.
     */
    virtual ~WUIWidgetBase();

    /**
     * Get the title of the widget.
     *
     * \return title as string
     */
    virtual std::string getTitle() const;

    /**
     * Show this widget if not yet visible.
     */
    virtual void show() = 0;

    /**
     * Hide/show this widget. Unlike close(), you can show the widget again using show().
     *
     * \param visible false to hide widget
     */
    virtual void setVisible( bool visible = true ) = 0;

    /**
     * Check if the widget is hidden or not.
     *
     * \return true if visible.
     */
    virtual bool isVisible() const = 0;

    /**
     * Close the widget. When done, the widget can be safely deleted. You cannot re-open the widget with \ref show(). If you want to hide widget,
     * use setVisible( false ) instead. This function cascades to all child widgets if any. So it is enough to use this for the top-level
     * (parent-less) widget only. This ensures proper close for all nested widgets.
     */
    void close();

    /**
     * Checks whether the widget was closed already.
     *
     * \return true if already closed
     */
    bool isClosed() const;

    /**
     * Return the condition that fires when the widgets closes.
     *
     * \return the condition fired whenever the widget closes
     */
    WCondition::SPtr getCloseCondition() const;

    /**
     * Get the parent widget of this widget if any. Can be NULL.
     *
     * \return the parent
     */
    virtual WUIWidgetBase::SPtr getParent() const;

    /**
     * Tell the user whether this kind of widget can be used as parent. In other word, whether it is allowed to nest other widgets into this one.
     *
     * \return true if nesting other widgets INTO this one is allowed
     */
    virtual bool allowNesting() const;

    /**
     * Define a property as action. Depending on the UI implementing this, adding actions to a nested widget can be useless. View the
     * corresponding WUI*Widget doc for details. Order of adding is order of appearance in the UI/GUI.
     *
     * \note They might not be visible. Only top-level widgets guarantee to show these properly.
     *
     * \param group the property to use.
     * \param icon the icon to use. Consider a minimum size of 32x32.
     */
    virtual void addAction( WPropGroup group, WGEImage::SPtr icon = WGEImage::SPtr() ) = 0;

    /**
     * Define a property as action. Depending on the UI implementing this, adding actions to a nested widget can be useless. View the
     * corresponding WUI*Widget doc for details. Order of adding is order of appearance in the UI/GUI.
     *
     * \note They might not be visible. Only top-level widgets guarantee to show these properly.
     *
     * \param trigger the property to use
     * \param icon the icon to use. Consider a minimum size of 32x32.
     */
    virtual void addAction( WPropTrigger trigger, WGEImage::SPtr icon = WGEImage::SPtr() ) = 0;

    /**
     * Define a property as action. Depending on the UI implementing this, adding actions to a nested widget can be useless. View the
     * corresponding WUI*Widget doc for details. Order of adding is order of appearance in the UI/GUI.
     *
     * \note They might not be visible. Only top-level widgets guarantee to show these properly.
     *
     * \param toggle the property to use
     * \param icon the icon to use. Consider a minimum size of 32x32.
     */
    virtual void addAction( WPropBool toggle, WGEImage::SPtr icon = WGEImage::SPtr() ) = 0;

protected:
    /**
     * Default constructor.
     *
     * \param title the title of the widget
     */
    explicit WUIWidgetBase( std::string title );

    /**
     * Close the widget. When done, the widget can be safely deleted. You cannot re-open the widget with \ref show(). If you want to hide widget,
     * use setVisible( false ) instead.
     */
    virtual void closeImpl() = 0;

    /**
     * Set the parent of this WUI widget. Do never call this after the widget was realized by the factory. This method is used by the factory. Do
     * not manually call it.
     *
     * \param parent the parent. Can be NULL if none.
     */
    virtual void setParent( WUIWidgetBase::SPtr parent );

    /**
     * Register widget as child. Called by the widget factory.
     *
     * \param child the child.
     */
    virtual void registerChild( WUIWidgetBase::SPtr child );

    /**
     * Keep track of our child widgets. Please note that we store shared_ptr here. This means, no widget ever gets deleted by the shared_ptr
     * accidentally in a module. All widgets keeps reference of their parent, and each widget keeps reference to its childs. Only a call to close
     * will recursively free the widgets.
     */
    typedef WSharedSequenceContainer< std::vector< WUIWidgetBase::SPtr > > ChildContainer;

    /**
     * Return the list of children.
     *
     * \return the children
     */
    ChildContainer& getChildren();

    /**
     * Return the list of children.
     *
     * \return the children
     */
    const ChildContainer& getChildren() const;

    /**
     * Comfortable function to recurse a close call. This iterates the children and closes them.
     */
    void closeChildren();

private:
    /**
     * The widget's title string.
     */
    std::string m_title;

    /**
     * Close condition. Notified when widget closes.
     */
    WCondition::SPtr m_closeCondition;

    /**
     * The parent. Can be NULL.
     */
    WUIWidgetBase::SPtr m_parent;

    /**
     * Child widgets.
     */
    ChildContainer m_childs;

    /**
     * Flag denotes whether the widget was closed already.
     */
    bool m_closed;
};

#endif  // WUIWIDGETBASE_H
