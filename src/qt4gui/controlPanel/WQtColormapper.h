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

#ifndef WQTCOLORMAPPER_H
#define WQTCOLORMAPPER_H

#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/signals2/signal.hpp>

#include <QtGui/QDockWidget>
#include <QtGui/QListWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QListWidgetItem>

#include "core/common/WSharedSequenceContainer.h"
#include "core/common/WSharedObject.h"
#include "core/graphicsEngine/WGETexture.h"

#include "../guiElements/WQtDockWidget.h"

class WDataSet;

/**
 * This widget controls the colormapper of openwalnut. It directly interacts with WGEColormapper.
 */
class WQtColormapper : public WQtDockWidget
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
    explicit WQtColormapper( QWidget* parent = 0 );

    /**
     * Destructor.
     */
    virtual ~WQtColormapper();

    /**
     * Select a certain texture in the texture sorter belonging to the specified dataset
     *
     * \param dataSet this data set will be selected after calling this method
     */
    void selectTexture( boost::shared_ptr< WDataSet > dataSet );

signals:
    /**
     * Indicates that a texture has been clicked and return the texture
     *
     * \param texture the texture that got selected
     */
    void textureSelectionChanged( osg::ref_ptr< WGETexture3D > texture );

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

    /**
     * Connection of the WGEColormapping signal "registered" to the member function pushUpdateEvent.
     */
    boost::signals2::connection m_registerConnection;

    /**
     * Connection of the WGEColormapping signal "deregistered" to the member function pushUpdateEvent.
     */
    boost::signals2::connection m_deregisterConnection;

    /**
     * Connection of the WGEColormapping signal "replaced" to the member function pushUpdateEvent.
     */
    boost::signals2::connection m_replaceConnection;

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

    /**
     * This class represents a texture item in the list widget. It provides the pointer to the texture it manages.
     */
    class WQtTextureListItem: public QListWidgetItem
    {
    public:  // NOLINT
        /**
         * Creates new instance of list item. It takes the texture it handles as argument.
         *
         * \param texture the texture to handle with this item
         * \param parent parent widget
         * \param cmapper the colormapper widget managing updates for this item.
         */
        WQtTextureListItem( const osg::ref_ptr< WGETexture3D > texture, WQtColormapper* cmapper, QListWidget* parent = 0 );

        /**
         * Destructor.
         */
        virtual ~WQtTextureListItem();

        /**
         * Returns the associated texture.
         *
         * \return the associated texture
         */
        const osg::ref_ptr< WGETexture3D > getTexture() const;

        /**
         * Create a new widget for this item.
         *
         * \return the widget
         */
        QWidget* getWidget() const;
    private:
        /**
         * The texture that gets handled
         */
        const osg::ref_ptr< WGETexture3D > m_texture;

        /**
         * Parent list.
         */
        QListWidget* m_parent;

        /**
         * Widget representing the item.
         */
        QWidget* m_itemWidget;
    };

private slots:

    /**
     * A item was moved into another row
     *
     * \param sourceParent parent of the item(s) moved.
     * \param sourceStart items moved start index
     * \param sourceEnd items moved end index (when multiple selected)
     * \param destinationParent item moved as childs for this item
     * \param destinationRow items moved to this position
     */
    void rowsMoved( const QModelIndex& sourceParent, int sourceStart, int sourceEnd, const QModelIndex& destinationParent, int destinationRow );

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

    /**
     * change order of items, move currently selected item to bottom
     */
    void moveItemBottom();

    /**
     * change order of items, move currently selected item to top
     */
    void moveItemTop();
};

#endif  // WQTCOLORMAPPER_H

