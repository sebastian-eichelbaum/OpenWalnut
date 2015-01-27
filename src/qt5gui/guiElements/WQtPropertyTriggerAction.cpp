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

#include <QApplication>

#include "core/common/WPropertyVariable.h"

#include "../events/WEventTypes.h"
#include "../events/WPropertyChangedEvent.h"

#include "WQtPropertyTriggerAction.h"

WQtPropertyTriggerAction::WQtPropertyTriggerAction( WPropTrigger property, QWidget* parent ):
    QAction( parent ),
    m_triggerProperty( property )
{
    setCheckable( true );

    setToolTip( QString::fromStdString( property->getDescription() ) );
    setText( QString::fromStdString( property->getName() ) );

    // initialize members
    update();

    m_connection = property->getUpdateCondition()->subscribeSignal( boost::bind( &WQtPropertyTriggerAction::propertyChangeNotifier, this ) );

    // connect the modification signal of m_checkbox with our callback
    connect( this, SIGNAL( toggled( bool ) ), this, SLOT( changed() ) );
}

WQtPropertyTriggerAction::~WQtPropertyTriggerAction()
{
    // cleanup
}

void WQtPropertyTriggerAction::update()
{
    // simply set the new state
    setChecked( m_triggerProperty->get() == WPVBaseTypes::PV_TRIGGER_TRIGGERED );
}

void WQtPropertyTriggerAction::changed()
{
    // the module should reset to PV_TRIGGER_READY
    if( !isChecked() )
    {
        setChecked( true );
        return;
    }
    m_triggerProperty->set( isChecked() ? WPVBaseTypes::PV_TRIGGER_TRIGGERED : WPVBaseTypes::PV_TRIGGER_READY );
    setChecked( m_triggerProperty->get() );
}

void WQtPropertyTriggerAction::propertyChangeNotifier()
{
    QCoreApplication::postEvent( this, new WPropertyChangedEvent() );
}

bool WQtPropertyTriggerAction::event( QEvent* event )
{
    // a property changed
    if( event->type() == WQT_PROPERTY_CHANGED_EVENT )
    {
        update();
        return true;
    }

    return QAction::event( event );
}

