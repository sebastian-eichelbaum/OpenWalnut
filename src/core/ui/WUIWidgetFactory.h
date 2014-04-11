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

#ifndef WUIWIDGETFACTORY_H
#define WUIWIDGETFACTORY_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "core/common/WException.h"
#include "core/common/WPropertyTypes.h"

#include "WUIGridWidget.h"
#include "WUITabbedWidget.h"
#include "WUIPropertyGroupWidget.h"
#include "WUIViewWidget.h"

/**
 * Create instances of WUI widgets. This needs to be implemented by the UI/GUI developer.
 */
class WUIWidgetFactory
{
public:
    /**
     * Convenience typedef for a boost::shared_ptr< WUIWidgetFactory >.
     */
    typedef boost::shared_ptr< WUIWidgetFactory > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WUIWidgetFactory >.
     */
    typedef boost::shared_ptr< const WUIWidgetFactory > ConstSPtr;

    /**
     * Create a grid widget. This kind of widget is basically empty. Add others to it. Parentless widgets are initially invisible. Use
     * WUIWidgetBase::show() to make them visible.
     *
     * \note this function blocks until the widget was created. Check the resulting pointer for NULL.
     * \throw WException if something was wrong (like parent does not allow nesting). You need to catch this.
     *
     * \param title the title
     * \param parent the parent widget which will contain this widget. Can be NULL.
     *
     * \return the widget. Might be NULL if something goes wrong.
     */
    virtual WUIGridWidget::SPtr createGridWidget( const std::string& title, WUIWidgetBase::SPtr parent = WUIWidgetBase::SPtr() ) const
    {
        if( parent )
        {
            if( !parent->allowNesting() )
            {
                throw WException( "Parent of widget \"" + title + "\" does not allow nesting." );
            }
        }
        return createGridWidgetImpl( title, parent );
    }

    /**
     * Create a tabed widget. This kind of widget is basically empty. Add others to it. Parentless widgets are initially invisible. Use
     * WUIWidgetBase::show() to make them visible.
     *
     * \note this function blocks until the widget was created. Check the resulting pointer for NULL.
     * \throw WException if something was wrong (like parent does not allow nesting). You need to catch this.
     *
     * \param title the title
     * \param parent the parent widget which will contain this widget. Can be NULL.
     *
     * \return the widget. Might be NULL if something goes wrong.
     */
    virtual WUITabbedWidget::SPtr createTabbedWidget( const std::string& title, WUIWidgetBase::SPtr parent = WUIWidgetBase::SPtr() ) const
    {
        if( parent )
        {
            if( !parent->allowNesting() )
            {
                throw WException( "Parent of widget \"" + title + "\" does not allow nesting." );
            }
        }
        return createTabbedWidgetImpl( title, parent );
    }


    /**
     * Create a property widget. Parentless widgets are initially invisible. Use
     * WUIWidgetBase::show() to make them visible.
     *
     * \note this function blocks until the widget was created. Check the resulting pointer for NULL.
     * \throw WException if something was wrong (like parent does not allow nesting). You need to catch this.
     *
     * \param title the title
     * \param properties the property group
     * \param parent the parent widget which will contain this widget. Can be NULL.
     *
     * \return the widget. Might be NULL if something goes wrong.
     */
    virtual WUIPropertyGroupWidget::SPtr createPropertyGroupWidget( const std::string& title, WPropGroup properties,
                                                                    WUIWidgetBase::SPtr parent = WUIWidgetBase::SPtr() ) const
    {
        if( parent )
        {
            if( !parent->allowNesting() )
            {
                throw WException( "Parent of widget \"" + title + "\" does not allow nesting." );
            }
        }
        return createPropertyGroupWidgetImpl( title, properties, parent );
    }

