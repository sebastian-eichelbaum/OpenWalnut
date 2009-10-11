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

#include <QtGui/QPushButton>
#include <QtGui/QToolBar>
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
     * setup functions for standard (fixed functionality) tabs
     */
    QWidget* setupFileTab();

    /**
     * setup functions for standard (fixed functionality) tabs
     */
    QWidget* setupModuleTab();


    /**
     * getter for standard control widgets
     */
    QPushButton* getLoadButton();
    QPushButton* getQuitButton();

    QPushButton* getAxiButton();
    QPushButton* getCorButton();
    QPushButton* getSagButton();

protected:
private:
    void setup();

    QIcon m_quitIcon;
    QIcon m_saveIcon;
    QIcon m_loadIcon;
    QIcon m_aboutIcon;

    QIcon m_axiIcon;
    QIcon m_corIcon;
    QIcon m_sagIcon;

    QPushButton* m_loadButton;
    QPushButton* m_saveButton;
    QPushButton* m_quitButton;

    QPushButton* m_axiButton;
    QPushButton* m_corButton;
    QPushButton* m_sagButton;
};

#endif  // WQTRIBBONMENU_H
