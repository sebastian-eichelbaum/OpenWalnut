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

#ifndef WUIQTWIDGETFACTORY_H
#define WUIQTWIDGETFACTORY_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "core/common/WCondition.h"

#include "core/ui/WUIWidgetFactory.h"

class WMainWindow;
class WUIQtWidgetBase;

/**
 * Implementation of \ref WUIWidgetFactory.
 */
class WUIQtWidgetFactory: public WUIWidgetFactory
{
public:
    /**
     * Convenience typedef for a boost::shared_ptr< WUIQtWidgetFactory >.
     */
    typedef boost::shared_ptr< WUIQtWidgetFactory > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WUIQtWidgetFactory >.
     */
    typedef boost::shared_ptr< const WUIQtWidgetFactory > ConstSPtr;

    /**
     * Default constructor.
     *
     * \param mainWindow the main window instance
     */
    explicit WUIQtWidgetFactory( WMainWindow* mainWindow );

    /**
     * Destructor.
     */
    virtual ~WUIQtWidgetFactory();

    /**
     * Convert the given WUIWidgetBase to the corresponding WUIQtWidgetBase.
     *
     * \param widget the widget to convert
     *
     * \return the converted widget.
     */
    static boost::shared_ptr< WUIQtWidgetBase > getAsQtWidgetBase( WUIWidgetBase::SPtr widget );
protected:
    /**
     * Create a grid widget. This kind of widget is basically empty. Add others to it. The widgets are initially invisible. Use
     * WUIWidgetBase::show() to make them visible.
     * If a widget with this name already exists, it will be returned.
     *
     * \note this function blocks until the widget was created. Check the resulting pointer for NULL.
     *
     * \param title the title
     * \param parent the parent widget which will contain this widget. Can be NULL.
     *
     * \return the widget. Might be NULL if something goes wrong.
     */
    virtual WUIGridWidget::SPtr createGridWidgetImpl( const std::string& title, WUIWidgetBase::SPtr parent = WUIWidgetBase::SPtr() ) const;

    /**
     * Create a property widget. Parentless widgets are initially invisible. Use
     * WUIWidgetBase::show() to make them visible.
     *
     * \note this function blocks until the widget was created. Check the resulting pointer for NULL.
     *
     * \param title the title
     * \param properties the property group
     * \param parent the parent widget which will contain this widget. Can be NULL.
     *
     * \return the widget. Might be NULL if something goes wrong.
     */
    virtual WUIPropertyGroupWidget::SPtr createPropertyGroupWidgetImpl( const std::string& title, WPropGroup properties,
                                                                        WUIWidgetBase::SPtr parent = WUIWidgetBase::SPtr() ) const;

    /**
     * Instruct to open a new view widget. The specified condition should be the shutdown condition of the module, as the function returns only
     * if the widget was created. To ensure that the creation is aborted properly if the module shuts down in the meantime, this condition is
     * used. The widgets are initially invisible. Use WUIWidgetBase::show() to make them visible.
     * If a widget with this name already exists, it will be returned.
     *
     * \note this function blocks until the widget was created. Check the resulting pointer for NULL.
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
            WUIWidgetBase::SPtr parent = WUIWidgetBase::SPtr() ) const;
private:
    /**
     * The main window instance.
     */
    WMainWindow* m_mainWindow;
};

#endif  // WUIQTWIDGETFACTORY_H

