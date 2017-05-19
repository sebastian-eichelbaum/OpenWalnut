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

#ifndef WUIPROPERTYGROUPWIDGET_H
#define WUIPROPERTYGROUPWIDGET_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "core/common/WPropertyTypes.h"

#include "WUIWidgetBase.h"

/**
 * Widget which is a container for a WPropertyGroup.
 */
class WUIPropertyGroupWidget: public WUIWidgetBase
{
public:
    /**
     * Convenience typedef for a boost::shared_ptr< WUIPropertyGroupWidget >.
     */
    typedef boost::shared_ptr< WUIPropertyGroupWidget > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WUIPropertyGroupWidget >.
     */
    typedef boost::shared_ptr< const WUIPropertyGroupWidget > ConstSPtr;

    /**
     * Destructor.
     */
    virtual ~WUIPropertyGroupWidget();

    /**
     * The property group shown by this widget.
     *
     * \return the group
     */
    virtual WPropGroup getPropertyGroup() const = 0;

protected:
    /**
     * Default constructor. Create an property widget.
     *
     * \param title the widget title
     */
    explicit WUIPropertyGroupWidget( std::string title );
private:
};

#endif  // WUIPROPERTYGROUPWIDGET_H

