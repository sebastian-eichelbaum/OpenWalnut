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

#ifndef WQTWIDGETBASE_H
#define WQTWIDGETBASE_H

#include <boost/shared_ptr.hpp>

#include "core/common/WCondition.h"

#include "../WMainWindow.h"

/**
 * Implementation of \ref WUIWidgetBase. Not really. It is an base class for all WUIWidgetBase implementations to comfortably allow thread-save
 * GUI ops.
 */
class WQtWidgetBase
{
public:

    /**
     * Convenience typedef for a boost::shared_ptr< WQtWidgetBase >.
     */
    typedef boost::shared_ptr< WQtWidgetBase > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WQtWidgetBase >.
     */
    typedef boost::shared_ptr< const WQtWidgetBase > ConstSPtr;

    /**
     * Default constructor.
     *
     * \param mainWindow the main window instance
     */
    explicit WQtWidgetBase( WMainWindow* mainWindow );

    /**
     * Destructor.
     */
    virtual ~WQtWidgetBase();

    /**
     * Realize the widget. This method blocks until the GUI thread created the widget.
     *
     * \param abortCondition a condition enforcing abort of widget creation.
     */
    virtual void realize( boost::shared_ptr< WCondition > abortCondition );
protected:
    /**
     * Realize the widget. This method blocks until the GUI thread created the widget. Called from within the GUI thread! So you can safely do Qt
     * stuff.
     */
    virtual void realizeImpl() = 0;

    /**
     * Forwards call from a boost function to the virtual realizeImpl method
     */
    void callMe();

    /**
     * The main window instance.
     */
    WMainWindow* m_mainWindow;
private:
};

#endif  // WQTWIDGETBASE_H

