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

#ifndef WQTMENUFILTERED_H
#define WQTMENUFILTERED_H

#include <QtGui/QKeyEvent>
#include <QtGui/QMenu>
#include <QtGui/QLineEdit>
#include <QtGui/QLabel>
#include <QtGui/QWidget>
#include <QtGui/QWidgetAction>
#include <QtGui/QHBoxLayout>

/**
 * A menu derived from QMenu with additional filtering.
 */
class WQtMenuFiltered: public QMenu
{
    Q_OBJECT
public:
    /**
     * Create filtered menu.
     *
     * \param parent parent widget
     * \param config the optional configure action
     */
    WQtMenuFiltered( QAction* config = 0, QWidget* parent = 0 );

    /**
     * Create filtered menu.
     *
     * \param title the menu title
     * \param parent the parent
     * \param config the optional configure action
     */
    WQtMenuFiltered( const QString& title, QAction* config = 0, QWidget* parent = 0 );

    /**
     * Destructor.
     */
    virtual ~WQtMenuFiltered();

public slots:
    /**
     * Updates the filter
     */
    void filterUpdate();

    /**
     * Resets the filter. All elements are visible again.
     */
    void resetFilter();
protected:
    /**
     * Grab key events. Used for filtering.
     *
     * \param e the key event
     */
    virtual void keyPressEvent( QKeyEvent* e );

    /**
     * Called when hiding the menu. Used to reset the filter.
     *
     * \param e the event
     */
    void hideEvent( QHideEvent* e );
private:
    /**
     * Setup filter on given menu
     *
     * \param to the menu
     * \param config optional config action shown in the filter
     */
    static void setupFilter( WQtMenuFiltered* to, QAction* config = 0 );

    /**
     * Filter textfield
     */
    QLineEdit* m_edit;
};

#endif  // WQTMENUFILTERED_H

