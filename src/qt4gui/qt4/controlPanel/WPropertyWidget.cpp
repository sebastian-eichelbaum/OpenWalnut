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

#include <string>

#include <QtGui/QApplication>

#include "../events/WEventTypes.h"
#include "../events/WPropertyChangedEvent.h"

#include "WPropertyWidget.h"
#include "WPropertyWidget.moc"

WPropertyWidget::WPropertyWidget(  boost::shared_ptr< WPropertyBase > property, QGridLayout* propertyGrid, QWidget* parent ):
    QStackedWidget( parent ),
    m_property( property ),
    m_propertyGrid( propertyGrid ),
    m_label( this ),
    m_useLabel( m_propertyGrid ),
    m_parameterWidgets(),       // parent gets set by the QStackWidget
    m_informationWidgets(),       // parent gets set by the QStackWidget
    m_invalid( false )
{
    if( m_useLabel )
    {
        // initialize members
        m_label.setText( property->getName().c_str() );
        // set tooltips
        m_label.setToolTip( getTooltip().c_str() );
        setToolTip( m_label.toolTip() );

        // setup grid layout
        int row = m_propertyGrid->rowCount();
        m_propertyGrid->addWidget( &m_label, row, 0 );
        m_propertyGrid->addWidget( this, row, 1 );
        m_propertyGrid->setColumnStretch( 0, 0.0 );
        m_propertyGrid->setColumnStretch( 1, 10000.0 );
    }

    // add both widgets to the stacked widget, it then uses the first as default.
    addWidget( &m_parameterWidgets );
    addWidget( &m_informationWidgets );

    // if the purpose of the property is INFORMTION -> activate the information widget
    if( m_property->getPurpose() == PV_PURPOSE_INFORMATION )
    {
        setCurrentIndex( 1 );
    }

    // if the property is hidden initially, hide widget too
    setHidden( m_property->isHidden() );
    m_label.setHidden( m_property->isHidden() );

    // setup the update callback
    m_connection = m_property->getUpdateCondition()->subscribeSignal( boost::bind( &WPropertyWidget::propertyChangeNotifier, this ) );
}

WPropertyWidget::~WPropertyWidget()
{
    // cleanup
    m_connection.disconnect();
}

void WPropertyWidget::propertyChangeNotifier()
{
    QCoreApplication::postEvent( this, new WPropertyChangedEvent() );
}

bool WPropertyWidget::event( QEvent* event )
{
    // a property changed
    if( event->type() == WQT_PROPERTY_CHANGED_EVENT )
    {
        setHidden( m_property->isHidden() );
        m_label.setHidden( m_property->isHidden() );
        update();
        return true;
    }

    return QWidget::event( event );
}

std::string WPropertyWidget::getTooltip() const
{
    std::string tip = "<b>Property: </b>" + m_property->getName() + "<br/>";
    tip += "<b>Status: </b>";
    tip += m_invalid ? "<font color=#FF0000><b>invalid</b></font>" : "valid";
    tip += "<br/><br/>";
    return tip + m_property->getDescription();
}

boost::shared_ptr< WPropertyBase > WPropertyWidget::getProperty()
{
    return m_property;
}

void WPropertyWidget::invalidate( bool invalid )
{
    m_invalid = invalid;

    if( m_useLabel )
    {
        // update tooltip
        m_label.setToolTip( getTooltip().c_str() );
        setToolTip( m_label.toolTip() );

        if( invalid )
        {
            m_label.setText( ( "<font color=#FF0000><b>" + m_property->getName() + "</b></font>" ).c_str() );
        }
        else
        {
            m_label.setText( m_property->getName().c_str() );
        }
    }
}

