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

#include "core/common/WLogger.h"

#include "../WQt4Gui.h"

#include "WQtWidgetBase.h"

WQtWidgetBase::WQtWidgetBase( WMainWindow* mainWindow ):
    m_mainWindow( mainWindow ),
    m_widget( NULL )
{
    // initialize members
}

WQtWidgetBase::~WQtWidgetBase()
{
    // cleanup
    if( m_widget )
    {
        delete m_widget;
    }
}

void WQtWidgetBase::realize( boost::shared_ptr< WCondition > abortCondition )
{
    WConditionSet conditionSet;
    conditionSet.setResetable( true, false );
    // there are several events we want to wait for:
    // 1) the caller of this function does not want to wait anymore:
    conditionSet.add( abortCondition );

    // 2) the execution call was done:
    WCondition::SPtr doneNotify( new WConditionOneShot() );
    conditionSet.add( doneNotify );

    // use the UI to do the GUI thread call
    WQt4Gui::execInGUIThreadAsync( boost::bind( &WQtWidgetBase::realizeGT, this ), doneNotify );

    // use the UI to do the GUI thread call
    WQt4Gui::execInGUIThreadAsync( boost::bind( &WMainWindow::registerCustomWidget, m_mainWindow, this ) );

    // wait ...
    conditionSet.wait();
}

void WQtWidgetBase::realizeGT()
{
    realizeImpl();
    if( m_widget )
    {
        // ensure the widget is deleted when it was closed.
        //m_widget->setAttribute( Qt::WA_DeleteOnClose, true );
    }
}

bool WQtWidgetBase::isReal()
{
    return ( m_widget );
}

void WQtWidgetBase::show()
{
    if( m_widget )
    {
        WQt4Gui::execInGUIThread( boost::bind( &WQtWidgetBase::showGT, this ) );
    }
}

void WQtWidgetBase::setVisible( bool visible )
{
    if( m_widget )
    {
        WQt4Gui::execInGUIThread( boost::bind( &WQtWidgetBase::setVisibleGT, this, visible ) );
    }
}

/**
 * Simple function to query the visibility in GUI thread.
 */
struct IsVisibleFunctor
{
    /**
     * Functor queries the visibility in GUI thread
     */
    void operator()()
    {
        m_return = m_widget->isVisible();
    }

    /**
     * The widget to query.
     */
    QWidget* m_widget;

    /**
     * The return value.
     */
    bool m_return;
};

bool WQtWidgetBase::isVisible() const
{
    if( m_widget )
    {
        IsVisibleFunctor f;
        f.m_widget = m_widget;
        WQt4Gui::execInGUIThread( f );
        return f.m_return;
    }
    return false;
}

void WQtWidgetBase::close()
{
    if( m_widget )
    {
        WQt4Gui::execInGUIThread( boost::bind( &WQtWidgetBase::closeGT, this ) );
    }
}

void WQtWidgetBase::showGT()
{
    m_widget->show();
}

void WQtWidgetBase::setVisibleGT( bool visible )
{
    m_widget->setVisible( visible );
}

bool WQtWidgetBase::isVisibleGT() const
{
    return m_widget->isVisible();
}

void WQtWidgetBase::closeGT()
{
    onClose();
    m_mainWindow->deregisterCustomWidget( this );
    // NOTE: due to the WA_DeleteOnClose attribute, the widget gets deleted properly on close
    m_widget->close();
    cleanUpGT();    // we do not delete the widget our self ... let the bridge implementor do it.
    m_widget = NULL;
}

void WQtWidgetBase::guiShutDown()
{
    if( m_widget )
    {
        closeGT();
    }
}

void WQtWidgetBase::onClose()
{
    // do nothing for now
}
