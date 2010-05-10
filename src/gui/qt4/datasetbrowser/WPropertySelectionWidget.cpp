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

#include "WPropertySelectionWidget.h"

WPropertySelectionWidget::WPropertySelectionWidget( WPropSelection property, QGridLayout* propertyGrid, QWidget* parent ):
    WPropertyWidget( property, propertyGrid, parent ),
    m_selectionProperty( property ),
    m_list( &m_parameterWidgets ),
    m_combo( &m_parameterWidgets ),
    m_layout( &m_parameterWidgets )
{
    // initialize members
    m_parameterWidgets.setLayout( &m_layout );

    // layout
    m_layout.addWidget( &m_combo );

    // set the initial values
    update();
}

WPropertySelectionWidget::~WPropertySelectionWidget()
{
    // cleanup
}

void WPropertySelectionWidget::update()
{
}


