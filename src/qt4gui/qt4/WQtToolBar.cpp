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

#include <list>

#include <QtGui/QAction>

#include "WMainWindow.h"

#include "WQtToolBar.h"

WQtToolBar::WQtToolBar( const QString & title, WMainWindow* parent )
    : WQtToolBarBase( title, parent )
{
}

WQtToolBar::~WQtToolBar()
{
}

void WQtToolBar::addAction( QAction* action )
{
    m_actions.push_back( action );
    QToolBar::addAction( action );
}

void WQtToolBar::clearButtons()
{
    clear();

    // iterate all items and delete them
    for( std::list< QAction* >::iterator it = m_actions.begin(); it != m_actions.end(); ++it )
    {
        delete ( *it );
    }

    // clear the lists
    m_actions.clear();
}