    /**
     * Instruct to open a new view widget. The specified condition should be the shutdown condition of the module, as the function returns only
     * if the widget was created. To ensure that the creation is aborted properly if the module shuts down in the meantime, this condition is
     * used. Parentless widgets are initially invisible. Use WUIWidgetBase::show() to make them visible.
     *
     * \note this function blocks until the widget was created. Check the resulting pointer for NULL.
     *
     * \throw WException if something was wrong (like parent does not allow nesting). You need to catch this.
     *
     * \param title the title of the widget
     * \param projectionMode the kind of projection which should be used
     * \param abordCondition a condition enforcing abort of widget creation. Can be NULL
     * \param parent the parent widget which will contain this widget. Can be NULL.
     *
     * \return the created widget
     */
    virtual WUIViewWidget::SPtr createViewWidget(
            std::string title,
            WGECamera::ProjectionMode projectionMode,
            boost::shared_ptr< WCondition > abordCondition = WCondition::SPtr(),
            WUIWidgetBase::SPtr parent = WUIWidgetBase::SPtr() ) const
    {
        if( parent )
        {
            if( !parent->allowNesting() )
            {
                throw WException( "Parent of widget \"" + title + "\" does not allow nesting." );
            }
        }
        return createViewWidgetImpl( title, projectionMode, abordCondition, parent );
    }

    /**
     * Destructor.
     */
    virtual ~WUIWidgetFactory()
    {
    }

    /**
     * Query whether the WUI instance supports the WUI Widget interface properly as UIs can simply ignore the WUIWidgetFactory calls done in
     * modules. This is used mainly by \ref WUIRequirement.
     *
     * \return true if the UI properly implements the WUI widget interface.
     */
    virtual bool implementsUI() const = 0;
protected:
    /**
     * Set the parent of a widget and notify parent about new child widget.
     * This is needed as WUIWidgetBase and WUIWidgetFactory are friends. Friendship is not derivable.
     *
     * \param widget the widget to set the parent to
     * \param parent the parent
     */
    void setParent( WUIWidgetBase::SPtr widget, WUIWidgetBase::SPtr parent ) const
    {
        if( parent )
        {
            widget->setParent( parent );

            // NOTE: the parent keeps a shared_ptr of its child. This avoids deletion of widgets. Only a call to WUIWidgetBase::close() cleans this
            // up and thus releases the shared_ptr properly.
            parent->registerChild( widget );
        }
    }

    /**
     * Implementation of \ref createGridWidget.
     *
     * \param title the title
     * \param parent the parent widget which will contain this widget. Can be NULL.
     *
     * \return the widget. Might be NULL if something goes wrong.
     */
    virtual WUIGridWidget::SPtr createGridWidgetImpl( const std::string& title, WUIWidgetBase::SPtr parent = WUIWidgetBase::SPtr() ) const = 0;

    /**
     * Implementation of \ref createTabbedWidget.
     *
     * \param title the title
     * \param parent the parent widget which will contain this widget. Can be NULL.
     *
     * \return the widget. Might be NULL if something goes wrong.
     */
    virtual WUITabbedWidget::SPtr createTabbedWidgetImpl( const std::string& title, WUIWidgetBase::SPtr parent = WUIWidgetBase::SPtr() ) const = 0;

    /**
     * Implementation of \ref createPropertyGroupWidget.
     *
     * \param title the title
     * \param properties the property group
     * \param parent the parent widget which will contain this widget. Can be NULL.
     *
     * \return the widget. Might be NULL if something goes wrong.
     */
    virtual WUIPropertyGroupWidget::SPtr createPropertyGroupWidgetImpl( const std::string& title, WPropGroup properties,
                                                                        WUIWidgetBase::SPtr parent = WUIWidgetBase::SPtr() ) const = 0;

    /**
     * Implementation of \ref createViewWidget.
     *
     * \param title the title of the widget
     * \param projectionMode the kind of projection which should be used
     * \param abordCondition a condition enforcing abort of widget creation. Can be NULL
     * \param parent the parent widget which will contain this widget. Can be NULL.
     *
     * \return the created widget
     */
    virtual WUIViewWidget::SPtr createViewWidgetImpl(
            std::string title,
            WGECamera::ProjectionMode projectionMode,
            boost::shared_ptr< WCondition > abordCondition = WCondition::SPtr(),
            WUIWidgetBase::SPtr parent = WUIWidgetBase::SPtr() ) const = 0;

private:
};

#endif  // WUIWIDGETFACTORY_H

