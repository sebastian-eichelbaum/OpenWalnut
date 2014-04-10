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

#ifndef WUIGRIDWIDGET_H
#define WUIGRIDWIDGET_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "WUIWidgetBase.h"

/**
 * Widget which is a container for others. The child widgets can be arranged in a grid. The WUIGridWidget itself is a widget, which means you can
 * embed grids in grid cells.
 */
class WUIGridWidget: public WUIWidgetBase
{
    friend class WUI;
public:
    /**
     * Convenience typedef for a boost::shared_ptr< WUIGridWidget >.
     */
    typedef boost::shared_ptr< WUIGridWidget > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WUIGridWidget >.
     */
    typedef boost::shared_ptr< const WUIGridWidget > ConstSPtr;

    /**
     * Destructor.
     */
    virtual ~WUIGridWidget();

    /**
     * Tell the user whether this kind of widget can be used as parent. In other word, whether it is allowed to nest other widgets into this one.
     *
     * \return true if nesting other widgets INTO this one is allowed
     */
    virtual bool allowNesting() const;

    /**
     * Place the given widget in this grid at the given coordinates. The widget to be placed must be created with this grid as parent or an
     * exception will be thrown.
     *
     * \param widget the widget
     * \param x x coord ( 0 is left )
     * \param y y coord ( 0 is top )
     */
    virtual void placeWidget( WUIWidgetBase::SPtr widget, int x, int y );

    /**
     * Sets the stretch factor of a row to stretch. The first row is number 0. The stretch factor is relative to the other rows in this grid.
     * Rows with a higher stretch factor take more of the available space. The default stretch factor is 0. If the stretch factor is 0 and no
     * other row in this table can grow at all, the row may still grow.
     *
     * \param row the row to set this value for
     * \param stretch the stretch
     */
    virtual void setRowStretch( int row, int stretch ) = 0;

    /**
     * Sets the stretch factor of a column to stretch. The first column is number 0. The stretch factor is relative to the other columns in this grid.
     * Columns with a higher stretch factor take more of the available space. The default stretch factor is 0. If the stretch factor is 0 and no
     * other column in this table can grow at all, the column may still grow.
     *
     * \param column the column to set this value for
     * \param stretch the stretch
     */
    virtual void setColumnStretch( int column, int stretch ) = 0;
protected:
    /**
     * Default constructor. Create an empty grid widget.
     *
     * \param title the widget title
     */
    explicit WUIGridWidget( std::string title );

    /**
     * Place the given widget in this grid at the given coordinates. The widget to be placed must be created with this grid as parent.
     *
     * \param widget the widget
     * \param x x coord ( 0 is left )
     * \param y y coord ( 0 is top )
     */
    virtual void placeWidgetImpl( WUIWidgetBase::SPtr widget, int x, int y ) = 0;
private:
};

#endif  // WUIGRIDWIDGET_H

