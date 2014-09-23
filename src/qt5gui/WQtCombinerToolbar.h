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

#ifndef WQTCOMBINERTOOLBAR_H
#define WQTCOMBINERTOOLBAR_H

#include <QToolBar>

#ifndef Q_MOC_RUN
// There is an issue with the moc compiler and some boost headers where
// moc for qt versions < 5.0 is not able to do a macro substitution.
// The issue occurs in the boost library versions 1.48 and higher,
// and is tested in boost version 1.52 using moc version 4.8.4
// Excluding the relevant headers removes the problem.
#include "core/kernel/WModuleCombinerTypes.h"
#endif

#ifndef Q_MOC_RUN
#include "WQtCombinerActionList.h"
#endif
#include "WQtToolBarBase.h"

class WMainWindow;

/**
 * This is a toolbar. Its main usage for now is the "compatible modules" toolbar.
 */
class WQtCombinerToolbar : public WQtToolBarBase
{
    Q_OBJECT

public:
    /**
     * Constructs the toolbar.
     * \param parent the parent widget of this widget, i.e. the widget that manages it.
     * \param compatibles the list of combiners
     */
    WQtCombinerToolbar( WMainWindow* parent, const WQtCombinerActionList& compatibles );

    /**
     * This creates an empty toolbar only containing the dummy button to reserve the size.
     *
     * \param parent the parent widget.
     */
    explicit WQtCombinerToolbar( WMainWindow* parent );

    /**
     * destructor
     */
    virtual ~WQtCombinerToolbar();

    /**
     * Update the toolbar to represent the compatibles given as parameter.
     *
     * \param compatibles The compatibles to produce the buttons for.
     */
    void updateButtons( const WQtCombinerActionList& compatibles );

    /**
     * Make the toolbar appear empty but not disappear.
     */
    void makeEmpty();

protected:
private:
    WMainWindow* m_parent; //!< The widget managing this widget.
};

#endif  // WQTCOMBINERTOOLBAR_H

