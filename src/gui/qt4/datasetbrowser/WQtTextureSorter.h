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

#ifndef WQTTEXTURESORTER_H
#define WQTTEXTURESORTER_H

#include <QtGui/QWidget>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>

/**
 * container widget for a tree widget with context menu and some control widgets
 */
class WQtTextureSorter : public QWidget
{
public:
    /**
     * Default constructor.
     *
     * \param parent Parent widget.
     *
     * \return
     */
    explicit WQtTextureSorter( QWidget* parent = 0 );

    /**
     * Destructor.
     */
    virtual ~WQtTextureSorter();

private:
    QListWidget* m_textureListWidget; //!< pointer to the tree widget
    QVBoxLayout* m_layout; //!< Layout of the widget

    QPushButton* m_downButton; //!< button down
    QPushButton* m_upButton; //!< button up

private slots:
    /**
     * change order of items, move currently selected item down
     */
    void moveItemDown();

    /**
     * change order of items, move currently selected item up
     */
    void moveItemUp();
};

#endif  // WQTTEXTURESORTER_H
