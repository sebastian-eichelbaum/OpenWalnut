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

#include <QtGui/QWidget>
#include <QtGui/QListWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPushButton>

#include "../../../common/WSharedSequenceContainer.h"
#include "../../../common/WSharedObject.h"

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
     * Update the list view from the list of data sets.
     */
    virtual void update();

    /**
     * select a certain texture in the texture sorter
     * \param dataSet this data set will be selected after calling this method
     */
    void selectTexture( boost::shared_ptr< WDataSet > dataSet );

signals:
    /**
     * Indicates that a texture has been clicked and return the dataset
     * \param dataSet The dataSet that changed.
     */
    void textureSelectionChanged( boost::shared_ptr< WDataSet > dataSet );

private:
    QListWidget* m_textureListWidget; //!< pointer to the tree widget
    QVBoxLayout* m_layout; //!< Layout of the widget

    QPushButton* m_downButton; //!< button down
    QPushButton* m_upButton; //!< button up

    typedef std::vector< boost::shared_ptr< WDataSet > > DatasetContainerType; //!< Short for container we want to access b< WSharedSequenceContainer
    typedef WSharedSequenceContainer< boost::shared_ptr< WDataSet >, DatasetContainerType > DatasetSharedContainerType; //!< The SharedContainer
    typedef DatasetSharedContainerType::WSharedAccess DatasetAccess; //!< Shorthand for the access handler.
    DatasetSharedContainerType m_textures; //!< Local list of of the textures to sort.

    /**
     * This function returns whether the dataset lhs is above rhs in the texture sorter.
     * \param lhs First dataset.
     * \param rhs Second dataset.
     */
    bool isLess( boost::shared_ptr< WDataSet > lhs, boost::shared_ptr< WDataSet > rhs );

    /**
     * Resorts the datasets in the subject according to the list in the texture sorter.
     */
    void sort();

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
