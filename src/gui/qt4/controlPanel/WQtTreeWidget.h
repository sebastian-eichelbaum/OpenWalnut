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

#ifndef WQTTREEWIDGET_H
#define WQTTREEWIDGET_H

#include <QtGui/QTreeWidget>

/**
 * tree widget for the control panel
 */
class WQtTreeWidget  : public QTreeWidget
{
    Q_OBJECT

public:
    /**
     * default constructor
     * \param parent the parent widget of this widget, i.e. the widget that manages this widget
     */
    explicit WQtTreeWidget( QWidget* parent = 0 );

    /**
     * destructor
     */
    virtual ~WQtTreeWidget();

   /**
    * Deletes an entry from the tree
    * \param item The given item will be removed from the tree
    */
    void deleteItem( QTreeWidgetItem* item );

    /**
     * handles the drop event for a tree item
     * \param event
     */
    virtual void dropEvent( QDropEvent *event );

protected:
private:

signals:
    void dragDrop(); //!< signal to notify higher ups of the drag&drop action
};

#endif  // WQTTREEWIDGET_H
