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

#ifndef WQTNETWORKEDITOR_H
#define WQTNETWORKEDITOR_H

#include <string>

#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsView>
#include <QtGui/QWidget>
#include <QtGui/QDockWidget>
#include <QtGui/QVBoxLayout>

#include "WQtNetworkScene.h"

class WMainWindow;

/**
 * container widget to hold as GL widget and a slider
 */
class WQtNetworkEditor : public QDockWidget
{
    Q_OBJECT

public:
    /**
     * default constructor
     *
     * \param title Title will be displayed above the widget
     * \param parent The widget that manages this widget
     * \param sliderTitle Name of the slider corresponding to the property it manipulates
     */
    explicit WQtNetworkEditor( QString title, WMainWindow* parent );

    /**
     * destructor.
     */
    virtual ~WQtNetworkEditor();

    void addModule();
    //void addModuleInput();
    //void addModuleOutput();

    //void itemSelected( QGraphicsItem *item );
protected:

    /**
     * Reference to the main window of the application.
     */
    WMainWindow* m_mainWindow;

private:

    WQtNetworkScene* m_scene; //!< GraphicsScene

    QGraphicsView* m_view; //!< GraphicsView

    QWidget* m_panel; //!< panel

    QVBoxLayout* m_layout; //!< Layout
};

#endif  // WQTNETWORKEDITOR_H
