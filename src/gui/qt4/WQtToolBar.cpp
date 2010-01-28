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

#include "WQtToolBar.h"


WQtToolBar::WQtToolBar( QWidget* parent )
    : QToolBar( parent )
{
    this->setAllowedAreas( Qt::TopToolBarArea | Qt::BottomToolBarArea );

    // The following makes the bar having button size from the beginning.
    QPushButton* dummyButton = new QPushButton;
    dummyButton->setFixedWidth( 0 );
    addWidget( dummyButton );
}

WQtToolBar::~WQtToolBar()
{
}

WQtPushButton* WQtToolBar::addPushButton( QString name, QIcon icon, QString label )
{
    WQtPushButton* button = new WQtPushButton( icon, label );

    button->setName( name );

    addWidget( button );

    return button;
}

void WQtToolBar::clearButtons()
{
    clear();

    // The following prevents the bar from changing size when it has no real buttons.
    QPushButton* dummyButton = new QPushButton;
    dummyButton->setFixedWidth( 0 );
    addWidget( dummyButton );
}
