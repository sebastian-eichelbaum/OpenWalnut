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

#include "WUIGridWidget.h"
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
     * Create a grid widget. This kind of widget is basically empty. Add others to it. The widgets are initially invisible. Use
     * WUIWidgetBase::show() to make them visible.
     * If a widget with this name already exists, it will be returned.
     *
     * \note this function blocks until the widget was created. Check the resulting pointer for NULL.
     *
     * \param title the title
     *
     * \return the widget. Might be NULL if something goes wrong.
     */
    virtual WUIGridWidget::SPtr createGridWidget( const std::string& title ) const = 0;

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
     * \param shutdownCondition a condition enforcing abort of widget creation.
     *
     * \return the created widget
     */
    virtual WUIViewWidget::SPtr createViewWidget(
            std::string title,
            WGECamera::ProjectionMode projectionMode,
            boost::shared_ptr< WCondition > shutdownCondition ) const = 0;

    /**
     * Destructor.
     */
    virtual ~WUIWidgetFactory()
    {
    }
protected:
private:
};

#endif  // WUIWIDGETFACTORY_H

