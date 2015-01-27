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

#include "../WGuiConsts.h"
#include "core/common/WAssert.h"
#include "WPropertyStructWidget.h"

WPropertyStructWidget::WPropertyStructWidget( WPropertyGroupBase::SPtr property, QGridLayout* propertyGrid, QWidget* parent ):
    WPropertyWidget( property, propertyGrid, parent ),
    m_struct( property ),
    m_layout( &m_parameterWidgets )
{
    // although we could also work with other group types, we limit this widget to PV_STRUCT properties, because they have fixed size and cannot
    // be modified somehow. If you remove this assert, ensure the widget is able to handle dynamic updates in the group!
    WAssert( property->getType() == PV_STRUCT, "WPropertyStructWidget is only compatible with WPropertyStruct instances." );

    // go through the widgets
    WPropertyGroupBase::PropertySharedContainerType::ReadTicket r = property->getProperties();
    for( WPropertyGroupBase::PropertyConstIterator iter = r->get().begin(); iter != r->get().end(); ++iter )
    {
        WAssert( ( *iter )->getType() != PV_GROUP, "WPropertyStructWidget does not support PV_GROUP properties in a struct." );

        // create the widget. Leave the control layout NULL to only get the info widget and parameter widget without label
        WPropertyWidget* w = WPropertyWidget::construct( *iter, NULL, this );
        WAssert( w, "WPropertyStructWidget does not support other unsupported properties in a struct." );

        // add the widgets
        m_layout.addWidget( w );

        // store the list of widgets to later forward the update calls
        m_widgets.push_back( w );
    }

    // layout both against each other
    m_layout.setMargin( WGLOBAL_MARGIN );
    m_layout.setSpacing( WGLOBAL_SPACING );
    m_parameterWidgets.setLayout( &m_layout );

    // NOTE: an info layout is not needed. This is automatically done by the child widgets
}

WPropertyStructWidget::~WPropertyStructWidget()
{
    // cleanup
}

void WPropertyStructWidget::update()
{
    for( WidgetList::const_iterator iter = m_widgets.begin(); iter != m_widgets.end(); ++iter )
    {
        ( *iter )->requestUpdate();
    }
}

