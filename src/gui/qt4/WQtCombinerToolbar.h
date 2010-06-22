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

#include <QtGui/QToolBar>

#include "../../kernel/WModuleCombinerTypes.h"

class WMainWindow;

/**
 * This is a toolbar. Its main usage for now is the "compatible modules" toolbar.
 */
class WQtCombinerToolbar : public QToolBar
{
    Q_OBJECT

public:
    /**
     * Constructs the toolbar.
     * \param parent the parent widget of this widget, i.e. the widget that manages it.
     * \param compatibles the list of combiners
     */
    explicit WQtCombinerToolbar( WMainWindow* parent, WCombinerTypes::WCompatiblesList compatibles );

    /**
     * destructor
     */
    virtual ~WQtCombinerToolbar();

protected:

private:
};

#endif  // WQTCOMBINERTOOLBAR_H
