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

#ifndef WQTCOMBINERACTIONLIST_H
#define WQTCOMBINERACTIONLIST_H

#include <QtCore/QList>
#include <QtGui/QAction>
#include <QtGui/QWidget>

#include "../../kernel/WModuleCombinerTypes.h"

#include "WIconManager.h"

/**
 * This class represents a list of actions to apply a bunch of modules to others. It basically interprets the WCompatiblesList or WDisconnectList and
 * builds a list of appropriate actions. It can be used to build menus, toolbars and so on.
 */
class WQtCombinerActionList: public QList< QAction* >
{
public:

    /**
     * This constructor creates a list of actions recursively from the specified compatibles list.
     * \param parent the parent widget of this widget, i.e. the widget that manages it.
     * \param icons the icon manager holding the right icons for all the prototypes
     * \param compatibles the list of combiners
     * \param advancedText if true, the complete connector/module name is displayed in all actions
     * \param ignoreWhiteList if true, the list won't be filtered using the whitelist
     */
    WQtCombinerActionList( QWidget* parent, WIconManager* icons, WCombinerTypes::WCompatiblesList compatibles, bool advancedText = false,
                                                                                                               bool ignoreWhiteList = false );
    /**
     * This constructor creates a list of actions recursively from the specified disconnects list.
     * \param parent the parent widget of this widget, i.e. the widget that manages it.
     * \param icons the icon manager holding the right icons for all the prototypes
     * \param disconnects the list of disconnections
     */
    WQtCombinerActionList( QWidget* parent, WIconManager* icons, WCombinerTypes::WDisconnectList disconnects );

    /**
     * Destructor.
     */
    virtual ~WQtCombinerActionList();

protected:

private:
};

#endif  // WQTCOMBINERACTIONLIST_H

