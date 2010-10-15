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

#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/signals2/signal.hpp>

#include <QtGui/QWidget>
#include <QtGui/QListWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPushButton>

#include "../../../common/WSharedSequenceContainer.h"
#include "../../../common/WSharedObject.h"
#include "../../../graphicsEngine/WGETexture.h"

class WDataSet;

/**
 * container widget for a tree widget with context menu and some control widgets
 */
class WQtTextureSorter : public QWidget
{
    Q_OBJECT

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

    /**
     * Select a certain texture in the texture sorter belonging to the specified dataset
     *
     * \param dataSet this data set will be selected after calling this method
     */
    void selectTexture( boost::shared_ptr< WDataSet > dataSet );

signals:
    /**
     * Indicates that a texture has been clicked and return the dataset
     * \param dataSet The dataSet that changed.
     */
    void textureSelectionChanged( boost::shared_ptr< WDataSet > dataSet );

protected:
    /**
     * Custom event dispatcher. Gets called by QT's Event system every time an event got sent to this widget. This event handler
     * processes several custom events.
     *
     * \param event the event that got transmitted.
     *
     * \return true if the event got handled properly.
     */
    virtual bool event( QEvent* event );

private:
    QListWidget* m_textureListWidget; //!< pointer to the tree widget
    QVBoxLayout* m_layout; //!< Layout of the widget

    QPushButton* m_downButton; //!< button down
    QPushButton* m_upButton; //!< button up

    /**
     * Connection of the WGEColormapping signal "registered" to the member function pushUpdateEvent.
     */
    boost::signals2::connection m_registerConnection;

    /**
     * Connection of the WGEColormapping signal "deregistered" to the member function pushUpdateEvent.
     */
    boost::signals2::connection m_deregisterConnection;

    /**
     * Connection of the WGEColormapping signal "Sort" to the member function pushUpdateEvent.
     */
    boost::signals2::connection m_sortConnection;

    /**
     * Called by the colormapper causing an update event being pushed to the event queue.
     */
    void pushUpdateEvent();

    /**
     * Update the list view from the list of data sets.
     */
    void update();

private slots:

    /**
     * Handles a click to a texture in the list
     */
    void handleTextureClicked();

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
