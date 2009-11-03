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

#ifndef WQTRIBBONMENU_H
#define WQTRIBBONMENU_H

#include <map>

#include <QtGui/QTabWidget>
#include <QtGui/QToolBar>

#include "WQtMenuPage.h"
#include "../guiElements/WQtPushButton.h"
/**
 * implementation of a ribbon menu like widget
 */
class WQtRibbonMenu  : public QToolBar
{
public:
    /**
     * default constructor
     */
    explicit WQtRibbonMenu( QWidget* parent );

    /**
     * destructor
     */
    virtual ~WQtRibbonMenu();

    /**
     *
     */
    WQtMenuPage* addTab( QString name );

    /**
     *
     */
    WQtPushButton* addPushButton( QString name, QString tabName, QIcon icon, QString label = 0 );

    /**
     *
     */
    WQtPushButton* getButton( QString name );

protected:
private:
    std::map< QString, WQtMenuPage*>m_tabList;

    std::map< QString, WQtPushButton*>m_buttonList;

    QTabWidget* m_tabWidget;
};

#endif  // WQTRIBBONMENU_H
