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

#include <QAction>
#include <QPushButton>
#include <QLabel>
#include <QMenu>

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleCombiner.h"

#include "WMainWindow.h"
#include "WQtToolBar.h"
#include "controlPanel/WQtControlPanel.h"

#include "WQtCombinerToolbar.h"

WQtCombinerToolbar::WQtCombinerToolbar( WMainWindow* parent, const WQtCombinerActionList& compatibles )
    : WQtToolBarBase( "Compatible Modules", parent ),
      m_parent( parent )
{
    QAction* m = parent->getControlPanel()->getMissingModuleAction();
    addAction( m );

    // create the list of actions possible
    addActions( compatibles );
}

WQtCombinerToolbar::WQtCombinerToolbar( WMainWindow* parent )
    : WQtToolBarBase( "Compatible Modules", parent ),
      m_parent( parent )
{
}

WQtCombinerToolbar::~WQtCombinerToolbar()
{
}

void WQtCombinerToolbar::makeEmpty()
{
    clear();
}

void WQtCombinerToolbar::updateButtons( const WQtCombinerActionList& compatibles )
{
    clear();

    // help action
    QAction* m = m_parent->getControlPanel()->getMissingModuleAction();
    addAction( m );

    addActions( compatibles );
}

