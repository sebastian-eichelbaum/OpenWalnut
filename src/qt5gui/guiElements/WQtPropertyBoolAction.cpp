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

#include <QtGui/QApplication>

#include "core/common/WPropertyVariable.h"

#include "../events/WEventTypes.h"
#include "../events/WPropertyChangedEvent.h"

#include "WQtPropertyBoolAction.h"

WQtPropertyBoolAction::WQtPropertyBoolAction( WPropBool property, QWidget* parent ):
    QAction( parent ),
    m_boolProperty( property )
{
    setCheckable( true );

    setToolTip( QString::fromStdString( property->getDescription() ) );
    setText( QString::fromStdString( property->getName() ) );

    // initialize members
    update();

    m_connection = property->getUpdateCondition()->subscribeSignal( boost::bind( &WQtPropertyBoolAction::propertyChangeNotifier, this ) );

    // connect the modification signal of m_checkbox with our callback
    connect( this, SIGNAL( toggled( bool ) ), this, SLOT( changed() ) );
}

WQtPropertyBoolAction::~WQtPropertyBoolAction()
{
    // cleanup
}

void WQtPropertyBoolAction::update()
{
    // simply set the new state
    setChecked( m_boolProperty->get() );
}

void WQtPropertyBoolAction::changed()
{
    m_boolProperty->set( isChecked() );
    setChecked( m_boolProperty->get() );
}

void WQtPropertyBoolAction::propertyChangeNotifier()
{
    QCoreApplication::postEvent( this, new WPropertyChangedEvent() );
}

bool WQtPropertyBoolAction::event( QEvent* event )
{
    // a property changed
    if( event->type() == WQT_PROPERTY_CHANGED_EVENT )
    {
        update();
        return true;
    }

    return QAction::event( event );
}

