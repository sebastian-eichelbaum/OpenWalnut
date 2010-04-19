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

#include <cmath>
#include <string>

#include <boost/lexical_cast.hpp>

#include "../../../common/WLogger.h"
#include "../../../common/WPropertyVariable.h"

#include "WPropertyDoubleWidget.h"

WPropertyDoubleWidget::WPropertyDoubleWidget( WPropDouble property, QGridLayout* propertyGrid, QWidget* parent ):
    WPropertyWidget( property, propertyGrid, parent ),
    m_doubleProperty( property ),
    m_spin( &m_parameterWidgets ),
    m_layout( &m_parameterWidgets )
{
    // initialize members
    m_parameterWidgets.setLayout( &m_layout );

    // layout both against each other
    m_layout.addWidget( &m_spin );

    update();

    // connect the modification signal of the edit and slider with our callback
    connect( &m_spin, SIGNAL( editingFinished() ), this, SLOT( spinChanged() ) );
}

WPropertyDoubleWidget::~WPropertyDoubleWidget()
{
    // cleanup
}

void WPropertyDoubleWidget::update()
{
    // get the min constraint
    WPVDouble::PropertyConstraintMin minC = m_doubleProperty->getMin();
    double min = 0.0;
    if ( minC.get() )
    {
        min = minC->getMin();
    }
    else
    {
        WLogger::getLogger()->addLogMessage(
                std::string( "The property has no minimum constraint. You should define it to avoid unexpected behaviour." ) +
                std::string( "Using default (" + boost::lexical_cast< std::string >( min ) + ")." ),
                "PropertyWidget( " + m_doubleProperty->getName() + " )", LL_WARNING );
    }

    // get the max constraint
    WPVDouble::PropertyConstraintMax maxC = m_doubleProperty->getMax();
    double max = 100.0;
    if ( maxC.get() )
    {
        max = maxC->getMax();
    }
    else
    {
        WLogger::getLogger()->addLogMessage(
                std::string( "The property has no maximum constraint. You should define it to avoid unexpected behaviour." ) +
                std::string( "Using default (" + boost::lexical_cast< std::string >( max ) + ")." ),
                "PropertyWidget( " + m_doubleProperty->getName() + " )", LL_WARNING );
    }

    // setup the slider
    m_spin.setMinimum( min );
    m_spin.setMaximum( max );

    // set display precision
    m_spin.setDecimals( 5 );

    // set the initial values
    m_spin.setValue( m_doubleProperty->get() );
    m_spin.setSingleStep( ( max - min ) / 100.0 );
}

void WPropertyDoubleWidget::spinChanged()
{
    // set to the property
    invalidate( !m_doubleProperty->set( m_spin.value() ) );
}

