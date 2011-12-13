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

#include <string>
#include <vector>

#include <QtGui/QMenu>
#include <QtCore/QList>
#include <QtGui/QAction>
#include <QtGui/QWidget>

#include "core/kernel/combiner/WApplyCombiner.h"
#include "core/kernel/combiner/WModuleOneToOneCombiner.h"
#include "core/kernel/WModuleCombinerTypes.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleCombiner.h"
#include "guiElements/WQtModuleOneToOneCombinerAction.h"
#include "WQt4Gui.h"
#include "WQtModuleConfig.h"

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
     * \param exclusionPredicate a predicate which checks for each module whether to exclude it or not. If 0, nothing is excluded.
     *
     * \tparam PredicateT the predicate used for excluding modules
     */
    WQtCombinerActionList( QWidget* parent, WIconManager* icons, WCombinerTypes::WCompatiblesList compatibles,
                           const WQtModuleConfig* exclusionPredicate = 0, bool advancedText = false );
    /**
     * This constructor creates a list of actions recursively from the specified disconnects list.
     * \param parent the parent widget of this widget, i.e. the widget that manages it.
     * \param icons the icon manager holding the right icons for all the prototypes
     * \param disconnects the list of disconnections
     */
    WQtCombinerActionList( QWidget* parent, WIconManager* icons, WCombinerTypes::WDisconnectList disconnects );

    /**
     * Creates a completely empty list.
     */
    WQtCombinerActionList();

    /**
     * Destructor.
     */
    virtual ~WQtCombinerActionList();

protected:
private:
};

#endif  // WQTCOMBINERACTIONLIST_H

