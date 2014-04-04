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

#include <boost/function.hpp>

#include "WQtWidgetBase.h"

WQtWidgetBase::WQtWidgetBase( WMainWindow* mainWindow ):
    m_mainWindow( mainWindow )
{
    // initialize members
}

WQtWidgetBase::~WQtWidgetBase()
{
    // cleanup
}

void WQtWidgetBase::realize( boost::shared_ptr< WCondition > /* abortCondition */ )
{
    m_mainWindow->execInGUIThread( boost::bind( &WQtWidgetBase::callMe, this ) );

    // FIXME: somehow incorporate the abortCondition

    /*WConditionSet conditionSet;
    conditionSet.setResetable( true, false );
    conditionSet.add( shutdownCondition );

    boost::shared_ptr< WFlag< boost::shared_ptr< WUIViewWidget > > > widgetFlag(
        new WFlag< boost::shared_ptr< WUIViewWidget > >( new WConditionOneShot, boost::shared_ptr< WUIViewWidget >() ) );
    conditionSet.add( widgetFlag->getCondition() );

    QCoreApplication::postEvent( m_mainWindow, new WOpenCustomDockWidgetEvent( title, projectionMode, widgetFlag ) );

    conditionSet.wait();
    return widgetFlag->get();*/
}

void WQtWidgetBase::callMe()
{
    realizeImpl();
}
